#include "test_syntax_tree.hpp"
#include <iostream>
#include <jlang-misc/xml.hpp>

using namespace JLang::misc;
using namespace JLang::frontend;

JBackendFormatTree::JBackendFormatTree()
  : indent(0)
{}
JBackendFormatTree::~JBackendFormatTree()
{}

void JBackendFormatTree::print_indent(void)
{
  for (int i = 0; i < 2*indent; i++) {
    printf(" ");
  }
}

void JBackendFormatTree::print_comment_multi_line(TerminalNonSyntax::raw_ptr node)
{
  print_indent();
  printf("<comment-multi-line>\n");
  indent++;
  print_indent();
  printf("%s\n", xml_to_cdata(node->get_data()).c_str());
  indent--;
  print_indent();
  printf("</comment-multi-line>\n");
}

void JBackendFormatTree::print_comment_single_line(TerminalNonSyntax::raw_ptr node)
{
  print_indent();
  printf("<comment-single-line>\n");
  indent++;
  print_indent();
  printf("%s\n", xml_to_cdata(node->get_data()).c_str());
  indent--;
  print_indent();
  printf("</comment-single-line>\n");
}

void JBackendFormatTree::print_whitespace(TerminalNonSyntax::raw_ptr node)
{
  print_indent();
  printf("<whitespace>%s</whitespace>\n", xml_escape_whitespace(node->get_data()).c_str());
}

void JBackendFormatTree::print_file_metadata(TerminalNonSyntax::raw_ptr node)
{
  printf("<metadata>%s</metadata>", xml_to_cdata(node->get_data()).c_str());
}


void JBackendFormatTree::print_non_syntax(TerminalNonSyntax::raw_ptr node)
{
  switch (node->get_type()) {
  case TerminalNonSyntaxType::EXTRA_COMMENT_MULTI_LINE:
    print_comment_multi_line(node);
    break;
  case TerminalNonSyntaxType::EXTRA_COMMENT_SINGLE_LINE:
    print_comment_single_line(node);
    break;
  case TerminalNonSyntaxType::EXTRA_WHITESPACE:
    print_whitespace(node);
    break;
  case TerminalNonSyntaxType::EXTRA_FILE_METADATA:
    print_file_metadata(node);
    break;
  }
}

int JBackendFormatTree::process(const SyntaxNode * node)
{
  print_indent();
  printf("<node type='%s'", xml_escape_attribute(node->get_type()).c_str());
  if (std::holds_alternative<Terminal*>(node->get_data())) {
    Terminal *terminal = std::get<Terminal*>(node->get_data());
    if (terminal->lineno > 0) {
      printf(" lineno='%ld'", terminal->lineno);
    }
    if (terminal->value.length() != 0) {
      if (terminal->typestr == std::string("IDENTIFIER") ||
          terminal->typestr == std::string("TYPE_NAME") ||
          terminal->typestr == std::string("NAMESPACE_NAME")
          ) {
        printf(" value='%s' fq='%s'",
               xml_escape_attribute(terminal->value).c_str(),
               xml_escape_attribute(terminal->fully_qualified_name).c_str()
               );
      }
      else {
        printf(" value='%s'", xml_escape_attribute(terminal->value).c_str());
      }
    }
  }

  std::vector<TerminalNonSyntax::raw_ptr> non_syntax;
  Terminal *maybe_terminal = nullptr;
  bool has_non_syntax_children = false;
  if (std::holds_alternative<Terminal*>(node->get_data())) {
    maybe_terminal = std::get<Terminal*>(node->get_data());
    has_non_syntax_children = maybe_terminal->non_syntax.size() != 0;
  }
      
  
  if (node->get_children().size() == 0 && !has_non_syntax_children) {
      printf("/>\n");
  }
  else {
      printf(">\n");
      indent++;
      if (std::holds_alternative<Terminal*>(node->get_data())) {
        Terminal *terminal = std::get<Terminal*>(node->get_data());
        for (const auto &non_syntax : terminal->non_syntax) {
          print_non_syntax(non_syntax.get());
        }
      }
      for (auto child : node->get_children()) {
        process(child);
      }
      indent--;
      print_indent();
      printf("</node>\n");
  }
  return 0;
}
