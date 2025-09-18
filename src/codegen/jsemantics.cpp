#include <jlang-codegen/jsemantics.hpp>

using namespace JLang::codegen::semantics;

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
semantics::ast_to_type(TranslationUnit::ptr translation_unit, ASTNode::ptr node)
{
  // TODO: We really should be looking this up
  // from the translation unit and returning what
  // we found there.
  auto name = node->children.at(1)->children.at(0)->fully_qualified_name;
  Type::ptr ret = translation_unit->types[name];
  return ret;
}
            
void
FunctionDeclaration::visit(Visitor<FunctionDeclaration> &visitor)
{
  visitor.visit(*this);
}
FunctionDeclaration::ptr
semantics::ast_to_file_statement_function_declaration(TranslationUnit::ptr translation_unit, ASTNode::ptr node)
{
  FunctionDeclaration::ptr ret = std::make_shared<FunctionDeclaration>();
  
  return ret;
}

FunctionDeclarationArg::FunctionDeclarationArg()
{}
FunctionDeclarationArg::~FunctionDeclarationArg()
{}
FunctionDeclarationArg::ptr
semantics::ast_to_function_declaration_arg(TranslationUnit::ptr translation_unit, ASTNode::ptr node)
{
  FunctionDeclarationArg::ptr ret = std::make_shared<FunctionDeclarationArg>();

  ret->type = ast_to_type(translation_unit, node->children.at(0));
  ret->name = node->children.at(1)->value;
  printf("Argument parsed %s\n", ret->name.c_str());
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
semantics::ast_to_file_statement_function_definition(TranslationUnit::ptr translation_unit, ASTNode::ptr node)
{
  FunctionDefinition::ptr ret = std::make_shared<FunctionDefinition>();
  ret->function_declaration = std::make_shared<FunctionDeclaration>();
  ret->function_declaration->name = node->children.at(3)->value;
  ret->function_declaration->return_type = ast_to_type(translation_unit, node->children.at(2));
  
  std::list<ASTNode::ptr> args = get_children_by_type(node->children.at(5), "function_definition_arg");
  for (ASTNode::ptr arg : args) {
    FunctionDeclarationArg::ptr farg = ast_to_function_declaration_arg(translation_unit, arg);
    ret->function_declaration->arg_types.push_back(farg);
  }

  ret->scope_body = ast_to_scope_body(translation_unit, node->children.at(7));
  
  return ret;
}
Statement::Statement()
{}
Statement::~Statement()
{}

StatementVariableDeclaration::StatementVariableDeclaration()
{}
StatementVariableDeclaration::~StatementVariableDeclaration()
{}

Statement::ptr
semantics::ast_to_statement(TranslationUnit::ptr translation_unit, ASTNode::ptr node)
{
  StatementVariableDeclaration::ptr ret = std::make_shared<StatementVariableDeclaration>();
  return ret;
}

ScopeBody::ScopeBody()
{}
ScopeBody::~ScopeBody()
{}

ScopeBody::ptr
semantics::ast_to_scope_body(TranslationUnit::ptr translation_unit, ASTNode::ptr node)
{
  ScopeBody::ptr ret = std::make_shared<ScopeBody>();

  ASTNode::ptr statement_list_node = node->children.at(2);
  
  for (ASTNode::ptr statement_node : statement_list_node->children) {
    Statement::ptr statement = ast_to_statement(translation_unit, statement_node);
    ret->statements.push_back(statement);
  }

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
semantics::ast_to_file_global_definition(TranslationUnit::ptr translation_unit, ASTNode::ptr node)
{
  GlobalVariableDefinition::ptr ret = std::make_shared<GlobalVariableDefinition>();
  ret->name = node->children.at(3)->value;
  return ret;
}

void
TranslationUnit::register_builtin_types()
{
  types["::void"] = std::make_shared<Type>("::void", 0);
  
  // Unsigned integers
  types["::u8"] = std::make_shared<Type>("::u8", 1);
  types["::u16"] = std::make_shared<Type>("::u16", 2);
  types["::u32"] = std::make_shared<Type>("::u32", 4);
  types["::u64"] = std::make_shared<Type>("::u64", 8);

  // Signed integers
  types["::i16"] = std::make_shared<Type>("::i16", 2);
  types["::i32"] = std::make_shared<Type>("::i32", 4);
  types["::i64"] = std::make_shared<Type>("::i64", 8);

  // 32 and 64 bit floating point (float, double respectively)
  types["::f32"] = std::make_shared<Type>("::f32", 2);
  types["::f64"] = std::make_shared<Type>("::f64", 4);
  
}

namespace JLang::codegen {
  class JSyntaxListener {
public:
  void eventFunctionDefinition(ASTNode::ptr node);
  void eventFunctionDeclaration(ASTNode::ptr node);
  void eventNamespace(ASTNode::ptr node);
};
};

void
JSyntaxListener::eventFunctionDefinition(ASTNode::ptr node)
{

}

std::list<ASTNode::ptr>
semantics::get_children_by_type(ASTNode::ptr node, std::string type)
{
  std::list<ASTNode::ptr> found;
  for (ASTNode::ptr child : node->children) {
    if (child->typestr == type) {
      found.push_back(child);
    }
  }
  return found;
}

int
semantics::process_type_definition(TranslationUnit::ptr translation_unit, ASTNode::ptr ast_typedef)
{
  Type::ptr type = ast_to_type(translation_unit, ast_typedef->children.at(2));
  std::string ns = ast_typedef->children.at(3)->fully_qualified_name;
  std::string name = ns + std::string("::") + ast_typedef->children.at(3)->value;
  translation_unit->types[name] = type;
  return 0;
}

int
semantics::file_statement_process(TranslationUnit::ptr translation_unit, ASTNode::ptr ast_statement_list)
{
  for (auto statement : ast_statement_list->children) {
    fprintf(stderr, "Statement %s\n", statement->typestr.c_str());
    if (statement->typestr == std::string("file_statement_function_definition")) {
      FunctionDefinition::ptr file_statement_function_definition = ast_to_file_statement_function_definition(translation_unit, statement);
      translation_unit->function_definitions.push_back(file_statement_function_definition);
    }
    else if (statement->typestr == std::string("file_statement_function_declaration")) {
      FunctionDeclaration::ptr file_statement_function_declaration = ast_to_file_statement_function_declaration(translation_unit, statement);
      translation_unit->function_declarations.push_back(file_statement_function_declaration);
    }
    else if (statement->typestr == std::string("class_definition")) {
    }
    else if (statement->typestr == std::string("enum_definition")) {
    }
    else if (statement->typestr == std::string("type_definition")) {
      int rc = process_type_definition(translation_unit, statement);
      if (rc) return rc;
    }
    else if (statement->typestr == std::string("file_statement_namespace")) {
      int rc = file_statement_process(translation_unit, statement->children.at(2));
      if (rc) return rc;
    }
    else if (statement->typestr == std::string("file_statement_using")) {
      // There are no semantics associated with this node,
      // this is used only for name and type resolution
      // at the syntax and lexical layer.
    }
    else if (statement->typestr == std::string("file_global_definition")) {
      GlobalVariableDefinition::ptr file_global_definition = ast_to_file_global_definition(translation_unit, statement);
      translation_unit->globals.push_back(file_global_definition);
    }
    else {
      fprintf(stderr, "Error: Line %ld : Invalid statement type found in syntax %s\n", statement->lineno, statement->typestr.c_str());
      return -1;
    }
  }
  return 0;
}

TranslationUnit::ptr
semantics::from_ast(ASTNode::ptr ast_translation_unit)
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

  int rc = file_statement_process(translation_unit, ast_statement_list);
  if (rc) {
    printf("Error processing\n");
  }
  
  
  return translation_unit;
}

