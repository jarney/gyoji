#include "jformat-tree.hpp"
#include <iostream>
#include <jlang-misc/xml.hpp>

using namespace JLang::misc;
using namespace JLang::frontend::ast;
using namespace JLang::cmdline;

JFormatTree::JFormatTree()
  : indent(0)
{}
JFormatTree::~JFormatTree()
{}

void JFormatTree::print_indent(void)
{
  for (int i = 0; i < 2*indent; i++) {
    printf(" ");
  }
}

void JFormatTree::print_comment_multi_line(const TerminalNonSyntax & node)
{
  print_indent();
  printf("<comment-multi-line>\n");
  indent++;
  print_indent();
  printf("%s\n", xml_to_cdata(node.get_data()).c_str());
  indent--;
  print_indent();
  printf("</comment-multi-line>\n");
}

void JFormatTree::print_comment_single_line(const TerminalNonSyntax & node)
{
  print_indent();
  printf("<comment-single-line>\n");
  indent++;
  print_indent();
  printf("%s\n", xml_to_cdata(node.get_data()).c_str());
  indent--;
  print_indent();
  printf("</comment-single-line>\n");
}

void JFormatTree::print_whitespace(const TerminalNonSyntax & node)
{
  print_indent();
  printf("<whitespace>%s</whitespace>\n", xml_escape_whitespace(node.get_data()).c_str());
}

void JFormatTree::print_file_metadata(const TerminalNonSyntax & node)
{
  printf("<metadata>%s</metadata>", xml_to_cdata(node.get_data()).c_str());
}


void JFormatTree::print_non_syntax(const TerminalNonSyntax & node)
{
  switch (node.get_type()) {
  case TerminalNonSyntax::Type::EXTRA_COMMENT_MULTI_LINE:
    print_comment_multi_line(node);
    break;
  case TerminalNonSyntax::Type::EXTRA_COMMENT_SINGLE_LINE:
    print_comment_single_line(node);
    break;
  case TerminalNonSyntax::Type::EXTRA_WHITESPACE:
    print_whitespace(node);
    break;
  case TerminalNonSyntax::Type::EXTRA_FILE_METADATA:
    print_file_metadata(node);
    break;
  }
}

int JFormatTree::process(const SyntaxNode & node)
{
  print_indent();
  printf("<node type='%s'", xml_escape_attribute(node.get_type()).c_str());
  if (node.has_data<Terminal>()) {
    const Terminal & terminal = node.get_data<Terminal>();
    if (terminal.get_line() > 0) {
      printf(" line='%ld' column='%ld'", terminal.get_line(), terminal.get_column());
    }
    if (terminal.get_value().length() != 0) {
      if (terminal.get_type() == std::string("IDENTIFIER") ||
          terminal.get_type() == std::string("TYPE_NAME") ||
          terminal.get_type() == std::string("NAMESPACE_NAME")
          ) {
        printf(" value='%s' fq='%s'",
               xml_escape_attribute(terminal.get_value()).c_str(),
               xml_escape_attribute(terminal.get_fully_qualified_name()).c_str()
               );
      }
      else {
        printf(" value='%s'", xml_escape_attribute(terminal.get_value()).c_str());
      }
    }
  }

  bool has_non_syntax_children = false;
  if (node.has_data<Terminal>()) {
    const Terminal & maybe_terminal = node.get_data<Terminal>();
    has_non_syntax_children = maybe_terminal.non_syntax.size() != 0;
  }
  
  if (node.get_children().size() == 0 && !has_non_syntax_children) {
      printf("/>\n");
  }
  else {
      printf(">\n");
      indent++;
      if (node.has_data<Terminal>()) {
        const Terminal & terminal = node.get_data<Terminal>();
        for (const auto &non_syntax : terminal.non_syntax) {
          print_non_syntax(*non_syntax);
        }
      }
      for (auto child : node.get_children()) {
        process(child);
      }
      indent--;
      print_indent();
      printf("</node>\n");
  }
  return 0;
}
