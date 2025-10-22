/* Copyright 2025 Jonathan S. Arney
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      https://github.com/jarney/gyoji/blob/master/LICENSE
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#include <gyoji-frontend/type-resolver.hpp>
#include <variant>
#include <stdio.h>
#include <gyoji-misc/jstring.hpp>

using namespace Gyoji::mir;
using namespace Gyoji::context;
using namespace Gyoji::frontend::tree;
using namespace Gyoji::frontend::lowering;
using namespace Gyoji::frontend::namespaces;

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
    Gyoji::owned<Type> type = std::make_unique<Type>(
	declaration.get_fully_qualified_name(),
	Type::TYPE_COMPOSITE,
	false,
	declaration.get_name_source_ref());
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
    const Type *return_type = extract_from_type_specifier(type_specifier.get_return_type());

    std::vector<std::string> arg_list;
    std::vector<Argument> fptr_arguments;

    for (const auto & type_specifier : type_specifier.get_args().get_arguments()) {
	const Type *argument_type = extract_from_type_specifier(*type_specifier);
	Argument arg(argument_type, type_specifier->get_source_ref());
	fptr_arguments.push_back(arg);
	arg_list.push_back(argument_type->get_name());
    }
    
//    bool is_unsafe = method_unsafe_modifier.is_unsafe();
    bool is_unsafe = false;
    std::string arg_string = Gyoji::misc::join(arg_list, ",");
    std::string unsafe_str = is_unsafe ? std::string("unsafe") : std::string("");
    std::string pointer_name = return_type->get_name() + std::string("(") + unsafe_str + std::string("*)") + std::string("(") + arg_string + std::string(")");
    Type *fptr_type = get_or_create(pointer_name, Type::TYPE_FUNCTION_POINTER, false, type_specifier.get_source_ref());
    
    if (!fptr_type->is_complete()) {
	fptr_type->complete_function_pointer_definition(
	    return_type,
	    fptr_arguments,
	    is_unsafe,
	    type_specifier.get_source_ref()
	    );
    }
    
    return fptr_type;
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
			  "Compiler bug!  Please report this message(2)",
			  "Unknown TypeSpecifier type in variant (compiler bug)"
	    );
    
    return nullptr;
}

void
TypeResolver::extract_from_class_method_types(
    Type &class_type,
    std::map<std::string, TypeMethod> & methods,
    std::string simple_name,
    std::string fully_qualified_name,
    Gyoji::mir::Symbol::SymbolType type,
    const Gyoji::frontend::tree::UnsafeModifier & method_unsafe_modifier,
    const Type *method_return_type,
    const Gyoji::frontend::tree::FunctionDefinitionArgList & function_definition_arg_list,
    const Gyoji::context::SourceReference & source_ref
    )
{
    std::vector<std::string> arg_list;
    std::vector<Argument> fptr_arguments;
    // First, pass the 'this' pointer
    // to the function.
    if (type == Gyoji::mir::Symbol::SYMBOL_MEMBER_METHOD ||
        type == Gyoji::mir::Symbol::SYMBOL_MEMBER_DESTRUCTOR) {
	const Type * this_type = mir.get_types().get_pointer_to(&class_type, class_type.get_defined_source_ref());
	Argument arg_this(this_type, source_ref);
    
	fptr_arguments.push_back(arg_this);
	arg_list.push_back(this_type->get_name());
    }
	    
    const std::vector<Gyoji::owned<FunctionDefinitionArg>> & function_definition_args = 
	function_definition_arg_list.get_arguments();
    for (const auto & function_definition_arg : function_definition_args) {
	const Type *argument_type = extract_from_type_specifier(function_definition_arg->get_type_specifier());
	Argument arg(argument_type, function_definition_arg->get_source_ref());
	fptr_arguments.push_back(arg);
	arg_list.push_back(argument_type->get_name());
    }
    
    TypeMethod method(
	simple_name,
	source_ref,
	&class_type,
	method_return_type,
	fptr_arguments
	);

    fprintf(stderr, "Defining method %s\n", simple_name.c_str());
    methods.insert(std::pair(simple_name, method));
    
    bool is_unsafe = method_unsafe_modifier.is_unsafe();
    std::string arg_string = Gyoji::misc::join(arg_list, ",");
    std::string unsafe_str = is_unsafe ? std::string("unsafe") : std::string("");
    std::string pointer_name = method_return_type->get_name() + std::string("(") + unsafe_str + std::string("*)") + std::string("(") + arg_string + std::string(")");
    Type *fptr_type = get_or_create(pointer_name, Type::TYPE_FUNCTION_POINTER, false, source_ref);
    
    if (!fptr_type->is_complete()) {
	fptr_type->complete_function_pointer_definition(
	    method_return_type,
	    fptr_arguments,
	    is_unsafe,
	    source_ref
	    );
    }
    fprintf(stderr, "Defining symbol %s\n", fully_qualified_name.c_str());
    
    mir.get_symbols().define_symbol(
	fully_qualified_name,
	type,
	fptr_type
	);
}


void
TypeResolver::extract_from_class_members(Type & class_type, const ClassDefinition & class_definition)
{
    std::vector<TypeMember> members;
    std::map<std::string, const TypeMember*> members_by_name;
    std::map<std::string, TypeMethod> methods;
    
    const auto & class_members = class_definition.get_members();
    size_t member_id = 0;
    for (const auto & class_member : class_members) {
	const auto & class_member_type = class_member->get_member();
	if (std::holds_alternative<Gyoji::owned<ClassMemberDeclarationVariable>>(class_member_type)) {
	    const auto & member_variable = std::get<Gyoji::owned<ClassMemberDeclarationVariable>>(class_member_type);
	    
	    if (member_variable->get_unsafe_modifier().is_unsafe()) {
		compiler_context
		    .get_errors()
		    .add_simple_error(member_variable->get_type_specifier().get_source_ref(),
				      "Member variables cannot be declared inherently unsafe.",
				      std::string("Member variable ") + member_variable->get_name() + std::string(" cannot be declared unsafe.  This would not have any valid meaning.")
			);
	    }
	    const Type *member_type = extract_from_type_specifier(member_variable->get_type_specifier());
	    if (member_type == nullptr) {
		compiler_context
		    .get_errors()
		    .add_simple_error(member_variable->get_type_specifier().get_source_ref(),
				      "Could not find type",
				      std::string("Could not extract type of member variable ") + member_variable->get_name()
			);
	    }
	    else {
		const auto existing_member_it = members_by_name.find(member_variable->get_name());
		if (existing_member_it != members_by_name.end()) {
		    std::unique_ptr<Gyoji::context::Error> error = std::make_unique<Gyoji::context::Error>("Duplicate member variable in class.");
		    error->add_message(member_variable->get_name_source_ref(),
				       std::string("Member variable ") +
				       member_variable->get_name() +
				       std::string(" in class ") +
				       class_definition.get_name() +
				       std::string(" was already defined"));
		    error->add_message(existing_member_it->second->get_source_ref(),
				       "Originally defined here.");
		    compiler_context
			.get_errors()
			.add_error(std::move(error));
		}
		else {
		    TypeMember add_member(member_variable->get_name(), member_id, member_type, member_variable->get_name_source_ref());
		    members.push_back(add_member);
		    members_by_name.insert(std::pair(member_variable->get_name(), &members.back()));
		    member_id++;
		}
	    }
	}
	else if (std::holds_alternative<Gyoji::owned<ClassMemberDeclarationMethodStatic>>(class_member_type)) {
	    const auto & member_method = std::get<Gyoji::owned<ClassMemberDeclarationMethodStatic>>(class_member_type);
	    const Type * method_return_type = extract_from_type_specifier(member_method->get_type_specifier());
	    if (method_return_type == nullptr) {
		fprintf(stderr, "Compiler bug! static method is missing a return type.  This should not be possible at the semantics layer.\n");
		exit(1);
	    }
	    
	    std::string simple_name = member_method->get_identifier().get_name();
	    std::string fully_qualified_name = member_method->get_identifier().get_fully_qualified_name();
	    
	    extract_from_class_method_types(
		class_type,
		methods,
		simple_name,
		fully_qualified_name,
		Gyoji::mir::Symbol::SYMBOL_STATIC_FUNCTION,
		member_method->get_unsafe_modifier(),
		method_return_type,
		member_method->get_arguments(),
		member_method->get_source_ref()
		);
	}
	else if (std::holds_alternative<Gyoji::owned<ClassMemberDeclarationMethod>>(class_member_type)) {
	    const auto & member_method = std::get<Gyoji::owned<ClassMemberDeclarationMethod>>(class_member_type);

	    // Regular methods have return types.  Destructors always return void.
	    const Type * method_return_type = extract_from_type_specifier(member_method->get_type_specifier());

	    std::string simple_name = member_method->get_identifier().get_name();
	    std::string fully_qualified_name = member_method->get_identifier().get_fully_qualified_name();
	    
	    extract_from_class_method_types(
		class_type,
		methods,
		simple_name,
		fully_qualified_name,
		Gyoji::mir::Symbol::SYMBOL_MEMBER_METHOD,
		member_method->get_unsafe_modifier(),
		method_return_type,
		member_method->get_arguments(),
		member_method->get_source_ref()
		);
	}
	else if (std::holds_alternative<Gyoji::owned<ClassMemberDeclarationDestructor>>(class_member_type)) {
	    const auto & member_method = std::get<Gyoji::owned<ClassMemberDeclarationDestructor>>(class_member_type);

	    if (member_method->get_arguments().get_arguments().size() != 0) {
		compiler_context
		    .get_errors()
		    .add_simple_error(
			member_method->get_source_ref(),
			"Destructors may not have arguments",
			std::string("Destructors may not have funciton arguments.")
			);
		continue;
	    }
	    
	    // Regular methods have return types.  Destructors always return void.
	    const Type * method_return_type = mir.get_types().get_type("void");

	    std::string simple_name = std::string("~") + class_type.get_simple_name();
	    std::string fully_qualified_name = class_type.get_name() + std::string("::") + simple_name;
	    fprintf(stderr, "Extracting destructor %s %s\n", simple_name.c_str(), fully_qualified_name.c_str());

	    extract_from_class_method_types(
		class_type,
		methods,
		simple_name,
		fully_qualified_name,
		Gyoji::mir::Symbol::SYMBOL_MEMBER_DESTRUCTOR,
		member_method->get_unsafe_modifier(),
		method_return_type,
		member_method->get_arguments(),
		member_method->get_source_ref()
		);
	}
    }
    
    class_type.complete_composite_definition(
	members,
	methods,
	class_definition.get_name_source_ref());
}

void
TypeResolver::extract_from_class_definition(const ClassDefinition & definition)
{
    const auto it = mir.get_types().get_types().find(definition.get_fully_qualified_name());
    
    if (it == mir.get_types().get_types().end()) {
	// Case 1: No forward declaration exists, fill in the definition
	// from the class.
	Gyoji::owned<Type> type = std::make_unique<Type>(
	    definition.get_fully_qualified_name(),
	    definition.get_name(),
	    Type::TYPE_COMPOSITE,
	    true,
	    definition.get_name_source_ref());
	Type & class_type = *type.get();
	mir.get_types().define_type(std::move(type));
	extract_from_class_members(class_type, definition);
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
	    std::unique_ptr<Gyoji::context::Error> error = std::make_unique<Gyoji::context::Error>(std::string("Duplicate class definition: ") + definition.get_fully_qualified_name());
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
    Gyoji::mir::Symbol::SymbolType symbol_type,
    const Type *return_type,
    const SourceReference & return_type_source_ref,
    const FunctionDefinitionArgList & function_argument_list,
    const UnsafeModifier & unsafe_modifier
    )
{

    NS2Entity *entity = name.get_ns2_entity();
    Type *maybe_class_type = mir.get_types().get_type(entity->get_parent()->get_fully_qualified_name());
    const Type *class_pointer_type = nullptr;
    bool is_method = false;
    if (maybe_class_type != nullptr) {
	class_pointer_type = mir.get_types().get_pointer_to(maybe_class_type, return_type_source_ref);
	is_method = true;
    }
    
    std::string fully_qualified_function_name = 
	name.get_fully_qualified_name();
    
    
//////
// Define the type of a function pointer.
//////
    std::vector<std::string> arg_list;
    std::vector<Argument> fptr_arguments;
    const auto & function_definition_args = function_argument_list.get_arguments();
    if (is_method) {
	arg_list.push_back(class_pointer_type->get_name());
	fptr_arguments.push_back(
	    Argument(class_pointer_type, name.get_source_ref())
	    );
    }
    for (const auto & function_definition_arg : function_definition_args) {
	std::string name = function_definition_arg->get_identifier().get_fully_qualified_name();
	const Type * t = extract_from_type_specifier(function_definition_arg->get_type_specifier());
	arg_list.push_back(t->get_name());
	fptr_arguments.push_back(
	    Argument(t, function_definition_arg->get_type_specifier().get_source_ref())
	    );
    }
    bool is_unsafe = unsafe_modifier.is_unsafe();
    std::string arg_string = Gyoji::misc::join(arg_list, ",");
    std::string unsafe_str = is_unsafe ? std::string("unsafe") : std::string("");
    std::string pointer_name = return_type->get_name() + std::string("(") + unsafe_str + std::string("*)") + std::string("(") + arg_string + std::string(")");
    Type *pointer_type = get_or_create(pointer_name, Type::TYPE_FUNCTION_POINTER, false, name.get_source_ref());

    if (!pointer_type->is_complete()) {
	pointer_type->complete_function_pointer_definition(
	    return_type,
	    fptr_arguments,
	    is_unsafe,
	    name.get_source_ref()
	    );
    }
//////
// Now that the type has been defined, we can
// define the symbol for this specific function.
//////
    mir.get_symbols().define_symbol(
	fully_qualified_function_name,
	symbol_type,
	pointer_type
	);
    
}

void
TypeResolver::extract_from_function_definition(const FileStatementFunctionDefinition & function_definition)
{
    const Type *return_type;
    const SourceReference *return_type_source_ref;
    if (function_definition.is_destructor()) {
	return_type = mir.get_types().get_type("void");
	return_type_source_ref = &function_definition.get_name().get_source_ref();
    }
    else {
	return_type = extract_from_type_specifier(function_definition.get_return_type());
	return_type_source_ref = &function_definition.get_return_type().get_source_ref();
	if (return_type == nullptr) {
	    compiler_context
		.get_errors()
		.add_simple_error(function_definition.get_return_type().get_source_ref(),
				  "Compiler bug!  Please report this message(3)",
				  "Function pointer type declared with invalid type"
		    );
	    return;
	}
    }
    extract_from_function_specifications(
	    function_definition.get_name(),
	    Gyoji::mir::Symbol::SYMBOL_STATIC_FUNCTION,
	    return_type,
	    *return_type_source_ref,
	    function_definition.get_arguments(),
	    function_definition.get_unsafe_modifier()
	    );
}

void
TypeResolver::extract_from_function_declaration(const FileStatementFunctionDeclaration & function_declaration)
{
    const Type *return_type;
    const SourceReference *return_type_source_ref;
    if (function_declaration.is_destructor()) {
	return_type = mir.get_types().get_type("void");
	return_type_source_ref = &function_declaration.get_name().get_source_ref();
    }
    else {
	return_type = extract_from_type_specifier(function_declaration.get_return_type());
	return_type_source_ref = &function_declaration.get_return_type().get_source_ref();
	if (return_type == nullptr) {
	    compiler_context
		.get_errors()
		.add_simple_error(function_declaration.get_return_type().get_source_ref(),
				  "Compiler bug!  Please report this message(3)",
				  "Function pointer type declared with invalid type"
		    );
	    return;
	}
    }
    extract_from_function_specifications(
	function_declaration.get_name(),
	Gyoji::mir::Symbol::SYMBOL_STATIC_FUNCTION,
	return_type,
	*return_type_source_ref,
	function_declaration.get_arguments(),
	function_declaration.get_unsafe_modifier()
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
	    // TODO: We should extract the global symbols
	    // from here.
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
				  "Compiler bug!  Please report this message(1)",
				  "Unknown statement type in variant, extracting statements from file (compiler bug)"
		    );
	}
    }
}

