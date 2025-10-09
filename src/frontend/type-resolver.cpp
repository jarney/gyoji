#include <gyoji-frontend/type-resolver.hpp>
#include <variant>
#include <stdio.h>
#include <gyoji-misc/jstring.hpp>

using namespace Gyoji::mir;
using namespace Gyoji::context;
using namespace Gyoji::frontend::tree;
using namespace Gyoji::frontend::lowering;

TypeResolver::TypeResolver(
    Gyoji::context::CompilerContext & _compiler_context,
    const Gyoji::frontend::tree::TranslationUnit & _translation_unit,
    Gyoji::mir::MIR & _mir)
    : mir(_mir)
    , compiler_context(_compiler_context)
    , translation_unit(_translation_unit)
{}
TypeResolver::~TypeResolver()
{}

void TypeResolver::resolve()
{
    // To resolve the types, we need only iterate the
    // input parse tree and pull out any type declarations,
    // resolving them down to their primitive types.
    extract_types(translation_unit.get_statements());
}

void
TypeResolver::extract_from_class_declaration(const ClassDeclaration & declaration)
{
    Gyoji::owned<Type> type = std::make_unique<Type>(declaration.get_name(), Type::TYPE_COMPOSITE, false, declaration.get_name_source_ref());
    mir.get_types().define_type(std::move(type));
}

Type *
TypeResolver::get_or_create(std::string pointer_name, Type::TypeType type_type, bool complete, const SourceReference & source_ref)
{
    Type *pointer_type = mir.get_types().get_type(pointer_name);
    if (pointer_type != nullptr) {
	return pointer_type;
    }
    else {
	Gyoji::owned<Type> pointer_type_created = std::make_unique<Type>(pointer_name, type_type, complete, source_ref);
	pointer_type = pointer_type_created.get();
	mir.get_types().define_type(std::move(pointer_type_created));
	return pointer_type;
    }
}

const Type*
TypeResolver::extract_from_type_specifier_simple(const TypeSpecifierSimple & type_specifier)
{
    const auto & type_name = type_specifier.get_type_name();
    if (type_name.is_expression()) {
	auto error = std::make_unique<Gyoji::context::Error>("Could not resolve type");
	error->add_message(type_name.get_name_source_ref(), "Specifying types from expressions is not yet supported.");
	compiler_context.get_errors().add_error(std::move(error));
	return nullptr;
    }
    std::string name = type_name.get_name();
    const Type *type = mir.get_types().get_type(name);
    if (type == nullptr) {
	compiler_context
	    .get_errors()
	    .add_simple_error(type_name.get_name_source_ref(),
			      "Could not find type",
			      std::string("Could not resolve type ") + name
		);
	return nullptr;
    }
    return type;
}

const Type*
TypeResolver::extract_from_type_specifier_template(const TypeSpecifierTemplate & type_specifier)
{
    compiler_context
	.get_errors()
	.add_simple_error(type_specifier.get_source_ref(),
			  "Could not find type",
			  "Template types are not supported yet."
	    );
    return nullptr;
}

const Type*
TypeResolver::extract_from_type_specifier_function_pointer(const TypeSpecifierFunctionPointer & type_specifier)
{
    compiler_context
	.get_errors()
	.add_simple_error(type_specifier.get_source_ref(),
			  "Could not find type",
			  "Function pointer types are not supported yet."
	    );
    return nullptr;
}

const Type*
TypeResolver::extract_from_type_specifier_pointer_to(const TypeSpecifierPointerTo & type_specifier)
{
    const Type *pointer_target = extract_from_type_specifier(type_specifier.get_type_specifier());
    if (pointer_target == nullptr) {
	compiler_context
	    .get_errors()
	    .add_simple_error(type_specifier.get_source_ref(),
			      "Could not find type",
			      "Could not resolve target of pointer"
		);
	return nullptr;
    }
    const Type *pointer_type = mir.get_types().get_pointer_to(pointer_target, type_specifier.get_source_ref());
    return pointer_type;
}

const Type*
TypeResolver::extract_from_type_specifier_reference_to(const TypeSpecifierReferenceTo & type_specifier)
{
    const Type *pointer_target = extract_from_type_specifier(type_specifier.get_type_specifier());
    if (pointer_target == nullptr) {
	compiler_context
	    .get_errors()
	    .add_simple_error(type_specifier.get_source_ref(),
			      "Could not find type",
			      "Could not resolve target of reference"
		);
	return nullptr;
    }
    const Type *pointer_type = mir.get_types().get_reference_to(pointer_target, type_specifier.get_source_ref());
    return pointer_type;
}

const Type*
TypeResolver::extract_from_type_specifier_array(const TypeSpecifierArray & type_specifier)
{
    const Type *pointer_target = extract_from_type_specifier(type_specifier.get_type_specifier());
    if (pointer_target == nullptr) {
	compiler_context
	    .get_errors()
	    .add_simple_error(type_specifier.get_literal_int_token().get_source_ref(),
			      "Could not parse type of array elements.",
			      "Array element type could not be parsed."
		);	
	return nullptr;
    }
    
    Gyoji::frontend::integers::ParseLiteralIntResult parse_result;
    bool parsed = parse_literal_int(compiler_context, mir.get_types(), type_specifier.get_literal_int_token(), parse_result);
    if (!parsed || parse_result.parsed_type == nullptr) {
	compiler_context
	    .get_errors()
	    .add_simple_error(type_specifier.get_literal_int_token().get_source_ref(),
			      "Array size invalid",
			      "Could not parse array size."
		);
	return nullptr;
    }
    if (parse_result.parsed_type->get_type() != Type::TYPE_PRIMITIVE_u32) {
	compiler_context
	    .get_errors()
	    .add_simple_error(type_specifier.get_literal_int_token().get_source_ref(),
			      "Array size invalid",
			      "Array size must be an unsigned 32-bit integer (u32) constant.  Sizes may not be computed at runtime."
		);
    }
    
    const Type *array_type = mir.get_types().get_array_of(pointer_target, parse_result.u32_value, type_specifier.get_source_ref());
    return array_type;
}

const Type *
TypeResolver::extract_from_type_specifier(const TypeSpecifier & type_specifier)
{
    const auto & type_specifier_type = type_specifier.get_type();
    if (std::holds_alternative<Gyoji::owned<TypeSpecifierSimple>>(type_specifier_type)) {
	const auto & type_specifier = std::get<Gyoji::owned<TypeSpecifierSimple>>(type_specifier_type);
	return extract_from_type_specifier_simple(*type_specifier);
    }
    else if (std::holds_alternative<Gyoji::owned<TypeSpecifierTemplate>>(type_specifier_type)) {
	const auto & type_specifier = std::get<Gyoji::owned<TypeSpecifierTemplate>>(type_specifier_type);
	return extract_from_type_specifier_template(*type_specifier);
    }
    else if (std::holds_alternative<Gyoji::owned<TypeSpecifierFunctionPointer>>(type_specifier_type)) {
	const auto & type_specifier = std::get<Gyoji::owned<TypeSpecifierFunctionPointer>>(type_specifier_type);
	return extract_from_type_specifier_function_pointer(*type_specifier);
    }
    else if (std::holds_alternative<Gyoji::owned<TypeSpecifierPointerTo>>(type_specifier_type)) {
	const auto & type_specifier = std::get<Gyoji::owned<TypeSpecifierPointerTo>>(type_specifier_type);
	return extract_from_type_specifier_pointer_to(*type_specifier);
    }
    else if (std::holds_alternative<Gyoji::owned<TypeSpecifierReferenceTo>>(type_specifier_type)) {
	const auto & type_specifier = std::get<Gyoji::owned<TypeSpecifierReferenceTo>>(type_specifier_type);
	return extract_from_type_specifier_reference_to(*type_specifier);
	
    }
    else if (std::holds_alternative<Gyoji::owned<TypeSpecifierArray>>(type_specifier_type)) {
	const auto & type_specifier = std::get<Gyoji::owned<TypeSpecifierArray>>(type_specifier_type);
	return extract_from_type_specifier_array(*type_specifier);
	
    }
    
    compiler_context
	.get_errors()
	.add_simple_error(type_specifier.get_source_ref(),
			  "Compiler bug!  Please report this message",
			  "Unknown TypeSpecifier type in variant (compiler bug)"
	    );
    
    return nullptr;
}

void
TypeResolver::extract_from_class_members(Type & type, const ClassDefinition & definition)
{
    std::vector<TypeMember> members;
    
    const auto & class_members = definition.get_members();
    size_t member_id = 0;
    for (const auto & class_member : class_members) {
	const auto & class_member_type = class_member->get_member();
	if (std::holds_alternative<Gyoji::owned<ClassMemberDeclarationVariable>>(class_member_type)) {
	    const auto & member_variable = std::get<Gyoji::owned<ClassMemberDeclarationVariable>>(class_member_type);
	    
	    const Type *member_type = extract_from_type_specifier(member_variable->get_type_specifier());
	    if (member_type == nullptr) {
		compiler_context
		    .get_errors()
		    .add_simple_error(member_variable->get_type_specifier().get_source_ref(),
				      "Could not find type",
				      "Could not extract type of member variable " + member_variable->get_name()
			);
	    }
	    else {
		// TODO: There must be a good way to ensure
		// that the names are unique.  This would allow two
		// fields with the same name which is clearly bad.
		TypeMember add_member(member_variable->get_name(), member_id, member_type, class_member->get_source_ref());
		members.push_back(add_member);
		member_id++;
	    }
	}
    }
    
    type.complete_composite_definition(members, definition.get_name_source_ref());
}

void
TypeResolver::extract_from_class_definition(const ClassDefinition & definition)
{
    const auto it = mir.get_types().get_types().find(definition.get_name());
    
    if (it == mir.get_types().get_types().end()) {
	// Case 1: No forward declaration exists, fill in the definition
	// from the class.
	Gyoji::owned<Type> type = std::make_unique<Type>(definition.get_name(), Type::TYPE_COMPOSITE, true, definition.get_name_source_ref());
	extract_from_class_members(*type, definition);
	mir.get_types().define_type(std::move(type));
    }
    else {
	auto & type = *it->second;
	// Case 2: Class is forward declared, but is incomplete, so fill in the declaration.
	if (!type.is_complete()) {
	    extract_from_class_members(type, definition);
	}
	else {
	    // Case 3: Class is declared and complete, but does not match our current definition,
	    // so this is a duplicate.  Raise an error to avoid ambiguity.
	    std::unique_ptr<Gyoji::context::Error> error = std::make_unique<Gyoji::context::Error>(std::string("Duplicate class definition: ") + definition.get_name());
	    error->add_message(type.get_defined_source_ref(),
			       "Originally defined here"
		);
	    error->add_message(definition.get_name_source_ref(),
			       "Re-declared here"
		);
	    compiler_context
		.get_errors()
		.add_error(std::move(error));
	}
	
    }
}

void
TypeResolver::extract_from_enum_definition(const EnumDefinition & enum_definition)
{
    const auto it = mir.get_types().get_types().find(enum_definition.get_name());
    if (it == mir.get_types().get_types().end()) {
	// No definition exists, create it.
	Gyoji::owned<Type> type = std::make_unique<Type>(enum_definition.get_name(), Type::TYPE_ENUM, true, enum_definition.get_name_source_ref());
	mir.get_types().define_type(std::move(type));

	for (const auto & ev : enum_definition.get_value_list().get_values()) {
	    fprintf(stderr, "Value is %s\n", ev->get_name().c_str());
	}
	
	
//	mir.get_symbols().define_symbol(
//	    fully_qualified_function_name,
//	    pointer_type
//	    );
    }
    else {
	// This is a duplicate, reference the original definition.
	// Case 3: Class is declared and complete, but does not match our current definition,
	// so this is a duplicate.  Raise an error to avoid ambiguity.
	auto & type = *it->second;
	std::unique_ptr<Gyoji::context::Error> error = std::make_unique<Gyoji::context::Error>(std::string("Duplicate enum definition: ") + enum_definition.get_name());
	error->add_message(type.get_defined_source_ref(),
			   "Originally defined here"
	    );
	error->add_message(enum_definition.get_name_source_ref(),
			   "Re-declared here"
	    );
	compiler_context
	    .get_errors()
	    .add_error(std::move(error));
    }
}

void
TypeResolver::extract_from_type_definition(const TypeDefinition & type_definition)
{
    const auto it = mir.get_types().get_types().find(type_definition.get_name());
    if (it == mir.get_types().get_types().end()) {
	// No definition exists, create it.
	const Type *defined_type = extract_from_type_specifier(type_definition.get_type_specifier());

	// We effectively make a new type as a copy of the old one.
	// This may seem redundant, but it's done
	// so that we can later modify the type by "instantiating"
	// it as a generic with specific type parameters
	// when we get to that point.
	Gyoji::owned<Type> type = std::make_unique<Type>(
	    type_definition.get_name(),
	    type_definition.get_type_specifier().get_source_ref(),
	    *defined_type
	    );
	mir.get_types().define_type(std::move(type));
    }
    else {
	// This is a duplicate, reference the original definition.
	// Case 3: Class is declared and complete, but does not match our current definition,
	// so this is a duplicate.  Raise an error to avoid ambiguity.
	auto & type = *it->second;
	std::unique_ptr<Gyoji::context::Error> error = std::make_unique<Gyoji::context::Error>(std::string("Duplicate enum definition: ") + type_definition.get_name());
	error->add_message(type.get_defined_source_ref(),
			   "Originally defined here"
	    );
	error->add_message(type_definition.get_name_source_ref(),
			   "Re-declared here"
	    );
	compiler_context
	    .get_errors()
	    .add_error(std::move(error));
    }
}


void
TypeResolver::extract_from_function_specifications(
    const Terminal & name,
    const TypeSpecifier & type_specifier,
    const FunctionDefinitionArgList & function_argument_list
    )
{

    std::string fully_qualified_function_name = 
	name.get_fully_qualified_name();
    
    const Type *type = extract_from_type_specifier(type_specifier);
    if (type == nullptr) {
	compiler_context
	    .get_errors()
	    .add_simple_error(type_specifier.get_source_ref(),
			      "Compiler bug!  Please report this message",
			      "Function pointer type declared with invalid type"
		);
	return;
    }    
//////
// Define the type of a function pointer.
//////
    std::vector<std::string> arg_list;
    std::vector<Argument> fptr_arguments;
    const auto & function_definition_args = function_argument_list.get_arguments();
    for (const auto & function_definition_arg : function_definition_args) {
	std::string name = function_definition_arg->get_name();
	const Type * t = extract_from_type_specifier(function_definition_arg->get_type_specifier());
	arg_list.push_back(t->get_name());
	fptr_arguments.push_back(
	    Argument(t, function_definition_arg->get_type_specifier().get_source_ref())
	    );
    }
    std::string arg_string = Gyoji::misc::join(arg_list, ",");
    std::string pointer_name = type->get_name() + std::string("(*)") + std::string("(") + arg_string + std::string(")");
    Type *pointer_type = get_or_create(pointer_name, Type::TYPE_FUNCTION_POINTER, false, name.get_source_ref());

    if (!pointer_type->is_complete()) {
	pointer_type->complete_function_pointer_definition(
	    type,
	    fptr_arguments,
	    name.get_source_ref()
	    );
    }
//////
// Now that the type has been defined, we can
// define the symbol for this specific function.
//////
    mir.get_symbols().define_symbol(
	fully_qualified_function_name,
	pointer_type
	);
    
}

void
TypeResolver::extract_from_function_definition(const FileStatementFunctionDefinition & function_definition)
{
    extract_from_function_specifications(
	function_definition.get_name(),
	function_definition.get_return_type(),
	function_definition.get_arguments()
	);
}

void
TypeResolver::extract_from_function_declaration(const FileStatementFunctionDeclaration & function_declaration)
{
    extract_from_function_specifications(
	function_declaration.get_name(),
	function_declaration.get_return_type(),
	function_declaration.get_arguments()
	);
}

void
TypeResolver::extract_from_namespace(const FileStatementNamespace & namespace_declaration)
{
    const auto & statements = namespace_declaration.get_statement_list().get_statements();
    extract_types(statements);
}

void
TypeResolver::extract_types(const std::vector<Gyoji::owned<FileStatement>> & statements)
{
    for (const auto & statement : statements) {
	const auto & file_statement = statement->get_statement();
	if (std::holds_alternative<Gyoji::owned<FileStatementFunctionDeclaration>>(file_statement)) {
	    extract_from_function_declaration(*std::get<Gyoji::owned<FileStatementFunctionDeclaration>>(file_statement));
	}
	else if (std::holds_alternative<Gyoji::owned<FileStatementFunctionDefinition>>(file_statement)) {
	    extract_from_function_definition(*std::get<Gyoji::owned<FileStatementFunctionDefinition>>(file_statement));
	}
	else if (std::holds_alternative<Gyoji::owned<FileStatementGlobalDefinition>>(file_statement)) {
	    // Nothing, no statements can be declared inside here.
	}
	else if (std::holds_alternative<Gyoji::owned<ClassDeclaration>>(file_statement)) {
	    extract_from_class_declaration(*std::get<Gyoji::owned<ClassDeclaration>>(file_statement));
	}
	else if (std::holds_alternative<Gyoji::owned<ClassDefinition>>(file_statement)) {
	    extract_from_class_definition(*std::get<Gyoji::owned<ClassDefinition>>(file_statement));
	}
	else if (std::holds_alternative<Gyoji::owned<EnumDefinition>>(file_statement)) {
	    extract_from_enum_definition(*std::get<Gyoji::owned<EnumDefinition>>(file_statement));
	}
	else if (std::holds_alternative<Gyoji::owned<TypeDefinition>>(file_statement)) {
	    extract_from_type_definition(*std::get<Gyoji::owned<TypeDefinition>>(file_statement));
	}
	else if (std::holds_alternative<Gyoji::owned<FileStatementNamespace>>(file_statement)) {
	    extract_from_namespace(*std::get<Gyoji::owned<FileStatementNamespace>>(file_statement));
	}
	else if (std::holds_alternative<Gyoji::owned<FileStatementUsing>>(file_statement)) {
	    // Nothing, no statements can be declared inside here.
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

