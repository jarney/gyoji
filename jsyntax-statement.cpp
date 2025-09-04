#include "jsyntax.hpp"

JSyntaxStatement::JSyntaxStatement(std::shared_ptr<JSyntaxStatementExpression> stmt)
  : type(JSyntaxStatementType::STMT_EXPRESSION)
  , value(stmt)
{}

JSyntaxStatement::JSyntaxStatement(std::shared_ptr<JSyntaxStatementVariableDeclaration> stmt)
  : type(JSyntaxStatementType::STMT_VARIABLE_DECLARATION)
  , value(stmt)
{}

JSyntaxStatement::JSyntaxStatement(std::shared_ptr<JSyntaxStatementAssignment> stmt)
  : type(JSyntaxStatementType::STMT_ASSIGNMENT)
  , value(stmt)
{}

JSyntaxStatement::JSyntaxStatement(std::shared_ptr<JSyntaxStatementIfElse> stmt)
  : type(JSyntaxStatementType::STMT_IFELSE)
  , value(stmt)
{}

JSyntaxStatement::JSyntaxStatement(std::shared_ptr<JSyntaxStatementWhile> stmt)
  : type(JSyntaxStatementType::STMT_WHILE)
  , value(stmt)
{}

JSyntaxStatement::JSyntaxStatement(std::shared_ptr<JSyntaxStatementReturn> stmt)
  : type(JSyntaxStatementType::STMT_RETURN)
  , value(stmt)
{}

JSyntaxStatement::~JSyntaxStatement()
{}

JSyntaxStatementList::JSyntaxStatementList()
{}

JSyntaxStatementList::~JSyntaxStatementList()
{}
                                          
JSyntaxStatementExpression::JSyntaxStatementExpression(std::shared_ptr<JSyntaxExpression> _expr)
  : expr(_expr)
{}
JSyntaxStatementExpression::~JSyntaxStatementExpression()
{}

JSyntaxStatementVariableDeclaration::JSyntaxStatementVariableDeclaration(
    std::shared_ptr<JSyntaxExpression> _type_expr,
    std::string _variable_name
)
  : type_expr(_type_expr)
  , variable_name(_variable_name)
{}

JSyntaxStatementVariableDeclaration::~JSyntaxStatementVariableDeclaration()
{}

JSyntaxStatementAssignment::JSyntaxStatementAssignment(
      std::string _identifier,
      std::shared_ptr<JSyntaxExpression> _value_expr
)
  : identifier(_identifier)
  , value_expr(_value_expr)
{}

JSyntaxStatementAssignment::~JSyntaxStatementAssignment()
{}

JSyntaxStatementIfElse::JSyntaxStatementIfElse(
                                               std::shared_ptr<JSyntaxExpression> _expr,
                                               std::shared_ptr<JSyntaxStatementList> _ifbody,
                                               std::shared_ptr<JSyntaxStatementList> _elsebody
                                               )
  : expr(_expr)
  , ifbody(_ifbody)
  , elsebody(_elsebody)
{}

JSyntaxStatementIfElse::~JSyntaxStatementIfElse()
{}

JSyntaxStatementWhile::JSyntaxStatementWhile(
                        std::shared_ptr<JSyntaxExpression> _expr,
                        std::shared_ptr<JSyntaxStatementList> _body
                        )
  : expr(_expr)
  , body(_body)
{}

JSyntaxStatementWhile::~JSyntaxStatementWhile()
{}

JSyntaxStatementReturn::JSyntaxStatementReturn(std::shared_ptr<JSyntaxExpression> _expr)
  : expr(_expr)
{}

JSyntaxStatementReturn::~JSyntaxStatementReturn()
{}


