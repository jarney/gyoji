#pragma once

#include <list>
#include <map>
#include <string>
#include <memory>
#include <variant>

#include "ast.hpp"
#include "namespace.hpp"

namespace JLang::frontend {
  typedef struct return_data_st {
    ASTNode::ptr translation_unit;
    NamespaceContext namespace_context;
  } return_data_t;

  namespace alt_imp {
  enum TerminalNonSyntaxType {
    EXTRA_COMMENT_SINGLE_LINE,
    EXTRA_COMMENT_MULTI_LINE,
    EXTRA_WHITESPACE,
    EXTRA_FILE_METADATA
  };
  
  class TerminalNonSyntax {
  public:
    typedef std::unique_ptr<TerminalNonSyntax> ptr;
    typedef TerminalNonSyntax *raw_ptr;
    TerminalNonSyntax(TerminalNonSyntaxType _type, std::string _data);
    ~TerminalNonSyntax();
    
    TerminalNonSyntaxType type;
    std::string data;
  };
  
  class Namespace;

    class Terminal;
    class TranslationUnit;
    class FileStatementList;

  class SyntaxNode {
  public:
    typedef SyntaxNode *raw_ptr;
    typedef std::unique_ptr<SyntaxNode> owned_ptr;
    typedef std::variant<
      Terminal*,
      TranslationUnit*,
      FileStatementList*> specific_type_t;
    
    SyntaxNode(std::string _type, specific_type_t _data);
    ~SyntaxNode();

    // Access to the child list should be
    // immutable because this is really
    // just a view over the list.
    std::vector<SyntaxNode::raw_ptr> & get_children();
    std::string & get_type();
    SyntaxNode::specific_type_t &get_data();

  private:
    // This list does NOT own its children, so
    // the class deriving from this one must
    // agree to own the pointers separately.
    std::vector<SyntaxNode::raw_ptr> children; 

  protected:
    // Children are owned by their parents, so this is
    // private and can only be called by the
    // deriving class.
    void add_child(SyntaxNode::raw_ptr node);
      
    std::string type;
    specific_type_t data;
  };

  /**
   * Terminals are the raw tokens received by the lexer.
   * They carry the token information as well as any
   * "Non-syntax" data like comments and whitespace.
   */
  class Terminal : public SyntaxNode {
  public:
    typedef Terminal *raw_ptr;
    typedef std::unique_ptr<Terminal> owned_ptr;
    Terminal();
    ~Terminal();
    int type;
    std::string typestr;
    std::string value;
    size_t lineno;
    size_t colno;
    std::string fully_qualified_name;
    std::vector<TerminalNonSyntax::raw_ptr> get_whitespace();

    // The terminal "owns" uniquely all of the non-syntax data
    // in this vector.  It may be returned to access it,
    // but these are owned pointers, so they must only
    // be de-referenced and never assigned to
    std::vector<TerminalNonSyntax::ptr> non_syntax;
  };

  class FileStatementList : public SyntaxNode {
  public:
    typedef FileStatementList                  *raw_ptr;
    typedef std::unique_ptr<FileStatementList> owned_ptr;
    FileStatementList();
    ~FileStatementList();
  };
  
  
  class TranslationUnit : public SyntaxNode {
  public:
    typedef TranslationUnit                  *raw_ptr;
    typedef std::unique_ptr<TranslationUnit> owned_ptr;

    TranslationUnit(
        FileStatementList::owned_ptr file_statement_list,
        Terminal::owned_ptr yyeof_token
    );
    ~TranslationUnit();
    FileStatementList::raw_ptr get_statements();
  private:
    Terminal::owned_ptr yyeof_token;
    FileStatementList::owned_ptr file_statement_list;
  };


  };
  
};

