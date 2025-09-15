#include <jlang-backend/jbackend-format-identity.hpp>
#include <iostream>

using namespace JLang::frontend;
using namespace JLang::backend;

JBackendFormatIdentity::JBackendFormatIdentity()
{}
JBackendFormatIdentity::~JBackendFormatIdentity()
{}

static void print_whitespace(const TerminalNonSyntax & node)
{
  printf("%s", node.get_data().c_str());
}
static void print_comment_single_line(const TerminalNonSyntax & node)
{
  printf("//%s", node.get_data().c_str());
}
static void print_comment_multi_line(const TerminalNonSyntax & node)
{
  printf("/*%s*/", node.get_data().c_str());
}
static void print_file_metadata(const TerminalNonSyntax & node)
{
  printf("%s", node.get_data().c_str());
}


static void print_non_syntax(const TerminalNonSyntax & node)
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

static void print_node(const SyntaxNode &node)
{
  if (node.has_data<Terminal>()) {
    const Terminal & terminal = node.get_data<Terminal>();
    for (const auto &non_syntax : terminal.non_syntax) {
      print_non_syntax(*non_syntax);
    }
    printf("%s", terminal.value.c_str());
  }
  for (auto child : node.get_children()) {
    print_node(child);
  }
}

int JBackendFormatIdentity::process(const SyntaxNode &file)
{
  print_node(file);
  return 0;
}
