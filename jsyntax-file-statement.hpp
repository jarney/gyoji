#pragma once

#include "jsyntax-function-definition.hpp"
#include "jsyntax-type-definition.hpp"

enum JSyntaxFileStatementType {
  FUNCTION_DEFINITION,
  TYPE_DEFINITION
};

class JSyntaxFileStatement {
public:
  typedef std::shared_ptr<JSyntaxFileStatement> ptr;
  JSyntaxFileStatement(std::shared_ptr<JSyntaxFunctionDefinition> fd);
  JSyntaxFileStatement(std::shared_ptr<JSyntaxTypeDefinition> td);
  ~JSyntaxFileStatement();
  JSyntaxFileStatementType type;
    std::variant<
          std::shared_ptr<JSyntaxFunctionDefinition>,
          std::shared_ptr<JSyntaxTypeDefinition>
          > value;
};

class JSyntaxFileStatementList {
public:
  typedef std::shared_ptr<JSyntaxFileStatementList> ptr;
  JSyntaxFileStatementList();
  ~JSyntaxFileStatementList();
  std::list<std::shared_ptr<JSyntaxFileStatement>> statements;
};
