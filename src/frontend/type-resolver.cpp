#include <jlang-frontend/type-resolver.hpp>
#include <variant>
#include <stdio.h>
#include <jlang-misc/jstring.hpp>

using namespace JLang::mir;
using namespace JLang::context;
using namespace JLang::frontend;
using namespace JLang::frontend::tree;

TypeResolver::TypeResolver(
    JLang::context::CompilerContext & _compiler_context,
    const JLang::frontend::tree::TranslationUnit & _translation_unit,
    JLang::mir::MIR & _mir)
    : compiler_context(_compiler_context)
    , translation_unit(_translation_unit)
    , mir(_mir)
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
    JLang::owned<Type> type = std::make_unique<Type>(declaration.get_name(), Type::TYPE_COMPOSITE, false, declaration.get_name_source_ref());
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
	JLang::owned<Type> pointer_type_created = std::make_unique<Type>(pointer_name, type_type, complete, source_ref);
	pointer_type = pointer_type_created.get();
	mir.get_types().define_type(std::move(pointer_type_created));
	return pointer_type;
    }
}

Type *
TypeResolver::extract_from_type_specifier(const TypeSpecifier & type_specifier) 
{
    const auto & type_specifier_type = type_specifier.get_type();
    if (std::holds_alternative<JLang::owned<TypeSpecifierSimple>>(type_specifier_type)) {
	const JLang::owned<TypeSpecifierSimple> & simple = std::get<JLang::owned<TypeSpecifierSimple>>(type_specifier_type);
	const auto & type_name = simple->get_type_name();
	if (type_name.is_expression()) {
	    auto error = std::make_unique<JLang::context::Error>("Could not resolve type");
	    error->add_message(type_name.get_name_source_ref(), "Specifying types from expressions is not yet supported.");
	    compiler_context.get_errors().add_error(std::move(error));
	    return nullptr;
	}
	std::string name = type_name.get_name();
	Type *type = mir.get_types().get_type(name);
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
    else if (std::holds_alternative<JLang::owned<TypeSpecifierTemplate>>(type_specifier_type)) {
	const auto & template_type = std::get<JLang::owned<TypeSpecifierTemplate>>(type_specifier_type);
	compiler_context
	    .get_errors()
	    .add_simple_error(template_type->get_source_ref(),
			      "Could not find type",
			      "Template types are not supported yet."
		);
	return nullptr;
    }
    else if (std::holds_alternative<JLang::owned<TypeSpecifierFunctionPointer>>(type_specifier_type)) {
	const auto & fptr_type = std::get<JLang::owned<TypeSpecifierFunctionPointer>>(type_specifier_type);
	compiler_context
	    .get_errors()
	    .add_simple_error(fptr_type->get_source_ref(),
			      "Could not find type",
			      "Function pointer types are not supported yet."
		);
	return nullptr;
    }
    else if (std::holds_alternative<JLang::owned<TypeSpecifierPointerTo>>(type_specifier_type)) {
	const auto & type_specifier_pointer_to = std::get<JLang::owned<TypeSpecifierPointerTo>>(type_specifier_type);
	Type *pointer_target = extract_from_type_specifier(type_specifier_pointer_to->get_type_specifier());
	if (pointer_target == nullptr) {
	    compiler_context
		.get_errors()
		.add_simple_error(type_specifier_pointer_to->get_source_ref(),
				  "Could not find type",
				  "Could not resolve target of pointer"
		    );
	    return nullptr;
	}
	std::string pointer_name = pointer_target->get_name() + std::string("*");
	Type *pointer_type = get_or_create(pointer_name, Type::TYPE_POINTER, false, type_specifier_pointer_to->get_source_ref());
	pointer_type->complete_pointer_definition(pointer_target, type_specifier_pointer_to->get_source_ref());
	return pointer_type;
    }
    else if (std::holds_alternative<JLang::owned<TypeSpecifierReferenceTo>>(type_specifier_type)) {
	const auto & type_specifier_reference_to = std::get<JLang::owned<TypeSpecifierReferenceTo>>(type_specifier_type);
	Type *pointer_target = extract_from_type_specifier(type_specifier_reference_to->get_type_specifier());
	if (pointer_target == nullptr) {
	    compiler_context
		.get_errors()
		.add_simple_error(type_specifier_reference_to->get_source_ref(),
				  "Could not find type",
				  "Could not resolve target of reference"
		    );
	    return nullptr;
	}
	std::string pointer_name = pointer_target->get_name() + std::string("&");
	Type *pointer_type = get_or_create(pointer_name, Type::TYPE_REFERENCE, false, type_specifier_reference_to->get_source_ref());
	pointer_type->complete_pointer_definition(pointer_target, type_specifier_reference_to->get_source_ref());
	return pointer_type;
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
    for (const auto & class_member : class_members) {
	const auto & class_member_type = class_member->get_member();
	if (std::holds_alternative<JLang::owned<ClassMemberDeclarationVariable>>(class_member_type)) {
	    const auto & member_variable = std::get<JLang::owned<ClassMemberDeclarationVariable>>(class_member_type);
	    
	    Type *member_type = extract_from_type_specifier(member_variable->get_type_specifier());
	    if (member_type == nullptr) {
		compiler_context
		    .get_errors()
		    .add_simple_error(member_variable->get_type_specifier().get_source_ref(),
				      "Could not find type",
				      "Could not extract type of member variable " + member_variable->get_name()
			);
	    }
	    else {
		TypeMember add_member(member_variable->get_name(), member_type, class_member->get_source_ref());
		members.push_back(add_member);
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
	JLang::owned<Type> type = std::make_unique<Type>(definition.get_name(), Type::TYPE_COMPOSITE, true, definition.get_name_source_ref());
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
	    std::unique_ptr<JLang::context::Error> error = std::make_unique<JLang::context::Error>(std::string("Duplicate class definition: ") + definition.get_name());
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
TypeResolver::extract_from_enum(const EnumDefinition & enum_definition)
{
    const auto it = mir.get_types().get_types().find(enum_definition.get_name());
    if (it == mir.get_types().get_types().end()) {
	// No definition exists, create it.
	JLang::owned<Type> type = std::make_unique<Type>(enum_definition.get_name(), Type::TYPE_ENUM, true, enum_definition.get_name_source_ref());
	mir.get_types().define_type(std::move(type));
    }
    else {
	// This is a duplicate, reference the original definition.
	// Case 3: Class is declared and complete, but does not match our current definition,
	// so this is a duplicate.  Raise an error to avoid ambiguity.
	auto & type = *it->second;
	std::unique_ptr<JLang::context::Error> error = std::make_unique<JLang::context::Error>(std::string("Duplicate enum definition: ") + enum_definition.get_name());
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
TypeResolver::extract_from_function_specifications(
    const Terminal & name,
    const TypeSpecifier & type_specifier,
    const FunctionDefinitionArgList & syntax_arguments
    )
{

    std::string fully_qualified_function_name = 
	name.get_fully_qualified_name();
    
    const FunctionPrototype *proto = mir.get_functions().get_prototype(fully_qualified_function_name);
    if (proto != nullptr) {
	return;
    }
    Type *type = extract_from_type_specifier(type_specifier);
    
    std::vector<FunctionArgument> arguments;
    const auto & function_argument_list = syntax_arguments;
    const auto & function_definition_args = function_argument_list.get_arguments();
    for (const auto & function_definition_arg : function_definition_args) {
	std::string name = function_definition_arg->get_name();
	JLang::mir::Type * mir_type = extract_from_type_specifier(function_definition_arg->get_type_specifier());
	std::string type = mir_type->get_name();
	FunctionArgument arg(name, type);
	arguments.push_back(arg);
    }
    fprintf(stderr, "Defined prototype %s\n", fully_qualified_function_name.c_str());
    JLang::owned<FunctionPrototype> prototype = std::make_unique<FunctionPrototype>(
	fully_qualified_function_name,
	type->get_name(),
	arguments
	);
    mir.get_functions().add_prototype(std::move(prototype));

//////
// The type-centric way.
//////
    std::vector<std::string> arg_list;
    std::vector<Argument> fptr_arguments;
    for (const auto & function_definition_arg : function_definition_args) {
	std::string name = function_definition_arg->get_name();
	Type * t = extract_from_type_specifier(function_definition_arg->get_type_specifier());
	arg_list.push_back(t->get_name());
	fptr_arguments.push_back(
	    Argument(t, function_definition_arg->get_type_specifier().get_source_ref())
	    );
	fprintf(stderr, "Added function argument name %s\n", t->get_name().c_str());
    }
    std::string arg_string = JLang::misc::join(arg_list, ",");
    std::string pointer_name = type->get_name() + std::string("(*)") + std::string("(") + arg_string + std::string(")");
    Type *pointer_type = get_or_create(pointer_name, Type::TYPE_FUNCTION_POINTER, false, name.get_source_ref());
    fprintf(stderr, "Creating function pointer type\n");
    if (type == nullptr) {
	fprintf(stderr, "This is bad, there is no return type\n");
    }
    if (!pointer_type->is_complete()) {
	fprintf(stderr, "Completing function pointer type\n");
	pointer_type->complete_function_pointer_definition(
	    type,
	    fptr_arguments,
	    name.get_source_ref()
	    );
    }

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
TypeResolver::extract_types(const std::vector<JLang::owned<FileStatement>> & statements)
{
    for (const auto & statement : statements) {
	const auto & file_statement = statement->get_statement();
	if (std::holds_alternative<JLang::owned<FileStatementFunctionDeclaration>>(file_statement)) {
	    extract_from_function_declaration(*std::get<JLang::owned<FileStatementFunctionDeclaration>>(file_statement));
	}
	else if (std::holds_alternative<JLang::owned<FileStatementFunctionDefinition>>(file_statement)) {
	    extract_from_function_definition(*std::get<JLang::owned<FileStatementFunctionDefinition>>(file_statement));
	}
	else if (std::holds_alternative<JLang::owned<FileStatementGlobalDefinition>>(file_statement)) {
	    // Nothing, no statements can be declared inside here.
	}
	else if (std::holds_alternative<JLang::owned<ClassDeclaration>>(file_statement)) {
	    extract_from_class_declaration(*std::get<JLang::owned<ClassDeclaration>>(file_statement));
	}
	else if (std::holds_alternative<JLang::owned<ClassDefinition>>(file_statement)) {
	    extract_from_class_definition(*std::get<JLang::owned<ClassDefinition>>(file_statement));
	}
	else if (std::holds_alternative<JLang::owned<EnumDefinition>>(file_statement)) {
	    extract_from_enum(*std::get<JLang::owned<EnumDefinition>>(file_statement));
	}
	else if (std::holds_alternative<JLang::owned<TypeDefinition>>(file_statement)) {
	    // Nothing, no statements can be declared inside here.
	}
	else if (std::holds_alternative<JLang::owned<FileStatementNamespace>>(file_statement)) {
	    extract_from_namespace(*std::get<JLang::owned<FileStatementNamespace>>(file_statement));
	}
	else if (std::holds_alternative<JLang::owned<FileStatementUsing>>(file_statement)) {
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

