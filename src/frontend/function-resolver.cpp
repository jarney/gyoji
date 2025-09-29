#include <jlang-frontend/function-resolver.hpp>
#include <variant>
#include <stdio.h>

using namespace JLang::mir;
using namespace JLang::context;
using namespace JLang::frontend;
using namespace JLang::frontend::tree;

FunctionResolver::FunctionResolver(
    JLang::context::CompilerContext & _compiler_context,
    const JLang::frontend::ParseResult & _parse_result,
    JLang::mir::MIR & _mir,
    JLang::frontend::TypeResolver & _type_resolver
    )
    : compiler_context(_compiler_context)
    , parse_result(_parse_result)
    , mir(_mir)
    , type_resolver(_type_resolver)
{}

FunctionResolver::~FunctionResolver()
{}

void FunctionResolver::resolve()
{
  // To resolve the functions, we need only iterate the
  // input parse tree and pull out any type declarations,
  // resolving them down to their primitive types.

  // TODO: Split the type extraction
  // out away from the function extraction(?).
    extract_functions(parse_result.get_translation_unit().get_statements());
}
void
FunctionResolver::extract_from_namespace(
    const FileStatementNamespace & namespace_declaration)
{
    const auto & statements = namespace_declaration.get_statement_list().get_statements();
    extract_functions(statements);
}

void
FunctionResolver::extract_from_class_definition(const ClassDefinition & definition)
{
    // TODO: We should extract members as
    // functions from classes so we can set up method calls.
    // and prepare resolution of class members as variables.
    // We'll do this last after all the other operations are
    // supported because we want to first work with 'normal'
    // C constructs before diving into the OO aspects.
    //
    //fprintf(stderr, "Extracting from class definition, constructors and destructors which are special-case functions.\n");
    // These must be linked back to their corresponding type definitions
    // so that we can generate their code.
}

void
FunctionResolver::extract_functions(const std::vector<JLang::owned<FileStatement>> & statements)
{
    for (const auto & statement : statements) {
	const auto & file_statement = statement->get_statement();
	if (std::holds_alternative<JLang::owned<FileStatementFunctionDeclaration>>(file_statement)) {
	    // Nothing, no functions can exist here.
	}
	else if (std::holds_alternative<JLang::owned<FileStatementFunctionDefinition>>(file_statement)) {
	    // This is the only place that functions can be extracted from.
	    // We make this a separate object because we want convenient
	    // access to certain pieces of context used in resolution.
	    FunctionDefinitionResolver function_def_resolver(
		compiler_context,
		*std::get<JLang::owned<FileStatementFunctionDefinition>>(file_statement),
		mir,
		type_resolver
		);
	    function_def_resolver.resolve();
	}
	else if (std::holds_alternative<JLang::owned<FileStatementGlobalDefinition>>(file_statement)) {
	    // TODO:
	    // We want to resolve global variables at this stage, but
	    // for now, let's handle resolution of local and stack variables
	    // before we dive into global resolution.
	}
	else if (std::holds_alternative<JLang::owned<ClassDeclaration>>(file_statement)) {
	    // Nothing, no functions can exist here.
	    // Class declarations should already be resolved by the type_resolver earlier.
	}
	else if (std::holds_alternative<JLang::owned<ClassDefinition>>(file_statement)) {
	    // Constructors, Destructors, and methods are special cases.
	    extract_from_class_definition(*std::get<JLang::owned<ClassDefinition>>(file_statement));
	}
	else if (std::holds_alternative<JLang::owned<EnumDefinition>>(file_statement)) {
	    // Nothing, no functions can exist here.
	    // Enums should already be resolved by the type_resolver earlier.
	}
	else if (std::holds_alternative<JLang::owned<TypeDefinition>>(file_statement)) {
	    // Nothing, no functions can exist here.
	    // Typedefs should already be resolved by the type_resolver earlier.
	}
	else if (std::holds_alternative<JLang::owned<FileStatementNamespace>>(file_statement)) {
	    extract_from_namespace(*std::get<JLang::owned<FileStatementNamespace>>(file_statement));
	}
	else if (std::holds_alternative<JLang::owned<FileStatementUsing>>(file_statement)) {
	    // Namespace using is largely handled by the parse stage, so we don't
	    // need to do any function resolution here.
	}
	else {
	    compiler_context
		.get_errors()
		.add_simple_error(statement->get_source_ref(),
				  "Compiler bug!  Please report this message",
				  "Unknown statement type in variant, extracting statements from file (compiler bug)"
		    );
	}
    }
}

////////////////////////////////////////////////
// FunctionDefinitionResolver
////////////////////////////////////////////////

FunctionDefinitionResolver::FunctionDefinitionResolver(
    JLang::context::CompilerContext & _compiler_context,
    const JLang::frontend::tree::FileStatementFunctionDefinition & _function_definition,
    JLang::mir::MIR & _mir,
    JLang::frontend::TypeResolver & _type_resolver
    )
    : compiler_context(_compiler_context)
    , function_definition(_function_definition)
    , mir(_mir)
    , type_resolver(_type_resolver)
{}
FunctionDefinitionResolver::~FunctionDefinitionResolver()
{}

void
FunctionDefinitionResolver::resolve()
{
    extract_from_function_definition(function_definition);
}

void
FunctionDefinitionResolver::extract_from_expression_primary_identifier(
    JLang::mir::Function & function,
    size_t & current_block,
    size_t & returned_tmpvar,
    const JLang::frontend::tree::ExpressionPrimaryIdentifier & expression
    )
{
    // At this point, we should try to identify what this
    // identifier actually refers to.
    // * First look inside the function's declared variables in scope.
    // * Next, look in the global namespace for functions.
    // * Finally, look in the global namespace for external
    //   variables like 'global' or 'static' variables.
    // If the identifier is a function.
    // There are a few cases:
    // * The it is a function and is used in an assignment
    //   to a function pointer.  In this case, we should
    //   return the type as a function pointer type
    // * If it is used in a function call directly, we should not
    //   emit the code, but should just use an 'immediate'
    //   execution of whatever it is.
    // * Otherwise, emit it as a 'load' and just follow our nose
    //   at runtime.
    // * Maybe we really should 'flatten' our access here.
    
    if (expression.get_identifier().get_identifier_type() == Terminal::IDENTIFIER_LOCAL_SCOPE) {
	const LocalVariable *localvar = function.get_local(
	    expression.get_identifier().get_value()
	    );
	if (localvar != nullptr) {
	    returned_tmpvar = function.tmpvar_define(localvar->get_type());
	    auto operation = std::make_unique<OperationLocalVariable>(
		expression.get_identifier().get_source_ref(),
		returned_tmpvar,
		expression.get_identifier().get_value(),
		localvar->get_type()
		);
	    function.get_basic_block(current_block).add_statement(std::move(operation));

	    return;
	}
	// TODO: Add enums and class members to the resolution
	// later when those are handled.
	
	// Next, we should check class members (if applicable).
	
	// Next, we should check for 'enum' identifiers.

	// If all else fails, we could not resolve it and should
	// emit an error.
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_source_ref(),
		"Local variable could not be resolved",
		std::string("Local variable ") + expression.get_identifier().get_value() + std::string(" was not found in this scope.")
		);
    }
    else if (expression.get_identifier().get_identifier_type() == Terminal::IDENTIFIER_GLOBAL_SCOPE) {
	// Type here should be the type of a function-pointer.

	// Instead of looking in a list of function prototypes (specifically)
	// the prior step (type resolution) should also extract a set of global
	// identifiers like classes, functions, global variables, methods, etc.
	// along with their associated types.  This will allow us to simply associate
	// an identifier with a type and move on instead of having to treat each
	// of these items separately.
	
	// Look in the list of functions,
	// this might be a function pointer assignment or
	// a global variable.
	const JLang::mir::Symbol *symbol = mir.get_symbols().get_symbol(
	    expression.get_identifier().get_fully_qualified_name()
	    );
	if (symbol == nullptr) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    expression.get_source_ref(),
		    "Unresolve symbol",
		    std::string("Local variable ") + expression.get_identifier().get_fully_qualified_name() + std::string(" was not found in this scope.")
		    );
	}
	returned_tmpvar = function.tmpvar_define(symbol->get_type());

	auto operation = std::make_unique<OperationSymbol>(
	    expression.get_identifier().get_source_ref(),
	    returned_tmpvar,
	    expression.get_identifier().get_fully_qualified_name()
	    );
	function.get_basic_block(current_block).add_statement(std::move(operation));
    }
}

void
FunctionDefinitionResolver::extract_from_expression_primary_nested(
    JLang::mir::Function & function,
    size_t & current_block,
    size_t & returned_tmpvar,
    const JLang::frontend::tree::ExpressionPrimaryNested & expression)
{
    // Nested expressions don't emit blocks on their own, just run whatever is nested.
    extract_from_expression(
	function,
	current_block,
	returned_tmpvar,
	expression.get_expression()
	);
}

void
FunctionDefinitionResolver::extract_from_expression_primary_literal_char(
    JLang::mir::Function & function,
    size_t & current_block,
    size_t & returned_tmpvar,
    const JLang::frontend::tree::ExpressionPrimaryLiteralChar & expression)
{
    returned_tmpvar = function.tmpvar_define(mir.get_types().get_type("u8"));
    auto operation = std::make_unique<OperationLiteralChar>(
	expression.get_source_ref(),
	returned_tmpvar,
	expression.get_value()
	);

    function
	.get_basic_block(current_block)
	.add_statement(std::move(operation));
}

void
FunctionDefinitionResolver::extract_from_expression_primary_literal_string(
    JLang::mir::Function & function,
    size_t & current_block,
    size_t & returned_tmpvar,
    const JLang::frontend::tree::ExpressionPrimaryLiteralString & expression)
{
    returned_tmpvar = function.tmpvar_define(mir.get_types().get_type("u8*"));
    auto operation = std::make_unique<OperationLiteralString>(
	expression.get_source_ref(),	    
	returned_tmpvar,
	expression.get_value()
	);
    function
	.get_basic_block(current_block)
	.add_statement(std::move(operation));
}

void
FunctionDefinitionResolver::extract_from_expression_primary_literal_int(
    JLang::mir::Function & function,
    size_t & current_block,
    size_t & returned_tmpvar,
    const JLang::frontend::tree::ExpressionPrimaryLiteralInt & expression)
{
    returned_tmpvar = function.tmpvar_define(mir.get_types().get_type(expression.get_type()));
    auto operation = std::make_unique<OperationLiteralInt>(
	expression.get_source_ref(),
	returned_tmpvar,
	expression.get_value()
	);
    function
	.get_basic_block(current_block)
	.add_statement(std::move(operation));
}

void
FunctionDefinitionResolver::extract_from_expression_primary_literal_float(
    JLang::mir::Function & function,
    size_t & current_block,
    size_t & returned_tmpvar,
    const JLang::frontend::tree::ExpressionPrimaryLiteralFloat & expression)
{
    returned_tmpvar = function.tmpvar_define(mir.get_types().get_type(expression.get_type()));
    auto operation = std::make_unique<OperationLiteralFloat>(
	expression.get_source_ref(),
	returned_tmpvar,
	expression.get_value()
	);
    function
	.get_basic_block(current_block)
	.add_statement(std::move(operation));
}

static bool is_bool_type(const Type* value)
{
    if (value->get_name() == "bool") {
	return true;
    }
    return false;
}

static bool is_index_type(const Type* value)
{
    if (value->get_name() == "u32") {
	return true;
    }
    return false;
}

void
FunctionDefinitionResolver::extract_from_expression_postfix_array_index(
    JLang::mir::Function & function,
    size_t & current_block,
    size_t & returned_tmpvar,
    const ExpressionPostfixArrayIndex & expression)
{
    size_t array_tmpvar;
    size_t index_tmpvar;
    extract_from_expression(function, current_block, array_tmpvar, expression.get_array());
    extract_from_expression(function, current_block, index_tmpvar, expression.get_index());

    const Type *array_type = function.tmpvar_get(array_tmpvar)->get_type();
    if (array_type->get_type() != Type::TYPE_POINTER) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_array().get_source_ref(),
		"Array type must be a pointer to another type",
		std::string("Type of array is not a pointer type.")
		);
    }
    
    if (!is_index_type(function.tmpvar_get(index_tmpvar)->get_type())) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_index().get_source_ref(),
		"Array index must be an integer type",
		std::string("Type of index is not an index")
		);
	return;
    }
    
    returned_tmpvar = function.tmpvar_define(array_type->get_pointer_target());
    auto operation = std::make_unique<OperationArrayIndex>(
	expression.get_source_ref(),
	returned_tmpvar,
	index_tmpvar,
	array_type
	);
    function
	.get_basic_block(current_block)
	.add_statement(std::move(operation));
}

void
FunctionDefinitionResolver::extract_from_expression_postfix_function_call(
    JLang::mir::Function & function,
    size_t & current_block,
    size_t & returned_tmpvar,
    const ExpressionPostfixFunctionCall & expression)
{
    // Extract the expression itself from the arguments.
    size_t function_type_tmpvar;
    extract_from_expression(function, current_block, function_type_tmpvar, expression.get_function());
    
    std::vector<size_t> arg_types;
    for (const auto & arg_expr : expression.get_arguments().get_arguments()) {
	size_t arg_returned_value;
	extract_from_expression(function, current_block, arg_returned_value, *arg_expr);
	arg_types.push_back(arg_returned_value);
    }

    const Type *function_pointer_type = function.tmpvar_get(function_type_tmpvar)->get_type();
    if (function_pointer_type->get_type() != Type::TYPE_FUNCTION_POINTER) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_function().get_source_ref(),
		"Called object is not a function.",
		std::string("Type of object being called is not a function, but is a ") + function_pointer_type->get_name() + std::string(" instead.")
		);
    }
	
    // We declare that we return the vale that the function
    // will return.
    returned_tmpvar = function.tmpvar_define(function_pointer_type->get_return_type());
    
    auto operation = std::make_unique<OperationFunctionCall>(
	expression.get_source_ref(),
	returned_tmpvar,
	function_type_tmpvar
	);
    
    std::string call_args = "";
    for (const auto & av : arg_types) {
	call_args += std::string("_") + std::to_string(av) + " ";
	operation->add_operand(av);
    }
    function
	.get_basic_block(current_block)
	.add_statement(std::move(operation));
    
}

void
FunctionDefinitionResolver::extract_from_expression_postfix_dot(
    JLang::mir::Function & function,
    size_t & current_block,
    size_t & returned_tmpvar,
    const ExpressionPostfixDot & expression)
{
    size_t class_tmpvar;
    extract_from_expression(function, current_block, class_tmpvar, expression.get_expression());

    const Type *class_type = function.tmpvar_get(class_tmpvar)->get_type();
    if (class_type->get_type() != Type::TYPE_COMPOSITE) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_expression().get_source_ref(),
		"Member access must be applied to a class.",
		std::string("Type of object being accessed is not a class, but is a ") + class_type->get_name() + std::string(" instead.")
		);
    }
    const std::string & member_name = expression.get_identifier();

    const TypeMember *member = class_type->member_get(member_name);
    if (member == nullptr) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_expression().get_source_ref(),
		"Class does not have this member.",
		std::string("Class does not have member '") + member_name + std::string("'.")
		);
    }

    returned_tmpvar = function.tmpvar_define(member->get_type());
    auto operation = std::make_unique<OperationDot>(
	expression.get_source_ref(),
	returned_tmpvar,
	member_name
	);
    function
	.get_basic_block(current_block)
	.add_statement(std::move(operation));
}

void
FunctionDefinitionResolver::extract_from_expression_postfix_arrow(
    JLang::mir::Function & function,
    size_t & current_block,
    size_t & returned_tmpvar,
    const ExpressionPostfixArrow & expression)
{
    size_t classptr_tmpvar;
    extract_from_expression(function, current_block, classptr_tmpvar, expression.get_expression());

    const Type *classptr_type = function.tmpvar_get(classptr_tmpvar)->get_type();
    if (classptr_type->get_type() != Type::TYPE_POINTER) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_expression().get_source_ref(),
		"Arrow (->) operator must be used on a pointer to a class.",
		std::string("Type of object being accessed is not a pointer to a class, but is a ") + classptr_type->get_name() + std::string(" instead.")
		);
    }
    const Type *class_type = classptr_type->get_pointer_target();
    
    if (class_type->get_type() != Type::TYPE_COMPOSITE) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_expression().get_source_ref(),
		"Arrow (->) access must be applied to a pointer to a class.",
		std::string("Type of object being accessed is not a pointer to a class , but is a pointer to ") + class_type->get_name() + std::string(" instead.")
		);
    }
    const std::string & member_name = expression.get_identifier();

    const TypeMember *member = class_type->member_get(member_name);
    if (member == nullptr) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_expression().get_source_ref(),
		"Class does not have this member.",
		std::string("Class does not have member '") + member_name + std::string("'.")
		);
    }

    returned_tmpvar = function.tmpvar_define(member->get_type());
    auto operation = std::make_unique<OperationArrow>(
	expression.get_source_ref(),
	returned_tmpvar,
	member_name
	);
    function
	.get_basic_block(current_block)
	.add_statement(std::move(operation));

}
void
FunctionDefinitionResolver::extract_from_expression_postfix_incdec(
    JLang::mir::Function & function,
    size_t & current_block,
    size_t & returned_tmpvar,
    const ExpressionPostfixIncDec & expression)
{
    size_t operand_tmpvar;
    extract_from_expression(function, current_block, operand_tmpvar, expression.get_expression());

    returned_tmpvar = function.tmpvar_duplicate(operand_tmpvar);
    
    if (expression.get_type() == ExpressionPostfixIncDec::INCREMENT) {
	auto operation = std::make_unique<OperationPostIncrement>(
	    expression.get_source_ref(),
	    returned_tmpvar,
	    operand_tmpvar
	    );
	function
	    .get_basic_block(current_block)
	    .add_statement(std::move(operation));
    }
    else if (expression.get_type() == ExpressionPostfixIncDec::DECREMENT) {
	auto operation = std::make_unique<OperationPostDecrement>(
	    expression.get_source_ref(),
	    returned_tmpvar,
	    operand_tmpvar
	    );
	function
	    .get_basic_block(current_block)
	    .add_statement(std::move(operation));
    }
    else {
	compiler_context
	    .get_errors()
	    .add_simple_error(expression.get_source_ref(),
			      "Compiler bug!  Please report this message",
			      "Unknown postfix operator encountered"
		);
    }
}



void
FunctionDefinitionResolver::extract_from_expression_unary_prefix(
    JLang::mir::Function & function,
    size_t & current_block,
    size_t & returned_tmpvar,
    const ExpressionUnaryPrefix & expression)
{

    // Extract the prior expression
    // and if not otherwise specified,
    // the operation will return the
    // same type as the operand.
    size_t operand_tmpvar;
    extract_from_expression(
	function,
	current_block,
	operand_tmpvar,
	expression.get_expression()
	);
    
    const Type *operand_type = function.tmpvar_get(operand_tmpvar)->get_type();
    if (operand_type == nullptr) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_source_ref(),
		"Compiler bug!  Please report this message",
		"Operand must be a valid type."
		);
    }

    if (expression.get_type() == ExpressionUnaryPrefix::INCREMENT) {
	returned_tmpvar = function.tmpvar_duplicate(operand_tmpvar);
	auto operation = std::make_unique<OperationPreIncrement>(
	    expression.get_source_ref(),
	    returned_tmpvar,
	    operand_tmpvar
	    );
	function
	    .get_basic_block(current_block)
	    .add_statement(std::move(operation));
    }
    else if (expression.get_type() == ExpressionUnaryPrefix::DECREMENT) {
	returned_tmpvar = function.tmpvar_duplicate(operand_tmpvar);
	auto operation = std::make_unique<OperationPreDecrement>(
	    expression.get_source_ref(),
	    returned_tmpvar,
	    operand_tmpvar
	    );
	function
	    .get_basic_block(current_block)
	    .add_statement(std::move(operation));
    }
    else if (expression.get_type() == ExpressionUnaryPrefix::ADDRESSOF) {
	const Type * pointer_to_operand_type = mir.get_types().get_pointer_to(operand_type, expression.get_source_ref());
	returned_tmpvar = function.tmpvar_define(pointer_to_operand_type);
	auto operation = std::make_unique<OperationAddressOf>(
	    expression.get_source_ref(),
	    returned_tmpvar,
	    operand_tmpvar
	    );
	function
	    .get_basic_block(current_block)
	    .add_statement(std::move(operation));
    }
    else if (expression.get_type() == ExpressionUnaryPrefix::DEREFERENCE) {
	if (operand_type->get_type() != Type::TYPE_POINTER) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    expression.get_expression().get_source_ref(),
		    "Cannot dereference non-pointer",
		    std::string("Attempting to de-reference non-pointer type ") + operand_type->get_name()
		    );
	    return;
	}
	returned_tmpvar = function.tmpvar_define(operand_type);
	auto operation = std::make_unique<OperationDereference>(
	    expression.get_source_ref(),	    
	    returned_tmpvar,
	    operand_tmpvar
	    );
	function
	    .get_basic_block(current_block)
	    .add_statement(std::move(operation));
    }
    else if (expression.get_type() == ExpressionUnaryPrefix::PLUS) {
	// Unary plus does nothing, really, so why bother?  We just don't
	// bother to do anything and just wire the return into the operand
	// directly instead of creating a new tmpvar and assigning it.
	returned_tmpvar = operand_tmpvar;
    }
    else if (expression.get_type() == ExpressionUnaryPrefix::MINUS) {
	returned_tmpvar = function.tmpvar_duplicate(operand_tmpvar);
	auto operation = std::make_unique<OperationNegate>(
	    expression.get_source_ref(),	    
	    returned_tmpvar,
	    operand_tmpvar
	    );
	function
	    .get_basic_block(current_block)
	    .add_statement(std::move(operation));
    }
    else if (expression.get_type() == ExpressionUnaryPrefix::BITWISE_NOT) {
	returned_tmpvar = function.tmpvar_duplicate(operand_tmpvar);
	auto operation = std::make_unique<OperationBitwiseNot>(
	    expression.get_source_ref(),	    
	    returned_tmpvar,
	    operand_tmpvar
	    );
	function
	    .get_basic_block(current_block)
	    .add_statement(std::move(operation));
    }
    else if (expression.get_type() == ExpressionUnaryPrefix::LOGICAL_NOT) {
	if (!is_bool_type(function.tmpvar_get(operand_tmpvar)->get_type())) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    expression.get_expression().get_source_ref(),
		    "Logical not (!) must operate on 'bool' expressions.",
		    std::string("Type of condition expression should be 'bool' and was ") + function.tmpvar_get(operand_tmpvar)->get_type()->get_name()
		    );
	}
	returned_tmpvar = function.tmpvar_duplicate(operand_tmpvar);
	auto operation = std::make_unique<OperationLogicalNot>(
	    expression.get_source_ref(),	    
	    returned_tmpvar,
	    operand_tmpvar
	    );
	function
	    .get_basic_block(current_block)
	    .add_statement(std::move(operation));
    }
    else {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_expression().get_source_ref(),
		"Compiler Bug!",
		"Encountered unknown unary prefix expression"
		);
    }
  
}
void
FunctionDefinitionResolver::extract_from_expression_unary_sizeof_type(
    JLang::mir::Function & function,
    size_t & current_block,
    size_t & returned_tmpvar,
    const ExpressionUnarySizeofType & expression)
{
    const Type * operand_type = type_resolver.extract_from_type_specifier(expression.get_type_specifier());
    const Type * u64_type = mir.get_types().get_type("u64");
    size_t operand_tmpvar = function.tmpvar_define(operand_type);
    returned_tmpvar = function.tmpvar_define(u64_type);
    auto operation = std::make_unique<OperationSizeofType>(
	expression.get_source_ref(),	    
	returned_tmpvar,
	operand_tmpvar
	);
    function
	.get_basic_block(current_block)
	.add_statement(std::move(operation));
}

void
FunctionDefinitionResolver::extract_from_expression_binary(
    JLang::mir::Function & function,
    size_t & current_block,
    size_t & returned_tmpvar,
    const ExpressionBinary & expression)
{
    size_t a_tmpvar;
    size_t b_tmpvar;
    
    extract_from_expression(function, current_block, a_tmpvar, expression.get_a());
    extract_from_expression(function, current_block, b_tmpvar, expression.get_b());

    std::string atypename = function.tmpvar_get(a_tmpvar)->get_type()->get_name();
    std::string btypename = function.tmpvar_get(b_tmpvar)->get_type()->get_name();
    if (atypename != btypename) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_source_ref(),
		"Type mismatch in binary operation",
		std::string("The type of operands should match: a= ") + atypename + std::string(" b=") + btypename
		);
	return;
    }
    ExpressionBinary::OperationType op_type;
    op_type = expression.get_operator();

    if (op_type == ExpressionBinary::ADD) {
	returned_tmpvar = function.tmpvar_duplicate(a_tmpvar);
	auto operation = std::make_unique<OperationBinary>(
	    Operation::OP_ADD,
	    expression.get_source_ref(),	    
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar
	    );
	function
	    .get_basic_block(current_block)
	    .add_statement(std::move(operation));
    }	
    else if (op_type == ExpressionBinary::SUBTRACT) {
	returned_tmpvar = function.tmpvar_duplicate(a_tmpvar);
	auto operation = std::make_unique<OperationBinary>(
	    Operation::OP_SUBTRACT,
	    expression.get_source_ref(),	    
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar
	    );
	function
	    .get_basic_block(current_block)
	    .add_statement(std::move(operation));
    }
    else if (op_type == ExpressionBinary::MULTIPLY) {
	returned_tmpvar = function.tmpvar_duplicate(a_tmpvar);
	auto operation = std::make_unique<OperationBinary>(
	    Operation::OP_MULTIPLY,
	    expression.get_source_ref(),	    
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar
	    );
	function
	    .get_basic_block(current_block)
	    .add_statement(std::move(operation));
    }
    else if (op_type == ExpressionBinary::DIVIDE) {
	returned_tmpvar = function.tmpvar_duplicate(a_tmpvar);
	auto operation = std::make_unique<OperationBinary>(
	    Operation::OP_MULTIPLY,
	    expression.get_source_ref(),	    
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar
	    );
	function
	    .get_basic_block(current_block)
	    .add_statement(std::move(operation));
    }
    else if (op_type == ExpressionBinary::MODULO) {
	returned_tmpvar = function.tmpvar_duplicate(a_tmpvar);
	auto operation = std::make_unique<OperationBinary>(
	    Operation::OP_MODULO,
	    expression.get_source_ref(),	    
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar
	    );
	function
	    .get_basic_block(current_block)
	    .add_statement(std::move(operation));
    }
    else if (op_type == ExpressionBinary::LOGICAL_AND) {
	returned_tmpvar = function.tmpvar_define(mir.get_types().get_type("bool"));
	auto operation = std::make_unique<OperationBinary>(
	    Operation::OP_LOGICAL_AND,
	    expression.get_source_ref(),	    
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar
	    );
	function
	    .get_basic_block(current_block)
	    .add_statement(std::move(operation));
    }
    else if (op_type == ExpressionBinary::LOGICAL_OR) {
	returned_tmpvar = function.tmpvar_define(mir.get_types().get_type("bool"));
	auto operation = std::make_unique<OperationBinary>(
	    Operation::OP_LOGICAL_OR,
	    expression.get_source_ref(),	    
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar
	    );
	function
	    .get_basic_block(current_block)
	    .add_statement(std::move(operation));
    }
    else if (op_type == ExpressionBinary::BITWISE_AND) {
	returned_tmpvar = function.tmpvar_duplicate(a_tmpvar);
	auto operation = std::make_unique<OperationBinary>(
	    Operation::OP_BITWISE_AND,
	    expression.get_source_ref(),	    
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar
	    );
	function
	    .get_basic_block(current_block)
	    .add_statement(std::move(operation));
    }
    else if (op_type == ExpressionBinary::BITWISE_OR) {
	returned_tmpvar = function.tmpvar_duplicate(a_tmpvar);
	auto operation = std::make_unique<OperationBinary>(
	    Operation::OP_BITWISE_OR,
	    expression.get_source_ref(),	    
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar
	    );
	function
	    .get_basic_block(current_block)
	    .add_statement(std::move(operation));
    }
    else if (op_type == ExpressionBinary::BITWISE_XOR) {
	returned_tmpvar = function.tmpvar_duplicate(a_tmpvar);
	auto operation = std::make_unique<OperationBinary>(
	    Operation::OP_BITWISE_XOR,
	    expression.get_source_ref(),	    
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar
	    );
	function
	    .get_basic_block(current_block)
	    .add_statement(std::move(operation));
    }
    else if (op_type == ExpressionBinary::SHIFT_LEFT) {
	returned_tmpvar = function.tmpvar_duplicate(a_tmpvar);
	auto operation = std::make_unique<OperationBinary>(
	    Operation::OP_SHIFT_LEFT,
	    expression.get_source_ref(),	    
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar
	    );
	function
	    .get_basic_block(current_block)
	    .add_statement(std::move(operation));
    }
    else if (op_type == ExpressionBinary::SHIFT_RIGHT) {
	returned_tmpvar = function.tmpvar_duplicate(a_tmpvar);
	auto operation = std::make_unique<OperationBinary>(
	    Operation::OP_SHIFT_RIGHT,
	    expression.get_source_ref(),	    
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar
	    );
	function
	    .get_basic_block(current_block)
	    .add_statement(std::move(operation));
    }
    else if (op_type == ExpressionBinary::COMPARE_LT) {
	returned_tmpvar = function.tmpvar_define(mir.get_types().get_type("bool"));
	auto operation = std::make_unique<OperationBinary>(
	    Operation::OP_COMPARE_LT,
	    expression.get_source_ref(),	    
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar
	    );
	function
	    .get_basic_block(current_block)
	    .add_statement(std::move(operation));
    }
    else if (op_type == ExpressionBinary::COMPARE_GT) {
	returned_tmpvar = function.tmpvar_define(mir.get_types().get_type("bool"));
	auto operation = std::make_unique<OperationBinary>(
	    Operation::OP_COMPARE_GT,
	    expression.get_source_ref(),	    
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar
	    );
	function
	    .get_basic_block(current_block)
	    .add_statement(std::move(operation));
    }
    else if (op_type == ExpressionBinary::COMPARE_LE) {
	returned_tmpvar = function.tmpvar_define(mir.get_types().get_type("bool"));
	auto operation = std::make_unique<OperationBinary>(
	    Operation::OP_COMPARE_LE,
	    expression.get_source_ref(),	    
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar
	    );
	function
	    .get_basic_block(current_block)
	    .add_statement(std::move(operation));
    }
    else if (op_type == ExpressionBinary::COMPARE_GE) {
	returned_tmpvar = function.tmpvar_define(mir.get_types().get_type("bool"));
	auto operation = std::make_unique<OperationBinary>(
	    Operation::OP_COMPARE_GE,
	    expression.get_source_ref(),	    
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar
	    );
	function
	    .get_basic_block(current_block)
	    .add_statement(std::move(operation));
    }
    else if (op_type == ExpressionBinary::COMPARE_EQ) {
	returned_tmpvar = function.tmpvar_define(mir.get_types().get_type("bool"));
	auto operation = std::make_unique<OperationBinary>(
	    Operation::OP_COMPARE_EQ,
	    expression.get_source_ref(),	    
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar
	    );
	function
	    .get_basic_block(current_block)
	    .add_statement(std::move(operation));
    }
    else if (op_type == ExpressionBinary::COMPARE_NE) {
	returned_tmpvar = function.tmpvar_define(mir.get_types().get_type("bool"));
	auto operation = std::make_unique<OperationBinary>(
	    Operation::OP_COMPARE_NE,
	    expression.get_source_ref(),	    
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar
	    );
	function
	    .get_basic_block(current_block)
	    .add_statement(std::move(operation));
    }
    else if (op_type == ExpressionBinary::EQUALS) {
	returned_tmpvar = function.tmpvar_duplicate(a_tmpvar);
	auto operation = std::make_unique<OperationBinary>(
	    Operation::OP_ASSIGN,
	    expression.get_source_ref(),	    
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar
	    );
	function
	    .get_basic_block(current_block)
	    .add_statement(std::move(operation));
    }	
    else if (op_type == ExpressionBinary::MUL_ASSIGN) {
	returned_tmpvar = function.tmpvar_duplicate(a_tmpvar);
    }
    else if (op_type == ExpressionBinary::DIV_ASSIGN) {
	returned_tmpvar = function.tmpvar_duplicate(a_tmpvar);
    }
    else if (op_type == ExpressionBinary::MOD_ASSIGN) {
	returned_tmpvar = function.tmpvar_duplicate(a_tmpvar);
    }
    else if (op_type == ExpressionBinary::ADD_ASSIGN) {
	returned_tmpvar = function.tmpvar_duplicate(a_tmpvar);
    }
    else if (op_type == ExpressionBinary::SUB_ASSIGN) {
	returned_tmpvar = function.tmpvar_duplicate(a_tmpvar);
    }
    else if (op_type == ExpressionBinary::LEFT_ASSIGN) {
	returned_tmpvar = function.tmpvar_duplicate(a_tmpvar);
    }
    else if (op_type == ExpressionBinary::RIGHT_ASSIGN) {
	returned_tmpvar = function.tmpvar_duplicate(a_tmpvar);
    }
    else if (op_type == ExpressionBinary::AND_ASSIGN) {
	returned_tmpvar = function.tmpvar_duplicate(a_tmpvar);
    }
    else if (op_type == ExpressionBinary::OR_ASSIGN) {
	returned_tmpvar = function.tmpvar_duplicate(a_tmpvar);
    }
    else if (op_type == ExpressionBinary::XOR_ASSIGN) {
	returned_tmpvar = function.tmpvar_duplicate(a_tmpvar);
    }
    else {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_source_ref(),
		"Compiler bug! unknown binary operator",
		std::string("Invalid binary operator type ") + std::to_string(op_type)
		);
    }
}
void
FunctionDefinitionResolver::extract_from_expression_trinary(
    JLang::mir::Function & function,
    size_t & current_block,
    size_t & returned_tmpvar,
    const ExpressionTrinary & expression)
{
//    function
//	.get_basic_block(current_block)
//	.add_statement(std::string("trinary operator "));
}
void
FunctionDefinitionResolver::extract_from_expression_cast(
    JLang::mir::Function & function,
    size_t & current_block,
    size_t & returned_tmpvar,
    const ExpressionCast & expression)
{
//    function
//	.get_basic_block(current_block)
//	.add_statement(std::string("cast"));
}


void
FunctionDefinitionResolver::extract_from_expression(
    JLang::mir::Function & function,
    size_t & current_block,
    size_t & returned_tmpvar,
    const Expression & expression_container)
{
  const auto & expression_type = expression_container.get_expression();

  if (std::holds_alternative<JLang::owned<ExpressionPrimaryIdentifier>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPrimaryIdentifier>>(expression_type);
    extract_from_expression_primary_identifier(function, current_block, returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPrimaryNested>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPrimaryNested>>(expression_type);
    extract_from_expression_primary_nested(function, current_block, returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPrimaryLiteralChar>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPrimaryLiteralChar>>(expression_type);
    extract_from_expression_primary_literal_char(function, current_block, returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPrimaryLiteralString>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPrimaryLiteralString>>(expression_type);
    extract_from_expression_primary_literal_string(function, current_block, returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPrimaryLiteralInt>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPrimaryLiteralInt>>(expression_type);
    extract_from_expression_primary_literal_int(function, current_block, returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPrimaryLiteralFloat>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPrimaryLiteralFloat>>(expression_type);
    extract_from_expression_primary_literal_float(function, current_block, returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPostfixArrayIndex>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPostfixArrayIndex>>(expression_type);
    extract_from_expression_postfix_array_index(function, current_block, returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPostfixFunctionCall>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPostfixFunctionCall>>(expression_type);
    extract_from_expression_postfix_function_call(function, current_block, returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPostfixDot>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPostfixDot>>(expression_type);
    extract_from_expression_postfix_dot(function, current_block, returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPostfixArrow>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPostfixArrow>>(expression_type);
    extract_from_expression_postfix_arrow(function, current_block, returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPostfixIncDec>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPostfixIncDec>>(expression_type);
    extract_from_expression_postfix_incdec(function, current_block, returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionUnaryPrefix>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionUnaryPrefix>>(expression_type);
    extract_from_expression_unary_prefix(function, current_block, returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionUnarySizeofType>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionUnarySizeofType>>(expression_type);
    extract_from_expression_unary_sizeof_type(function, current_block, returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionBinary>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionBinary>>(expression_type);
    extract_from_expression_binary(function, current_block, returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionTrinary>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionTrinary>>(expression_type);
    extract_from_expression_trinary(function, current_block, returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionCast>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionCast>>(expression_type);
    extract_from_expression_cast(function, current_block, returned_tmpvar, *expression);
  }
  else {
    fprintf(stderr, "Compiler bug, invalid expression type\n");
    exit(1);
  }
}

void
FunctionDefinitionResolver::extract_from_statement_return(
    JLang::mir::Function & function,
    size_t & current_block,
    const StatementReturn & statement
    )
{
    size_t expression_tmpvar;
    extract_from_expression(function, current_block, expression_tmpvar, statement.get_expression());

    auto operation = std::make_unique<OperationReturn>(
	statement.get_source_ref(),
	expression_tmpvar
	);
    function.get_basic_block(current_block).add_statement(std::move(operation));
}


void
FunctionDefinitionResolver::extract_from_statement_ifelse(
    JLang::mir::Function & function,
    size_t & current_block,
    const StatementIfElse & statement
    )
{
    size_t condition_tmpvar;
    extract_from_expression(function, current_block, condition_tmpvar, statement.get_expression());
    if (!is_bool_type(function.tmpvar_get(condition_tmpvar)->get_type())) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		statement.get_expression().get_source_ref(),
		"Invalid condition in if statement.",
		std::string("Type of condition expression should be 'bool' and was ") + function.tmpvar_get(condition_tmpvar)->get_type()->get_name()
		);
    }
    
    size_t blockid_done = function.add_block();
    size_t blockid_else = -1;
    if (statement.has_else() || statement.has_else_if()) {
	blockid_else = function.add_block();
	auto operation = std::make_unique<OperationJumpIfEqual>(
	    statement.get_source_ref(),
	    condition_tmpvar,
	    std::to_string(blockid_else)
	    );
	function.get_basic_block(current_block).add_statement(std::move(operation));
    }
    else {
	auto operation = std::make_unique<OperationJumpIfEqual>(
	    statement.get_source_ref(),
	    condition_tmpvar,
	    std::to_string(blockid_done)
	    );
	function.get_basic_block(current_block).add_statement(std::move(operation));
    }
    function.push_block(current_block);
    current_block = function.add_block();
    
    extract_from_statement_list(
	function,
	current_block,
	statement.get_if_scope_body().get_statements()
	);
    
    if (statement.has_else()) {
	auto operation = std::make_unique<OperationJump>(
	    statement.get_source_ref(),
	    std::to_string(blockid_done)
	    );
	function.get_basic_block(current_block).add_statement(std::move(operation));
	function.push_block(current_block);
	
	extract_from_statement_list(
	    function,
	    blockid_else,
	    statement.get_else_scope_body().get_statements()
	    );
	function.push_block(blockid_else);
    }
    else if (statement.has_else_if()) {
	extract_from_statement_ifelse(
	    function,
	    current_block,
	    statement.get_else_if()
	    );
    }
    current_block = blockid_done;
}


void
FunctionDefinitionResolver::extract_from_statement_list(
                                              JLang::mir::Function & function,
                                              size_t & start_block,
                                              const StatementList & statement_list)
{
    std::vector<std::string> unwind;
    
    size_t current_block = start_block;
    
    for (const auto & statement_el : statement_list.get_statements()) {
	const auto & statement_type = statement_el->get_statement();
	if (std::holds_alternative<JLang::owned<StatementVariableDeclaration>>(statement_type)) {
	    const auto & statement = std::get<JLang::owned<StatementVariableDeclaration>>(statement_type);
	    
	    const JLang::mir::Type * mir_type = type_resolver.extract_from_type_specifier(statement->get_type_specifier());

	    LocalVariable local(statement->get_name(), mir_type, statement->get_type_specifier().get_source_ref());
	    
	    if (!function.add_local(local)) {
		compiler_context
		    .get_errors()
		    .add_simple_error(
			statement->get_name_source_ref(),
			"Duplicate Local Variable.",
			std::string("Variable with name ") + local.get_name() + std::string(" is already in scope and cannot be duplicated in this function.")
			);
	    }

	    auto operation = std::make_unique<OperationLocalDeclare>(
		statement->get_source_ref(),
		statement->get_name(),
		mir_type->get_name()
		);
	    function.get_basic_block(current_block).add_statement(std::move(operation));
	    unwind.push_back(statement->get_name());
	}
	else if (std::holds_alternative<JLang::owned<StatementBlock>>(statement_type)) {
	    const auto & statement = std::get<JLang::owned<StatementBlock>>(statement_type);
	    extract_from_statement_list(function, current_block, statement->get_scope_body().get_statements());
	}
	else if (std::holds_alternative<JLang::owned<StatementExpression>>(statement_type)) {
	    const auto & statement = std::get<JLang::owned<StatementExpression>>(statement_type);
	    size_t returned_tmpvar;
	    extract_from_expression(function, current_block, returned_tmpvar, statement->get_expression());
	}
	else if (std::holds_alternative<JLang::owned<StatementIfElse>>(statement_type)) {
	    const auto & statement = std::get<JLang::owned<StatementIfElse>>(statement_type);
	    extract_from_statement_ifelse(function, current_block, *statement);
	}
	else if (std::holds_alternative<JLang::owned<StatementWhile>>(statement_type)) {
	    const auto & statement = std::get<JLang::owned<StatementWhile>>(statement_type);
	    fprintf(stderr, "while\n");
	}
	else if (std::holds_alternative<JLang::owned<StatementFor>>(statement_type)) {
	    const auto & statement = std::get<JLang::owned<StatementFor>>(statement_type);
	    fprintf(stderr, "for\n");
	}
	else if (std::holds_alternative<JLang::owned<StatementLabel>>(statement_type)) {
	    const auto & statement = std::get<JLang::owned<StatementLabel>>(statement_type);
	    fprintf(stderr, "label\n");
	}
	else if (std::holds_alternative<JLang::owned<StatementGoto>>(statement_type)) {
	    const auto & statement = std::get<JLang::owned<StatementGoto>>(statement_type);
	    fprintf(stderr, "goto\n");
	}
	else if (std::holds_alternative<JLang::owned<StatementBreak>>(statement_type)) {
	    const auto & statement = std::get<JLang::owned<StatementBreak>>(statement_type);
	    fprintf(stderr, "break\n");
	}
	else if (std::holds_alternative<JLang::owned<StatementContinue>>(statement_type)) {
	    const auto & statement = std::get<JLang::owned<StatementContinue>>(statement_type);
	    fprintf(stderr, "continue\n");
	}
	else if (std::holds_alternative<JLang::owned<StatementReturn>>(statement_type)) {
	    const auto & statement = std::get<JLang::owned<StatementReturn>>(statement_type);
	    extract_from_statement_return(function, current_block, *statement);
	}
	else {
	    fprintf(stderr, "Compiler bug, invalid statement type\n");
	    exit(1);
	}
    }
    for (const auto & undecl : unwind) {
	function.remove_local(undecl);
	fprintf(stderr, "Undeclaring %s in block %ld\n", undecl.c_str(), current_block);
	    
	auto operation = std::make_unique<OperationLocalUndeclare>(
	    statement_list.get_source_ref(),
	    undecl);
	function.get_basic_block(current_block).add_statement(std::move(operation));
    }
    start_block = current_block;
}


void
FunctionDefinitionResolver::extract_from_function_definition(const FileStatementFunctionDefinition & function_definition)
{
    std::string fully_qualified_function_name = 
	function_definition.get_name().get_fully_qualified_name();
    
    fprintf(stderr, "Extracting function %s\n",
	    fully_qualified_function_name.c_str());
    
    const TypeSpecifier & type_specifier = function_definition.get_return_type();
    const Type *return_type = type_resolver.extract_from_type_specifier(type_specifier);
    
    if (return_type == nullptr) {
	fprintf(stderr, "Could not find return type\n");
	return;
	
    }
    std::vector<FunctionArgument> arguments;
    const auto & function_argument_list = function_definition.get_arguments();
    const auto & function_definition_args = function_argument_list.get_arguments();
    for (const auto & function_definition_arg : function_definition_args) {
       std::string name = function_definition_arg->get_name();
       const JLang::mir::Type * mir_type = type_resolver.extract_from_type_specifier(function_definition_arg->get_type_specifier());
       
       FunctionArgument arg(name, mir_type);
       arguments.push_back(arg);
    }
    
    JLang::owned<Function> fn = std::make_unique<Function>(
	fully_qualified_function_name,
	return_type,
	arguments,
	function_definition.get_source_ref());
    
    // Create a new basic block for the start
    
    fprintf(stderr, "START BBlocks\n");
    fprintf(stderr, "BB0:\n");
    size_t start_block = fn->add_block();
    
    extract_from_statement_list(*fn, start_block, function_definition.get_scope_body().get_statements());
    
    fn->push_block(start_block);
    
    // Debug dump basic blocks.
    fn->dump();
    
    mir.get_functions().add_function(std::move(fn));
    
}
