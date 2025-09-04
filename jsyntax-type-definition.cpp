#include "jsyntax.hpp"

JSyntaxTypeDefinition::JSyntaxTypeDefinition(std::string _identifier, std::shared_ptr<JSyntaxExpression> _expr)
  : identifier(_identifier)
  , expr(_expr)
{}

JSyntaxTypeDefinition::~JSyntaxTypeDefinition()
{}

