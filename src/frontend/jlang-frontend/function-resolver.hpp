#pragma once

#include <jlang-mir.hpp>
#include <jlang-frontend.hpp>

namespace JLang::frontend {
  class TypeResolver;
  
  //! Function Resolver.
  /**
   * This is the 'lowering' process for functions.
   * It reads the parse result syntax tree and constructs
   * functions and the "BasicBlock" control-flow graph
   * in order to produce the MIR which can later drive
   * semantic analysis like borrow checking and then
   * code-generation.
   */
  class FunctionResolver {
  public:
    FunctionResolver(
                 JLang::context::CompilerContext & _compiler_context,
                 const JLang::frontend::tree::TranslationUnit & _translation_unit,
                 JLang::mir::MIR & _mir,
                 JLang::frontend::TypeResolver & _type_resolver
                 );
    ~FunctionResolver();
    void resolve();
  private:
    JLang::context::CompilerContext & compiler_context;
    const JLang::frontend::tree::TranslationUnit & translation_unit;
    JLang::mir::MIR & mir;
    TypeResolver & type_resolver;

    void extract_from_expression_primary(const JLang::frontend::tree::ExpressionPrimary & expression);
    void extract_from_expression_primary(const JLang::frontend::tree::ExpressionPostfixArrayIndex & expression);
    void extract_from_expression_primary(const JLang::frontend::tree::ExpressionPostfixFunctionCall & expression);
    void extract_from_expression_primary(const JLang::frontend::tree::ExpressionPostfixDot & expression);
    void extract_from_expression_primary(const JLang::frontend::tree::ExpressionPostfixArrow & expression);
    void extract_from_expression_primary(const JLang::frontend::tree::ExpressionPostfixIncDec & expression);
    void extract_from_expression_primary(const JLang::frontend::tree::ExpressionUnaryPrefix & expression);
    void extract_from_expression_primary(const JLang::frontend::tree::ExpressionUnarySizeofType & expression);
    void extract_from_expression_primary(const JLang::frontend::tree::ExpressionBinary & expression);
    void extract_from_expression_primary(const JLang::frontend::tree::ExpressionTrinary & expression);
    void extract_from_expression_primary(const JLang::frontend::tree::ExpressionCast & expression);

    void extract_from_expression(const JLang::frontend::tree::Expression & expression);
    void extract_from_statement_list(const JLang::frontend::tree::StatementList & statement_list);
    
    void extract_from_function_definition(const JLang::frontend::tree::FileStatementFunctionDefinition & function_definition);
    void extract_from_class_definition(const JLang::frontend::tree::ClassDefinition & definition);
    void extract_from_namespace(const JLang::frontend::tree::FileStatementNamespace & namespace_declaration);
    void extract_types(const std::vector<JLang::owned<JLang::frontend::tree::FileStatement>> & statements);

  };
  
};
