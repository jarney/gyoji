#include <jlang-frontend.hpp>

using namespace JLang::frontend;
using namespace JLang::frontend::ast;

///////////////////////////////////////////////////
SyntaxNode::SyntaxNode(std::string _type, SyntaxNode::specific_type_t _data)
  : type(_type)
  , data(_data)
{}
SyntaxNode::~SyntaxNode()
{}
void
SyntaxNode::add_child(const SyntaxNode & node)
{
  children.push_back(node);
}
void
SyntaxNode::prepend_child(const SyntaxNode & node)
{
  children.insert(children.begin(), node);
}
const std::vector<std::reference_wrapper<const SyntaxNode>> &
SyntaxNode::get_children() const
{
  return children;
}
const std::string & SyntaxNode::get_type() const
{
  return type;
}
const SyntaxNode &
SyntaxNode::get_syntax_node() const
{ return *this; }

