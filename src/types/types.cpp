#include <jlang-types.hpp>
#include <variant>
#include <stdio.h>

using namespace JLang::types;
using namespace JLang::frontend;
using namespace JLang::frontend::tree;

static void extract_types(Types & types, const std::vector<::JLang::owned<FileStatement>> & statements);

static void extract_from_class_declaration(Types & types, const ClassDeclaration & declaration)
{
}

static void extract_from_class_definition(Types & types, const ClassDefinition & definition)
{
}

static void extract_from_enum(Types & types, const EnumDefinition & enum_definition)
{
}

static void extract_from_namespace(Types & types, const FileStatementNamespace & namespace_declaration)
{
  printf("Recursively handling namespace %s\n", namespace_declaration.get_declaration().get_name().get_value().c_str());
  const auto & statements = namespace_declaration.get_statement_list().get_statements();
  extract_types(types, statements);
}

static void extract_types(Types & types, const std::vector<::JLang::owned<FileStatement>> & statements)
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
      printf("(forward) Declaring class\n");
    }
    else if (std::holds_alternative<JLang::owned<ClassDefinition>>(file_statement)) {
      printf("Defining class\n");
    }
    else if (std::holds_alternative<JLang::owned<EnumDefinition>>(file_statement)) {
      printf("Defining enum\n");
    }
    else if (std::holds_alternative<JLang::owned<TypeDefinition>>(file_statement)) {
      // Nothing, no statements can be declared inside here.
    }
    else if (std::holds_alternative<JLang::owned<FileStatementNamespace>>(file_statement)) {
      extract_from_namespace(types, *std::get<JLang::owned<FileStatementNamespace>>(file_statement));
    }
    else if (std::holds_alternative<JLang::owned<FileStatementUsing>>(file_statement)) {
      // Nothing, no statements can be declared inside here.
    }
    else {
      printf("Unknown statement type\n");
    }
  }
}

JLang::owned<Types> JLang::types::resolve_types(const JLang::frontend::ParseResult & parse_result)
{
  auto types = std::make_unique<Types>();

  if (!parse_result.has_translation_unit()) {
    return types;
  }
  
  // To resolve the types, we need only iterate the
  // input parse tree and pull out any type declarations,
  // resolving them down to their primitive types.
  const auto & translation_unit = parse_result.get_translation_unit();
  extract_types(*types, translation_unit.get_statements());
  
  return types;
}


////////////////////////////////////////
// Types
////////////////////////////////////////

Types::Types()
{}

Types::~Types()
{}

const Type &
Types::get_type(std::string type) const
{
  const auto & it = type_map.find(type);
  if (it == type_map.end()) {
    return *((Type*)0); // XXX TODO we should find a better return semantic than this.
  }
  return *it->second;
}

void
Types::define_type(std::string type_name, JLang::owned<Type> type)
{
  type_map.insert(std::pair<std::string, JLang::owned<Type>>(type_name, std::move(type)));
}

void
Types::dump()
{}

////////////////////////////////////////
// Type
////////////////////////////////////////
Type::Type(std::string _name, TypeType _type)
  : complete(true)
  , name(_name)
  , type(_type)
{}

Type::~Type()
{}
