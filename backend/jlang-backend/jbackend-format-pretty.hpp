#pragma once

#include <jlang-backend/jbackend.hpp>

namespace JLang::backend {
  using namespace JLang::frontend;
  /**
   * This is the identity transformation that should always
   * format the code exactly as it appeared in the input,
   * assuming that the input is a valid syntax tree.
   */
  class JBackendFormatPretty : public JBackend {
  public:
    JBackendFormatPretty();
    ~JBackendFormatPretty();
    virtual int process(const SyntaxNode * file);
    
    std::string collect_comments(const SyntaxNode * node);
    std::string break_multiline_comment(std::string str);
    
    void print_whitespace(ASTDataNonSyntax::ptr node);
    void print_comments(std::vector<ASTDataNonSyntax::ptr> &non_syntax_list);
    void print_comment_single_line(ASTDataNonSyntax::ptr node);
    void print_comment_multi_line(ASTDataNonSyntax::ptr node);
    void print_file_metadata(ASTDataNonSyntax::ptr node);
    void print_non_syntax(ASTDataNonSyntax::ptr node);
    
    void print_node_generic(const SyntaxNode * node);
    void print_node_plain(const SyntaxNode * node);
    void print_node(const SyntaxNode * node);
    void print_scope_body(const SyntaxNode * node);
    void newline();
    
    void print_node_function_def(const SyntaxNode * node);
    
    int indent_level;
    int depth;
  };

};
