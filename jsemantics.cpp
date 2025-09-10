#include "jsemantics.hpp"

using namespace JSemantics;

TranslationUnit::TranslationUnit()
{}
TranslationUnit::~TranslationUnit()
{}

void
TranslationUnit::visit(Visitor<TranslationUnit> &visitor)
{
  visitor.visit(*this);
}

FunctionDeclaration::FunctionDeclaration()
{}

FunctionDeclaration::~FunctionDeclaration()
{}

/*
      <node type='type_specifier'>
        <node type='type_access_qualifier'/>
        <node type='type_name'>
          <node type='TYPE_NAME' lineno='10' value='void'>
            <whitespace>&#xA;</whitespace>
            <whitespace>&#xA;</whitespace>
          </node>
        </node>
      </node>
*/
Type::Type()
{}
Type::~Type()
{}

Type::ptr ast_to_type(ASTNode::ptr node)
{
  Type::ptr ret = std::make_shared<Type>();

  ret->name = node->children.at(1)->children.at(0)->value;
  
  return ret;
}
            
void
FunctionDeclaration::visit(Visitor<FunctionDeclaration> &visitor)
{
  visitor.visit(*this);
}
FunctionDeclaration::ptr ast_to_file_statement_function_declaration(ASTNode::ptr node)
{
  FunctionDeclaration::ptr ret = std::make_shared<FunctionDeclaration>();
  
  return ret;
}

FunctionDefinition::FunctionDefinition()
{}
FunctionDefinition::~FunctionDefinition()
{}
void
FunctionDefinition::visit(Visitor<FunctionDefinition> &visitor)
{
  visitor.visit(*this);
}
FunctionDefinition::ptr ast_to_file_statement_function_definition(ASTNode::ptr node)
{
  FunctionDefinition::ptr ret = std::make_shared<FunctionDefinition>();
  ret->function_declaration = std::make_shared<FunctionDeclaration>();
  ret->function_declaration->name = node->children.at(3)->value;
  ret->function_declaration->return_type = ast_to_type(node->children.at(2))->name;
  node->children.at(5);
  ret->function_declaration->arg_types.push_back(std::make_pair("double", "first"));
  ret->function_declaration->arg_types.push_back(std::make_pair("double", "second"));
  return ret;
}

GlobalVariableDefinition::GlobalVariableDefinition()
{}
GlobalVariableDefinition::~GlobalVariableDefinition()
{}
void
GlobalVariableDefinition::visit(Visitor<GlobalVariableDefinition> &visitor)
{
  visitor.visit(*this);
}

GlobalVariableDefinition::ptr
JSemantics::ast_to_file_global_definition(ASTNode::ptr node)
{
  GlobalVariableDefinition::ptr ret = std::make_shared<GlobalVariableDefinition>();
  ret->name = node->children.at(3)->value;
  return ret;
}


TranslationUnit::ptr
JSemantics::from_ast(ASTNode::ptr ast_translation_unit)
{
  TranslationUnit::ptr translation_unit = std::make_shared<TranslationUnit>();

  if (ast_translation_unit->children.size() != 2) {
    fprintf(stderr, "Error: Translation unit does not have any statements\n");
    return nullptr;
  }
  ASTNode::ptr ast_statement_list = ast_translation_unit->children.at(0);
  for (auto statement : ast_statement_list->children) {
    fprintf(stderr, "Statement %s\n", statement->typestr.c_str());
    if (statement->typestr == std::string("file_statement_function_definition")) {
      FunctionDefinition::ptr file_statement_function_definition = ast_to_file_statement_function_definition(statement);
      translation_unit->function_definitions.push_back(file_statement_function_definition);
    }
    else if (statement->typestr == std::string("file_statement_function_declaration")) {
      FunctionDeclaration::ptr file_statement_function_declaration = ast_to_file_statement_function_declaration(statement);
      translation_unit->function_declarations.push_back(file_statement_function_declaration);
    }
    else if (statement->typestr == std::string("class_definition")) {
    }
    else if (statement->typestr == std::string("enum_definition")) {
    }
    else if (statement->typestr == std::string("type_definition")) {
    }
    else if (statement->typestr == std::string("file_statement_namespace")) {
    }
    else if (statement->typestr == std::string("file_statement_using")) {
    }
    else if (statement->typestr == std::string("file_global_definition")) {
      GlobalVariableDefinition::ptr file_global_definition = ast_to_file_global_definition(statement);
      translation_unit->globals.push_back(file_global_definition);
    }
    else {
      fprintf(stderr, "Error: Line %ld : Invalid statement type found in syntax %s\n", statement->lineno, statement->typestr.c_str());
      return nullptr;
    }
  }
  
  return translation_unit;
}

