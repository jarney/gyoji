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
Type::Type(std::string _name, size_t _size)
  : name(_name)
  , size(_size)
{}
Type::~Type()
{}
size_t
Type::get_size_bytes()
{
  return size;
}

Type::ptr
TranslationUnit::ast_to_type(ASTNode::ptr node)
{
  // TODO: We really should be looking this up
  // from the translation unit and returning what
  // we found there.
  printf("Node name %s\n", node->typestr.c_str());
  printf("Node name %s\n", node->children.at(1)->typestr.c_str());
  printf("Node name %s\n", node->children.at(1)->children.at(0)->typestr.c_str());
  auto name = node->children.at(1)->children.at(0)->value;
  printf("Value is %s\n", name.c_str());
  Type::ptr ret = types[name];
  return ret;
}
            
void
FunctionDeclaration::visit(Visitor<FunctionDeclaration> &visitor)
{
  visitor.visit(*this);
}
FunctionDeclaration::ptr
TranslationUnit::ast_to_file_statement_function_declaration(ASTNode::ptr node)
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
FunctionDefinition::ptr
TranslationUnit::ast_to_file_statement_function_definition(ASTNode::ptr node)
{
  FunctionDefinition::ptr ret = std::make_shared<FunctionDefinition>();
  ret->function_declaration = std::make_shared<FunctionDeclaration>();
  ret->function_declaration->name = node->children.at(3)->value;
  ret->function_declaration->return_type = ast_to_type(node->children.at(2))->name;
  node->children.at(5);
  ret->function_declaration->arg_types.push_back(std::make_pair("f64", "first"));
  ret->function_declaration->arg_types.push_back(std::make_pair("i32", "second"));
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
TranslationUnit::ast_to_file_global_definition(ASTNode::ptr node)
{
  GlobalVariableDefinition::ptr ret = std::make_shared<GlobalVariableDefinition>();
  ret->name = node->children.at(3)->value;
  return ret;
}

void
TranslationUnit::register_builtin_types()
{
  types["void"] = std::make_shared<Type>("void", 0);
  
  // Unsigned integers
  types["u8"] = std::make_shared<Type>("u8", 1);
  types["u16"] = std::make_shared<Type>("u16", 2);
  types["u32"] = std::make_shared<Type>("u32", 4);
  types["u64"] = std::make_shared<Type>("u64", 8);

  // Signed integers
  types["i16"] = std::make_shared<Type>("i16", 2);
  types["i32"] = std::make_shared<Type>("i32", 4);
  types["i64"] = std::make_shared<Type>("i64", 8);

  // 32 and 64 bit floating point (float, double respectively)
  types["f32"] = std::make_shared<Type>("f32", 2);
  types["f64"] = std::make_shared<Type>("f64", 4);
  
}

class JSyntaxListener {
public:
  void eventFunctionDefinition(ASTNode::ptr node);
  void eventFunctionDeclaration(ASTNode::ptr node);
  void eventNamespace(ASTNode::ptr node);
};

void
JSyntaxListener::eventFunctionDefinition(ASTNode::ptr node)
{

}

TranslationUnit::ptr
JSemantics::from_ast(ASTNode::ptr ast_translation_unit)
{
  if (ast_translation_unit->children.size() != 2) {
    fprintf(stderr, "Error: Translation unit does not have any statements\n");
    return nullptr;
  }

  // This seems like a bad place to put it,
  // but we need to add built-in types at this
  // level so we can reference them later during the
  // semantic phase.
  TranslationUnit::ptr translation_unit = std::make_shared<TranslationUnit>();

  translation_unit->register_builtin_types();

  // Process types:
  JSyntaxListener listener;
  
  // We need to extract the type information first.
  // This is so that we can use the type definitions
  // anywhere in the code later.
  //extract_types(ast_translation_unit);
  //extract_definitions();
  ASTNode::ptr ast_statement_list = ast_translation_unit->children.at(0);
  for (auto statement : ast_statement_list->children) {
    fprintf(stderr, "Statement %s\n", statement->typestr.c_str());
    if (statement->typestr == std::string("file_statement_function_definition")) {
      FunctionDefinition::ptr file_statement_function_definition = translation_unit->ast_to_file_statement_function_definition(statement);
      translation_unit->function_definitions.push_back(file_statement_function_definition);
    }
    else if (statement->typestr == std::string("file_statement_function_declaration")) {
      FunctionDeclaration::ptr file_statement_function_declaration = translation_unit->ast_to_file_statement_function_declaration(statement);
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
      GlobalVariableDefinition::ptr file_global_definition = translation_unit->ast_to_file_global_definition(statement);
      translation_unit->globals.push_back(file_global_definition);
    }
    else {
      fprintf(stderr, "Error: Line %ld : Invalid statement type found in syntax %s\n", statement->lineno, statement->typestr.c_str());
      return nullptr;
    }
  }
  
  return translation_unit;
}

