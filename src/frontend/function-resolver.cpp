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

bool FunctionResolver::resolve()
{
  // To resolve the functions, we need only iterate the
  // input parse tree and pull out any type declarations,
  // resolving them down to their primitive types.

  // TODO: Split the type extraction
  // out away from the function extraction(?).
    return extract_functions(parse_result.get_translation_unit().get_statements());
}
bool
FunctionResolver::extract_from_namespace(
    const FileStatementNamespace & namespace_declaration)
{
    const auto & statements = namespace_declaration.get_statement_list().get_statements();
    return extract_functions(statements);
}

bool
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
    return true;
}

bool
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
	    if (!function_def_resolver.resolve()) {
		return false;
	    }
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
	    if (!extract_from_class_definition(*std::get<JLang::owned<ClassDefinition>>(file_statement))) {
		return false;
	    }
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
	    if (!extract_from_namespace(*std::get<JLang::owned<FileStatementNamespace>>(file_statement))) {
		return false;
	    }
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
	    return false;
	}
    }
    return true;
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

bool
FunctionDefinitionResolver::resolve()
{
    return extract_from_function_definition(function_definition);
}

bool
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
	    function.get_basic_block(current_block).add_operation(std::move(operation));

	    return true;
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
	return false;
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
	    return false;
	}
	returned_tmpvar = function.tmpvar_define(symbol->get_type());

	auto operation = std::make_unique<OperationSymbol>(
	    expression.get_identifier().get_source_ref(),
	    returned_tmpvar,
	    expression.get_identifier().get_fully_qualified_name()
	    );
	function.get_basic_block(current_block).add_operation(std::move(operation));
	return true;
    }
    return false;
}

bool
FunctionDefinitionResolver::extract_from_expression_primary_nested(
    JLang::mir::Function & function,
    size_t & current_block,
    size_t & returned_tmpvar,
    const JLang::frontend::tree::ExpressionPrimaryNested & expression)
{
    // Nested expressions don't emit blocks on their own, just run whatever is nested.
    return extract_from_expression(
	function,
	current_block,
	returned_tmpvar,
	expression.get_expression()
	);
}

bool
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
	.add_operation(std::move(operation));
    return true;
}

bool
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
	.add_operation(std::move(operation));
    return true;
}

bool
FunctionDefinitionResolver::extract_from_expression_primary_literal_int(
    JLang::mir::Function & function,
    size_t & current_block,
    size_t & returned_tmpvar,
    const JLang::frontend::tree::ExpressionPrimaryLiteralInt & expression)
{
    returned_tmpvar = function.tmpvar_define(mir.get_types().get_type(expression.get_type()));
    const Type * literal_type = mir.get_types().get_type(expression.get_type());
    auto operation = std::make_unique<OperationLiteralInt>(
	expression.get_source_ref(),
	returned_tmpvar,
	expression.get_value(),
	literal_type
	);
    function
	.get_basic_block(current_block)
	.add_operation(std::move(operation));
    return true;
}

bool
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
	.add_operation(std::move(operation));
    return true;
}

bool
FunctionDefinitionResolver::extract_from_expression_postfix_array_index(
    JLang::mir::Function & function,
    size_t & current_block,
    size_t & returned_tmpvar,
    const ExpressionPostfixArrayIndex & expression)
{
    size_t array_tmpvar;
    size_t index_tmpvar;
    if (!extract_from_expression(function, current_block, array_tmpvar, expression.get_array())) {
	return false;
    }
    if (!extract_from_expression(function, current_block, index_tmpvar, expression.get_index())) {
	return false;
    }

    const Type *array_type = function.tmpvar_get(array_tmpvar);
    if (array_type->get_type() != Type::TYPE_POINTER) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_array().get_source_ref(),
		"Array type must be a pointer to another type",
		std::string("Type of array is not a pointer type.")
		);
	return false;
    }
    
    if (!function.tmpvar_get(index_tmpvar)->is_unsigned()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_index().get_source_ref(),
		"Array index must be an integer type",
		std::string("Type of index is not an index")
		);
	return false;
    }
    
    returned_tmpvar = function.tmpvar_define(array_type->get_pointer_target());
    auto operation = std::make_unique<OperationArrayIndex>(
	expression.get_source_ref(),
	returned_tmpvar,
	array_tmpvar,
	index_tmpvar
	);
    function
	.get_basic_block(current_block)
	.add_operation(std::move(operation));

    return true;
}

bool
FunctionDefinitionResolver::extract_from_expression_postfix_function_call(
    JLang::mir::Function & function,
    size_t & current_block,
    size_t & returned_tmpvar,
    const ExpressionPostfixFunctionCall & expression)
{
    // Extract the expression itself from the arguments.
    size_t function_type_tmpvar;
    if (!extract_from_expression(function, current_block, function_type_tmpvar, expression.get_function())) {
	fprintf(stderr, "Not extracting function early return\n");
	return false;
    }
    
    std::vector<size_t> arg_types;
    for (const auto & arg_expr : expression.get_arguments().get_arguments()) {
	size_t arg_returned_value;
	if (!extract_from_expression(function, current_block, arg_returned_value, *arg_expr)) {
	    fprintf(stderr, "Not extracting function arg expression\n");
	    return false;
	}
	arg_types.push_back(arg_returned_value);
    }

    const Type *function_pointer_type = function.tmpvar_get(function_type_tmpvar);
    if (function_pointer_type->get_type() != Type::TYPE_FUNCTION_POINTER) {
	fprintf(stderr, "Not extracting function invalid type\n");
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_function().get_source_ref(),
		"Called object is not a function.",
		std::string("Type of object being called is not a function, but is a ") + function_pointer_type->get_name() + std::string(" instead.")
		);
	return false;
    }
	
    // We declare that we return the vale that the function
    // will return.
    returned_tmpvar = function.tmpvar_define(function_pointer_type->get_return_type());
    
    fprintf(stderr, "Extracting function, adding to bb\n");
    auto operation = std::make_unique<OperationFunctionCall>(
	expression.get_source_ref(),
	returned_tmpvar,
	function_type_tmpvar,
	arg_types
	);
    
    function
	.get_basic_block(current_block)
	.add_operation(std::move(operation));

    return true;
}

bool
FunctionDefinitionResolver::extract_from_expression_postfix_dot(
    JLang::mir::Function & function,
    size_t & current_block,
    size_t & returned_tmpvar,
    const ExpressionPostfixDot & expression)
{
    size_t class_tmpvar;
    if (!extract_from_expression(function, current_block, class_tmpvar, expression.get_expression())) {
	return false;
    }

    const Type *class_type = function.tmpvar_get(class_tmpvar);
    if (class_type->get_type() != Type::TYPE_COMPOSITE) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_expression().get_source_ref(),
		"Member access must be applied to a class.",
		std::string("Type of object being accessed is not a class, but is a ") + class_type->get_name() + std::string(" instead.")
		);
	return false;
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
	class_tmpvar,
	member_name
	);
    function
	.get_basic_block(current_block)
	.add_operation(std::move(operation));
    return true;
}

bool
FunctionDefinitionResolver::extract_from_expression_postfix_arrow(
    JLang::mir::Function & function,
    size_t & current_block,
    size_t & returned_tmpvar,
    const ExpressionPostfixArrow & expression)
{
    size_t classptr_tmpvar;
    if (!extract_from_expression(function, current_block, classptr_tmpvar, expression.get_expression())) {
	return false;
    }

    const Type *classptr_type = function.tmpvar_get(classptr_tmpvar);
    if (classptr_type->get_type() != Type::TYPE_POINTER) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_expression().get_source_ref(),
		"Arrow (->) operator must be used on a pointer to a class.",
		std::string("Type of object being accessed is not a pointer to a class, but is a ") + classptr_type->get_name() + std::string(" instead.")
		);
	return false;
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
	return false;
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
	return false;
    }

    returned_tmpvar = function.tmpvar_define(member->get_type());
    auto operation = std::make_unique<OperationArrow>(
	expression.get_source_ref(),
	returned_tmpvar,
	classptr_tmpvar,
	member_name
	);
    function
	.get_basic_block(current_block)
	.add_operation(std::move(operation));
    return true;

}

bool
FunctionDefinitionResolver::extract_from_expression_postfix_incdec(
    JLang::mir::Function & function,
    size_t & current_block,
    size_t & returned_tmpvar,
    const ExpressionPostfixIncDec & expression)
{
    size_t operand_tmpvar;
    if (!extract_from_expression(function, current_block, operand_tmpvar, expression.get_expression())) {
	return false;
    }

    returned_tmpvar = function.tmpvar_duplicate(operand_tmpvar);
    
    if (expression.get_type() == ExpressionPostfixIncDec::INCREMENT) {
	auto operation = std::make_unique<OperationUnary>(
	    Operation::OP_POST_INCREMENT,
	    expression.get_source_ref(),
	    returned_tmpvar,
	    operand_tmpvar
	    );
	function
	    .get_basic_block(current_block)
	    .add_operation(std::move(operation));
    }
    else if (expression.get_type() == ExpressionPostfixIncDec::DECREMENT) {
	auto operation = std::make_unique<OperationUnary>(
	    Operation::OP_POST_DECREMENT,
	    expression.get_source_ref(),
	    returned_tmpvar,
	    operand_tmpvar
	    );
	function
	    .get_basic_block(current_block)
	    .add_operation(std::move(operation));
    }
    else {
	compiler_context
	    .get_errors()
	    .add_simple_error(expression.get_source_ref(),
			      "Compiler bug!  Please report this message",
			      "Unknown postfix operator encountered"
		);
	return false;
    }
    return true;
}



bool
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
    if (!extract_from_expression(
	    function,
	    current_block,
	    operand_tmpvar,
	    expression.get_expression()
	    )) {
	return false;
    }
    
    const Type *operand_type = function.tmpvar_get(operand_tmpvar);
    if (operand_type == nullptr) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_source_ref(),
		"Compiler bug!  Please report this message",
		"Operand must be a valid type."
		);
    }

    ExpressionUnaryPrefix::OperationType op_type = expression.get_type();
    switch (op_type) {    
    case ExpressionUnaryPrefix::INCREMENT:
        {
	returned_tmpvar = function.tmpvar_duplicate(operand_tmpvar);
	auto operation = std::make_unique<OperationUnary>(
	    Operation::OP_PRE_INCREMENT,
	    expression.get_source_ref(),
	    returned_tmpvar,
	    operand_tmpvar
	    );
	function
	    .get_basic_block(current_block)
	    .add_operation(std::move(operation));
        }
        break;
    case ExpressionUnaryPrefix::DECREMENT:
        {
	returned_tmpvar = function.tmpvar_duplicate(operand_tmpvar);
	auto operation = std::make_unique<OperationUnary>(
	    Operation::OP_PRE_DECREMENT,
	    expression.get_source_ref(),
	    returned_tmpvar,
	    operand_tmpvar
	    );
	function
	    .get_basic_block(current_block)
	    .add_operation(std::move(operation));
        }
        break;
    case ExpressionUnaryPrefix::ADDRESSOF:
        {
	const Type * pointer_to_operand_type = mir.get_types().get_pointer_to(operand_type, expression.get_source_ref());
	returned_tmpvar = function.tmpvar_define(pointer_to_operand_type);
	auto operation = std::make_unique<OperationUnary>(
	    Operation::OP_ADDRESSOF,
	    expression.get_source_ref(),
	    returned_tmpvar,
	    operand_tmpvar
	    );
	function
	    .get_basic_block(current_block)
	    .add_operation(std::move(operation));
	}
        break;
    case ExpressionUnaryPrefix::DEREFERENCE:
        {
	if (operand_type->get_type() != Type::TYPE_POINTER) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    expression.get_expression().get_source_ref(),
		    "Cannot dereference non-pointer",
		    std::string("Attempting to de-reference non-pointer type ") + operand_type->get_name()
		    );
	    return false;
	}
	returned_tmpvar = function.tmpvar_define(operand_type);
	auto operation = std::make_unique<OperationUnary>(
	    Operation::OP_DEREFERENCE,
	    expression.get_source_ref(),
	    returned_tmpvar,
	    operand_tmpvar
	    );
	function
	    .get_basic_block(current_block)
	    .add_operation(std::move(operation));
        }
        break;
    case ExpressionUnaryPrefix::PLUS:
        {
	// Unary plus does nothing, really, so why bother?  We just don't
	// bother to do anything and just wire the return into the operand
	// directly instead of creating a new tmpvar and assigning it.
	returned_tmpvar = operand_tmpvar;
        }
    break;
    case ExpressionUnaryPrefix::MINUS:
        {
	returned_tmpvar = function.tmpvar_duplicate(operand_tmpvar);
	auto operation = std::make_unique<OperationUnary>(
	    Operation::OP_NEGATE,
	    expression.get_source_ref(),	    
	    returned_tmpvar,
	    operand_tmpvar
	    );
	function
	    .get_basic_block(current_block)
	    .add_operation(std::move(operation));
        }
        break;
    case ExpressionUnaryPrefix::BITWISE_NOT:
        {
	returned_tmpvar = function.tmpvar_duplicate(operand_tmpvar);
	auto operation = std::make_unique<OperationUnary>(
	    Operation::OP_BITWISE_NOT,
	    expression.get_source_ref(),	    
	    returned_tmpvar,
	    operand_tmpvar
	    );
	function
	    .get_basic_block(current_block)
	    .add_operation(std::move(operation));
        }
        break;
    case ExpressionUnaryPrefix::LOGICAL_NOT:
        {
	if (!function.tmpvar_get(operand_tmpvar)->is_bool()) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    expression.get_expression().get_source_ref(),
		    "Logical not (!) must operate on 'bool' expressions.",
		    std::string("Type of condition expression should be 'bool' and was ") + function.tmpvar_get(operand_tmpvar)->get_name()
		    );
	}
	returned_tmpvar = function.tmpvar_duplicate(operand_tmpvar);
	auto operation = std::make_unique<OperationUnary>(
	    Operation::OP_LOGICAL_NOT,
	    expression.get_source_ref(),	    
	    returned_tmpvar,
	    operand_tmpvar
	    );
	function
	    .get_basic_block(current_block)
	    .add_operation(std::move(operation));
	}
        break;
    default:
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_expression().get_source_ref(),
		"Compiler Bug!",
		"Encountered unknown unary prefix expression"
		);
	return false;
    }
    return true;
}
bool
FunctionDefinitionResolver::extract_from_expression_unary_sizeof_type(
    JLang::mir::Function & function,
    size_t & current_block,
    size_t & returned_tmpvar,
    const ExpressionUnarySizeofType & expression)
{
    const Type * operand_type = type_resolver.extract_from_type_specifier(expression.get_type_specifier());
    const Type * u64_type = mir.get_types().get_type("u64");
    returned_tmpvar = function.tmpvar_define(u64_type);
    auto operation = std::make_unique<OperationSizeofType>(
	expression.get_source_ref(),	    
	returned_tmpvar,
	operand_type
	);
    function
	.get_basic_block(current_block)
	.add_operation(std::move(operation));
    return true;
}

bool
FunctionDefinitionResolver::numeric_widen(
    JLang::mir::Function & function,
    size_t & current_block,
    const JLang::context::SourceReference & _src_ref,
    size_t & _widen_var,
    const Type *widen_to
    )
{
    // The new b is the result of widening b
    // to the type of a.
    Operation::OperationType widen_type;
    if (widen_to->is_integer() && widen_to->is_signed()) {
	widen_type = Operation::OP_WIDEN_SIGNED;
    }
    else if (widen_to->is_integer() && widen_to->is_unsigned()) {
	widen_type = Operation::OP_WIDEN_UNSIGNED;
    }
    else if (widen_to->is_float()) {
	widen_type = Operation::OP_WIDEN_FLOAT;
    }
    else {
	fprintf(stderr, "Compiler Bug! Widening number of unknown type\n");
	return false;
    }
    
    size_t widened_var = function.tmpvar_define(widen_to);
    auto operation = std::make_unique<OperationCast>(
	widen_type,
	_src_ref,
	widened_var,
	_widen_var,
	widen_to
	);
    function
	.get_basic_block(current_block)
	.add_operation(std::move(operation));
    _widen_var = widened_var;
    return true;
}
bool
FunctionDefinitionResolver::numeric_widen_binary_operation(
    JLang::mir::Function &function,
    size_t current_block,
    const JLang::context::SourceReference & _src_ref,
    size_t & a_tmpvar,
    size_t & b_tmpvar,
    const JLang::mir::Type *atype,
    const JLang::mir::Type *btype,
    const JLang::mir::Type **widened
    )
{
    if (atype->is_integer()) {
	// Deal with signed-ness.  We can't mix signed and unsigned.
	if (atype->is_signed() && btype->is_signed()) {
	    if (atype->get_primitive_size() > btype->get_primitive_size()) {
		if (!numeric_widen(function, current_block, _src_ref, b_tmpvar, atype)) {
		    return false;
		}
		*widened = atype;
	    }
	    else if (atype->get_primitive_size() < btype->get_primitive_size()) {
		if (!numeric_widen(function, current_block, _src_ref, a_tmpvar, btype)) {
		    return false;
		}
		*widened = btype;
	    }
	    else {
		// Both types are the same size already, so we do nothing
		// and it doesn't matter whether we picked 'a' or 'b'.
		*widened = atype;
	    }
	}
	else if (atype->is_unsigned() && btype->is_unsigned()) {
	    if (atype->get_primitive_size() > btype->get_primitive_size()) {
		if (!numeric_widen(function, current_block, _src_ref, b_tmpvar, atype)) {
		    return false;
		}
		*widened = atype;
	    }
	    else if (atype->get_primitive_size() < btype->get_primitive_size()) {
		if (!numeric_widen(function, current_block, _src_ref, a_tmpvar, btype)) {
		    return false;
		}
		fprintf(stderr, "Widened UA %ld %ld\n", a_tmpvar, b_tmpvar);
		*widened = btype;
	    }
	    else {
		// Both types are the same size already, so we do nothing
		// and it doesn't matter whether we picked 'a' or 'b'.
		*widened = atype;
	    }
	}
	else {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    _src_ref,
		    "Type mismatch in binary operation",
		    std::string("The type of operands both signed or unsigned.  Automatic cast from signed to unsigned is not supported. a= ") +
		    atype->get_name() + std::string(" b=") + btype->get_name()
		    );
	    return false;
	}
    }
    // We don't have to check because we already
    // know that we have a float in the else.
    else {
	if (atype->get_primitive_size() > btype->get_primitive_size()) {
	    if (!numeric_widen(function, current_block, _src_ref, b_tmpvar, atype)) {
		return false;
	    }
	    *widened = atype;
	}
	else if (atype->get_primitive_size() < btype->get_primitive_size()) {
	    if (!numeric_widen(function, current_block, _src_ref, a_tmpvar, btype)) {
		return false;
	    }
	    *widened = btype;
	}
	else {
	    *widened = atype;
	}
    }
    return true;
}

bool
FunctionDefinitionResolver::handle_binary_operation_arithmetic(
    JLang::mir::Function & function,
    const JLang::context::SourceReference & _src_ref,
    Operation::OperationType type,
    size_t & current_block,
    size_t & returned_tmpvar,
    size_t a_tmpvar,
    size_t b_tmpvar
    )
{
    const Type *atype = function.tmpvar_get(a_tmpvar);
    const Type *btype = function.tmpvar_get(b_tmpvar);
    // Check that both operands are numeric.
    if (!atype->is_numeric() ||	!btype->is_numeric()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		_src_ref,
		"Type mismatch in binary operation",
		std::string("The type of operands should be numeric, but were: a= ") + atype->get_name() + std::string(" b=") + btype->get_name()
		);
	return false;
    }
    // Special-case for modulo because it doesn't support floating-point types.
    if (type == Operation::OP_MODULO) {
	if (atype->is_float() || btype->is_float()) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    _src_ref,
		    "Type mismatch in binary operation",
		    std::string("The type of operands should be integer, but were a= ") + atype->get_name() + std::string(" b=") + btype->get_name()
		    );
	    return false;
	}
    }
    
    // The arguments must be either both integer
    // or both float.
    if 	(!(
	     (atype->is_integer() && btype->is_integer()) ||
	     (atype->is_float() && btype->is_float())
        )) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		_src_ref,
		"Type mismatch in binary operation",
		std::string("The type of operands both integer or floating-point types.  Automatic cast from int to float is not supported. a= ") +
		atype->get_name() + std::string(" b=") + btype->get_name()
		);
	return false;
    }

    // Now, both of them are either int or float, so we need to handle each separately
    // so that we can choose the appropriate cast type.
    const Type *widened = nullptr;
    if (!numeric_widen_binary_operation(function, current_block, _src_ref, a_tmpvar, b_tmpvar, atype, btype, &widened)) {
	return false;
    }

    // The return type is whatever
    // we widened the add to be.
    returned_tmpvar = function.tmpvar_define(widened);
    
    // We emit the appropriate operation as either an integer or
    // floating-point add depending on which one it was.
    // The back-end may assume that the operand types are both
    // integer or floating point of the same type and
    // the return type will also be of the same type.
    auto operation = std::make_unique<OperationBinary>(
	type,
	_src_ref,
	returned_tmpvar,
	a_tmpvar,
	b_tmpvar
	);
    function
	.get_basic_block(current_block)
	.add_operation(std::move(operation));
    
    return true;
}

bool
FunctionDefinitionResolver::handle_binary_operation_logical(
    JLang::mir::Function & function,
    const JLang::context::SourceReference & _src_ref,
    Operation::OperationType type,
    size_t & current_block,
    size_t & returned_tmpvar,
    size_t a_tmpvar,
    size_t b_tmpvar
    )
{
    const Type *atype = function.tmpvar_get(a_tmpvar);
    const Type *btype = function.tmpvar_get(b_tmpvar);
    // Check that both operands are numeric.
    if (!atype->is_bool() || !btype->is_bool()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		_src_ref,
		"Type mismatch in logical operation",
		std::string("The type of operands should be bool , but were: a= ") + atype->get_name() + std::string(" b=") + btype->get_name()
		);
	return false;
    }
    auto operation = std::make_unique<OperationBinary>(
	type,
	_src_ref,
	returned_tmpvar,
	a_tmpvar,
	b_tmpvar
	);
    function
	.get_basic_block(current_block)
	.add_operation(std::move(operation));
    return true;
}

bool
FunctionDefinitionResolver::handle_binary_operation_bitwise(
    JLang::mir::Function & function,
    const JLang::context::SourceReference & _src_ref,
    Operation::OperationType type,
    size_t & current_block,
    size_t & returned_tmpvar,
    size_t a_tmpvar,
    size_t b_tmpvar
    )
{
    const Type *atype = function.tmpvar_get(a_tmpvar);
    const Type *btype = function.tmpvar_get(b_tmpvar);
    // Check that both operands are numeric.
    if (!atype->is_unsigned() || !btype->is_unsigned()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		_src_ref,
		"Type mismatch in binary operation",
		std::string("The type of operands should be unsigned integers, but were: a= ") + atype->get_name() + std::string(" b=") + btype->get_name()
		);
	return false;
    }
    // Now widen them to the appropriate sizes.
    const Type *widened = nullptr;
    if (!numeric_widen_binary_operation(function, current_block, _src_ref, a_tmpvar, b_tmpvar, atype, btype, &widened)) {
	return false;
    }

    // The return type is whatever
    // we widened the add to be.
    returned_tmpvar = function.tmpvar_define(widened);
    
    // We emit the appropriate operation as either an integer or
    // floating-point add depending on which one it was.
    // The back-end may assume that the operand types are both
    // integer or floating point of the same type and
    // the return type will also be of the same type.
    auto operation = std::make_unique<OperationBinary>(
	type,
	_src_ref,
	returned_tmpvar,
	a_tmpvar,
	b_tmpvar
	);
    function
	.get_basic_block(current_block)
	.add_operation(std::move(operation));
    
    return true;
}


bool
FunctionDefinitionResolver::handle_binary_operation_shift(
    JLang::mir::Function & function,
    const JLang::context::SourceReference & _src_ref,
    Operation::OperationType type,
    size_t & current_block,
    size_t & returned_tmpvar,
    size_t a_tmpvar,
    size_t b_tmpvar
    )
{
    const Type *atype = function.tmpvar_get(a_tmpvar);
    const Type *btype = function.tmpvar_get(b_tmpvar);
    // Check that both operands are numeric.
    if (!atype->is_unsigned() || !btype->is_unsigned()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		_src_ref,
		"Type mismatch in binary operation",
		std::string("The type of operands should be unsigned integers, but were: a= ") + atype->get_name() + std::string(" b=") + btype->get_name()
		);
	return false;
    }

    // Notably, we never widen a shift operation.
    // Instead, we operate on whatever it is because
    // it will end up being masked down to an 8-bit
    // value to avoid overflowing the LHS when the
    // shift happens if it's any bigger.

    // We always return the same size that the LHS is.
    returned_tmpvar = function.tmpvar_define(atype);
    
    // We emit the appropriate operation as either an integer or
    // floating-point add depending on which one it was.
    // The back-end may assume that the operand types are both
    // integer or floating point of the same type and
    // the return type will also be of the same type.
    auto operation = std::make_unique<OperationBinary>(
	type,
	_src_ref,
	returned_tmpvar,
	a_tmpvar,
	b_tmpvar
	);
    function
	.get_basic_block(current_block)
	.add_operation(std::move(operation));
    
    return true;
}

bool
FunctionDefinitionResolver::handle_binary_operation_compare(
    JLang::mir::Function & function,
    const JLang::context::SourceReference & _src_ref,
    Operation::OperationType type,
    size_t & current_block,
    size_t & returned_tmpvar,
    size_t a_tmpvar,
    size_t b_tmpvar
    )
{
    const Type *atype = function.tmpvar_get(a_tmpvar);
    const Type *btype = function.tmpvar_get(b_tmpvar);
    // Check that both operands are the same type.
    if (atype->get_name() != btype->get_name()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		_src_ref,
		"Type mismatch in compare operation",
		std::string("The operands of a comparision should be the same type, but were: a= ") + atype->get_name() + std::string(" b=") + btype->get_name()
		);
	return false;
    }
    if (atype->is_void()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		_src_ref,
		"Type mismatch in compare operation",
		std::string("The operands of a comparison must not be void, but were: a= ") + atype->get_name() + std::string(" b=") + btype->get_name()
		);
	return false;
    }
    if (atype->is_composite()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		_src_ref,
		"Type mismatch in compare operation",
		std::string("The operands of a comparison must not be composite structures or classes, but were: a= ") + atype->get_name() + std::string(" b=") + btype->get_name()
		);
	return false;
    }
    if (
	(atype->is_pointer() || atype->is_reference())
	&&
	!(type == Operation::OP_COMPARE_EQ ||
	  type == Operation::OP_COMPARE_NE)
	) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		_src_ref,
		"Type mismatch in compare operation",
		std::string("The operands of a comparison of pointers and references may not be used except for equality comparisions, but were: a= ") + atype->get_name() + std::string(" b=") + btype->get_name()
		);
	return false;	
    }
    

    // Notably, we never widen a shift operation.
    // Instead, we operate on whatever it is because
    // it will end up being masked down to an 8-bit
    // value to avoid overflowing the LHS when the
    // shift happens if it's any bigger.

    // We always return the same size that the LHS is.
    returned_tmpvar = function.tmpvar_define(mir.get_types().get_type("bool"));
    
    // We emit the appropriate operation as either an integer or
    // floating-point add depending on which one it was.
    // The back-end may assume that the operand types are both
    // integer or floating point of the same type and
    // the return type will also be of the same type.
    auto operation = std::make_unique<OperationBinary>(
	type,
	_src_ref,
	returned_tmpvar,
	a_tmpvar,
	b_tmpvar
	);
    function
	.get_basic_block(current_block)
	.add_operation(std::move(operation));
    
    return true;
}

bool
FunctionDefinitionResolver::handle_binary_operation_assignment(
    JLang::mir::Function & function,
    const JLang::context::SourceReference & _src_ref,
    Operation::OperationType type,
    size_t & current_block,
    size_t & returned_tmpvar,
    size_t a_tmpvar,
    size_t b_tmpvar
    )
{
    const Type *atype = function.tmpvar_get(a_tmpvar);
    const Type *btype = function.tmpvar_get(b_tmpvar);
    // Check that both operands are the same type.
    if (atype->get_name() != btype->get_name()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		_src_ref,
		"Type mismatch in compare operation",
		std::string("The operands of an assignment should be the same type, but were: a= ") + atype->get_name() + std::string(" b=") + btype->get_name()
		);
	return false;
    }
    if (atype->is_void()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		_src_ref,
		"Type mismatch in assignment operation",
		std::string("The operands of an assignment must not be void, but were: a= ") + atype->get_name() + std::string(" b=") + btype->get_name()
		);
	return false;
    }
    if (atype->is_composite()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		_src_ref,
		"Type mismatch in assignment operation",
		std::string("The operands of an assignment must not be composite structures or classes, but were: a= ") + atype->get_name() + std::string(" b=") + btype->get_name()
		);
	return false;
    }

    // Notably, we never widen a shift operation.
    // Instead, we operate on whatever it is because
    // it will end up being masked down to an 8-bit
    // value to avoid overflowing the LHS when the
    // shift happens if it's any bigger.

    // We always return the same size that the LHS is.
    returned_tmpvar = function.tmpvar_define(atype);
    
    // We emit the appropriate operation as either an integer or
    // floating-point add depending on which one it was.
    // The back-end may assume that the operand types are both
    // integer or floating point of the same type and
    // the return type will also be of the same type.
    auto operation = std::make_unique<OperationBinary>(
	type,
	_src_ref,
	returned_tmpvar,
	a_tmpvar,
	b_tmpvar
	);
    function
	.get_basic_block(current_block)
	.add_operation(std::move(operation));
    
    return true;
}

bool
FunctionDefinitionResolver::extract_from_expression_binary(
    JLang::mir::Function & function,
    size_t & current_block,
    size_t & returned_tmpvar,
    const ExpressionBinary & expression)
{
    size_t a_tmpvar;
    size_t b_tmpvar;
    
    if (!extract_from_expression(function, current_block, a_tmpvar, expression.get_a())) {
	return false;
    }
    if (!extract_from_expression(function, current_block, b_tmpvar, expression.get_b())) {
	return false;
    }

    ExpressionBinary::OperationType op_type;
    op_type = expression.get_operator();

    if (op_type == ExpressionBinary::ADD) {
	if (!handle_binary_operation_arithmetic(
	    function,
	    expression.get_source_ref(),
	    Operation::OP_ADD,
	    current_block,
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar)) {
	    return false;
	}
    }	
    else if (op_type == ExpressionBinary::SUBTRACT) {
	if (!handle_binary_operation_arithmetic(
	    function,
	    expression.get_source_ref(),
	    Operation::OP_SUBTRACT,
	    current_block,
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::MULTIPLY) {
	if (!handle_binary_operation_arithmetic(
	    function,
	    expression.get_source_ref(),
	    Operation::OP_MULTIPLY,
	    current_block,
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::DIVIDE) {
	if (!handle_binary_operation_arithmetic(
	    function,
	    expression.get_source_ref(),
	    Operation::OP_DIVIDE,
	    current_block,
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::MODULO) {
	if (!handle_binary_operation_arithmetic(
	    function,
	    expression.get_source_ref(),
	    Operation::OP_MODULO,
	    current_block,
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::LOGICAL_AND) {
	if (!handle_binary_operation_logical(
	    function,
	    expression.get_source_ref(),
	    Operation::OP_LOGICAL_AND,
	    current_block,
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::LOGICAL_OR) {
	if (!handle_binary_operation_logical(
	    function,
	    expression.get_source_ref(),
	    Operation::OP_LOGICAL_OR,
	    current_block,
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::BITWISE_AND) {
	if (!handle_binary_operation_bitwise(
	    function,
	    expression.get_source_ref(),
	    Operation::OP_BITWISE_AND,
	    current_block,
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::BITWISE_OR) {
	if (!handle_binary_operation_bitwise(
	    function,
	    expression.get_source_ref(),
	    Operation::OP_BITWISE_OR,
	    current_block,
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::BITWISE_XOR) {
	if (!handle_binary_operation_bitwise(
	    function,
	    expression.get_source_ref(),
	    Operation::OP_BITWISE_XOR,
	    current_block,
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::SHIFT_LEFT) {
	if (!handle_binary_operation_shift(
	    function,
	    expression.get_source_ref(),
	    Operation::OP_SHIFT_LEFT,
	    current_block,
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::SHIFT_RIGHT) {
	if (!handle_binary_operation_shift(
	    function,
	    expression.get_source_ref(),
	    Operation::OP_SHIFT_RIGHT,
	    current_block,
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::COMPARE_LT) {
	if (!handle_binary_operation_compare(
		function,
		expression.get_source_ref(),
		Operation::OP_COMPARE_LT,
		current_block,
		returned_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::COMPARE_GT) {
	if (!handle_binary_operation_compare(
		function,
		expression.get_source_ref(),
		Operation::OP_COMPARE_GT,
		current_block,
		returned_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::COMPARE_LE) {
	if (!handle_binary_operation_compare(
		function,
		expression.get_source_ref(),
		Operation::OP_COMPARE_LE,
		current_block,
		returned_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::COMPARE_GE) {
	if (!handle_binary_operation_compare(
		function,
		expression.get_source_ref(),
		Operation::OP_COMPARE_GE,
		current_block,
		returned_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::COMPARE_EQ) {
	if (!handle_binary_operation_compare(
		function,
		expression.get_source_ref(),
		Operation::OP_COMPARE_EQ,
		current_block,
		returned_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::COMPARE_NE) {
	if (!handle_binary_operation_compare(
		function,
		expression.get_source_ref(),
		Operation::OP_COMPARE_NE,
		current_block,
		returned_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::EQUALS) {
	if (!handle_binary_operation_assignment(
		function,
		expression.get_source_ref(),
		Operation::OP_ASSIGN,
		current_block,
		returned_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
    }	
    else if (op_type == ExpressionBinary::MUL_ASSIGN) {
	// This is just syntax sugar for * followed by =
	size_t arithmetic_tmpvar;
	if (!handle_binary_operation_arithmetic(
		function,
		expression.get_source_ref(),
		Operation::OP_MULTIPLY,
		current_block,
		arithmetic_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
	if (!handle_binary_operation_assignment(
		function,
		expression.get_source_ref(),
		Operation::OP_ASSIGN,
		current_block,
		returned_tmpvar,
		a_tmpvar,
		arithmetic_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::DIV_ASSIGN) {
	// This is just syntax sugar for / followed by =
	size_t arithmetic_tmpvar;
	if (!handle_binary_operation_arithmetic(
		function,
		expression.get_source_ref(),
		Operation::OP_DIVIDE,
		current_block,
		arithmetic_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
	if (!handle_binary_operation_assignment(
		function,
		expression.get_source_ref(),
		Operation::OP_ASSIGN,
		current_block,
		returned_tmpvar,
		a_tmpvar,
		arithmetic_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::MOD_ASSIGN) {
	// This is just syntax sugar for % followed by =
	size_t arithmetic_tmpvar;
	if (!handle_binary_operation_arithmetic(
		function,
		expression.get_source_ref(),
		Operation::OP_MODULO,
		current_block,
		arithmetic_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
	if (!handle_binary_operation_assignment(
		function,
		expression.get_source_ref(),
		Operation::OP_ASSIGN,
		current_block,
		returned_tmpvar,
		a_tmpvar,
		arithmetic_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::ADD_ASSIGN) {
	// This is just syntax sugar for + followed by =
	size_t arithmetic_tmpvar;
	if (!handle_binary_operation_arithmetic(
		function,
		expression.get_source_ref(),
		Operation::OP_ADD,
		current_block,
		arithmetic_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
	if (!handle_binary_operation_assignment(
		function,
		expression.get_source_ref(),
		Operation::OP_ASSIGN,
		current_block,
		returned_tmpvar,
		a_tmpvar,
		arithmetic_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::SUB_ASSIGN) {
	// This is just syntax sugar for - followed by =
	size_t arithmetic_tmpvar;
	if (!handle_binary_operation_arithmetic(
		function,
		expression.get_source_ref(),
		Operation::OP_SUBTRACT,
		current_block,
		arithmetic_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
	if (!handle_binary_operation_assignment(
		function,
		expression.get_source_ref(),
		Operation::OP_ASSIGN,
		current_block,
		returned_tmpvar,
		a_tmpvar,
		arithmetic_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::LEFT_ASSIGN) {
	// This is just syntax sugar for << followed by =
	size_t arithmetic_tmpvar;
	if (!handle_binary_operation_shift(
		function,
		expression.get_source_ref(),
		Operation::OP_SHIFT_LEFT,
		current_block,
		arithmetic_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
	if (!handle_binary_operation_assignment(
		function,
		expression.get_source_ref(),
		Operation::OP_ASSIGN,
		current_block,
		returned_tmpvar,
		a_tmpvar,
		arithmetic_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::RIGHT_ASSIGN) {
	// This is just syntax sugar for >> followed by =
	size_t arithmetic_tmpvar;
	if (!handle_binary_operation_shift(
		function,
		expression.get_source_ref(),
		Operation::OP_SHIFT_RIGHT,
		current_block,
		arithmetic_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
	if (!handle_binary_operation_assignment(
		function,
		expression.get_source_ref(),
		Operation::OP_ASSIGN,
		current_block,
		returned_tmpvar,
		a_tmpvar,
		arithmetic_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::AND_ASSIGN) {
	// This is just syntax sugar for & followed by =
	size_t arithmetic_tmpvar;
	if (!handle_binary_operation_bitwise(
		function,
		expression.get_source_ref(),
		Operation::OP_BITWISE_AND,
		current_block,
		arithmetic_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
	if (!handle_binary_operation_assignment(
		function,
		expression.get_source_ref(),
		Operation::OP_ASSIGN,
		current_block,
		returned_tmpvar,
		a_tmpvar,
		arithmetic_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::OR_ASSIGN) {
	// This is just syntax sugar for | followed by =
	size_t arithmetic_tmpvar;
	if (!handle_binary_operation_bitwise(
		function,
		expression.get_source_ref(),
		Operation::OP_BITWISE_OR,
		current_block,
		arithmetic_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
	if (!handle_binary_operation_assignment(
		function,
		expression.get_source_ref(),
		Operation::OP_ASSIGN,
		current_block,
		returned_tmpvar,
		a_tmpvar,
		arithmetic_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::XOR_ASSIGN) {
	// This is just syntax sugar for ^ followed by =
	size_t arithmetic_tmpvar;
	if (!handle_binary_operation_bitwise(
		function,
		expression.get_source_ref(),
		Operation::OP_BITWISE_XOR,
		current_block,
		arithmetic_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
	if (!handle_binary_operation_assignment(
		function,
		expression.get_source_ref(),
		Operation::OP_ASSIGN,
		current_block,
		returned_tmpvar,
		a_tmpvar,
		arithmetic_tmpvar
		)) {
	    return false;
	}
    }
    else {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_source_ref(),
		"Compiler bug! unknown binary operator",
		std::string("Invalid binary operator type ") + std::to_string(op_type)
		);
	return false;
    }
    return true;
}
bool
FunctionDefinitionResolver::extract_from_expression_trinary(
    JLang::mir::Function & function,
    size_t & current_block,
    size_t & returned_tmpvar,
    const ExpressionTrinary & expression)
{
//    function
//	.get_basic_block(current_block)
//	.add_operation(std::string("trinary operator "));
    return false;
}
bool
FunctionDefinitionResolver::extract_from_expression_cast(
    JLang::mir::Function & function,
    size_t & current_block,
    size_t & returned_tmpvar,
    const ExpressionCast & expression)
{
//    function
//	.get_basic_block(current_block)
//	.add_operation(std::string("cast"));
    return false;
}


bool
FunctionDefinitionResolver::extract_from_expression(
    JLang::mir::Function & function,
    size_t & current_block,
    size_t & returned_tmpvar,
    const Expression & expression_container)
{
  const auto & expression_type = expression_container.get_expression();

  if (std::holds_alternative<JLang::owned<ExpressionPrimaryIdentifier>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPrimaryIdentifier>>(expression_type);
    return extract_from_expression_primary_identifier(function, current_block, returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPrimaryNested>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPrimaryNested>>(expression_type);
    return extract_from_expression_primary_nested(function, current_block, returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPrimaryLiteralChar>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPrimaryLiteralChar>>(expression_type);
    return extract_from_expression_primary_literal_char(function, current_block, returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPrimaryLiteralString>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPrimaryLiteralString>>(expression_type);
    return extract_from_expression_primary_literal_string(function, current_block, returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPrimaryLiteralInt>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPrimaryLiteralInt>>(expression_type);
    return extract_from_expression_primary_literal_int(function, current_block, returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPrimaryLiteralFloat>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPrimaryLiteralFloat>>(expression_type);
    return extract_from_expression_primary_literal_float(function, current_block, returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPostfixArrayIndex>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPostfixArrayIndex>>(expression_type);
    return extract_from_expression_postfix_array_index(function, current_block, returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPostfixFunctionCall>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPostfixFunctionCall>>(expression_type);
    return extract_from_expression_postfix_function_call(function, current_block, returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPostfixDot>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPostfixDot>>(expression_type);
    return extract_from_expression_postfix_dot(function, current_block, returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPostfixArrow>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPostfixArrow>>(expression_type);
    return extract_from_expression_postfix_arrow(function, current_block, returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPostfixIncDec>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPostfixIncDec>>(expression_type);
    return extract_from_expression_postfix_incdec(function, current_block, returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionUnaryPrefix>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionUnaryPrefix>>(expression_type);
    return extract_from_expression_unary_prefix(function, current_block, returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionUnarySizeofType>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionUnarySizeofType>>(expression_type);
    return extract_from_expression_unary_sizeof_type(function, current_block, returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionBinary>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionBinary>>(expression_type);
    return extract_from_expression_binary(function, current_block, returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionTrinary>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionTrinary>>(expression_type);
    return extract_from_expression_trinary(function, current_block, returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionCast>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionCast>>(expression_type);
    return extract_from_expression_cast(function, current_block, returned_tmpvar, *expression);
  }
  else {
    fprintf(stderr, "Compiler bug, invalid expression type\n");
    return false;
  }
}

bool
FunctionDefinitionResolver::extract_from_statement_return(
    JLang::mir::Function & function,
    size_t & current_block,
    const StatementReturn & statement
    )
{
    size_t expression_tmpvar;
    if (!extract_from_expression(function, current_block, expression_tmpvar, statement.get_expression())) {
	return false;
    }

    auto operation = std::make_unique<OperationReturn>(
	statement.get_source_ref(),
	expression_tmpvar
	);
    function.get_basic_block(current_block).add_operation(std::move(operation));
    return true;
}


bool
FunctionDefinitionResolver::extract_from_statement_ifelse(
    JLang::mir::Function & function,
    size_t & current_block,
    const StatementIfElse & statement
    )
{
    size_t condition_tmpvar;
    if (!extract_from_expression(function, current_block, condition_tmpvar, statement.get_expression())) {
	return false;
    }
    // First, evaluate the expression to get our condition.
    if (!function.tmpvar_get(condition_tmpvar)->is_bool()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		statement.get_expression().get_source_ref(),
		"Invalid condition in if statement.",
		std::string("Type of condition expression should be 'bool' and was ") + function.tmpvar_get(condition_tmpvar)->get_name()
		);
	return false;
    }
    
    size_t blockid_if = function.add_block();
    size_t blockid_done = function.add_block();
    size_t blockid_else = -1;
    if (statement.has_else() || statement.has_else_if()) {
	// If we have an 'else', then
	// dump to it based on the condition.
	blockid_else = function.add_block();
	auto operation = std::make_unique<OperationJumpIfEqual>(
	    statement.get_source_ref(),
	    condition_tmpvar,
	    blockid_if,
	    blockid_else
	    );
	function.get_basic_block(current_block).add_operation(std::move(operation));
    }
    else {
	// Otherwise, jump to done
	// based on condition.
	auto operation = std::make_unique<OperationJumpIfEqual>(
	    statement.get_source_ref(),
	    condition_tmpvar,
	    blockid_if,
	    blockid_done
	    );
	function.get_basic_block(current_block).add_operation(std::move(operation));
    }
    
    current_block = blockid_if;

    // Perform the stuff inside the 'if' block.
    if (!extract_from_statement_list(
	function,
	current_block,
	statement.get_if_scope_body().get_statements()
	    )) {
	return false;
    }
    
    // Unconditionally jump to done.
    auto operation = std::make_unique<OperationJump>(
	statement.get_source_ref(),
	blockid_done
	);
    function.get_basic_block(current_block).add_operation(std::move(operation));
    
    if (statement.has_else()) {
	// Perform the stuff in the 'else' block.
	if (!extract_from_statement_list(
	    function,
	    blockid_else,
	    statement.get_else_scope_body().get_statements()
		)) {
	    return false;
	}
	auto operation = std::make_unique<OperationJump>(
	    statement.get_source_ref(),
	    blockid_done
	    );
	function.get_basic_block(blockid_else).add_operation(std::move(operation));
	// Jump to the 'done' block when the 'else' block is finished.
    }
    else if (statement.has_else_if()) {
	if (!extract_from_statement_ifelse(
	    function,
	    current_block,
	    statement.get_else_if()
		)) {
	    return false;
	}
    }
    current_block = blockid_done;
    return true;
}


bool
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
		return false;
	    }

	    auto operation = std::make_unique<OperationLocalDeclare>(
		statement->get_source_ref(),
		statement->get_name(),
		mir_type->get_name()
		);
	    function.get_basic_block(current_block).add_operation(std::move(operation));
	    unwind.push_back(statement->get_name());
	}
	else if (std::holds_alternative<JLang::owned<StatementBlock>>(statement_type)) {
	    const auto & statement = std::get<JLang::owned<StatementBlock>>(statement_type);
	    if (!extract_from_statement_list(function, current_block, statement->get_scope_body().get_statements())) {
		return false;
	    }
	}
	else if (std::holds_alternative<JLang::owned<StatementExpression>>(statement_type)) {
	    const auto & statement = std::get<JLang::owned<StatementExpression>>(statement_type);
	    size_t returned_tmpvar;
	    if (!extract_from_expression(function, current_block, returned_tmpvar, statement->get_expression())) {
		return false;
	    }
	}
	else if (std::holds_alternative<JLang::owned<StatementIfElse>>(statement_type)) {
	    const auto & statement = std::get<JLang::owned<StatementIfElse>>(statement_type);
	    if (!extract_from_statement_ifelse(function, current_block, *statement)) {
		return false;
	    }
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
	    if (!extract_from_statement_return(function, current_block, *statement)) {
		return false;
	    }
	}
	else {
	    fprintf(stderr, "Compiler bug, invalid statement type\n");
	    return false;
	}
    }
    for (const auto & undecl : unwind) {
	function.remove_local(undecl);
	fprintf(stderr, "Undeclaring %s in block %ld\n", undecl.c_str(), current_block);
	    
	auto operation = std::make_unique<OperationLocalUndeclare>(
	    statement_list.get_source_ref(),
	    undecl);
	function.get_basic_block(current_block).add_operation(std::move(operation));
    }
    start_block = current_block;
    return true;
}


bool
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
	return false;
	
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
    
    if (!extract_from_statement_list(*fn, start_block, function_definition.get_scope_body().get_statements())) {
	return false;
    }
    
    mir.get_functions().add_function(std::move(fn));

    return true;
}
