#include <jlang-frontend/function-resolver.hpp>
#include <jlang-misc/jstring.hpp>
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
    , scope_tracker(_compiler_context)
{}
FunctionDefinitionResolver::~FunctionDefinitionResolver()
{}

bool
FunctionDefinitionResolver::resolve()
{
    std::string fully_qualified_function_name = 
	function_definition.get_name().get_fully_qualified_name();
    
    fprintf(stderr, " - Extracting function %s\n",
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
       fprintf(stderr, "Function argument %s\n", name.c_str());
       if (!scope_tracker.add_variable(name, mir_type, function_definition_arg->get_source_ref())) {
	   fprintf(stderr, "Existing, stopping process\n");
	   return false;
       }
    }
    
    function = std::make_unique<Function>(
	fully_qualified_function_name,
	return_type,
	arguments,
	function_definition.get_source_ref());

    
    // Create a new basic block for the start
    
    current_block = function->add_block();
    
    std::map<std::string, FunctionLabel> labels;

    if (!extract_from_statement_list(
	    labels,
	    function_definition.get_scope_body().get_statements())) {
	return false;
    }
    
    mir.get_functions().add_function(std::move(function));

    return true;
}

bool
FunctionDefinitionResolver::extract_from_expression_primary_identifier(
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
	const JLang::frontend::LocalVariable *localvar = scope_tracker.get_variable(
	    expression.get_identifier().get_value()
	    );
	if (localvar != nullptr) {
	    returned_tmpvar = function->tmpvar_define(localvar->get_type());
	    auto operation = std::make_unique<OperationLocalVariable>(
		expression.get_identifier().get_source_ref(),
		returned_tmpvar,
		expression.get_identifier().get_value(),
		localvar->get_type()
		);
	    function->get_basic_block(current_block).add_operation(std::move(operation));

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
	returned_tmpvar = function->tmpvar_define(symbol->get_type());

	auto operation = std::make_unique<OperationSymbol>(
	    expression.get_identifier().get_source_ref(),
	    returned_tmpvar,
	    expression.get_identifier().get_fully_qualified_name()
	    );
	function->get_basic_block(current_block).add_operation(std::move(operation));
	return true;
    }
    return false;
}

bool
FunctionDefinitionResolver::extract_from_expression_primary_nested(
    size_t & returned_tmpvar,
    const JLang::frontend::tree::ExpressionPrimaryNested & expression)
{
    // Nested expressions don't emit blocks on their own, just run whatever is nested.
    return extract_from_expression(
	returned_tmpvar,
	expression.get_expression()
	);
}

bool
FunctionDefinitionResolver::extract_from_expression_primary_literal_char(
    size_t & returned_tmpvar,
    const JLang::frontend::tree::ExpressionPrimaryLiteralChar & expression)
{
    std::string string_unescaped;
    size_t location;
    bool escape_success = JLang::misc::string_c_unescape(string_unescaped, location, expression.get_value(), true);
    char c;
    if (!escape_success) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_source_ref(),
		"Invalid Character Literal",
		std::string("Unknown escape sequence found at character offset ") + std::to_string(location) + std::string(" in character literal")
		);
	c = '!';
    }
    else {
	if (string_unescaped.size() != 1) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    expression.get_source_ref(),
		    "Invalid Character Literal",
		    std::string("Character literal must consist of a single byte.")
		    );
	    c = '%';
	}
	else {
	    // Finally, we're sure it's valid and
	    // has one element, we can pass it down
	    // to the operation.
	    c = string_unescaped[0];
	}
    }

    
    returned_tmpvar = function->tmpvar_define(mir.get_types().get_type("u8"));
    auto operation = std::make_unique<OperationLiteralChar>(
	expression.get_source_ref(),
	returned_tmpvar,
	c
	);

    function
	->get_basic_block(current_block)
	.add_operation(std::move(operation));
    return true;
}

bool
FunctionDefinitionResolver::extract_from_expression_primary_literal_string(
    size_t & returned_tmpvar,
    const JLang::frontend::tree::ExpressionPrimaryLiteralString & expression)
{
    // The hardest part here is that we need to extract the escape sequences from
    // the source representation and place the raw data into the operation
    // so that it is working with the actual literal value that will be
    // placed into the machine code during the code-generation stage.

    std::string string_unescaped;
    size_t location;
    bool escape_success = JLang::misc::string_c_unescape(string_unescaped, location, expression.get_value(), false);
    if (!escape_success) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_source_ref(),
		"Invalid String Literal",
		std::string("Unknown escape sequence found at character offset ") + std::to_string(location) + std::string(" in string")
		);
	// We can actually continue processing
	// the file so we can extract more errors later.
	// We've already found an error, so there's no danger
	// or producing an invalid file because the code-generator
	// won't run if there's an error.
    }

    
    
    returned_tmpvar = function->tmpvar_define(mir.get_types().get_type("u8*"));
    auto operation = std::make_unique<OperationLiteralString>(
	expression.get_source_ref(),
	returned_tmpvar,
	string_unescaped
	);
    function
	->get_basic_block(current_block)
	.add_operation(std::move(operation));
    return true;
}

bool
FunctionDefinitionResolver::create_constant_integer_one(
    const JLang::mir::Type *type,
    size_t & returned_tmpvar,
    const JLang::context::SourceReference & _src_ref
)
{
    JLang::frontend::integers::ParseLiteralIntResult parse_result;
    parse_result.parsed_type = type;

    switch (type->get_type()) {
    case Type::TYPE_PRIMITIVE_u8:
	parse_result.u8_value = (unsigned char)1;
	break;
    case Type::TYPE_PRIMITIVE_u16:
	parse_result.u16_value = (unsigned short)1;
	break;
    case Type::TYPE_PRIMITIVE_u32:
	parse_result.u32_value = (unsigned int)1;
	break;
    case Type::TYPE_PRIMITIVE_u64:
	parse_result.u64_value = (unsigned long)1;
	break;
    case Type::TYPE_PRIMITIVE_i8:
	parse_result.i8_value = (char)1;
	break;
    case Type::TYPE_PRIMITIVE_i16:
	parse_result.i16_value = (short)1;
	break;
    case Type::TYPE_PRIMITIVE_i32:
	parse_result.i32_value = (int)1;
	break;
    case Type::TYPE_PRIMITIVE_i64:
	parse_result.i64_value = (long)1;
	break;
    default:
	compiler_context
	    .get_errors()
	    .add_simple_error(
		_src_ref,
		"Compiler Bug! Invalid integer literal",
		std::string("Unsupported primitive literal type creating literal one value") + type->get_name()
		);
	return false;
    }
    return create_constant_integer(parse_result, returned_tmpvar, _src_ref);
}

bool
FunctionDefinitionResolver::create_constant_integer(
    const JLang::frontend::integers::ParseLiteralIntResult & parse_result,
    size_t & returned_tmpvar,
    const JLang::context::SourceReference & _src_ref
    )
{
    const Type *type_part = parse_result.parsed_type;
    returned_tmpvar = function->tmpvar_define(type_part);
    
    JLang::owned<JLang::mir::Operation> operation;
    switch (type_part->get_type()) {
    case Type::TYPE_PRIMITIVE_u8:
    {
	operation = std::make_unique<OperationLiteralInt>(
	    _src_ref,
	    returned_tmpvar,
	    type_part->get_type(),
	    parse_result.u8_value
	    );
    }
    case Type::TYPE_PRIMITIVE_u16:
    {
	operation = std::make_unique<OperationLiteralInt>(
	    _src_ref,
	    returned_tmpvar,
	    type_part->get_type(),
	    parse_result.u16_value
	    );
    }
	break;
    case Type::TYPE_PRIMITIVE_u32:
    {
	operation = std::make_unique<OperationLiteralInt>(
	    _src_ref,
	    returned_tmpvar,
	    type_part->get_type(),
	    parse_result.u32_value
	    );
    }
	break;
    case Type::TYPE_PRIMITIVE_u64:
    {
	operation = std::make_unique<OperationLiteralInt>(
	    _src_ref,
	    returned_tmpvar,
	    type_part->get_type(),
	    parse_result.u64_value
	    );
    }
	break;

    // Signed
    case Type::TYPE_PRIMITIVE_i8:
    {
	operation = std::make_unique<OperationLiteralInt>(
	    _src_ref,
	    returned_tmpvar,
	    type_part->get_type(),
	    parse_result.i8_value
	    );
    }
    case Type::TYPE_PRIMITIVE_i16:
    {
	operation = std::make_unique<OperationLiteralInt>(
	    _src_ref,
	    returned_tmpvar,
	    type_part->get_type(),
	    parse_result.i16_value
	    );
    }
	break;
    case Type::TYPE_PRIMITIVE_i32:
    {
	operation = std::make_unique<OperationLiteralInt>(
	    _src_ref,
	    returned_tmpvar,
	    type_part->get_type(),
	    parse_result.i32_value
	    );
    }
	break;
    case Type::TYPE_PRIMITIVE_i64:
    {
	operation = std::make_unique<OperationLiteralInt>(
	    _src_ref,
	    returned_tmpvar,
	    type_part->get_type(),
	    parse_result.i64_value
	    );
    }
	break;

    default:
	compiler_context
	    .get_errors()
	    .add_simple_error(
		_src_ref,
		"Compiler Bug! Invalid integer literal",
		std::string("Unsupported primitive literal type ") + type_part->get_name()
		);
	return false;
    }
    function
	->get_basic_block(current_block)
	.add_operation(std::move(operation));

    return true;
}

// There is a LOT of logic here for handling various
// kinds of integer literals that might be specified here.
// Of course, the simplest is something like
//     u32 a = 10;
// We also support more interesting
// things like hexidecimal, octal, and binary
// such at:
//     u64 a = 0xfeeda747u64;
//     u16 b = 0o03423;
// We also allow separators for readability, consider:
//     u64 h = 0xfeed_a_747_u64;
//     u16 o = 0o034_23;
//     u16 b = 0b0100_0200_u16;
// Combine that with range-checking all of the
// possible representations, and you have a lot of things
// that can go wrong such as a number being badly formatted
// or with characters outside the radix.
// Condier what happens when the user specifies these:
//     u8 n = 342343u8; // Number too big for u8
//     u8 n = -12u8;    // It's an unsigned, but you put a negative there.
//     u8 n = 0b23334u8;  // It's a binary number, but it's not really binary (and too big also).
// SO many error messages so that the user knows PRECISELY what they did wrong and where.
bool
FunctionDefinitionResolver::extract_from_expression_primary_literal_int(
    size_t & returned_tmpvar,
    const JLang::frontend::tree::ExpressionPrimaryLiteralInt & expression)
{
    JLang::frontend::integers::ParseLiteralIntResult parse_result;
    const Terminal & literal_int_token = expression.get_literal_int_token();
    bool parsed = parse_literal_int(compiler_context, mir.get_types(), literal_int_token, parse_result);
    if (!parsed || parse_result.parsed_type == nullptr) {
	return false;
    }

    return create_constant_integer(
	    parse_result,
	    returned_tmpvar,
	    literal_int_token.get_source_ref());
}

bool
FunctionDefinitionResolver::extract_from_expression_primary_literal_float(
    size_t & returned_tmpvar,
    const JLang::frontend::tree::ExpressionPrimaryLiteralFloat & expression)
{
    std::string literal_type_name = expression.get_type();
    returned_tmpvar = function->tmpvar_define(mir.get_types().get_type(literal_type_name));
    JLang::owned<OperationLiteralFloat> operation;
    char *endptr;
    const char *source_cstring = expression.get_value().c_str();
    size_t length = expression.get_value().size();
    errno = 0;
    if (literal_type_name == "f32") {

	float converted_value = strtof(source_cstring, &endptr);
	if (endptr != (source_cstring + length)) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    expression.get_source_ref(),
		    "Invalid floating-point literal",
		    std::string("Could not correctly parse the literal value.")
		    );
	    return false;
	}
	if (errno == ERANGE) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    expression.get_source_ref(),
		    "Invalid floating-point literal",
		    std::string("Floating-point literal does not fit in the range of an f32.")
		    );
	    return false;
	}
	operation = std::make_unique<OperationLiteralFloat>(
	    expression.get_source_ref(),
	    returned_tmpvar,
	    (float)converted_value
	    );
    }
    else {
	double converted_value = strtod(source_cstring, &endptr);
	if (endptr != (source_cstring + length)) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    expression.get_source_ref(),
		    "Invalid floating-point literal",
		    std::string("Could not correctly parse the literal value.")
		    );
	    return false;
	}
	if (errno == ERANGE) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    expression.get_source_ref(),
		    "Invalid floating-point literal",
		    std::string("Floating-point literal does not fit in the range of an f64.")
		    );
	    return false;
	}
	operation = std::make_unique<OperationLiteralFloat>(
	    expression.get_source_ref(),
	    returned_tmpvar,
	    converted_value
	    );
    }
    function
	->get_basic_block(current_block)
	.add_operation(std::move(operation));
    return true;
}

bool
FunctionDefinitionResolver::extract_from_expression_postfix_array_index(
    size_t & returned_tmpvar,
    const ExpressionPostfixArrayIndex & expression)
{
    size_t array_tmpvar;
    size_t index_tmpvar;
    if (!extract_from_expression(array_tmpvar, expression.get_array())) {
	return false;
    }
    if (!extract_from_expression(index_tmpvar, expression.get_index())) {
	return false;
    }

    const Type *array_type = function->tmpvar_get(array_tmpvar);
    if (!array_type->is_array()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_array().get_source_ref(),
		"Array type must be an array type",
		std::string("Type of array is not an array type.")
		);
	return false;
    }

    const Type *index_type = function->tmpvar_get(index_tmpvar);
    if (index_type->get_type() != Type::TYPE_PRIMITIVE_u32) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_index().get_source_ref(),
		"Array index must be an unsigned 32-bit (u32) type",
		std::string("Type of index is not a u32 index")
		);
	return false;
    }
    
    returned_tmpvar = function->tmpvar_define(array_type->get_pointer_target());
    auto operation = std::make_unique<OperationArrayIndex>(
	expression.get_source_ref(),
	returned_tmpvar,
	array_tmpvar,
	index_tmpvar
	);
    function
	->get_basic_block(current_block)
	.add_operation(std::move(operation));

    return true;
}

bool
FunctionDefinitionResolver::extract_from_expression_postfix_function_call(
    size_t & returned_tmpvar,
    const ExpressionPostfixFunctionCall & expression)
{
    // Extract the expression itself from the arguments.
    size_t function_type_tmpvar;
    if (!extract_from_expression(function_type_tmpvar, expression.get_function())) {
	fprintf(stderr, "Not extracting function early return\n");
	return false;
    }
    
    std::vector<size_t> arg_types;
    for (const auto & arg_expr : expression.get_arguments().get_arguments()) {
	size_t arg_returned_value;
	if (!extract_from_expression(arg_returned_value, *arg_expr)) {
	    fprintf(stderr, "Not extracting function arg expression\n");
	    return false;
	}
	arg_types.push_back(arg_returned_value);
    }

    const Type *function_pointer_type = function->tmpvar_get(function_type_tmpvar);
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
    returned_tmpvar = function->tmpvar_define(function_pointer_type->get_return_type());
    
    auto operation = std::make_unique<OperationFunctionCall>(
	expression.get_source_ref(),
	returned_tmpvar,
	function_type_tmpvar,
	arg_types
	);
    
    function
	->get_basic_block(current_block)
	.add_operation(std::move(operation));

    return true;
}

bool
FunctionDefinitionResolver::extract_from_expression_postfix_dot(
    size_t & returned_tmpvar,
    const ExpressionPostfixDot & expression)
{
    size_t class_tmpvar;
    if (!extract_from_expression(class_tmpvar, expression.get_expression())) {
	return false;
    }

    const Type *class_type = function->tmpvar_get(class_tmpvar);
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

    returned_tmpvar = function->tmpvar_define(member->get_type());
    auto operation = std::make_unique<OperationDot>(
	expression.get_source_ref(),
	returned_tmpvar,
	class_tmpvar,
	member_name
	);
    function
	->get_basic_block(current_block)
	.add_operation(std::move(operation));
    return true;
}

bool
FunctionDefinitionResolver::extract_from_expression_postfix_arrow(
    size_t & returned_tmpvar,
    const ExpressionPostfixArrow & expression)
{
    size_t classptr_tmpvar;
    if (!extract_from_expression(classptr_tmpvar, expression.get_expression())) {
	return false;
    }

    const Type *classptr_type = function->tmpvar_get(classptr_tmpvar);
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

    // First takes 'operand' as a pointer
    // and de-references it into an lvalue.
    size_t class_reference_tmpvar = function->tmpvar_define(class_type);
    auto dereference_operation = std::make_unique<OperationUnary>(
	    Operation::OP_DEREFERENCE,
	    expression.get_source_ref(),
	    class_reference_tmpvar,
	    classptr_tmpvar
	    );
	function
	    ->get_basic_block(current_block)
	    .add_operation(std::move(dereference_operation));
    
    const std::string & member_name = expression.get_identifier();
    const TypeMember *member = class_type->member_get(member_name);
    
    returned_tmpvar = function->tmpvar_define(member->get_type());
    auto dot_operation = std::make_unique<OperationDot>(
	expression.get_source_ref(),
	returned_tmpvar,
	class_reference_tmpvar,
	member_name
	);
    function
	->get_basic_block(current_block)
	.add_operation(std::move(dot_operation));
    return true;

}

bool
FunctionDefinitionResolver::extract_from_expression_postfix_incdec(
    size_t & returned_tmpvar,
    const ExpressionPostfixIncDec & expression)
{
    size_t operand_tmpvar;
    if (!extract_from_expression(operand_tmpvar, expression.get_expression())) {
	return false;
    }

    return create_incdec_operation(
	expression.get_source_ref(),
	returned_tmpvar,
	operand_tmpvar,
	(expression.get_type() == ExpressionPostfixIncDec::INCREMENT), // is_increment
	true // is_postfix
	);
}

bool
FunctionDefinitionResolver::create_incdec_operation(
    const JLang::context::SourceReference & src_ref,
    size_t & returned_tmpvar,
    const size_t & operand_tmpvar,
    bool is_increment,
    bool is_postfix
    )
{
    // This should be implemented as:
    //         _1 = load(variable)
    //         _2 = constant(1)
    //         _3 = add/sub _1 _2 <==== Add or subtract depending on is_increment
    //              store(_3, variable)
    //         _4 = <==== This will be either _1 or _3 depending on is_postfix.
    //
    
    const Type *operand_type = function->tmpvar_get(operand_tmpvar);
    size_t constant_one_tmpvar;
    if (!create_constant_integer_one(
	    operand_type,
	    constant_one_tmpvar,
	    src_ref
	    )) {
	return false;
    }
    
    
    size_t addresult_tmpvar = function->tmpvar_duplicate(operand_tmpvar);
    if (is_increment) {
	auto operation = std::make_unique<OperationBinary>(
	    Operation::OP_ADD,
	    src_ref,
	    addresult_tmpvar,
	    operand_tmpvar,
	    constant_one_tmpvar
	    );
	function
	    ->get_basic_block(current_block)
	    .add_operation(std::move(operation));
    }
    else {
	auto operation = std::make_unique<OperationBinary>(
	    Operation::OP_SUBTRACT,
	    src_ref,
	    addresult_tmpvar,
	    operand_tmpvar,
	    constant_one_tmpvar
	    );
	function
	    ->get_basic_block(current_block)
	    .add_operation(std::move(operation));
    }

    // We perform a 'store' to store
    // the value back into the variable.
    size_t ignore_tmpvar = function->tmpvar_duplicate(operand_tmpvar);
    auto operation_store = std::make_unique<OperationBinary>(
	Operation::OP_ASSIGN,
	src_ref,
	ignore_tmpvar,
	operand_tmpvar,
	addresult_tmpvar
	);
    function
	->get_basic_block(current_block)
	.add_operation(std::move(operation_store));
    
    // This is a post-decrement, so we return
    // the value as it was before we incremented
    // it.
    returned_tmpvar = is_postfix ? operand_tmpvar : addresult_tmpvar;
    return true;
}

bool
FunctionDefinitionResolver::extract_from_expression_unary_prefix(
    size_t & returned_tmpvar,
    const ExpressionUnaryPrefix & expression)
{

    // Extract the prior expression
    // and if not otherwise specified,
    // the operation will return the
    // same type as the operand.
    size_t operand_tmpvar;
    if (!extract_from_expression(
	    operand_tmpvar,
	    expression.get_expression()
	    )) {
	return false;
    }
    
    const Type *operand_type = function->tmpvar_get(operand_tmpvar);
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
	    return create_incdec_operation(
		expression.get_source_ref(),
		returned_tmpvar,
		operand_tmpvar,
		true, // is_increment
		false // is_postfix
		);
        }
        break;
    case ExpressionUnaryPrefix::DECREMENT:
        {
	    return create_incdec_operation(
		expression.get_source_ref(),
		returned_tmpvar,
		operand_tmpvar,
		false, // is_increment
		false // is_postfix
		);
        }
        break;
    case ExpressionUnaryPrefix::ADDRESSOF:
        {
	const Type * pointer_to_operand_type = mir.get_types().get_pointer_to(operand_type, expression.get_source_ref());
	returned_tmpvar = function->tmpvar_define(pointer_to_operand_type);
	auto operation = std::make_unique<OperationUnary>(
	    Operation::OP_ADDRESSOF,
	    expression.get_source_ref(),
	    returned_tmpvar,
	    operand_tmpvar
	    );
	function
	    ->get_basic_block(current_block)
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
	returned_tmpvar = function->tmpvar_define(operand_type->get_pointer_target());
	auto operation = std::make_unique<OperationUnary>(
	    Operation::OP_DEREFERENCE,
	    expression.get_source_ref(),
	    returned_tmpvar,
	    operand_tmpvar
	    );
	function
	    ->get_basic_block(current_block)
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
	returned_tmpvar = function->tmpvar_duplicate(operand_tmpvar);
	auto operation = std::make_unique<OperationUnary>(
	    Operation::OP_NEGATE,
	    expression.get_source_ref(),	    
	    returned_tmpvar,
	    operand_tmpvar
	    );
	function
	    ->get_basic_block(current_block)
	    .add_operation(std::move(operation));
        }
        break;
    case ExpressionUnaryPrefix::BITWISE_NOT:
        {
	returned_tmpvar = function->tmpvar_duplicate(operand_tmpvar);
	auto operation = std::make_unique<OperationUnary>(
	    Operation::OP_BITWISE_NOT,
	    expression.get_source_ref(),	    
	    returned_tmpvar,
	    operand_tmpvar
	    );
	function
	    ->get_basic_block(current_block)
	    .add_operation(std::move(operation));
        }
        break;
    case ExpressionUnaryPrefix::LOGICAL_NOT:
        {
	if (!function->tmpvar_get(operand_tmpvar)->is_bool()) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    expression.get_expression().get_source_ref(),
		    "Logical not (!) must operate on 'bool' expressions.",
		    std::string("Type of condition expression should be 'bool' and was ") + function->tmpvar_get(operand_tmpvar)->get_name()
		    );
	}
	returned_tmpvar = function->tmpvar_duplicate(operand_tmpvar);
	auto operation = std::make_unique<OperationUnary>(
	    Operation::OP_LOGICAL_NOT,
	    expression.get_source_ref(),	    
	    returned_tmpvar,
	    operand_tmpvar
	    );
	function
	    ->get_basic_block(current_block)
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
    size_t & returned_tmpvar,
    const ExpressionUnarySizeofType & expression)
{
    const Type * operand_type = type_resolver.extract_from_type_specifier(expression.get_type_specifier());
    const Type * u64_type = mir.get_types().get_type("u64");
    returned_tmpvar = function->tmpvar_define(u64_type);
    auto operation = std::make_unique<OperationSizeofType>(
	expression.get_source_ref(),	    
	returned_tmpvar,
	operand_type
	);
    function
	->get_basic_block(current_block)
	.add_operation(std::move(operation));
    return true;
}

bool
FunctionDefinitionResolver::numeric_widen(
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
    
    size_t widened_var = function->tmpvar_define(widen_to);
    auto operation = std::make_unique<OperationCast>(
	widen_type,
	_src_ref,
	widened_var,
	_widen_var,
	widen_to
	);
    function
	->get_basic_block(current_block)
	.add_operation(std::move(operation));
    _widen_var = widened_var;
    return true;
}
bool
FunctionDefinitionResolver::numeric_widen_binary_operation(
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
		if (!numeric_widen(_src_ref, b_tmpvar, atype)) {
		    return false;
		}
		*widened = atype;
	    }
	    else if (atype->get_primitive_size() < btype->get_primitive_size()) {
		if (!numeric_widen(_src_ref, a_tmpvar, btype)) {
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
		if (!numeric_widen(_src_ref, b_tmpvar, atype)) {
		    return false;
		}
		*widened = atype;
	    }
	    else if (atype->get_primitive_size() < btype->get_primitive_size()) {
		if (!numeric_widen(_src_ref, a_tmpvar, btype)) {
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
	    if (!numeric_widen(_src_ref, b_tmpvar, atype)) {
		return false;
	    }
	    *widened = atype;
	}
	else if (atype->get_primitive_size() < btype->get_primitive_size()) {
	    if (!numeric_widen(_src_ref, a_tmpvar, btype)) {
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
    const JLang::context::SourceReference & _src_ref,
    Operation::OperationType type,
    size_t & returned_tmpvar,
    size_t a_tmpvar,
    size_t b_tmpvar
    )
{
    const Type *atype = function->tmpvar_get(a_tmpvar);
    const Type *btype = function->tmpvar_get(b_tmpvar);
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
    if (!numeric_widen_binary_operation(_src_ref, a_tmpvar, b_tmpvar, atype, btype, &widened)) {
	return false;
    }

    // The return type is whatever
    // we widened the add to be.
    returned_tmpvar = function->tmpvar_define(widened);
    
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
	->get_basic_block(current_block)
	.add_operation(std::move(operation));
    
    return true;
}

bool
FunctionDefinitionResolver::handle_binary_operation_logical(
    const JLang::context::SourceReference & _src_ref,
    Operation::OperationType type,
    size_t & returned_tmpvar,
    size_t a_tmpvar,
    size_t b_tmpvar
    )
{
    const Type *atype = function->tmpvar_get(a_tmpvar);
    const Type *btype = function->tmpvar_get(b_tmpvar);
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
	->get_basic_block(current_block)
	.add_operation(std::move(operation));
    return true;
}

bool
FunctionDefinitionResolver::handle_binary_operation_bitwise(
    const JLang::context::SourceReference & _src_ref,
    Operation::OperationType type,
    size_t & returned_tmpvar,
    size_t a_tmpvar,
    size_t b_tmpvar
    )
{
    const Type *atype = function->tmpvar_get(a_tmpvar);
    const Type *btype = function->tmpvar_get(b_tmpvar);
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
    if (!numeric_widen_binary_operation(_src_ref, a_tmpvar, b_tmpvar, atype, btype, &widened)) {
	return false;
    }

    // The return type is whatever
    // we widened the add to be.
    returned_tmpvar = function->tmpvar_define(widened);
    
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
	->get_basic_block(current_block)
	.add_operation(std::move(operation));
    
    return true;
}


bool
FunctionDefinitionResolver::handle_binary_operation_shift(
    const JLang::context::SourceReference & _src_ref,
    Operation::OperationType type,
    size_t & returned_tmpvar,
    size_t a_tmpvar,
    size_t b_tmpvar
    )
{
    const Type *atype = function->tmpvar_get(a_tmpvar);
    const Type *btype = function->tmpvar_get(b_tmpvar);
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
    returned_tmpvar = function->tmpvar_define(atype);
    
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
	->get_basic_block(current_block)
	.add_operation(std::move(operation));
    
    return true;
}

bool
FunctionDefinitionResolver::handle_binary_operation_compare(
    const JLang::context::SourceReference & _src_ref,
    Operation::OperationType type,
    size_t & returned_tmpvar,
    size_t a_tmpvar,
    size_t b_tmpvar
    )
{
    const Type *atype = function->tmpvar_get(a_tmpvar);
    const Type *btype = function->tmpvar_get(b_tmpvar);
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
	!(type == Operation::OP_COMPARE_EQUAL ||
	  type == Operation::OP_COMPARE_NOT_EQUAL)
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
    returned_tmpvar = function->tmpvar_define(mir.get_types().get_type("bool"));
    
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
	->get_basic_block(current_block)
	.add_operation(std::move(operation));
    
    return true;
}

bool
FunctionDefinitionResolver::handle_binary_operation_assignment(
    const JLang::context::SourceReference & _src_ref,
    Operation::OperationType type,
    size_t & returned_tmpvar,
    size_t a_tmpvar,
    size_t b_tmpvar
    )
{
    const Type *atype = function->tmpvar_get(a_tmpvar);
    const Type *btype = function->tmpvar_get(b_tmpvar);
    // Check that both operands are the same type.
    if (atype->get_name() != btype->get_name()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		_src_ref,
		"Type mismatch in assignment operation",
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
    returned_tmpvar = function->tmpvar_define(atype);
    
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
	->get_basic_block(current_block)
	.add_operation(std::move(operation));
    
    return true;
}

bool
FunctionDefinitionResolver::extract_from_expression_binary(
    size_t & returned_tmpvar,
    const ExpressionBinary & expression)
{
    size_t a_tmpvar;
    size_t b_tmpvar;
    
    if (!extract_from_expression(a_tmpvar, expression.get_a())) {
	return false;
    }
    if (!extract_from_expression(b_tmpvar, expression.get_b())) {
	return false;
    }

    ExpressionBinary::OperationType op_type;
    op_type = expression.get_operator();

    if (op_type == ExpressionBinary::ADD) {
	if (!handle_binary_operation_arithmetic(
	    expression.get_source_ref(),
	    Operation::OP_ADD,
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar)) {
	    return false;
	}
    }	
    else if (op_type == ExpressionBinary::SUBTRACT) {
	if (!handle_binary_operation_arithmetic(
	    expression.get_source_ref(),
	    Operation::OP_SUBTRACT,
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::MULTIPLY) {
	if (!handle_binary_operation_arithmetic(
	    expression.get_source_ref(),
	    Operation::OP_MULTIPLY,
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::DIVIDE) {
	if (!handle_binary_operation_arithmetic(
	    expression.get_source_ref(),
	    Operation::OP_DIVIDE,
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::MODULO) {
	if (!handle_binary_operation_arithmetic(
	    expression.get_source_ref(),
	    Operation::OP_MODULO,
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::LOGICAL_AND) {
	if (!handle_binary_operation_logical(
	    expression.get_source_ref(),
	    Operation::OP_LOGICAL_AND,
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::LOGICAL_OR) {
	if (!handle_binary_operation_logical(
	    expression.get_source_ref(),
	    Operation::OP_LOGICAL_OR,
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::BITWISE_AND) {
	if (!handle_binary_operation_bitwise(
	    expression.get_source_ref(),
	    Operation::OP_BITWISE_AND,
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::BITWISE_OR) {
	if (!handle_binary_operation_bitwise(
	    expression.get_source_ref(),
	    Operation::OP_BITWISE_OR,
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::BITWISE_XOR) {
	if (!handle_binary_operation_bitwise(
	    expression.get_source_ref(),
	    Operation::OP_BITWISE_XOR,
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::SHIFT_LEFT) {
	if (!handle_binary_operation_shift(
	    expression.get_source_ref(),
	    Operation::OP_SHIFT_LEFT,
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::SHIFT_RIGHT) {
	if (!handle_binary_operation_shift(
	    expression.get_source_ref(),
	    Operation::OP_SHIFT_RIGHT,
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::COMPARE_LESS) {
	if (!handle_binary_operation_compare(
		expression.get_source_ref(),
		Operation::OP_COMPARE_LESS,
		returned_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::COMPARE_GREATER) {
	if (!handle_binary_operation_compare(
		expression.get_source_ref(),
		Operation::OP_COMPARE_GREATER,
		returned_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::COMPARE_LESS_EQUAL) {
	if (!handle_binary_operation_compare(
		expression.get_source_ref(),
		Operation::OP_COMPARE_LESS_EQUAL,
		returned_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::COMPARE_GREATER_EQUAL) {
	if (!handle_binary_operation_compare(
		expression.get_source_ref(),
		Operation::OP_COMPARE_GREATER_EQUAL,
		returned_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::COMPARE_EQUAL) {
	if (!handle_binary_operation_compare(
		expression.get_source_ref(),
		Operation::OP_COMPARE_EQUAL,
		returned_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::COMPARE_NOT_EQUAL) {
	if (!handle_binary_operation_compare(
		expression.get_source_ref(),
		Operation::OP_COMPARE_NOT_EQUAL,
		returned_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::ASSIGNMENT) {
	if (!handle_binary_operation_assignment(
		expression.get_source_ref(),
		Operation::OP_ASSIGN,
		returned_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
    }	
    else if (op_type == ExpressionBinary::MUL_ASSIGNMENT) {
	// This is just syntax sugar for * followed by =
	size_t arithmetic_tmpvar;
	if (!handle_binary_operation_arithmetic(
		expression.get_source_ref(),
		Operation::OP_MULTIPLY,
		arithmetic_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
	if (!handle_binary_operation_assignment(
		expression.get_source_ref(),
		Operation::OP_ASSIGN,
		returned_tmpvar,
		a_tmpvar,
		arithmetic_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::DIV_ASSIGNMENT) {
	// This is just syntax sugar for / followed by =
	size_t arithmetic_tmpvar;
	if (!handle_binary_operation_arithmetic(
		expression.get_source_ref(),
		Operation::OP_DIVIDE,
		arithmetic_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
	if (!handle_binary_operation_assignment(
		expression.get_source_ref(),
		Operation::OP_ASSIGN,
		returned_tmpvar,
		a_tmpvar,
		arithmetic_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::MOD_ASSIGNMENT) {
	// This is just syntax sugar for % followed by =
	size_t arithmetic_tmpvar;
	if (!handle_binary_operation_arithmetic(
		expression.get_source_ref(),
		Operation::OP_MODULO,
		arithmetic_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
	if (!handle_binary_operation_assignment(
		expression.get_source_ref(),
		Operation::OP_ASSIGN,
		returned_tmpvar,
		a_tmpvar,
		arithmetic_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::ADD_ASSIGNMENT) {
	// This is just syntax sugar for + followed by =
	size_t arithmetic_tmpvar;
	if (!handle_binary_operation_arithmetic(
		expression.get_source_ref(),
		Operation::OP_ADD,
		arithmetic_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
	if (!handle_binary_operation_assignment(
		expression.get_source_ref(),
		Operation::OP_ASSIGN,
		returned_tmpvar,
		a_tmpvar,
		arithmetic_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::SUB_ASSIGNMENT) {
	// This is just syntax sugar for - followed by =
	size_t arithmetic_tmpvar;
	if (!handle_binary_operation_arithmetic(
		expression.get_source_ref(),
		Operation::OP_SUBTRACT,
		arithmetic_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
	if (!handle_binary_operation_assignment(
		expression.get_source_ref(),
		Operation::OP_ASSIGN,
		returned_tmpvar,
		a_tmpvar,
		arithmetic_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::LEFT_ASSIGNMENT) {
	// This is just syntax sugar for << followed by =
	size_t arithmetic_tmpvar;
	if (!handle_binary_operation_shift(
		expression.get_source_ref(),
		Operation::OP_SHIFT_LEFT,
		arithmetic_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
	if (!handle_binary_operation_assignment(
		expression.get_source_ref(),
		Operation::OP_ASSIGN,
		returned_tmpvar,
		a_tmpvar,
		arithmetic_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::RIGHT_ASSIGNMENT) {
	// This is just syntax sugar for >> followed by =
	size_t arithmetic_tmpvar;
	if (!handle_binary_operation_shift(
		expression.get_source_ref(),
		Operation::OP_SHIFT_RIGHT,
		arithmetic_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
	if (!handle_binary_operation_assignment(
		expression.get_source_ref(),
		Operation::OP_ASSIGN,
		returned_tmpvar,
		a_tmpvar,
		arithmetic_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::AND_ASSIGNMENT) {
	// This is just syntax sugar for & followed by =
	size_t arithmetic_tmpvar;
	if (!handle_binary_operation_bitwise(
		expression.get_source_ref(),
		Operation::OP_BITWISE_AND,
		arithmetic_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
	if (!handle_binary_operation_assignment(
		expression.get_source_ref(),
		Operation::OP_ASSIGN,
		returned_tmpvar,
		a_tmpvar,
		arithmetic_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::OR_ASSIGNMENT) {
	// This is just syntax sugar for | followed by =
	size_t arithmetic_tmpvar;
	if (!handle_binary_operation_bitwise(
		expression.get_source_ref(),
		Operation::OP_BITWISE_OR,
		arithmetic_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
	if (!handle_binary_operation_assignment(
		expression.get_source_ref(),
		Operation::OP_ASSIGN,
		returned_tmpvar,
		a_tmpvar,
		arithmetic_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::XOR_ASSIGNMENT) {
	// This is just syntax sugar for ^ followed by =
	size_t arithmetic_tmpvar;
	if (!handle_binary_operation_bitwise(
		expression.get_source_ref(),
		Operation::OP_BITWISE_XOR,
		arithmetic_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
	if (!handle_binary_operation_assignment(
		expression.get_source_ref(),
		Operation::OP_ASSIGN,
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
    size_t & returned_tmpvar,
    const ExpressionTrinary & expression)
{
//    function
//	->get_basic_block(current_block)
//	.add_operation(std::string("trinary operator "));
    return false;
}
bool
FunctionDefinitionResolver::extract_from_expression_cast(
    size_t & returned_tmpvar,
    const ExpressionCast & expression)
{
//    function
//	->get_basic_block(current_block)
//	.add_operation(std::string("cast"));
    return false;
}


bool
FunctionDefinitionResolver::extract_from_expression(
    size_t & returned_tmpvar,
    const Expression & expression_container)
{
  const auto & expression_type = expression_container.get_expression();

  if (std::holds_alternative<JLang::owned<ExpressionPrimaryIdentifier>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPrimaryIdentifier>>(expression_type);
    return extract_from_expression_primary_identifier(returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPrimaryNested>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPrimaryNested>>(expression_type);
    return extract_from_expression_primary_nested(returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPrimaryLiteralChar>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPrimaryLiteralChar>>(expression_type);
    return extract_from_expression_primary_literal_char(returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPrimaryLiteralString>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPrimaryLiteralString>>(expression_type);
    return extract_from_expression_primary_literal_string(returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPrimaryLiteralInt>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPrimaryLiteralInt>>(expression_type);
    return extract_from_expression_primary_literal_int(returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPrimaryLiteralFloat>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPrimaryLiteralFloat>>(expression_type);
    return extract_from_expression_primary_literal_float(returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPostfixArrayIndex>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPostfixArrayIndex>>(expression_type);
    return extract_from_expression_postfix_array_index(returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPostfixFunctionCall>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPostfixFunctionCall>>(expression_type);
    return extract_from_expression_postfix_function_call(returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPostfixDot>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPostfixDot>>(expression_type);
    return extract_from_expression_postfix_dot(returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPostfixArrow>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPostfixArrow>>(expression_type);
    return extract_from_expression_postfix_arrow(returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPostfixIncDec>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPostfixIncDec>>(expression_type);
    return extract_from_expression_postfix_incdec(returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionUnaryPrefix>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionUnaryPrefix>>(expression_type);
    return extract_from_expression_unary_prefix(returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionUnarySizeofType>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionUnarySizeofType>>(expression_type);
    return extract_from_expression_unary_sizeof_type(returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionBinary>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionBinary>>(expression_type);
    return extract_from_expression_binary(returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionTrinary>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionTrinary>>(expression_type);
    return extract_from_expression_trinary(returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionCast>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionCast>>(expression_type);
    return extract_from_expression_cast(returned_tmpvar, *expression);
  }
  else {
    fprintf(stderr, "Compiler bug, invalid expression type\n");
    return false;
  }
}

bool
FunctionDefinitionResolver::local_declare_or_error(
    const JLang::mir::Type *mir_type,
    const std::string & name,
    const SourceReference & source_ref
    )
{
    const JLang::frontend::LocalVariable *maybe_existing = scope_tracker.get_variable(name);
	
    if (maybe_existing != nullptr) {
	std::unique_ptr<JLang::context::Error> error = std::make_unique<JLang::context::Error>("Duplicate Local Variable.");
	error->add_message(source_ref,
			   std::string("Variable with name ") + name + " is already in scope and cannot be duplicated.");
	error->add_message(maybe_existing->get_source_ref(),
			   "First declared here.");
	
	compiler_context
	    .get_errors()
	    .add_error(std::move(error));
	return false;
    }
    scope_tracker.add_variable(name, mir_type, source_ref);
    
    auto operation = std::make_unique<OperationLocalDeclare>(
	source_ref,
	name,
	mir_type
	);
    function->get_basic_block(current_block).add_operation(std::move(operation));

    return true;
}

bool
FunctionDefinitionResolver::extract_from_statement_variable_declaration(
    const StatementVariableDeclaration & statement
    )
{
    // TODO:
    // For arrays, it is at this point that we define a type
    // for the 'array' so we can make it the correct size based on the literal given
    // for the 'opt_array' stuff.
    
    // Once the variable exists, we can start performing the initialization
    // and assigning the value to something.
    const JLang::mir::Type * mir_type = type_resolver.extract_from_type_specifier(statement.get_type_specifier());
    
    if (!local_declare_or_error(
	    mir_type,
	    statement.get_name(),
	    statement.get_name_source_ref()
	    )) {
	return false;
    }
    
    const auto & initializer_expression = statement.get_initializer_expression();
    if (initializer_expression.has_expression()) {
	// From here, we need to:
	// 1) call LocalVariable
	// 2) Evaluate the expression
	// 3) Perform an assignment.
	size_t variable_tmpvar = function->tmpvar_define(mir_type);
	auto operation = std::make_unique<OperationLocalVariable>(
	    statement.get_source_ref(),
	    variable_tmpvar,
	    statement.get_name(),
	    mir_type
	    );
	function->get_basic_block(current_block).add_operation(std::move(operation));

	size_t initial_value_tmpvar;
	if (!extract_from_expression(initial_value_tmpvar, initializer_expression.get_expression())) {
	    return false;
	}

	size_t returned_tmpvar; // We don't save the returned val because nobody wants it.
	if (!handle_binary_operation_assignment(
		initializer_expression.get_source_ref(),
		Operation::OP_ASSIGN,
		returned_tmpvar,
		variable_tmpvar,
		initial_value_tmpvar
		)) {
	    return false;
	}
    }
    else {
	// TODO: In order to avoid undefined behavior, we should
	// always make sure that the variable has an initial default
	// value even if there isn't one provided.  We should look
	// at the type system and ask it for a 'default' value
	// for the type.
	// In many cases, this would be the constructor if it
	// is available for that type.
    }

    
    return true;
}

bool
FunctionDefinitionResolver::extract_from_statement_ifelse(
    std::map<std::string, FunctionLabel> & labels,
    const StatementIfElse & statement
    )
{
    size_t condition_tmpvar;
    if (!extract_from_expression(condition_tmpvar, statement.get_expression())) {
	return false;
    }
    // First, evaluate the expression to get our condition.
    if (!function->tmpvar_get(condition_tmpvar)->is_bool()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		statement.get_expression().get_source_ref(),
		"Invalid condition in if statement.",
		std::string("Type of condition expression should be 'bool' and was ") + function->tmpvar_get(condition_tmpvar)->get_name()
		);
	return false;
    }
    
    size_t blockid_if = function->add_block();
    size_t blockid_done = function->add_block();
    size_t blockid_else = -1;
    if (statement.has_else() || statement.has_else_if()) {
	// If we have an 'else', then
	// dump to it based on the condition.
	blockid_else = function->add_block();
	auto operation = std::make_unique<OperationJumpConditional>(
	    statement.get_source_ref(),
	    condition_tmpvar,
	    blockid_if,
	    blockid_else
	    );
	function->get_basic_block(current_block).add_operation(std::move(operation));
    }
    else {
	// Otherwise, jump to done
	// based on condition.
	auto operation = std::make_unique<OperationJumpConditional>(
	    statement.get_source_ref(),
	    condition_tmpvar,
	    blockid_if,
	    blockid_done
	    );
	function->get_basic_block(current_block).add_operation(std::move(operation));
    }
    
    current_block = blockid_if;

    // Perform the stuff inside the 'if' block.
    scope_tracker.scope_push();
    if (!extract_from_statement_list(
	labels,
	statement.get_if_scope_body().get_statements()
	    )) {
	return false;
    }
    scope_tracker.scope_pop();
    
    // Unconditionally jump to done
    // unless the block is alreayd terminated by
    // another jump or a return.
    if (!function->get_basic_block(current_block).contains_terminator()) {
	auto operation = std::make_unique<OperationJump>(
	    statement.get_source_ref(),
	    blockid_done
	    );
	function->get_basic_block(current_block).add_operation(std::move(operation));
    }
    
    if (statement.has_else()) {
	// Perform the stuff in the 'else' block.
	scope_tracker.scope_push();
	size_t blockid_tmp = current_block;
	current_block = blockid_else;
	if (!extract_from_statement_list(
	    labels,
	    statement.get_else_scope_body().get_statements()
		)) {
	    return false;
	}
	current_block = blockid_tmp;
	scope_tracker.scope_pop();
	// Jump to the 'done' block when the 'else' block is finished
	// unless it has terminated already.
	if (!function->get_basic_block(blockid_else).contains_terminator()) {
	    auto operation = std::make_unique<OperationJump>(
		statement.get_source_ref(),
		blockid_done
		);
	    function->get_basic_block(blockid_else).add_operation(std::move(operation));
	}
    }
    else if (statement.has_else_if()) {
	if (!extract_from_statement_ifelse(
	    labels,
	    statement.get_else_if()
		)) {
	    return false;
	}
    }
    current_block = blockid_done;
    return true;
}

bool
FunctionDefinitionResolver::extract_from_statement_while(
    std::map<std::string, JLang::mir::FunctionLabel> & labels,
    const JLang::frontend::tree::StatementWhile & statement
    )
{
    size_t condition_tmpvar;

    size_t blockid_evaluate_expression = function->add_block();
    size_t blockid_if = function->add_block();
    size_t blockid_done = function->add_block();

    auto operation_jump_initial = std::make_unique<OperationJump>(
	statement.get_source_ref(),
	blockid_evaluate_expression
	);
    function->get_basic_block(current_block).add_operation(std::move(operation_jump_initial));

    size_t blockid_tmp = current_block;
    current_block = blockid_evaluate_expression;
    
    if (!extract_from_expression(condition_tmpvar, statement.get_expression())) {
	return false;
    }
    current_block = blockid_tmp;

    auto operation_jump_conditional = std::make_unique<OperationJumpConditional>(
	statement.get_source_ref(),
	condition_tmpvar,
	blockid_if,
	blockid_done
	);
    function->get_basic_block(blockid_evaluate_expression).add_operation(std::move(operation_jump_conditional));

    // Push a loop scope.
    scope_tracker.scope_push_loop(blockid_done, blockid_evaluate_expression);
    size_t blockid_tmp_if = current_block;
    current_block = blockid_if;
    if (!extract_from_statement_list(
	labels,
	statement.get_scope_body().get_statements()
	    )) {
	return false;
    }
    current_block = blockid_tmp_if;
    
    // Pop back from the scope.
    scope_tracker.scope_pop();
    
    auto operation_jump_to_evaluate = std::make_unique<OperationJump>(
	statement.get_source_ref(),
	blockid_evaluate_expression
	);
    function->get_basic_block(blockid_if).add_operation(std::move(operation_jump_to_evaluate));
    
    current_block = blockid_done;
    
    return true;
}

bool
FunctionDefinitionResolver::extract_from_statement_for(
    std::map<std::string, JLang::mir::FunctionLabel> & labels,
    const JLang::frontend::tree::StatementFor & statement
    )
{
    size_t condition_tmpvar;

    size_t blockid_evaluate_expression_termination = function->add_block();
    size_t blockid_if = function->add_block();
    size_t blockid_done = function->add_block();

    if (statement.is_declaration()) {
	const JLang::mir::Type * mir_type = type_resolver.extract_from_type_specifier(statement.get_type_specifier());
	
	if (!local_declare_or_error(
		mir_type,
		statement.get_identifier(),
		statement.get_identifier_source_ref()
		)) {
	    return false;
	}
    }
    
    // Evaluate the initialization expression
    if (!extract_from_expression(condition_tmpvar, statement.get_expression_initial())) {
	return false;
    }
    
    auto operation_jump_initial = std::make_unique<OperationJump>(
	statement.get_source_ref(),
	blockid_evaluate_expression_termination
	);
    function->get_basic_block(current_block).add_operation(std::move(operation_jump_initial));

    // Evaluate the termination condition.
    size_t blockid_tmp = current_block;
    current_block = blockid_evaluate_expression_termination;
    if (!extract_from_expression(condition_tmpvar, statement.get_expression_termination())) {
	return false;
    }
    current_block = blockid_tmp;

    auto operation_jump_conditional = std::make_unique<OperationJumpConditional>(
	statement.get_source_ref(),
	condition_tmpvar,
	blockid_if,
	blockid_done
	);
    function->get_basic_block(blockid_evaluate_expression_termination).add_operation(std::move(operation_jump_conditional));

    scope_tracker.scope_push_loop(blockid_done, blockid_evaluate_expression_termination);
    
    size_t blockid_tmp_if = current_block;
    current_block = blockid_if;
    if (!extract_from_statement_list(
	labels,
	statement.get_scope_body().get_statements()
	    )) {
	return false;
    }
    
    scope_tracker.scope_pop();

    // Evaluate the 'increment' expression
    if (!extract_from_expression(condition_tmpvar, statement.get_expression_increment())) {
	return false;
    }
    current_block = blockid_tmp_if;
    
    auto operation_jump_to_evaluate = std::make_unique<OperationJump>(
	statement.get_source_ref(),
	blockid_evaluate_expression_termination
	);
    function->get_basic_block(blockid_if).add_operation(std::move(operation_jump_to_evaluate));
    
    current_block = blockid_done;

    // Un-declare the variable we declared.
//    leave_scope(function, current_block, statement.get_source_ref(), unwind);
    
    return true;
}
	


bool
FunctionDefinitionResolver::extract_from_statement_break(
    std::map<std::string, JLang::mir::FunctionLabel> & labels,
    const JLang::frontend::tree::StatementBreak & statement
    )
{
    if (!scope_tracker.is_in_loop()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(statement.get_source_ref(),
			      "'break' statement not in loop or switch statement",
			      "'break' keyword must appear inside a loop (for/while)"
		);
	return true;
    }

    std::vector<std::string> unwind_break = scope_tracker.get_variables_to_unwind_for_break();
    leave_scope(statement.get_source_ref(), unwind_break);
    
    auto operation_jump_to_break = std::make_unique<OperationJump>(
	statement.get_source_ref(),
	scope_tracker.get_loop_break_blockid()
	);
    function->get_basic_block(current_block).add_operation(std::move(operation_jump_to_break));
    current_block = function->add_block();
    
    return true;
}

bool
FunctionDefinitionResolver::extract_from_statement_continue(
    std::map<std::string, JLang::mir::FunctionLabel> & labels,
    const JLang::frontend::tree::StatementContinue & statement
    )
{
    if (!scope_tracker.is_in_loop()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(statement.get_source_ref(),
			      "'continue' statement not in loop or switch statement",
			      "'continue' keyword must appear inside a loop (for/while)"
		);
	return true;
    }
    auto operation_jump_to_continue = std::make_unique<OperationJump>(
	statement.get_source_ref(),
	scope_tracker.get_loop_continue_blockid()
	);
    function->get_basic_block(current_block).add_operation(std::move(operation_jump_to_continue));
    current_block = function->add_block();
    
    return true;
}


bool
FunctionDefinitionResolver::extract_from_statement_label(
    std::map<std::string, FunctionLabel> & labels,
    const JLang::frontend::tree::StatementLabel & statement
    )
{
    // We're starting a new label, so this is, by definition,
    // a new basic block.  This means we need to create a new
    // block and issue a 'jump' to it.

    const std::string & label_name = statement.get_name();
    const auto & it = labels.find(label_name);
    size_t label_block;
    if (it == labels.end()) {
	label_block = function->add_block();
	FunctionLabel label_desc(label_name, label_block);
	label_desc.set_label(statement.get_name_source_ref());
	labels.insert(std::pair(label_name, label_desc));
    }
    // There's a label record.  It may be a 'goto', but
    // if it is an already completed label, then issue an error
    // because it's a duplicate.
    else if (it->second.is_resolved()) { 
	std::unique_ptr<JLang::context::Error> error = std::make_unique<JLang::context::Error>("Labels in functions must be unique");
	error->add_message(statement.get_name_source_ref(),
			   std::string("Duplicate label ") + label_name);
	error->add_message(it->second.get_source_ref(),
			   "First declared here.");
	compiler_context
	    .get_errors()
	    .add_error(std::move(error));
	return true;
    }
    // There's a label and it's not complete, so it must
    // be a goto statement that hasn't seen its target
    // label yet, so we just complete it.
    else {
	it->second.set_label(statement.get_name_source_ref());
	label_block = it->second.get_block();
    }
    
    auto operation = std::make_unique<OperationJump>(
	statement.get_source_ref(),
	label_block
	);
    function->get_basic_block(current_block).add_operation(std::move(operation));
    // Then whatever we add next will be in this new block.
    current_block = label_block;
    
    return true;
}

bool
FunctionDefinitionResolver::extract_from_statement_goto(
    std::map<std::string, FunctionLabel> & labels,
    const JLang::frontend::tree::StatementGoto & statement
    )
{
    const std::string & label_name = statement.get_label();
    const auto & it = labels.find(label_name);

    // Label is not yet found, we need to create it.
    // but we can't resolve it yet because we don't
    // yet know the ID of the target.
    if (it == labels.end()) {
	size_t label_block = function->add_block();
	FunctionLabel label_desc(label_name, label_block);
	labels.insert(std::pair(label_name, label_desc));
    }
    else {
	auto operation = std::make_unique<OperationJump>(
	    statement.get_source_ref(),
	    it->second.get_block()
	    );
	function->get_basic_block(current_block).add_operation(std::move(operation));
	// This jump ends the basic block, so we start a new one.
	size_t next_block = function->add_block();
	current_block = next_block;
    }
    return true;
}
	
bool
FunctionDefinitionResolver::extract_from_statement_return(
    const StatementReturn & statement
    )
{
    size_t expression_tmpvar;
    if (!extract_from_expression(expression_tmpvar, statement.get_expression())) {
	return false;
    }

    std::vector<std::string> unwind_root = scope_tracker.get_variables_to_unwind_for_root();
    leave_scope(statement.get_source_ref(), unwind_root);
    
    auto operation = std::make_unique<OperationReturn>(
	statement.get_source_ref(),
	expression_tmpvar
	);
    function->get_basic_block(current_block).add_operation(std::move(operation));
    return true;
}


void
FunctionDefinitionResolver::leave_scope(
    const SourceReference & src_ref,
    std::vector<std::string> & unwind)
{
    
    for (const auto & undecl : unwind) {
	auto operation = std::make_unique<OperationLocalUndeclare>(
	    src_ref,
	    undecl);
	function->get_basic_block(current_block).add_operation(std::move(operation));
    }
    unwind.clear();
}
	    
bool
FunctionDefinitionResolver::extract_from_statement_list(
    std::map<std::string, FunctionLabel> & labels,
    const StatementList & statement_list)
{
    
    for (const auto & statement_el : statement_list.get_statements()) {
	const auto & statement_type = statement_el->get_statement();
	if (std::holds_alternative<JLang::owned<StatementVariableDeclaration>>(statement_type)) {
	    const auto & statement = std::get<JLang::owned<StatementVariableDeclaration>>(statement_type);
	    if (!extract_from_statement_variable_declaration(*statement)) {
		return false;
	    }
	}
	else if (std::holds_alternative<JLang::owned<StatementBlock>>(statement_type)) {
	    const auto & statement = std::get<JLang::owned<StatementBlock>>(statement_type);
	    scope_tracker.scope_push();
	    if (!extract_from_statement_list(
		    labels,
		    statement->get_scope_body().get_statements())) {
		return false;
	    }
	    scope_tracker.scope_pop();
	}
	else if (std::holds_alternative<JLang::owned<StatementExpression>>(statement_type)) {
	    const auto & statement = std::get<JLang::owned<StatementExpression>>(statement_type);
	    size_t returned_tmpvar;
	    if (!extract_from_expression(
		    returned_tmpvar,
		    statement->get_expression())) {
		return false;
	    }
	}
	else if (std::holds_alternative<JLang::owned<StatementIfElse>>(statement_type)) {
	    const auto & statement = std::get<JLang::owned<StatementIfElse>>(statement_type);
	    if (!extract_from_statement_ifelse(
		    labels,
		    *statement)) {
		return false;
	    }
	}
	else if (std::holds_alternative<JLang::owned<StatementWhile>>(statement_type)) {
	    const auto & statement = std::get<JLang::owned<StatementWhile>>(statement_type);
	    if (!extract_from_statement_while(
		    labels,
		    *statement)) {
		return false;
	    }
	}
	else if (std::holds_alternative<JLang::owned<StatementFor>>(statement_type)) {
	    const auto & statement = std::get<JLang::owned<StatementFor>>(statement_type);
	    if (!extract_from_statement_for(
		    labels,
		    *statement)) {
		return false;
	    }
	}
	else if (std::holds_alternative<JLang::owned<StatementLabel>>(statement_type)) {
	    const auto & statement = std::get<JLang::owned<StatementLabel>>(statement_type);
	    if (!extract_from_statement_label(labels, *statement)) {
		return false;
	    }
	}
	else if (std::holds_alternative<JLang::owned<StatementGoto>>(statement_type)) {
	    const auto & statement = std::get<JLang::owned<StatementGoto>>(statement_type);
	    if (!extract_from_statement_goto(labels, *statement)) {
		return false;
	    }
	}
	else if (std::holds_alternative<JLang::owned<StatementBreak>>(statement_type)) {
	    const auto & statement = std::get<JLang::owned<StatementBreak>>(statement_type);
	    if (!extract_from_statement_break(
		    labels,
		    *statement)) {
		return false;
	    }
	}
	else if (std::holds_alternative<JLang::owned<StatementContinue>>(statement_type)) {
	    const auto & statement = std::get<JLang::owned<StatementContinue>>(statement_type);
	    if (!extract_from_statement_continue(
		    labels,
		    *statement)) {
		return false;
	    }
	}
	else if (std::holds_alternative<JLang::owned<StatementReturn>>(statement_type)) {
	    const auto & statement = std::get<JLang::owned<StatementReturn>>(statement_type);
	    // The return may need to unwind local declarations
	    // and ensure destructors are called.
	    if (!extract_from_statement_return(*statement)) {
		return false;
	    }
	}
	else {
	    fprintf(stderr, "Compiler bug, invalid statement type\n");
	    return false;
	}
    }
    std::vector<std::string> unwind_scope = scope_tracker.get_variables_to_unwind_for_scope();
    leave_scope(statement_list.get_source_ref(), unwind_scope);

    return true;
}

