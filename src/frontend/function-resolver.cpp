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
    extract_types(parse_result.get_translation_unit().get_statements());
}
void
FunctionResolver::extract_from_namespace(
    const FileStatementNamespace & namespace_declaration)
{
    const auto & statements = namespace_declaration.get_statement_list().get_statements();
    extract_types(statements);
}

void
FunctionResolver::extract_from_class_definition(const ClassDefinition & definition)
{
    //fprintf(stderr, "Extracting from class definition, constructors and destructors which are special-case functions.\n");
    // These must be linked back to their corresponding type definitions
    // so that we can generate their code.
}

void
FunctionResolver::extract_types(const std::vector<JLang::owned<FileStatement>> & statements)
{
    for (const auto & statement : statements) {
	const auto & file_statement = statement->get_statement();
	if (std::holds_alternative<JLang::owned<FileStatementFunctionDeclaration>>(file_statement)) {
	    // Nothing, no functions can exist here.
	}
	else if (std::holds_alternative<JLang::owned<FileStatementFunctionDefinition>>(file_statement)) {
	    // This is the only place that functions can be extracted from.
	    FunctionDefinitionResolver function_def_resolver(
		compiler_context,
		*std::get<JLang::owned<FileStatementFunctionDefinition>>(file_statement),
		mir,
		type_resolver
		);
	    function_def_resolver.resolve();
	}
	else if (std::holds_alternative<JLang::owned<FileStatementGlobalDefinition>>(file_statement)) {
	    // Nothing, no functions can exist here.
	}
	else if (std::holds_alternative<JLang::owned<ClassDeclaration>>(file_statement)) {
	    // Nothing, no functions can exist here.
	}
	else if (std::holds_alternative<JLang::owned<ClassDefinition>>(file_statement)) {
	    // Constructors, Destructors, and methods are special cases.
	    extract_from_class_definition(*std::get<JLang::owned<ClassDefinition>>(file_statement));
	}
	else if (std::holds_alternative<JLang::owned<EnumDefinition>>(file_statement)) {
	    // Nothing, no functions can exist here.
	}
	else if (std::holds_alternative<JLang::owned<TypeDefinition>>(file_statement)) {
	    // Nothing, no functions can exist here.
	}
	else if (std::holds_alternative<JLang::owned<FileStatementNamespace>>(file_statement)) {
	    extract_from_namespace(*std::get<JLang::owned<FileStatementNamespace>>(file_statement));
	}
	else if (std::holds_alternative<JLang::owned<FileStatementUsing>>(file_statement)) {
	    // Nothing, no functions can exist here.
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
    , new_tmpvar_id(0)
{}
FunctionDefinitionResolver::~FunctionDefinitionResolver()
{}

void
FunctionDefinitionResolver::resolve()
{
    extract_from_function_definition(function_definition);
}

size_t
FunctionDefinitionResolver::get_new_tmpvar()
{ return new_tmpvar_id++; }


void
FunctionDefinitionResolver::extract_from_expression_primary_identifier(
    JLang::mir::Function & function,
    size_t & current_block,
    ExpressionValue & returned_value,
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
	    returned_value.type = ExpressionValue::TYPE_VALUE;
	    returned_value.value = localvar->type;
	    returned_value.variable_id = get_new_tmpvar();
	    fprintf(stderr, "Local variable %ld\n", returned_value.variable_id);
	    
	    auto operation = std::make_unique<OperationLocalVariable>(
		returned_value.variable_id,
		expression.get_identifier().get_value(),
		localvar->type
		);
	    function.get_basic_block(current_block).add_statement(std::move(operation));

	    // TODO
//	    function.get_basic_block(current_block).add_statement(
//		returned_value.value + std::string(" ") +
//		std::string("_") + std::to_string(returned_value.variable_id) + std::string(" = ") + 
//		std::string("local-identifier ") + expression.get_identifier().get_value()
//		);
	    
	    // XXX We should try to collect all of them and
	    // emit an error if there is an ambiguity (i.e. shadowing of parameters,
	    // or class members).
	    return;
	}
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
	returned_value.type = ExpressionValue::TYPE_VALUE;
	returned_value.value = symbol->get_type()->get_name();
	returned_value.variable_id = get_new_tmpvar();

	auto operation = std::make_unique<OperationSymbol>(
	    returned_value.variable_id,
	    expression.get_identifier().get_fully_qualified_name()
	    );
	function.get_basic_block(current_block).add_statement(std::move(operation));
    }
}

void
FunctionDefinitionResolver::extract_from_expression_primary_nested(
    JLang::mir::Function & function,
    size_t & current_block,
    ExpressionValue & returned_value,
    const JLang::frontend::tree::ExpressionPrimaryNested & expression)
{
// Nested expressions don't emit blocks on their own, just run whatever is nested.
    extract_from_expression(
	function,
	current_block,
	returned_value, // Return whatever value the nested expression returns.
	expression.get_expression()
	);
}

void
FunctionDefinitionResolver::extract_from_expression_primary_literal_char(
    JLang::mir::Function & function,
    size_t & current_block,
    ExpressionValue & returned_value,
    const JLang::frontend::tree::ExpressionPrimaryLiteralChar & expression)
{
    returned_value.type = ExpressionValue::TYPE_VALUE;
    returned_value.value = std::string("u8");
    returned_value.variable_id = get_new_tmpvar();
    
    auto operation = std::make_unique<OperationLiteralChar>(
	returned_value.variable_id,
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
    ExpressionValue & returned_value,
    const JLang::frontend::tree::ExpressionPrimaryLiteralString & expression)
{
    returned_value.type = ExpressionValue::TYPE_VALUE;
    returned_value.value = std::string("u8*");
    returned_value.variable_id = get_new_tmpvar();

    auto operation = std::make_unique<OperationLiteralString>(
	returned_value.variable_id,
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
    ExpressionValue & returned_value,
    const JLang::frontend::tree::ExpressionPrimaryLiteralInt & expression)
{
    returned_value.type = ExpressionValue::TYPE_VALUE;
    returned_value.value = std::string("u32"); // Where should we distinguish u8,u16,u32,u64,i8,i16,i32,i64?
    returned_value.variable_id = get_new_tmpvar();

    auto operation = std::make_unique<OperationLiteralInt>(
	returned_value.variable_id,
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
    ExpressionValue & returned_value,
    const JLang::frontend::tree::ExpressionPrimaryLiteralFloat & expression)
{
    returned_value.type = ExpressionValue::TYPE_VALUE;
    returned_value.value = std::string("f64"); // Where should we distinguish f32,f64?
    returned_value.variable_id = get_new_tmpvar();

    auto operation = std::make_unique<OperationLiteralFloat>(
	returned_value.variable_id,
	expression.get_value()
	);
    function
	.get_basic_block(current_block)
	.add_statement(std::move(operation));
}

static bool is_immediate_function_type(const ExpressionValue & value)
{
    if (value.type == ExpressionValue::TYPE_GLOBAL_FUNCTION) {
	return true;
    }
    return false;
}

static bool is_bool_type(const ExpressionValue & value)
{
    if (value.type == ExpressionValue::TYPE_VALUE) {
	if (value.value == "bool") {
	    return true;
	}
    }
    return false;
}

static bool is_index_type(const ExpressionValue & value)
{
    if (value.type == ExpressionValue::TYPE_VALUE) {
	if (value.value == "u32") {
	    return true;
	}
	return false;
    }
    return false;
}

void
FunctionDefinitionResolver::extract_from_expression_postfix_array_index(
    JLang::mir::Function & function,
    size_t & current_block,
    ExpressionValue & returned_value,
    const ExpressionPostfixArrayIndex & expression)
{
    ExpressionValue array_type;
    ExpressionValue index_type;
    extract_from_expression(function, current_block, array_type, expression.get_array());
    extract_from_expression(function, current_block, index_type, expression.get_index());

    if (!is_index_type(index_type)) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_index().get_source_ref(),
		"Array index must be an integer type",
		std::string("Type of index is not an index")
		);
	return;
    }
    // Look up the type of the array 'pointer to' type
    // TODO
//    function
//	.get_basic_block(current_block)
//	.add_statement("arrayindex[]");
}
void
FunctionDefinitionResolver::extract_from_expression_postfix_function_call(
    JLang::mir::Function & function,
    size_t & current_block,
    ExpressionValue & returned_value,
    const ExpressionPostfixFunctionCall & expression)
{
    // Extract the expression itself from the arguments.
    ExpressionValue function_type;
    extract_from_expression(function, current_block, function_type, expression.get_function());
    
    std::vector<ExpressionValue> arg_types;
    for (const auto & arg_expr : expression.get_arguments().get_arguments()) {
	ExpressionValue arg_type;
	extract_from_expression(function, current_block, arg_type, *arg_expr);
	arg_types.push_back(arg_type);
    }

    const Expression & function_expression = expression.get_function();
    const Expression::ExpressionType & function_expression_type = function_expression.get_expression();

    std::string function_pointer_type_name = function_type.value;
    const Type *function_pointer_type = mir.get_types().get_type(function_pointer_type_name);
	
    // We declare that we return the vale that the function
    // will return.
    returned_value.type = ExpressionValue::TYPE_VALUE;
    returned_value.value = function_pointer_type->get_return_type()->get_name();
    returned_value.variable_id = get_new_tmpvar();
    
    auto operation = std::make_unique<OperationFunctionCall>(
	returned_value.variable_id
	);
    operation->add_operand(function_type.variable_id);  // The function to call.
    
    std::string call_args = "";
    for (const auto & av : arg_types) {
	call_args += std::string("_") + std::to_string(av.variable_id) + " ";
	operation->add_operand(av.variable_id);
    }
    function
	.get_basic_block(current_block)
	.add_statement(std::move(operation));
    
}

void
FunctionDefinitionResolver::extract_from_expression_postfix_dot(
    JLang::mir::Function & function,
    size_t & current_block,
    ExpressionValue & returned_value,
    const ExpressionPostfixDot & expression)
{
    extract_from_expression(function, current_block, returned_value, expression.get_expression());
    // TODO
//    function
//	.get_basic_block(current_block)
//	.add_statement(std::string("dot ") + expression.get_identifier());
}
void
FunctionDefinitionResolver::extract_from_expression_postfix_arrow(
    JLang::mir::Function & function,
    size_t & current_block,
    ExpressionValue & returned_value,
    const ExpressionPostfixArrow & expression)
{
    extract_from_expression(function, current_block, returned_value, expression.get_expression());
    // TODO
//    function
//	.get_basic_block(current_block)
//	.add_statement(std::string("arrow ") + expression.get_identifier());
}
void
FunctionDefinitionResolver::extract_from_expression_postfix_incdec(
    JLang::mir::Function & function,
    size_t & current_block,
    ExpressionValue & returned_value,
    const ExpressionPostfixIncDec & expression)
{
    ExpressionValue operand_value;
    extract_from_expression(function, current_block, operand_value, expression.get_expression());

    returned_value.type = ExpressionValue::TYPE_VALUE;
    returned_value.value = operand_value.value;
    returned_value.variable_id = get_new_tmpvar();
    
    std::string op = "unknown";
    switch (expression.get_type()) {
    case ExpressionPostfixIncDec::INCREMENT:
    {
	op = "++";
	OperationPostIncrement operation(
	    returned_value.variable_id, operand_value.variable_id
	    );
    }
	break;
    case ExpressionPostfixIncDec::DECREMENT:
    {
	op = "--";
	OperationPostDecrement operation(
	    returned_value.variable_id, operand_value.variable_id
	    );
    }
	break;
    default:
	break;
    }
    
//    function
//	.get_basic_block(current_block)
//	.add_statement(
//	    returned_value.value + std::string(" ") +
//	    std::string("_") + std::to_string(returned_value.variable_id) + std::string(" = ") + 
//	    std::string("incdec ") + op
//	    );
    
}



void
FunctionDefinitionResolver::extract_from_expression_unary_prefix(
    JLang::mir::Function & function,
    size_t & current_block,
    ExpressionValue & returned_value,
    const ExpressionUnaryPrefix & expression)
{

    // Extract the prior expression
    // and if not otherwise specified,
    // the operation will return the
    // same type as the operand.
    ExpressionValue operand_value;
    extract_from_expression(
	function,
	current_block,
	operand_value,
	expression.get_expression()
	);
    
    returned_value.type = operand_value.type;
    returned_value.value = operand_value.value;
    returned_value.variable_id = get_new_tmpvar();

    std::string op = "";
    switch (expression.get_type()) {
    case ExpressionUnaryPrefix::INCREMENT:
	
	op = "++";
	break;
    case ExpressionUnaryPrefix::DECREMENT:
	op = "--";
	break;
    case ExpressionUnaryPrefix::ADDRESSOF:
	// The addressof changes the type of the
	// return value to "pointer to" the type returned.
	op = "&";
	returned_value.value = returned_value.value + std::string("*");
	break;
    case ExpressionUnaryPrefix::DEREFERENCE:
    {
	op = "*";

	Type *mir_type = mir.get_types().get_type(returned_value.value);
	if (mir_type == nullptr) {
	    fprintf(stderr, "Compiler Bug!  Looking for type %s\n", returned_value.value.c_str());
	    exit(1);
	}
	if (mir_type->get_type() != Type::TYPE_POINTER) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    expression.get_expression().get_source_ref(),
		    "Cannot dereference non-pointer",
		    std::string("Attempting to de-reference non-pointer type ") + returned_value.value
		    );
	    return;
	}
    }
	break;
    case ExpressionUnaryPrefix::PLUS:
	op = "+";
	break;
    case ExpressionUnaryPrefix::MINUS:
	op = "-";
	break;
    case ExpressionUnaryPrefix::BITWISE_NOT:
	op = "~";
	break;
    case ExpressionUnaryPrefix::LOGICAL_NOT:
	if (!is_bool_type(operand_value)) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    expression.get_expression().get_source_ref(),
		    "Logical not (!) must operate on 'bool' expressions.",
		    std::string("Type of condition expression should be 'bool' and was ") + returned_value.value
		    );
	}
	op = "!";
	break;
    default:
	fprintf(stderr, "Compiler bug! unknown unary operator\n");
	exit(1);
	break;
    }

    // TODO
//    function
//	.get_basic_block(current_block)
//	.add_statement(
//	    returned_value.value + std::string(" ") +
//	    std::string("_") + std::to_string(returned_value.variable_id) + std::string(" = ") + 
//	    std::string("unary ") + op + std::string("_") + std::to_string(operand_value.variable_id)
//	    );
  
}
void
FunctionDefinitionResolver::extract_from_expression_unary_sizeof_type(
    JLang::mir::Function & function,
    size_t & current_block,
    ExpressionValue & returned_value,
    const ExpressionUnarySizeofType & expression)
{
    // TODO
//    function
//	.get_basic_block(current_block)
//	.add_statement("sizeof");
}

void
FunctionDefinitionResolver::extract_from_expression_binary(
    JLang::mir::Function & function,
    size_t & current_block,
    ExpressionValue & returned_value,
    const ExpressionBinary & expression)
{
    std::string op = "";

    ExpressionValue aval;
    ExpressionValue bval;
    extract_from_expression(function, current_block, aval, expression.get_a());
    extract_from_expression(function, current_block, bval, expression.get_b());
    if (aval.type != bval.type || aval.value != bval.value) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_source_ref(),
		"Type mismatch in binary operation",
		std::string("The type of operands should match: a= ") + aval.value + std::string(" b=") + bval.value
		);
	return;
    }
    returned_value.variable_id = get_new_tmpvar();
    
    switch (expression.get_operator()) {
    case ExpressionBinary::ADD:
	{
	op = "+";
	returned_value.type = ExpressionValue::TYPE_VALUE;
	returned_value.value = aval.value;

	auto operation = std::make_unique<OperationAdd>(
	    returned_value.variable_id,
	    aval.variable_id,
	    bval.variable_id
	    );
	function
	    .get_basic_block(current_block)
	    .add_statement(std::move(operation));
	}	
	break;
    case ExpressionBinary::SUBTRACT:
	op = "-";
	returned_value.type = ExpressionValue::TYPE_VALUE;
	returned_value.value = "u32";
	break;
    case ExpressionBinary::MULTIPLY:
	op = "*";
	returned_value.type = ExpressionValue::TYPE_VALUE;
	returned_value.value = "u32";
	break;
    case ExpressionBinary::DIVIDE:
	op = "/";
	returned_value.type = ExpressionValue::TYPE_VALUE;
	returned_value.value = "u32";
	break;
    case ExpressionBinary::MODULO:
	op = "%";
	returned_value.type = ExpressionValue::TYPE_VALUE;
	returned_value.value = "u32";
	break;
    case ExpressionBinary::LOGICAL_AND:
	op = "&&";
	returned_value.type = ExpressionValue::TYPE_VALUE;
	returned_value.value = "bool";
	break;
    case ExpressionBinary::LOGICAL_OR:
	op = "||";
	returned_value.type = ExpressionValue::TYPE_VALUE;
	returned_value.value = "bool";
	break;
	
    case ExpressionBinary::BITWISE_AND:
	op = "&";
	break;
    case ExpressionBinary::BITWISE_OR:
	op = "|";
	break;
    case ExpressionBinary::BITWISE_XOR:
	op = "^";
	break;
    case ExpressionBinary::SHIFT_LEFT:
	op = "<<";
	break;
    case ExpressionBinary::SHIFT_RIGHT:
	op = ">>";
	break;
	
    case ExpressionBinary::COMPARE_LT:
	op = "<";
	returned_value.type = ExpressionValue::TYPE_VALUE;
	returned_value.value = "bool";
	break;
    case ExpressionBinary::COMPARE_GT:
	op = ">";
	returned_value.type = ExpressionValue::TYPE_VALUE;
	returned_value.value = "bool";
	break;
    case ExpressionBinary::COMPARE_LE:
	op = "<=";
	returned_value.type = ExpressionValue::TYPE_VALUE;
	returned_value.value = "bool";
	break;
    case ExpressionBinary::COMPARE_GE:
	op = ">=";
	returned_value.type = ExpressionValue::TYPE_VALUE;
	returned_value.value = "bool";
	break;
    case ExpressionBinary::COMPARE_EQ:
	op = "==";
	returned_value.type = ExpressionValue::TYPE_VALUE;
	returned_value.value = "bool";
	break;
    case ExpressionBinary::COMPARE_NE:
	op = "!=";
	returned_value.type = ExpressionValue::TYPE_VALUE;
	returned_value.value = "bool";
	break;
	
    case ExpressionBinary::EQUALS:
    {
	op = "=";
	auto operation = std::make_unique<OperationAssign>(
	    returned_value.variable_id,
	    aval.variable_id,
	    bval.variable_id
	    );
	function
	    .get_basic_block(current_block)
	    .add_statement(std::move(operation));
    }	
        break;
    case ExpressionBinary::MUL_ASSIGN:
	op = "*=";
	break;
    case ExpressionBinary::DIV_ASSIGN:
	op = "/=";
	break;
    case ExpressionBinary::MOD_ASSIGN:
	op = "%=";
	break;
    case ExpressionBinary::ADD_ASSIGN:
	op = "+=";
	break;
    case ExpressionBinary::SUB_ASSIGN:
	op = "-=";
	break;
    case ExpressionBinary::LEFT_ASSIGN:
	op = "<<=";
	break;
    case ExpressionBinary::RIGHT_ASSIGN:
	op = ">>=";
	break;
    case ExpressionBinary::AND_ASSIGN:
	op = "&=";
	break;
    case ExpressionBinary::OR_ASSIGN:
	op = "|=";
	break;
    case ExpressionBinary::XOR_ASSIGN:
	op = "^=";
	break;
    default:
	fprintf(stderr, "Compiler bug! unknown binary operator\n");
	exit(1);
	break;
    }

//    function
//	.get_basic_block(current_block)
//	.add_statement(
//	    returned_value.value + std::string(" ") +
//	    std::string("_") + std::to_string(returned_value.variable_id) + std::string(" = ") + 
//	    std::string("binary operator ") + op + std::string(" a=") + std::to_string(aval.variable_id) + std::string(" b=") + std::to_string(bval.variable_id)
//	    );
}
void
FunctionDefinitionResolver::extract_from_expression_trinary(
    JLang::mir::Function & function,
    size_t & current_block,
    ExpressionValue & returned_value,
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
    ExpressionValue & returned_value,
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
    ExpressionValue & returned_value,
    const Expression & expression_container)
{
  const auto & expression_type = expression_container.get_expression();

  if (std::holds_alternative<JLang::owned<ExpressionPrimaryIdentifier>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPrimaryIdentifier>>(expression_type);
    extract_from_expression_primary_identifier(function, current_block, returned_value, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPrimaryNested>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPrimaryNested>>(expression_type);
    extract_from_expression_primary_nested(function, current_block, returned_value, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPrimaryLiteralChar>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPrimaryLiteralChar>>(expression_type);
    extract_from_expression_primary_literal_char(function, current_block, returned_value, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPrimaryLiteralString>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPrimaryLiteralString>>(expression_type);
    extract_from_expression_primary_literal_string(function, current_block, returned_value, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPrimaryLiteralInt>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPrimaryLiteralInt>>(expression_type);
    extract_from_expression_primary_literal_int(function, current_block, returned_value, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPrimaryLiteralFloat>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPrimaryLiteralFloat>>(expression_type);
    extract_from_expression_primary_literal_float(function, current_block, returned_value, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPostfixArrayIndex>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPostfixArrayIndex>>(expression_type);
    extract_from_expression_postfix_array_index(function, current_block, returned_value, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPostfixFunctionCall>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPostfixFunctionCall>>(expression_type);
    extract_from_expression_postfix_function_call(function, current_block, returned_value, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPostfixDot>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPostfixDot>>(expression_type);
    extract_from_expression_postfix_dot(function, current_block, returned_value, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPostfixArrow>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPostfixArrow>>(expression_type);
    extract_from_expression_postfix_arrow(function, current_block, returned_value, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPostfixIncDec>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPostfixIncDec>>(expression_type);
    extract_from_expression_postfix_incdec(function, current_block, returned_value, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionUnaryPrefix>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionUnaryPrefix>>(expression_type);
    extract_from_expression_unary_prefix(function, current_block, returned_value, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionUnarySizeofType>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionUnarySizeofType>>(expression_type);
    extract_from_expression_unary_sizeof_type(function, current_block, returned_value, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionBinary>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionBinary>>(expression_type);
    extract_from_expression_binary(function, current_block, returned_value, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionTrinary>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionTrinary>>(expression_type);
    extract_from_expression_trinary(function, current_block, returned_value, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionCast>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionCast>>(expression_type);
    extract_from_expression_cast(function, current_block, returned_value, *expression);
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
    ExpressionValue retval;
    extract_from_expression(function, current_block, retval, statement.get_expression());

    auto operation = std::make_unique<OperationReturn>(
	retval.variable_id
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
    ExpressionValue condition_value;
    extract_from_expression(function, current_block, condition_value, statement.get_expression());
    if (!is_bool_type(condition_value)) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		statement.get_expression().get_source_ref(),
		"Invalid condition in if statement.",
		std::string("Type of condition expression should be 'bool' and was ") + condition_value.value
		);
    }
    
    size_t blockid_done = function.add_block();
    size_t blockid_else = -1;
    if (statement.has_else() || statement.has_else_if()) {
	blockid_else = function.add_block();
	auto operation = std::make_unique<OperationJumpIfEqual>(
	    condition_value.variable_id,
	    std::to_string(blockid_else)
	    );
	function.get_basic_block(current_block).add_statement(std::move(operation));
    }
    else {
	auto operation = std::make_unique<OperationJumpIfEqual>(
	    condition_value.variable_id,
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
	    
	    JLang::mir::Type * mir_type = type_resolver.extract_from_type_specifier(statement->get_type_specifier());

	    LocalVariable local(statement->get_name(), mir_type->get_name(), statement->get_source_ref());
	    
	    if (!function.add_local(local)) {
		compiler_context
		    .get_errors()
		    .add_simple_error(
			statement->get_type_specifier().get_source_ref(),
			"Duplicate Local Variable.",
			std::string("Variable with name ") + local.name + std::string(" is already in scope and cannot be duplicated in this function.")
			);
	    }

	    auto operation = std::make_unique<OperationLocalDeclare>(statement->get_name(), mir_type->get_name());
	    function.get_basic_block(current_block).add_statement(std::move(operation));
	    unwind.push_back(statement->get_name());
	}
	else if (std::holds_alternative<JLang::owned<StatementBlock>>(statement_type)) {
	    const auto & statement = std::get<JLang::owned<StatementBlock>>(statement_type);
	    extract_from_statement_list(function, current_block, statement->get_scope_body().get_statements());
	}
	else if (std::holds_alternative<JLang::owned<StatementExpression>>(statement_type)) {
	    const auto & statement = std::get<JLang::owned<StatementExpression>>(statement_type);
	    ExpressionValue returned_value;
	    extract_from_expression(function, current_block, returned_value, statement->get_expression());
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
	    
	auto operation = std::make_unique<OperationLocalUndeclare>(undecl);
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
    Type *return_type = type_resolver.extract_from_type_specifier(type_specifier);
    
    if (return_type == nullptr) {
	fprintf(stderr, "Could not find return type\n");
	return;
	
    }
    std::vector<FunctionArgument> arguments;
    const auto & function_argument_list = function_definition.get_arguments();
    const auto & function_definition_args = function_argument_list.get_arguments();
    for (const auto & function_definition_arg : function_definition_args) {
       std::string name = function_definition_arg->get_name();
       JLang::mir::Type * mir_type = type_resolver.extract_from_type_specifier(function_definition_arg->get_type_specifier());
       
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
