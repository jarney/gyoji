#include <jlang-frontend/function-resolver.hpp>
#include <variant>
#include <stdio.h>

using namespace JLang::mir;
using namespace JLang::context;
using namespace JLang::frontend;
using namespace JLang::frontend::tree;

FunctionResolver::FunctionResolver(
    JLang::context::CompilerContext & _compiler_context,
    const JLang::frontend::tree::TranslationUnit & _translation_unit,
    JLang::mir::MIR & _mir,
    JLang::frontend::TypeResolver & _type_resolver
    )
    : compiler_context(_compiler_context)
    , translation_unit(_translation_unit)
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
  extract_types(translation_unit.get_statements());
}

void
FunctionResolver::extract_from_namespace(
    const FileStatementNamespace & namespace_declaration)
{
    const auto & statements = namespace_declaration.get_statement_list().get_statements();
    extract_types(statements);
}

static int blockid = 0;

static int tmpvar = 0;


void
FunctionResolver::extract_from_expression_primary_identifier(
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
    
    function.get_basic_block(current_block).add_statement(
	std::string("identifier ") +
	expression.get_identifier().get_fully_qualified_name() + "::" + 
	expression.get_identifier().get_value()
	);
    returned_value.type = ExpressionValue::TYPE_IDENTIFIER;
    returned_value.value =
	expression.get_identifier().get_fully_qualified_name() + "::" + 
	expression.get_identifier().get_value();
}

void
FunctionResolver::extract_from_expression_primary_nested(
    JLang::mir::Function & function,
    size_t & current_block,
    ExpressionValue & returned_value,
    const JLang::frontend::tree::ExpressionPrimaryNested & expression)
{
    extract_from_expression(
	function,
	current_block,
	returned_value, // Return whatever value the nested expression returns.
	expression.get_expression()
	);
    function
	.get_basic_block(current_block)
	.add_statement("(expression)");
}

void
FunctionResolver::extract_from_expression_primary_literal_char(
    JLang::mir::Function & function,
    size_t & current_block,
    ExpressionValue & returned_value,
    const JLang::frontend::tree::ExpressionPrimaryLiteralChar & expression)
{
    function
	.get_basic_block(current_block)
	.add_statement(
	    std::string("char literal ") + expression.get_value()
	    );
    returned_value.type = ExpressionValue::TYPE_CHAR_LITERAL;
    returned_value.value = expression.get_value();
}

void
FunctionResolver::extract_from_expression_primary_literal_string(
    JLang::mir::Function & function,
    size_t & current_block,
    ExpressionValue & returned_value,
    const JLang::frontend::tree::ExpressionPrimaryLiteralString & expression)
{
    function
	.get_basic_block(current_block)
	.add_statement(std::string("string literal ") + expression.get_value());
    returned_value.type = ExpressionValue::TYPE_STRING_LITERAL;
    returned_value.value = expression.get_value();
}

void
FunctionResolver::extract_from_expression_primary_literal_int(
    JLang::mir::Function & function,
    size_t & current_block,
    ExpressionValue & returned_value,
    const JLang::frontend::tree::ExpressionPrimaryLiteralInt & expression)
{
    function
	.get_basic_block(current_block)
	.add_statement(std::string("int literal ") + expression.get_value());
    returned_value.type = ExpressionValue::TYPE_INT_LITERAL;
    returned_value.value = expression.get_value();
}

void
FunctionResolver::extract_from_expression_primary_literal_float(
    JLang::mir::Function & function,
    size_t & current_block,
    ExpressionValue & returned_value,
    const JLang::frontend::tree::ExpressionPrimaryLiteralFloat & expression)
{
    function
	.get_basic_block(current_block)
	.add_statement(std::string("float literal ") + expression.get_value());
    returned_value.type = ExpressionValue::TYPE_FLOAT_LITERAL;
    returned_value.value = expression.get_value();
}

static bool is_bool_type(ExpressionValue & expression_value)
{
    if (expression_value.type == ExpressionValue::TYPE_TYPE) {
	if (expression_value.value == "bool") {
	    return true;
	}
    }
    return false;
}

static bool is_index_type(ExpressionValue & index_type)
{
    if (index_type.type == ExpressionValue::TYPE_TYPE) {
	if (index_type.value == "u32") {
	    return true;
	}
	return false;
    }
    if (index_type.type == ExpressionValue::TYPE_INT_LITERAL) {
	return true;
    }
    return false;
}

void
FunctionResolver::extract_from_expression_postfix_array_index(
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
    
    
    function
	.get_basic_block(current_block)
	.add_statement("arrayindex[]");
}
void
FunctionResolver::extract_from_expression_postfix_function_call(
    JLang::mir::Function & function,
    size_t & current_block,
    ExpressionValue & returned_value,
    const ExpressionPostfixFunctionCall & expression)
{
    // We should skip the expression evaluation
    // if we can extract a name directly and differentiate
    // between direct and indirect function calls.
    //
    // Case: ordinary function call that can be resolved at compile-time.
    // Case: method call that can be resolved at compile-time.
    // Case: Indirect method call that requires further expression evaluation.
    //
    // This one is actually pretty complicated.
    
    // XXX We should actually look up the function
    // and verify that the arguments are equal to
    // the signature we're looking for so we can verify if this
    // is a valid function call and formulate it correctly.

    // Evaluate all of the expressions
    // for the argument list.
    // Then, evaluate the expression for the name of the function
    // and finally gather the name and types for each
    // of the arguments and build a FunctionPrototype object
    // from it.  Use that FunctionPrototype to build
    // the "mangled" name of the actual function to call.
    //const ArgumentExpressionList & get_arguments() const;

    std::vector<ExpressionValue> arg_types;
    for (const auto & arg_expr : expression.get_arguments().get_arguments()) {
	ExpressionValue arg_type;
	extract_from_expression(function, current_block, arg_type, *arg_expr);
	arg_types.push_back(arg_type);
    }
    const Expression & function_expression = expression.get_function();
    const Expression::ExpressionType & function_expression_type = function_expression.get_expression();
    
    if (std::holds_alternative<JLang::owned<ExpressionPrimaryIdentifier>>(function_expression_type)) {
	const auto & function_identifier = std::get<JLang::owned<ExpressionPrimaryIdentifier>>(function_expression_type);
	// If this expression is a primary expression, then this is
	// an immediate function call and we can directly emit
	// a call to that function.

	// XXX This is a hack because
	// we should really be letting the
	// namespace do all the heavy lifting here
	// to format the identifier correctly.
	std::string function_name =
	    function_identifier->get_identifier().get_fully_qualified_name()
	    + std::string("::")
	    + function_identifier->get_identifier().get_value();
	fprintf(stderr, "Looking up prototype %s\n", function_name.c_str());
	const FunctionPrototype * prototype = mir.get_functions().get_prototype(function_name);
	fprintf(stderr, "Found? %p\n", prototype);
	if (prototype == nullptr) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    expression.get_function().get_source_ref(),
		    "Unknown function",
		    std::string("Function call ") + function_name + std::string(" could not be found")
		    );
	    return;
	}
	function
	    .get_basic_block(current_block)
	    .add_statement(std::string("function-call-immediate ") + prototype->get_name());
	
	// XXX We should find the return value of the function
	// and report it here.  For now, we'll hard-code u32
	// because we don't have that yet.
	returned_value.type = ExpressionValue::TYPE_TYPE;
	returned_value.value = prototype->get_return_type();
	
    }
    else {
	// Otherwise, this expression should be interpreted as an expression
	// returning a function-pointer value and we should perform an indirect
	// call to the value that is evaluated in that expression.
	ExpressionValue function_id_value;
	extract_from_expression(
	    function,
	    current_block,
	    returned_value,
	    expression.get_function()
	    );
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_function().get_source_ref(),
		"Compiler todo! Function pointers are not yet supported",
		"Function is dependent on evaluating an expression which is not yet supported."
		);
	return;
    }

}

void
FunctionResolver::extract_from_expression_postfix_dot(
    JLang::mir::Function & function,
    size_t & current_block,
    ExpressionValue & returned_value,
    const ExpressionPostfixDot & expression)
{
    extract_from_expression(function, current_block, returned_value, expression.get_expression());
    function
	.get_basic_block(current_block)
	.add_statement(std::string("dot ") + expression.get_identifier());
}
void
FunctionResolver::extract_from_expression_postfix_arrow(
    JLang::mir::Function & function,
    size_t & current_block,
    ExpressionValue & returned_value,
    const ExpressionPostfixArrow & expression)
{
    extract_from_expression(function, current_block, returned_value, expression.get_expression());
    function
	.get_basic_block(current_block)
	.add_statement(std::string("arrow ") + expression.get_identifier());
}
void
FunctionResolver::extract_from_expression_postfix_incdec(
    JLang::mir::Function & function,
    size_t & current_block,
    ExpressionValue & returned_value,
    const ExpressionPostfixIncDec & expression)
{
    extract_from_expression(function, current_block, returned_value, expression.get_expression());
    function
	.get_basic_block(current_block)
	.add_statement(std::string("++/-- "));
}



void
FunctionResolver::extract_from_expression_unary_prefix(
    JLang::mir::Function & function,
    size_t & current_block,
    ExpressionValue & returned_value,
    const ExpressionUnaryPrefix & expression)
{

    // Extract the prior expression
    // and if not otherwise specified,
    // the operation will return the
    // same type as the operand.
    extract_from_expression(
	function,
	current_block,
	returned_value,
	expression.get_expression()
	);
    
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
	if (!is_bool_type(returned_value)) {
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
    function
	.get_basic_block(current_block)
	.add_statement(std::string("unary ") + op);
  
}
void
FunctionResolver::extract_from_expression_unary_sizeof_type(
    JLang::mir::Function & function,
    size_t & current_block,
    ExpressionValue & returned_value,
    const ExpressionUnarySizeofType & expression)
{
    function
	.get_basic_block(current_block)
	.add_statement("sizeof");
}

void
FunctionResolver::extract_from_expression_binary(
    JLang::mir::Function & function,
    size_t & current_block,
    ExpressionValue & returned_value,
    const ExpressionBinary & expression)
{
    std::string op = "";
    extract_from_expression(function, current_block, returned_value, expression.get_a());
    extract_from_expression(function, current_block, returned_value, expression.get_b());
    switch (expression.get_operator()) {
    case ExpressionBinary::ADD:
	op = "+";
	returned_value.type = ExpressionValue::TYPE_TYPE;
	returned_value.value = "u32";
	break;
    case ExpressionBinary::SUBTRACT:
	op = "-";
	returned_value.type = ExpressionValue::TYPE_TYPE;
	returned_value.value = "u32";
	break;
    case ExpressionBinary::MULTIPLY:
	op = "*";
	returned_value.type = ExpressionValue::TYPE_TYPE;
	returned_value.value = "u32";
	break;
    case ExpressionBinary::DIVIDE:
	op = "/";
	returned_value.type = ExpressionValue::TYPE_TYPE;
	returned_value.value = "u32";
	break;
    case ExpressionBinary::MODULO:
	op = "%";
	returned_value.type = ExpressionValue::TYPE_TYPE;
	returned_value.value = "u32";
	break;
    case ExpressionBinary::LOGICAL_AND:
	op = "&&";
	returned_value.type = ExpressionValue::TYPE_TYPE;
	returned_value.value = "bool";
	break;
    case ExpressionBinary::LOGICAL_OR:
	op = "||";
	returned_value.type = ExpressionValue::TYPE_TYPE;
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
	returned_value.type = ExpressionValue::TYPE_TYPE;
	returned_value.value = "bool";
	break;
    case ExpressionBinary::COMPARE_GT:
	op = ">";
	returned_value.type = ExpressionValue::TYPE_TYPE;
	returned_value.value = "bool";
	break;
    case ExpressionBinary::COMPARE_LE:
	op = "<=";
	returned_value.type = ExpressionValue::TYPE_TYPE;
	returned_value.value = "bool";
	break;
    case ExpressionBinary::COMPARE_GE:
	op = ">=";
	returned_value.type = ExpressionValue::TYPE_TYPE;
	returned_value.value = "bool";
	break;
    case ExpressionBinary::COMPARE_EQ:
	op = "==";
	returned_value.type = ExpressionValue::TYPE_TYPE;
	returned_value.value = "bool";
	break;
    case ExpressionBinary::COMPARE_NE:
	op = "!=";
	returned_value.type = ExpressionValue::TYPE_TYPE;
	returned_value.value = "bool";
	break;
	
    case ExpressionBinary::EQUALS:
	op = "=";
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
    
    function
	.get_basic_block(current_block)
	.add_statement(std::string("binary operator ") + op);
}
void
FunctionResolver::extract_from_expression_trinary(
    JLang::mir::Function & function,
    size_t & current_block,
    ExpressionValue & returned_value,
    const ExpressionTrinary & expression)
{
    function
	.get_basic_block(current_block)
	.add_statement(std::string("trinary operator "));
}
void
FunctionResolver::extract_from_expression_cast(
    JLang::mir::Function & function,
    size_t & current_block,
    ExpressionValue & returned_value,
    const ExpressionCast & expression)
{
    function
	.get_basic_block(current_block)
	.add_statement(std::string("cast"));
}


void
FunctionResolver::extract_from_expression(
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
FunctionResolver::extract_from_statement_ifelse(
    JLang::mir::Function & function,
    size_t & current_block,
    const StatementIfElse & statement
    )
{
    ExpressionValue condition_value;
    extract_from_expression(function, current_block, condition_value, statement.get_expression());
    if (condition_value.type != ExpressionValue::TYPE_TYPE || condition_value.value != std::string("bool")) {
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
	function.get_basic_block(current_block).add_statement(std::string("jne BB") + std::to_string(blockid_else));
    }
    else {
	function.get_basic_block(current_block).add_statement(std::string("jne BB") + std::to_string(blockid_done));
    }
    function.push_block(current_block);
    current_block = function.add_block();
    
    extract_from_statement_list(
	function,
	current_block,
	statement.get_if_scope_body().get_statements()
	);
    
    if (statement.has_else()) {
	function.get_basic_block(current_block).add_statement(std::string("jmp BB") + std::to_string(blockid_done));
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
FunctionResolver::extract_from_statement_list(
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
	    function.get_basic_block(current_block).add_statement(std::string("declare ") + statement->get_name());
	    
	    fprintf(stderr, "Declaring variable %s\n", statement->get_name().c_str());
	    JLang::mir::Type * mir_type = type_resolver.extract_from_type_specifier(statement->get_type_specifier());

	    LocalVariable local(statement->get_name(), mir_type->get_name());
	    
	    if (!function.add_local(local)) {
		compiler_context
		    .get_errors()
		    .add_simple_error(
			statement->get_type_specifier().get_source_ref(),
			"Duplicate Local Variable.",
			std::string("Variable with name ") + local.name + std::string(" is already in scope and cannot be duplicated in this function.")
			);
	    }
	    
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
	    fprintf(stderr, "return\n");
	}
	else {
	    fprintf(stderr, "Compiler bug, invalid statement type\n");
	    exit(1);
	}
    }
    for (const auto & undecl : unwind) {
	function.remove_local(undecl);
	fprintf(stderr, "Undeclaring %s in block %ld\n", undecl.c_str(), current_block);
	function.get_basic_block(current_block).add_statement(std::string("undeclare ") + undecl);
    }
    start_block = current_block;
}


void
FunctionResolver::extract_from_function_definition(const FileStatementFunctionDefinition & function_definition)
{
    fprintf(stderr, "Extracting function %s %s\n",
	    function_definition.get_name().get_fully_qualified_name().c_str(),
	    function_definition.get_name().get_value().c_str());
    
    std::string fully_qualified_function_name = 
	function_definition.get_name().get_fully_qualified_name();
    
    const TypeSpecifier & type_specifier = function_definition.get_return_type();
    Type *type = type_resolver.extract_from_type_specifier(type_specifier);
    
    if (type == nullptr) {
	fprintf(stderr, "Could not find return type\n");
	return;
	
    }
    
    std::vector<FunctionArgument> arguments;
    const auto & function_argument_list = function_definition.get_arguments();
    const auto & function_definition_args = function_argument_list.get_arguments();
    for (const auto & function_definition_arg : function_definition_args) {
	std::string name = function_definition_arg->get_name();
	JLang::mir::Type * mir_type = type_resolver.extract_from_type_specifier(function_definition_arg->get_type_specifier());
	std::string type = mir_type->get_name();
	
	FunctionArgument arg(name, type);
	arguments.push_back(arg);
    }
    FunctionPrototype prototype_lookup(
	fully_qualified_function_name,
	type->get_name(),
	arguments
	);
    const FunctionPrototype * prototype = mir.get_functions().get_prototype(prototype_lookup.get_mangled_name());
    fprintf(stderr, "Found? %p\n", prototype);
    if (prototype == nullptr) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		function_definition.get_source_ref(),
		"Unknown function",
		std::string("Function call ") + prototype_lookup.get_mangled_name() + std::string(" could not be found")
		);
	return;
    }
    
    JLang::owned<Function> fn = std::make_unique<Function>(*prototype);
    
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
	    extract_from_function_definition(*std::get<JLang::owned<FileStatementFunctionDefinition>>(file_statement));
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


// if (a) {
//   int b;
//   if (c) {
//        int d;
//        b = d;
//   }
//   else () {
//        int e;
//        b = e;
//   }
//   q();
// }
// else {
//    r();
// }

// For the first phase, take the outer BB if statement:
//
// BB0 (if)
//   compare
//   jeq BB2
// BB1
//    int b;
//    other things....
//    q();
// BB2
//    r();

// So then the recursion step is just to take the things that
// would be left inside "BB1" and expand them again.

// BB1
//    int b;
//    other things....
//    q();
// Becomes

// BB1
//    int q;
//    compare
//    q();

// So each phase has:

// ##Basic data-structure.##
// List of basic blocks "before" the jump.
// List of statements that have not yet
// been emitted.

//## Basic algorithm. ##
// Add statements to the "before"
// until we encounter a branch/control structure.
// When that happens, create a new BB containing
// everything after the branch as a new BB.
// Close this BB and make the conditional block(s) depend on
// the new one.
//
// Process the next BB.

// Can we do a quick prototype of this structure?
// without the fully-blown syntax?

