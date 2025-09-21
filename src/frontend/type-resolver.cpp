#include <jlang-frontend/type-resolver.hpp>
#include <variant>
#include <stdio.h>

using namespace JLang::mir;
using namespace JLang::frontend;
using namespace JLang::frontend::tree;

void
TypeResolver::extract_from_class_declaration(const ClassDeclaration & declaration)
{
  JLang::owned<Type> type = std::make_unique<Type>(declaration.get_name(), Type::TYPE_COMPOSITE, false);
  types.define_type(std::move(type));
}

Type *
TypeResolver::get_or_create(std::string pointer_name, Type *pointer_target, Type::TypeType type_type)
{
    Type *pointer_type = types.get_type(pointer_name);
    if (pointer_type != nullptr) {
      return pointer_type;
    }
    else {
      JLang::owned<Type> pointer_type_created = std::make_unique<Type>(pointer_name, type_type, true);
      pointer_type_created->complete_pointer_definition(pointer_target);
      pointer_type = pointer_type_created.get();
      types.define_type(std::move(pointer_type_created));
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
      fprintf(stderr, "Extracting types from expressions is not yet supported\n");
      return nullptr;
    }
    std::string name = type_name.get_name();
    Type *type = types.get_type(name);
    if (type == nullptr) {
      fprintf(stderr, "Could not find type %s\n", name.c_str());
      return nullptr;
    }
    return type;
  }
  else if (std::holds_alternative<JLang::owned<TypeSpecifierTemplate>>(type_specifier_type)) {
    const auto & template_type = std::get<JLang::owned<TypeSpecifierTemplate>>(type_specifier_type);
    return nullptr;
  }
  else if (std::holds_alternative<JLang::owned<TypeSpecifierFunctionPointer>>(type_specifier_type)) {
    const auto & fptr_type = std::get<JLang::owned<TypeSpecifierFunctionPointer>>(type_specifier_type);
    return nullptr;
  }
  else if (std::holds_alternative<JLang::owned<TypeSpecifierPointerTo>>(type_specifier_type)) {
    const auto & type_specifier_pointer_to = std::get<JLang::owned<TypeSpecifierPointerTo>>(type_specifier_type);
    Type *pointer_target = extract_from_type_specifier(type_specifier_pointer_to->get_type_specifier());
    if (pointer_target == nullptr) {
      fprintf(stderr, "Could not find target of pointer\n");
      return nullptr;
    }
    std::string pointer_name = pointer_target->get_name() + std::string("*");
    Type *pointer_type = get_or_create(pointer_name, pointer_target, Type::TYPE_POINTER);
    return pointer_type;
  }
  else if (std::holds_alternative<JLang::owned<TypeSpecifierReferenceTo>>(type_specifier_type)) {
    const auto & type_specifier_reference_to = std::get<JLang::owned<TypeSpecifierReferenceTo>>(type_specifier_type);
    Type *pointer_target = extract_from_type_specifier(type_specifier_reference_to->get_type_specifier());
    if (pointer_target == nullptr) {
      fprintf(stderr, "Could not find target of reference\n");
      return nullptr;
    }
    std::string pointer_name = pointer_target->get_name() + std::string("&");
    Type *pointer_type = get_or_create(pointer_name, pointer_target, Type::TYPE_REFERENCE);
    return pointer_type;
  }
  else {
    fprintf(stderr, "Error: Un-handled type (compiler bug)\n");
  }
  
  fprintf(stderr, "Error: type not found\n");
  return nullptr;
}

void
TypeResolver::extract_from_class_members(Type & type, const ClassDefinition & definition)
{
  std::vector<std::pair<std::string, Type *>> members;

  const auto & class_members = definition.get_members();
  for (const auto & class_member : class_members) {
    const auto & class_member_type = class_member->get_member();
    if (std::holds_alternative<JLang::owned<ClassMemberDeclarationVariable>>(class_member_type)) {
      const auto & member_variable = std::get<JLang::owned<ClassMemberDeclarationVariable>>(class_member_type);

      Type *member_type = extract_from_type_specifier(member_variable->get_type_specifier());
      if (member_type == nullptr) {
        printf("Error: member variable type %s not resolved\n", member_variable->get_name().c_str());
      }
      else {
        members.push_back(std::pair<std::string, Type*>(member_variable->get_name(), member_type));
        printf("Got member variable %s\n", member_variable->get_name().c_str());
      }
    }
  }
  
  type.complete_composite_definition(members);
}

void
TypeResolver::extract_from_class_definition(const ClassDefinition & definition)
{
  auto it = types.type_map.find(definition.get_name());
  
  if (it == types.type_map.end()) {
    // Case 1: No forward declaration exists, fill in the definition
    // from the class.
    JLang::owned<Type> type = std::make_unique<Type>(definition.get_name(), Type::TYPE_COMPOSITE, true);
    extract_from_class_members(*type, definition);
    types.define_type(std::move(type));
  }
  else {
    auto & type = *it->second;
    // Case 2: Class is forward declared, but is incomplete, so fill in the declaration.
    if (!type.is_complete()) {
      extract_from_class_members(type, definition);
    }
    else {
      auto & errors = parse_result.get_errors();
      std::unique_ptr<JLang::context::Error> error = std::make_unique<JLang::context::Error>("Type Resolution Error");
      error->add_message(parse_result.get_token_stream().context(0, 10),
                         0, // LINE
                         0, // COLUMN
                         "Duplicate type");
      
    }

    // Case 3: Class is declared and complete, but does not match our current definition,
    // so this is a duplicate.  Raise an error to avoid ambiguity.
    
  }
}

void
TypeResolver::extract_from_enum(const EnumDefinition & enum_definition)
{
}

void
TypeResolver::extract_from_namespace(const FileStatementNamespace & namespace_declaration)
{
  printf("Recursively handling namespace %s\n", namespace_declaration.get_declaration().get_name().get_value().c_str());
  const auto & statements = namespace_declaration.get_statement_list().get_statements();
  extract_types(statements);
}

void
TypeResolver::extract_types(const std::vector<JLang::owned<FileStatement>> & statements)
{
  for (const auto & statement : statements) {
    const auto & file_statement = statement->get_statement();
    if (std::holds_alternative<JLang::owned<FileStatementFunctionDefinition>>(file_statement)) {
      // Nothing, no statements can be declared inside here.
    }
    else if (std::holds_alternative<JLang::owned<FileStatementFunctionDefinition>>(file_statement)) {
      // Nothing, no statements can be declared inside here.
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
      printf("Unknown statement type\n");
    }
  }
}

TypeResolver::TypeResolver(const JLang::frontend::ParseResult & _parse_result, Types & _types)
  : parse_result(_parse_result)
  , types(_types)
{}
TypeResolver::~TypeResolver()
{}

void TypeResolver::check_complete_type(Type *type) const
{
  if (type->get_type() == Type::TYPE_COMPOSITE) {
    for (const auto & member : type->get_members()) {
      if (!member.second->is_complete()) {
        fprintf(stderr, "Incomplete type %s\n", member.second->get_name().c_str());
        std::unique_ptr<JLang::context::Error> error = std::make_unique<JLang::context::Error>("Class contains incomplete type");
        error->add_message(parse_result.get_token_stream().context(12, 18),
                           14,
                           19,
                           std::string("Incomplete type in member ") + member.first + std::string(" of type ") + type->get_name() + std::string("\n"));
        parse_result.get_errors().add_error(std::move(error));
      }
      check_complete_type(member.second);
    }
  }
}

void TypeResolver::resolve_types()
{
  // To resolve the types, we need only iterate the
  // input parse tree and pull out any type declarations,
  // resolving them down to their primitive types.
  const auto & translation_unit = parse_result.get_translation_unit();
  extract_types(translation_unit.get_statements());

  // TODO: Go back through all of the types and make sure that every type
  // that is used in a structure 'inline' is actually complete.
  // If not, produce a compile error to that effect.
  // This needs to be recursive!
  for (const auto & type : types.type_map) {
    check_complete_type(type.second.get());
  }
}

void
JLang::frontend::resolve_types(
                               Types & types,
                               const JLang::frontend::ParseResult & parse_result
                               )
{
  // We don't need to report an error at this point
  // because lack of a translation unit means
  // that our caller should not even have called us
  // and should report a syntax error at the
  // higher level.
  if (!parse_result.has_translation_unit()) {
    return;
  }
  
  TypeResolver resolver(parse_result, types);

  resolver.resolve_types();
}

