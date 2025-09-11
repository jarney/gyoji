#include "ast.hpp"

ASTDataNonSyntax::ASTDataNonSyntax(ASTNonSyntaxType _type, std::string _data)
  : type(_type)
  , data(_data)
{}

ASTDataNonSyntax::~ASTDataNonSyntax()
{}

ASTNode::ASTNode()
{}

ASTNode::~ASTNode()
{}

