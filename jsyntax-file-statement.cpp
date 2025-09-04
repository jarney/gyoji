#include "jsyntax.hpp"

/*******************/

JSyntaxFileStatementList::JSyntaxFileStatementList()
{}
JSyntaxFileStatementList::~JSyntaxFileStatementList()
{}

/*******************/

JSyntaxFileStatement::JSyntaxFileStatement(std::shared_ptr<JSyntaxTypeDefinition> td)
  : type(JSyntaxFileStatementType::TYPE_DEFINITION)
  , value(td)
{
}

JSyntaxFileStatement::JSyntaxFileStatement(std::shared_ptr<JSyntaxFunctionDefinition> fd)
  : type(JSyntaxFileStatementType::FUNCTION_DEFINITION)
  , value(fd)
{
}


JSyntaxFileStatement::~JSyntaxFileStatement()
{}
