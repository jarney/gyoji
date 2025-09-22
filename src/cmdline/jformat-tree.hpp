#pragma once

#include <jlang-frontend.hpp>

namespace JLang::cmdline {
  using namespace JLang::frontend::ast;
  using namespace JLang::frontend::tree;
  /**
   * This is the identity transformation that should always
   * format the code exactly as it appeared in the input,
   * assuming that the input is a valid syntax tree.
   */
  class JFormatTree {
  public:
    JFormatTree();
    ~JFormatTree();
    int process(const SyntaxNode & file);
  private:
    int indent;
    void print_non_syntax(const TerminalNonSyntax & non_syntax);
    void print_indent(void);
    void print_comment_multi_line(const TerminalNonSyntax & node);
    void print_comment_single_line(const TerminalNonSyntax & node);
    void print_whitespace(const TerminalNonSyntax & node);
    void print_file_metadata(const TerminalNonSyntax & node);
    
  };

};
