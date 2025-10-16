/* Copyright 2025 Jonathan S. Arney
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      https://github.com/jarney/gyoji/blob/master/LICENSE
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include <gyoji-backend/jbackend-toc.hpp>

using namespace Gyoji::frontend::ast;

// TODO: This should not be so tightly coupled.
#include <gyoji.l.hpp>
#include <gyoji.y.hpp>
#include <iostream>

JBackendToC::JBackendToC()
{}
JBackendToC::~JBackendToC()
{}

/**
 * The core idea here is that each lvalue has associated with it
 * a type.  That type implies a lifecycle of operations that may
 * be performed on it.  For primitive types, any lifecycle is
 * allowed (i.e. additions, divisions, other arithmetic operations
 * are all allowed).  Other types like pointers must have
 * operations performed in a specific way.
 * ptr^ : Owning pointer
 *    Must begin with a "malloc" function.
 *    Must end with a "free" function
 *    No other operations are permitted.
 *    malloc (dereference|assignment)* free
 *
 * For a non-owning pointer (ptr*), we must inherit part of the
 * lifecycle of the thing we were assigned from.
 *
 * ptr*
 *
 * Here, if free happens and anything else
 * happens after that, an error is generated.
 *
 * struct: inherits lifecycle requirements of all members.
 *         Other lifecycle requirements may be added for the struct
 *         itself.
 */

void print_whitespace(ASTDataNonSyntax::ptr node)
{
  printf("%s", node->data.c_str());
}
void print_comment_single_line(ASTDataNonSyntax::ptr node)
{
  printf("//%s", node->data.c_str());
}
void print_comment_multi_line(ASTDataNonSyntax::ptr node)
{
  printf("/*%s*/", node->data.c_str());
}
void print_file_metadata(ASTDataNonSyntax::ptr node)
{
  printf("%s", node->data.c_str());
}

void print_non_syntax(ASTDataNonSyntax::ptr node)
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

void print_comments(std::vector<ASTDataNonSyntax::ptr> &non_syntax_list)
{
  for (auto non_syntax : non_syntax_list) {
    if (non_syntax->type != ASTNonSyntaxType::NST_COMMENT_MULTI_LINE &&
        non_syntax->type != ASTNonSyntaxType::NST_COMMENT_SINGLE_LINE) {
      continue;
    }
    print_non_syntax(non_syntax);
  }
}

void print_node_generic(ASTNode::ptr node);

void print_node_plain(ASTNode::ptr node)
{
  printf("%s", node->value.c_str());
  for (auto child : node->children) {
    print_node_generic(child);
  }
}

void print_node_generic(ASTNode::ptr node)
{  
  for (auto non_syntax : node->non_syntax) {
    print_non_syntax(non_syntax);
  }
  print_node_plain(node);
}

void JBackendToC::newline() {
  printf("\n");
  for (int i = 0; i < depth; i++) {
    printf("        ");
  }
}

std::shared_ptr<Type> resolve_type_expression(ASTNode::ptr node);

void resolve_type_struct_members(ASTNode::ptr node, std::list<std::shared_ptr<Member>> & members)
{
  for (auto child : node->children) {
    std::shared_ptr<Member> member = std::make_shared<Member>();
    //printf("<print>");
    //print_node_plain(child);
    //printf("</print>\n");
    printf("Struct member %s\n", child->children.at(1)->value.c_str());
    member->type = resolve_type_expression(child->children.at(0));
    member->name = child->children.at(0)->value;
    members.push_back(member);
  }

}


std::shared_ptr<Type> resolve_type_expression(ASTNode::ptr node)
{
  std::shared_ptr<Type> type = std::make_shared<Type>();
  if (node->type == calc::Parser::symbol_kind_type::S_type_expression_primitive) {
    type->type = TypeType::TYPE_PRIMITIVE;
    type->primitive = node->value;
    printf("Primitive %s\n", type->primitive.c_str());
  }
  else if (node->type == calc::Parser::symbol_kind_type::S_type_expression_pointer) {
    type->type = TypeType::TYPE_POINTER;
    type->target = resolve_type_expression(node->children.at(0));
    printf("Pointer\n");
  }
  else if (node->type == calc::Parser::symbol_kind_type::S_type_expression_struct) {
    type->type = TypeType::TYPE_STRUCT;
    printf("Struct\n");
    resolve_type_struct_members(node->children.at(2), type->members);
  }
  
  return type;
}

// Evaluate the arguments of the function.
void fill_arguments(std::shared_ptr<Function> fn, ASTNode::ptr node)
{
  for (auto arg : node->children) {
    // Ignore the comma
      printf("Node type is %d\n", arg->type);
    if (arg->type == calc::Parser::symbol_kind_type::S_function_definition_arg) {
      printf("Getting name from line %d\n", arg->lineno);
    
      std::string arg_name = arg->children.at(1)->value;
      printf("Argument: %s\n", arg_name.c_str());
      
      std::shared_ptr<FunctionArgument> farg = std::make_shared<FunctionArgument>();
      farg->name = arg_name;
      farg->type = resolve_type_expression(arg->children.at(0));
      fn->arguments.push_back(farg);
    }
  }
}

void JBackendToC::print_node_function_definition(ASTNode::ptr node)
{
  std::shared_ptr<Function> fn = std::make_shared<Function>();
  fn->name = node->children.at(1)->value;
  printf("filling arguments");
  fill_arguments(fn, node->children.at(3));
  
  function_table.functions.push_back(fn);
  
  print_node_generic(node->children.at(0));
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

void JBackendToC::print_scope_body(ASTNode::ptr node)
{
  printf("{");
  depth++;
  print_node_plain(node->children.at(1));
  depth--;
  newline();
  printf("}");
  newline();
}

std::string JBackendToC::collect_comments(ASTNode::ptr node)
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

std::string JBackendToC::break_multiline_comment(std::string str)
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

void JBackendToC::print_import(ASTNode::ptr node)
{
  print_node_generic(node->children.at(0));
  print_node_generic(node->children.at(1));
  print_node_generic(node->children.at(2));
}

void JBackendToC::print_typedef(ASTNode::ptr node)
{
  std::string name = node->children.at(2)->value;
  std::shared_ptr<Type> type = resolve_type_expression(node->children.at(1));
  type_table.typedefs[name] = type;
}

void JBackendToC::process_definition(ASTNode::ptr node)
{
  switch (node->type) {
  case calc::Parser::symbol_kind_type::S_function_definition:
    print_node_function_definition(node);
    break;
  case calc::Parser::symbol_kind_type::S_type_def:
    print_typedef(node);
    break;
  default:
    printf("Error: Invalid node\n");
    break;
  }
}

void JBackendToC::dump_functions()
{
  for (std::shared_ptr<Function> f : function_table.functions) {
    printf("Declared function %s\n", f->name.c_str());
    printf("Arguments\n");
    for (auto arg : f->arguments) {
      printf("-   %s\n", arg->name.c_str());
    }
  }
}

void JBackendToC::dump_types()
{
  for (auto t : type_table.typedefs) {
    printf("Type %s\n", t.first.c_str());
  }
}


int JBackendToC::process(ASTNode::ptr file)
{


  
  for (auto import_stmt : file->children.at(0)->children) {
    print_import(import_stmt);
  }

  for (auto stmt : file->children.at(1)->children) {
    printf("Node is %d\n", stmt->type);
    process_definition(stmt);
  }
  dump_types();
  dump_functions();
}


// How compile:
// * Gather functions and type definitions
// * Evaluate type definitions to resolve concrete types. (maybe start with types as concrete things before allowing type functions)
// * Resolve function definitions with concrete types instead of type expressions.
// * Evaluate mutations and "side effects" of types to gather function contract summary.
