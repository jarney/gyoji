#pragma once

#include <jlang-backend/jbackend.hpp>

namespace JLang::backend {

  using namespace JLang::frontend;
  /**
   * This is the identity transformation that should always
   * format the code exactly as it appeared in the input,
   * assuming that the input is a valid syntax tree.
   */
  class JBackendFormatTree : public JBackend {
  private:
    int indent;
    void print_non_syntax(ASTDataNonSyntax::ptr non_syntax);
    void print_indent(void);
    void print_comment_multi_line(ASTDataNonSyntax::ptr node);
    void print_comment_single_line(ASTDataNonSyntax::ptr node);
    void print_whitespace(ASTDataNonSyntax::ptr node);
    void print_file_metadata(ASTDataNonSyntax::ptr node);
    
  public:
    JBackendFormatTree();
    ~JBackendFormatTree();
    virtual int process(ASTNode::ptr file);
  };

};
