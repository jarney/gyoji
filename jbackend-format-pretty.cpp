#include "jbackend-format-pretty.hpp"
#include "target/lexer.l.hpp"
#include "target/syntax.y.hpp"
#include <iostream>

JBackendFormatPretty::JBackendFormatPretty()
{}
JBackendFormatPretty::~JBackendFormatPretty()
{}

void JBackendFormatPretty::print_whitespace(ASTDataNonSyntax::ptr node)
{
  printf("%s", node->data.c_str());
}
void JBackendFormatPretty::print_comment_single_line(ASTDataNonSyntax::ptr node)
{
  printf("//%s", node->data.c_str());
}
void JBackendFormatPretty::print_comment_multi_line(ASTDataNonSyntax::ptr node)
{
  printf("/*%s*/", node->data.c_str());
}
void JBackendFormatPretty::print_file_metadata(ASTDataNonSyntax::ptr node)
{
  printf("%s", node->data.c_str());
}

void JBackendFormatPretty::print_comments(std::vector<ASTDataNonSyntax::ptr> &non_syntax_list)
{
  for (auto non_syntax : non_syntax_list) {
    if (non_syntax->type != ASTNonSyntaxType::NST_COMMENT_MULTI_LINE &&
        non_syntax->type != ASTNonSyntaxType::NST_COMMENT_SINGLE_LINE) {
      continue;
    }
    print_non_syntax(non_syntax);
  }
}

void JBackendFormatPretty::print_non_syntax(ASTDataNonSyntax::ptr node)
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

void JBackendFormatPretty::print_node_generic(ASTNode::ptr node)
{  
  for (auto non_syntax : node->non_syntax) {
    print_non_syntax(non_syntax);
  }
  print_node_plain(node);
}

void JBackendFormatPretty::print_node_plain(ASTNode::ptr node)
{
  printf("%s", node->value.c_str());
  for (auto child : node->children) {
    print_node(child);
  }
}

void JBackendFormatPretty::newline() {
  printf("\n");
  for (int i = 0; i < depth; i++) {
    printf("        ");
  }
}

void JBackendFormatPretty::print_node_function_def(ASTNode::ptr node)
{
  print_node(node->children.at(0));
  printf("\n");
  print_node_plain(node->children.at(1));
  printf("(");
  depth++;
  newline();
  print_node_plain(node->children.at(3));
  depth--;
  newline();
  printf(")");
  newline();
  print_scope_body(node->children.at(5));
}

void JBackendFormatPretty::print_scope_body(ASTNode::ptr node)
{
  printf("{");
  depth++;
  print_node_plain(node->children.at(1));
  depth--;
  newline();
  printf("}");
  newline();
}

std::string JBackendFormatPretty::collect_comments(ASTNode::ptr node)
{
  std::string comments_normalized;
  for (auto &non_syntax : node->non_syntax) {
    if (non_syntax->type == ASTNonSyntaxType::NST_COMMENT_MULTI_LINE ||
        non_syntax->type == ASTNonSyntaxType::NST_COMMENT_SINGLE_LINE) {
      printf("Collected comment :%s:\n", non_syntax->data.c_str());
      comments_normalized = comments_normalized + non_syntax->data;
    }
  }
  for (auto &child : node->children) {
    comments_normalized = comments_normalized + collect_comments(child);
  }
  return comments_normalized;
}

std::string JBackendFormatPretty::break_multiline_comment(std::string str)
{
  std::string ret;
  char last = '\0';
  int linelen = 0;
  int pos = 0;

  if (str[0] == '*') {
    ret += "*\n";
    pos = 1;
  }
  for (int i = pos ; i < str.size(); i++) {
    char chr = str[i];
    // Normalize all whitespace characters to be
    // a single space in comments.
    if (chr == '\t' || chr == '\n') {
      chr = ' ';
    }
    if (last == ' ' && chr == ' ') {
      continue;
    }
    ret += chr;
    linelen++;
    if (chr == ' ' && linelen > 75) {
      ret += "\n * ";
      linelen = 0;
    }
    last = chr;
  }
  return ret;
}

void JBackendFormatPretty::print_node(ASTNode::ptr node)
{
  switch (node->type) {
  case calc::Parser::symbol_kind_type::S_function_def:
    print_node_function_def(node);
    break;
  default:
    print_node_generic(node);
    break;
  }
}

int JBackendFormatPretty::process(ASTNode::ptr file)
{
  print_node_generic(file);
  return 0;
}
