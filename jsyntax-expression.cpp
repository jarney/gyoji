#include "jsyntax.hpp"

JSyntaxExpression::JSyntaxExpression(std::shared_ptr<JSyntaxExpressionIdentifier> _identifier)
  : type(JSyntaxExpressionType::EXPR_IDENTIFIER)
  , value(_identifier)
{}

JSyntaxExpression::JSyntaxExpression(std::shared_ptr<JSyntaxExpressionParenthetical> _parenthetical)
  : type(JSyntaxExpressionType::EXPR_PARENTHETICAL)
  , value(_parenthetical)
{}

JSyntaxExpression::JSyntaxExpression(std::shared_ptr<JSyntaxExpressionFunctionCall> _identifier)
  : type(JSyntaxExpressionType::EXPR_FUNCTION_CALL)
  , value(_identifier)
{}

JSyntaxExpression::~JSyntaxExpression()
{}


JSyntaxExpressionIdentifier::JSyntaxExpressionIdentifier(std::string _val)
  : val(_val)
{}

JSyntaxExpressionIdentifier::~JSyntaxExpressionIdentifier()
{}

JSyntaxExpressionParenthetical::JSyntaxExpressionParenthetical(std::shared_ptr<JSyntaxExpression> _val)
  : val(_val)
{}

JSyntaxExpressionParenthetical::~JSyntaxExpressionParenthetical()
{}


JSyntaxExpressionFunctionCall::JSyntaxExpressionFunctionCall(std::shared_ptr<JSyntaxExpression> _fn, std::shared_ptr<JSyntaxExpressionList> _args)
  : fn(_fn)
  , args(_args)
{}

JSyntaxExpressionFunctionCall::~JSyntaxExpressionFunctionCall()
{}

JSyntaxExpressionList::JSyntaxExpressionList()
{}

JSyntaxExpressionList::~JSyntaxExpressionList()
{}

