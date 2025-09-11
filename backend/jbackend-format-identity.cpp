#include <jlang-backend/jbackend-format-identity.hpp>
#include <iostream>

JBackendFormatIdentity::JBackendFormatIdentity()
{}
JBackendFormatIdentity::~JBackendFormatIdentity()
{}

static void print_whitespace(ASTDataNonSyntax::ptr node)
{
    printf("%s", node->data.c_str());
}
static void print_comment_single_line(ASTDataNonSyntax::ptr node)
{
  printf("//%s", node->data.c_str());
}
static void print_comment_multi_line(ASTDataNonSyntax::ptr node)
{
  printf("/*%s*/", node->data.c_str());
}
static void print_file_metadata(ASTDataNonSyntax::ptr node)
{
  printf("%s", node->data.c_str());
}


static void print_non_syntax(ASTDataNonSyntax::ptr node)
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

static void print_node(ASTNode::ptr node)
{
  for (auto non_syntax : node->non_syntax) {
    print_non_syntax(non_syntax);
  }
  printf("%s", node->value.c_str());
  for (auto child : node->children) {
    print_node(child);
  }
}

int JBackendFormatIdentity::process(ASTNode::ptr file)
{
  print_node(file);
  return 0;
}
