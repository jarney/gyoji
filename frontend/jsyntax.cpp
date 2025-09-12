#include <jlang-frontend/jsyntax.hpp>

using namespace JLang::frontend::alt_imp;

Terminal::Terminal()
  : SyntaxNode("terminal", this)
{}
Terminal::~Terminal()
{}
std::vector<TerminalNonSyntax::raw_ptr> Terminal::get_whitespace()
{
  return std::vector<TerminalNonSyntax::raw_ptr>();
}
TerminalNonSyntax::TerminalNonSyntax(TerminalNonSyntaxType _type, std::string _data)
  : type(_type)
  , data(_data)
{}
TerminalNonSyntax::~TerminalNonSyntax()
{}

SyntaxNode::SyntaxNode(std::string _type, SyntaxNode::specific_type_t _data)
  : type(_type)
  , data(_data)
{}
SyntaxNode::~SyntaxNode()
{}
void
SyntaxNode::add_child(SyntaxNode::raw_ptr node)
{
  children.push_back(node);
}
std::vector<SyntaxNode::raw_ptr> &
SyntaxNode::get_children()
{
  return children;
}
std::string & SyntaxNode::get_type()
{
  return type;
}
SyntaxNode::specific_type_t &SyntaxNode::get_data()
{
  return data;
}

FileStatementList::FileStatementList()
  : SyntaxNode("file_statement_list", this)
{}
FileStatementList::~FileStatementList()
{}



TranslationUnit::TranslationUnit(
                                 FileStatementList::owned_ptr _file_statement_list,
                                 Terminal::owned_ptr _yyeof_token)
  : SyntaxNode("translation_unit", this)
  , file_statement_list(std::move(_file_statement_list))
  , yyeof_token(std::move(_yyeof_token))
{
  add_child(file_statement_list.get());
  add_child(yyeof_token.get());
}
TranslationUnit::~TranslationUnit()
{}

FileStatementList::raw_ptr
TranslationUnit::get_statements()
{
  return file_statement_list.get();
}
