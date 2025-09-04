#pragma once

class JSyntaxTypeDefinition {
public:
  typedef std::shared_ptr<JSyntaxTypeDefinition> ptr;
  JSyntaxTypeDefinition(std::string _identifier, JSyntaxExpression::ptr _expr);
  ~JSyntaxTypeDefinition();
  std::string identifier;
  JSyntaxExpression::ptr expr;
};
