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

static int blockid = 0;
int tmpvar = 0;
void
FunctionResolver::extract_from_expression_primary(const ExpressionPrimary & expression)
{
  
}

void
FunctionResolver::extract_from_expression_primary(const ExpressionPostfixArrayIndex & expression)
{
}
void
FunctionResolver::extract_from_expression_primary(const ExpressionPostfixFunctionCall & expression)
{
}
void
FunctionResolver::extract_from_expression_primary(const ExpressionPostfixDot & expression)
{
}
void
FunctionResolver::extract_from_expression_primary(const ExpressionPostfixArrow & expression)
{
}
void
FunctionResolver::extract_from_expression_primary(const ExpressionPostfixIncDec & expression)
{
}
void
FunctionResolver::extract_from_expression_primary(const ExpressionUnaryPrefix & expression)
{
}
void
FunctionResolver::extract_from_expression_primary(const ExpressionUnarySizeofType & expression)
{
}
void
FunctionResolver::extract_from_expression_primary(const ExpressionBinary & expression)
{
}
void
FunctionResolver::extract_from_expression_primary(const ExpressionTrinary & expression)
{
}
void
FunctionResolver::extract_from_expression_primary(const ExpressionCast & expression)
{
}


void
FunctionResolver::extract_from_expression(const Expression & expression_container)
{
  fprintf(stderr, "    Expression\n");
  const auto & expression_type = expression_container.get_expression();
  if (std::holds_alternative<JLang::owned<ExpressionPrimary>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPrimary>>(expression_type);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPostfixArrayIndex>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPostfixArrayIndex>>(expression_type);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPostfixFunctionCall>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPostfixFunctionCall>>(expression_type);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPostfixDot>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPostfixDot>>(expression_type);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPostfixArrow>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPostfixArrow>>(expression_type);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionPostfixIncDec>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionPostfixIncDec>>(expression_type);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionUnaryPrefix>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionUnaryPrefix>>(expression_type);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionUnarySizeofType>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionUnarySizeofType>>(expression_type);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionBinary>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionBinary>>(expression_type);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionTrinary>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionTrinary>>(expression_type);
  }
  else if (std::holds_alternative<JLang::owned<ExpressionCast>>(expression_type)) {
    const auto & expression = std::get<JLang::owned<ExpressionCast>>(expression_type);
  }
  else {
    fprintf(stderr, "Compiler bug, invalid expression type\n");
    exit(1);
  }
}

void
FunctionResolver::extract_from_statement_list(const StatementList & statement_list)
{
  for (const auto & statement_el : statement_list.get_statements()) {
    const auto & statement_type = statement_el->get_statement();
    if (std::holds_alternative<JLang::owned<StatementVariableDeclaration>>(statement_type)) {
      const auto & statement = std::get<JLang::owned<StatementVariableDeclaration>>(statement_type);
      fprintf(stderr, "    Var decl\n");
    }
    else if (std::holds_alternative<JLang::owned<StatementBlock>>(statement_type)) {
      const auto & statement = std::get<JLang::owned<StatementBlock>>(statement_type);
      blockid++;
      fprintf(stderr, "BB%d\n", blockid);

      extract_from_statement_list(statement->get_scope_body().get_statements());

      blockid++;
      fprintf(stderr, "BB%d\n", blockid);
    }
    else if (std::holds_alternative<JLang::owned<StatementExpression>>(statement_type)) {
      const auto & statement = std::get<JLang::owned<StatementExpression>>(statement_type);
      extract_from_expression(statement->get_expression());
    }
    else if (std::holds_alternative<JLang::owned<StatementIfElse>>(statement_type)) {
      const auto & statement = std::get<JLang::owned<StatementIfElse>>(statement_type);
      fprintf(stderr, "    expression\n");
      fprintf(stderr, "    jne BB-??\n");
      blockid++;
      fprintf(stderr, "BB%d\n", blockid);
      extract_from_statement_list(statement->get_if_scope_body().get_statements());
      if (statement->has_else()) {
        blockid++;
        fprintf(stderr, "BB%d\n", blockid);
        extract_from_statement_list(statement->get_else_scope_body().get_statements());
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
      fprintf(stderr, "return\n");
    }
    else {
      fprintf(stderr, "Compiler bug, invalid statement type\n");
      exit(1);
    }
  }
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

  fprintf(stderr, "START BBlocks\n");
  fprintf(stderr, "BB0:\n");
  extract_from_statement_list(function_definition.get_scope_body().get_statements());
  fprintf(stderr, "END BBlocks\n");
  
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

