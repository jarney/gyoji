#pragma once
#include <memory>
#include <string>
#include <list>
#include <map>
#include "ast.hpp"

// Compute the semantic tree
// from the given AST.  The purpose
// of this layer is to determine the
// semantic components of what's being represented
// here.  We should represent
// types, variables, and functions that operate on them.
// We should also be able to
//   * Derive what's needed to generate the LLVM output.
//   * Derive what's needed to run the borrow checker
//
// This layer is the abstract layer and the 'concrete' layers
// like the borrow checker and LLVM should be powered by the
// same underlying representation.

namespace JSemantics {

  class TranslationUnit;

  class Type;

  class FunctionDeclaration;
  class FunctionDefinition;

  class ScopeBlock;
  
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

  class FunctionDeclaration {
  public:
    typedef std::shared_ptr<FunctionDeclaration> ptr;
    FunctionDeclaration();
    ~FunctionDeclaration();
    void visit(Visitor<FunctionDeclaration> &visitor);
    std::string name;
    std::string return_type;
    std::vector<std::pair<std::string, std::string>> arg_types;
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

    std::shared_ptr<ScopeBlock> scope_block;
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

  class StatementScopeBlock : public Statement {
  public:
    StatementScopeBlock();
    ~StatementScopeBlock();
    std::shared_ptr<ScopeBlock> scope_block;
  };
  
  class StatementFunctionCall : public Statement {
  public:
    StatementFunctionCall();
    ~StatementFunctionCall();
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
  
  class ScopeBlock {
  public:
    typedef std::shared_ptr<ScopeBlock> ptr;
    ScopeBlock();
    ~ScopeBlock();
    void visit(Visitor<ScopeBlock> &visitor);
    std::list<Statement::ptr> statements;
  };
  
  class TranslationUnit {
  public:
    typedef std::shared_ptr<TranslationUnit> ptr;
    TranslationUnit();
    ~TranslationUnit();
    void visit(Visitor<TranslationUnit> &visitor);
    void register_builtin_types();

    FunctionDefinition::ptr ast_to_file_statement_function_definition(ASTNode::ptr node);
    FunctionDeclaration::ptr ast_to_file_statement_function_declaration(ASTNode::ptr node);
    GlobalVariableDefinition::ptr ast_to_file_global_definition(ASTNode::ptr node);
    Type::ptr ast_to_type(ASTNode::ptr node);

    
    std::map<std::string, Type::ptr> types;
    std::list<FunctionDefinition::ptr> function_definitions;
    std::list<FunctionDeclaration::ptr> function_declarations;
    std::list<GlobalVariableDefinition::ptr> globals;
  };

  TranslationUnit::ptr from_ast(ASTNode::ptr ast);
};
