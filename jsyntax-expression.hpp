#pragma once

enum JSyntaxExpressionType {
  EXPR_IDENTIFIER,
  EXPR_PARENTHETICAL,
  EXPR_FUNCTION_CALL
};
class JSyntaxExpressionIdentifier;
class JSyntaxExpressionParenthetical;
class JSyntaxExpressionFunctionCall;


class JSyntaxExpression {
public:
  typedef std::shared_ptr<JSyntaxExpression> ptr;
  JSyntaxExpression(std::shared_ptr<JSyntaxExpressionIdentifier> _identifier);
  JSyntaxExpression(std::shared_ptr<JSyntaxExpressionParenthetical> _parenthetical);
  JSyntaxExpression(std::shared_ptr<JSyntaxExpressionFunctionCall> _function_call);
  ~JSyntaxExpression();
  JSyntaxExpressionType type;
  std::variant<
        std::shared_ptr<JSyntaxExpressionIdentifier>,
        std::shared_ptr<JSyntaxExpressionParenthetical>,
        std::shared_ptr<JSyntaxExpressionFunctionCall>
        > value;
};


class JSyntaxExpressionList {
public:
  typedef std::shared_ptr<JSyntaxExpressionList> ptr;
  JSyntaxExpressionList();
  ~JSyntaxExpressionList();
  std::list<JSyntaxExpression::ptr> list;
};

class JSyntaxExpressionIdentifier {
public:
  typedef std::shared_ptr<JSyntaxExpressionIdentifier> ptr;
  JSyntaxExpressionIdentifier(std::string _val);
  ~JSyntaxExpressionIdentifier();
  std::string val;
};

class JSyntaxExpressionParenthetical {
public:
  typedef std::shared_ptr<JSyntaxExpressionParenthetical> ptr;
  JSyntaxExpressionParenthetical(JSyntaxExpression::ptr _val);
  ~JSyntaxExpressionParenthetical();
  std::shared_ptr<JSyntaxExpression> val;
};

class JSyntaxExpressionFunctionCall {
public:
  typedef std::shared_ptr<JSyntaxExpressionFunctionCall> ptr;
  JSyntaxExpressionFunctionCall(JSyntaxExpression::ptr _fn, JSyntaxExpressionList::ptr _args);
  ~JSyntaxExpressionFunctionCall();
  std::shared_ptr<JSyntaxExpression> fn;
  std::shared_ptr<JSyntaxExpressionList> args;
};

