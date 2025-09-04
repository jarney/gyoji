#pragma once

#include <string>
#include <vector>
#include <memory>

enum ASTNonSyntaxType {
  NST_COMMENT_SINGLE_LINE,
  NST_COMMENT_MULTI_LINE,
  NST_WHITESPACE,
  NST_FILE_METADATA
};

class ASTDataNonSyntax {
public:
  typedef std::shared_ptr<ASTDataNonSyntax> ptr;
  ASTDataNonSyntax(ASTNonSyntaxType _type, std::string _data);
  ~ASTDataNonSyntax();
  
  ASTNonSyntaxType type;
  std::string data;
};

class ASTNode {
public:
  typedef std::shared_ptr<ASTNode> ptr;

  ASTNode();
  ~ASTNode();

  void add_node(ASTNode::ptr node);
  
  int type;
  std::string type_name;
  std::string value;
  int lineno;
  int colno;
  std::vector<ASTDataNonSyntax::ptr> non_syntax;
  std::vector<ASTNode::ptr> all_nodes;
  std::vector<ASTNode::ptr> children;

};
