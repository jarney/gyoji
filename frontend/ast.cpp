#include <jlang-frontend/ast.hpp>

using namespace JLang::frontend;

ASTDataNonSyntax::ASTDataNonSyntax(ASTNonSyntaxType _type, std::string _data)
  : type(_type)
  , data(_data)
{}

ASTDataNonSyntax::~ASTDataNonSyntax()
{}

ASTNode::ASTNode()
  : lineno(0)
  , colno(0)
{}

ASTNode::~ASTNode()
{}

