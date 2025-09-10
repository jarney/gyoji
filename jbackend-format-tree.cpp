#include "jbackend-format-tree.hpp"
#include "target/jlang.l.hpp"
#include "target/jlang.y.hpp"
#include <iostream>
#include "xml.hpp"

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

void JBackendFormatTree::print_comment_multi_line(ASTDataNonSyntax::ptr node)
{
  print_indent();
  printf("<comment-multi-line>\n");
  indent++;
  print_indent();
  printf("%s\n", xml_to_cdata(node->data).c_str());
  indent--;
  print_indent();
  printf("</comment-multi-line>\n");
}

void JBackendFormatTree::print_comment_single_line(ASTDataNonSyntax::ptr node)
{
  print_indent();
  printf("<comment-single-line>\n");
  indent++;
  print_indent();
  printf("%s\n", xml_to_cdata(node->data).c_str());
  indent--;
  print_indent();
  printf("</comment-single-line>\n");
}

void JBackendFormatTree::print_whitespace(ASTDataNonSyntax::ptr node)
{
  print_indent();
  printf("<whitespace>%s</whitespace>\n", xml_escape_whitespace(node->data).c_str());
}

void JBackendFormatTree::print_file_metadata(ASTDataNonSyntax::ptr node)
{
  printf("<metadata>%s</metadata>", xml_to_cdata(node->data).c_str());
}


void JBackendFormatTree::print_non_syntax(ASTDataNonSyntax::ptr node)
{
  switch (node->type) {
  case ASTNonSyntaxType::NST_COMMENT_MULTI_LINE:
    print_comment_multi_line(node);
    break;
  case ASTNonSyntaxType::NST_COMMENT_SINGLE_LINE:
    print_comment_single_line(node);
    break;
  case ASTNonSyntaxType::NST_WHITESPACE:
    print_whitespace(node);
    break;
  case ASTNonSyntaxType::NST_FILE_METADATA:
    print_file_metadata(node);
    break;
  }
}

int JBackendFormatTree::process(ASTNode::ptr node)
{
  print_indent();
  printf("<node type='%s'", xml_escape_attribute(node->typestr).c_str());
  if (node->lineno > 0) {
    printf(" lineno='%ld'", node->lineno);
  }
  if (node->value.length() != 0) {
    printf(" value='%s'", xml_escape_attribute(node->value).c_str());
  }
  std::vector<ASTDataNonSyntax::ptr> non_syntax;
  if (node->children.size() == 0 && node->non_syntax.size() == 0) {
      printf("/>\n");
  }
  else {
      printf(">\n");
      indent++;
      for (auto non_syntax : node->non_syntax) {
        print_non_syntax(non_syntax);
      }
      for (auto child : node->children) {
        process(child);
      }
      indent--;
      print_indent();
      printf("</node>\n");
  }
  return 0;
}
