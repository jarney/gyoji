#include <jlang-frontend/ast.hpp>

using namespace JLang::frontend;

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

