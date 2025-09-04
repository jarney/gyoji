#pragma once

#include "jbackend.hpp"

/**
 * This is the identity transformation that should always
 * format the code exactly as it appeared in the input,
 * assuming that the input is a valid syntax tree.
 */
class JBackendFormatPretty : public JBackend {
public:
  JBackendFormatPretty();
  ~JBackendFormatPretty();
  virtual void process(ASTNode::ptr file);

  std::string collect_comments(ASTNode::ptr node);
  std::string break_multiline_comment(std::string str);

  void print_whitespace(ASTDataNonSyntax::ptr node);
  void print_comments(std::vector<ASTDataNonSyntax::ptr> &non_syntax_list);
  void print_comment_single_line(ASTDataNonSyntax::ptr node);
  void print_comment_multi_line(ASTDataNonSyntax::ptr node);
  void print_file_metadata(ASTDataNonSyntax::ptr node);
  void print_non_syntax(ASTDataNonSyntax::ptr node);
  
  void print_node_generic(ASTNode::ptr node);
  void print_node_plain(ASTNode::ptr node);
  void print_node(ASTNode::ptr node);
  void print_scope_body(ASTNode::ptr node);
  void newline();
  
  void print_node_function_def(ASTNode::ptr node);

  int indent_level;
  int depth;
};
