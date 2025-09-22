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

  extract_types(translation_unit.get_statements());
  fprintf(stderr, "Resolving functions...\n");
}

void
FunctionResolver::extract_from_namespace(const FileStatementNamespace & namespace_declaration)
{
  const auto & statements = namespace_declaration.get_statement_list().get_statements();
  extract_types(statements);
}

void
FunctionResolver::extract_from_function_definition(const FileStatementFunctionDefinition & function_definition)
{
  fprintf(stderr, "Extracting function %s %s\n",
          function_definition.get_name().get_fully_qualified_name().c_str(),
          function_definition.get_name().get_value().c_str());
  
  std::string fully_qualified_function_name = 
    function_definition.get_name().get_fully_qualified_name() +
    std::string("::") + 
    function_definition.get_name().get_value().c_str();
  
  const TypeSpecifier & type_specifier = function_definition.get_type_specifier();
  Type *type = type_resolver.extract_from_type_specifier(type_specifier);

  if (type == nullptr) {
    fprintf(stderr, "Could not find return type\n");
    return;
  }

  std::vector<FunctionArgument> arguments;
  JLang::owned<Function> fn = std::make_unique<Function>(
                                                         fully_qualified_function_name,
                                                         type->get_name(),
                                                         arguments);
  mir.get_functions().add_function(std::move(fn));
  fprintf(stderr, "Return value %s\n", type->get_name().c_str());
  
}
void
FunctionResolver::extract_from_class_definition(const ClassDefinition & definition)
{
  fprintf(stderr, "Extracting from class definition, constructors and destructors which are special-case functions.\n");
  // These must be linked back to their corresponding type definitions
  // so that we can generate their code.
}

void
FunctionResolver::extract_types(const std::vector<JLang::owned<FileStatement>> & statements)
{
  for (const auto & statement : statements) {
    fprintf(stderr, "Extracting a statement\n");
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

