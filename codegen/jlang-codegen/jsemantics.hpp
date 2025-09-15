#pragma once
#include <memory>
#include <string>
#include <list>
#include <map>
#include <jlang-frontend/ast.hpp>

/*!
 *  \addtogroup Codegen
 *  @{
 */
//! This is the code generation layer.
/*!
 * <pre>
 *   This is some diagram or something
 *   |--------------------------------|
 *   | Some thing that is interesting |
 *   |--------------------------------|
 * </pre>
 */
namespace JLang::codegen {
};
/*! @} End of Doxygen Groups*/

/*!
 *  \addtogroup Codegen
 *  @{
 */
//! Headline News for the semantics layer.
/*!
 * The role of the semantics layer is
 * to represent the output in an intermediate
 * representation suitable for type checking,
 * borrow-checking, and code generation.
 * This namespace does not actually perform
 * those functions, but is responsible for
 * representing the program's data in a way
 * that is suitable for those purposes.
 */
namespace JLang::codegen::semantics {

  using namespace JLang::frontend;
  
  class TranslationUnit;

  class Type;

  class FunctionDeclaration;
  class FunctionDefinition;

  class ScopeBody;
  
  class Statement;
  class StatementFunctionCall;
  class StatementExpression;
  class StatementVariableDeclaration;
  
  class Expression;
  class ExpressionLiteralInt;
  class ExpressionVariable;
  class ExpressionAssignment;
  class ExpressionAddition;
  
  class GlobalVariableDefinition;

  template <class T> class Visitor {
  public:
    virtual void visit(T &visitable) = 0;
  };

  // This represents everything we need to know about
  // the type.  This includes references to other types
  // and may represent a primitive type or also a
  // derived type like a 'class'.  Note that types
  // have two parts: The part that drives the behavior
  // like the size to allocate on the stack, etc, and also
  // the part that lives inside a namespace and can be resolved
  // by a universal name in that namespace.
  class Type {
  public:
    typedef std::shared_ptr<Type> ptr;
    Type(std::string _name, size_t _size);
    ~Type();
    size_t get_size_bytes();
    std::string name; // Fully-qualified name of the type.
    size_t size;
    Type::ptr supertype;
    std::vector<Type::ptr> members;
    std::vector<FunctionDeclaration> methods;
    // TODO: access modifier (volatile,const)
  };

  class FunctionDeclarationArg {
  public:
    typedef std::shared_ptr<FunctionDeclarationArg> ptr;
    FunctionDeclarationArg();
    ~FunctionDeclarationArg();
    std::string name;
    Type::ptr type;
  };
  
  class FunctionDeclaration {
  public:
    typedef std::shared_ptr<FunctionDeclaration> ptr;
    FunctionDeclaration();
    ~FunctionDeclaration();
    void visit(Visitor<FunctionDeclaration> &visitor);
    std::string name;
    Type::ptr return_type;
    std::vector<FunctionDeclarationArg::ptr> arg_types;
  };

  // This is the main unit of work for the LLVM because
  // it is what drives code generation for symbols.
  class FunctionDefinition {
  public:
    typedef std::shared_ptr<FunctionDefinition> ptr;
    FunctionDefinition();
    ~FunctionDefinition();
    void visit(Visitor<FunctionDefinition> &visitor);
    FunctionDeclaration::ptr function_declaration;

    std::shared_ptr<ScopeBody> scope_body;
  };

  class GlobalVariableDefinition {
  public:
    typedef std::shared_ptr<GlobalVariableDefinition> ptr;
    GlobalVariableDefinition();
    ~GlobalVariableDefinition();
    void visit(Visitor<GlobalVariableDefinition> &visitor);
    std::string name;
    Type::ptr type;
  };

  class Statement {
  public:
    typedef std::shared_ptr<Statement> ptr;
    Statement();
    ~Statement();
  };

  class StatementScopeBody : public Statement {
  public:
    StatementScopeBody();
    ~StatementScopeBody();
    std::shared_ptr<ScopeBody> scope_body;
  };
  
  class StatementFunctionCall : public Statement {
  public:
    StatementFunctionCall();
    ~StatementFunctionCall();
  };

  class StatementVariableDeclaration : public Statement {
  public:
     StatementVariableDeclaration();
    ~StatementVariableDeclaration();
  };

  class Expression {
  public:
    typedef std::shared_ptr<Expression> ptr;
    Expression();
    ~Expression();
  };

  class ExpressionLiteralInt : public Expression {
  public:
    ExpressionLiteralInt(int _value);
    ~ExpressionLiteralInt();
    int value;
  };

  class ExpressionVariable : public Expression {
  public:
    ExpressionVariable(std::string _name);
    ~ExpressionVariable();
    std::string name;
  };

  class ExpressionAssignment : public Expression {
  public:
    typedef enum {
      EQUALS,
      PLUS_EQUALS,
      MINUS_EQUALS,
      TIMES_EQUALS,
      SHL_EQUALS,
      SHR_EQUALS,
      DIV_EQUALS,
      MOD_EQUALS
    } AssignmentType;
    ExpressionAssignment(Expression::ptr, AssignmentType type, Expression::ptr);
    ~ExpressionAssignment();
  };
  
  class ScopeBody {
  public:
    typedef std::shared_ptr<ScopeBody> ptr;
    ScopeBody();
    ~ScopeBody();
    void visit(Visitor<ScopeBody> &visitor);
    std::list<Statement::ptr> statements;
  };
  
  class TranslationUnit {
  public:
    typedef std::shared_ptr<TranslationUnit> ptr;
    TranslationUnit();
    ~TranslationUnit();
    void visit(Visitor<TranslationUnit> &visitor);
    void register_builtin_types();

    std::map<std::string, Type::ptr> types;
    std::list<FunctionDefinition::ptr> function_definitions;
    std::list<FunctionDeclaration::ptr> function_declarations;
    std::list<GlobalVariableDefinition::ptr> globals;
  };

  int file_statement_process(TranslationUnit::ptr translation_unit, ASTNode::ptr ast_statement_list);
  int process_type_definition(TranslationUnit::ptr translation_unit, ASTNode::ptr ast_statement_list);


  Type::ptr ast_to_type(TranslationUnit::ptr translation_unit, ASTNode::ptr node);
  Statement::ptr ast_to_statement(TranslationUnit::ptr translation_unit, ASTNode::ptr node);
  FunctionDefinition::ptr ast_to_file_statement_function_definition(TranslationUnit::ptr translation_unit, ASTNode::ptr node);
  ScopeBody::ptr ast_to_scope_body(TranslationUnit::ptr translation_unit, ASTNode::ptr node);
  
  FunctionDeclaration::ptr ast_to_file_statement_function_declaration(TranslationUnit::ptr translation_unit, ASTNode::ptr node);
  FunctionDeclarationArg::ptr ast_to_function_declaration_arg(TranslationUnit::ptr translation_unit, ASTNode::ptr node);
  

  GlobalVariableDefinition::ptr ast_to_file_global_definition(TranslationUnit::ptr translation_unit, ASTNode::ptr node);
  
  std::list<ASTNode::ptr> get_children_by_type(ASTNode::ptr node, std::string type);
  
  TranslationUnit::ptr from_ast(ASTNode::ptr ast);
};
/*! @} End of Doxygen Groups*/
