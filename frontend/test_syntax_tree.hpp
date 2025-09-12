#pragma once

#include <jlang-frontend/jsyntax.hpp>

namespace JLang::frontend::alt_imp {

  /**
   * This is the identity transformation that should always
   * format the code exactly as it appeared in the input,
   * assuming that the input is a valid syntax tree.
   */
  class JBackendFormatTree {
  private:
    int indent;
    void print_non_syntax(TerminalNonSyntax::raw_ptr non_syntax);
    void print_indent(void);
    void print_comment_multi_line(TerminalNonSyntax::raw_ptr node);
    void print_comment_single_line(TerminalNonSyntax::raw_ptr node);
    void print_whitespace(TerminalNonSyntax::raw_ptr node);
    void print_file_metadata(TerminalNonSyntax::raw_ptr node);
    
  public:
    JBackendFormatTree();
    ~JBackendFormatTree();
    virtual int process(SyntaxNode::raw_ptr file);
  };

};
