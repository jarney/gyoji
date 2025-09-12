#include <jlang-frontend/jsyntax.hpp>

using namespace JLang::frontend::alt_imp;

Terminal::Terminal()
  : SyntaxNode("terminal", this)
{}
Terminal::~Terminal()
{}
std::vector<TerminalNonSyntax::raw_ptr> Terminal::get_whitespace()
{
  return std::vector<TerminalNonSyntax::raw_ptr>();
}
TerminalNonSyntax::TerminalNonSyntax(TerminalNonSyntaxType _type, std::string _data)
  : type(_type)
  , data(_data)
{}
TerminalNonSyntax::~TerminalNonSyntax()
{}
const TerminalNonSyntaxType &
TerminalNonSyntax::get_type() const
{
  return type;
}
const std::string & TerminalNonSyntax::get_data() const
{
  return data;
}

SyntaxNode::SyntaxNode(std::string _type, SyntaxNode::specific_type_t _data)
  : type(_type)
  , data(_data)
{}
SyntaxNode::~SyntaxNode()
{}
void
SyntaxNode::add_child(const SyntaxNode* node)
{
  children.push_back(node);
}
const std::vector<const SyntaxNode*> &
SyntaxNode::get_children() const
{
  return children;
}
const std::string & SyntaxNode::get_type() const
{
  return type;
}
const SyntaxNode::specific_type_t &SyntaxNode::get_data() const
{
  return data;
}

AccessModifier::AccessModifier()
  : SyntaxNode("access_modifier", this)
{}
AccessModifier::~AccessModifier()
{}

UnsafeModifier::UnsafeModifier()
  : SyntaxNode("unsafe_modifier", this)
{}
UnsafeModifier::~UnsafeModifier()
{}

TypeSpecifier::TypeSpecifier()
  : SyntaxNode("type_specifier", this)
{}
TypeSpecifier::~TypeSpecifier()
{}

FunctionDefinitionArgList::FunctionDefinitionArgList()
  : SyntaxNode("function_definition_arg_list", this)
{}
FunctionDefinitionArgList::~FunctionDefinitionArgList()
{}



///////////////////////////////////////////////////
FileStatementFunctionDeclaration::FileStatementFunctionDeclaration(
    AccessModifier::owned_ptr _access_modifier,
    UnsafeModifier::owned_ptr _unsafe_modifier,
    TypeSpecifier::owned_ptr _type_specifier,
    Terminal::owned_ptr _name,
    Terminal::owned_ptr _paren_l,
    FunctionDefinitionArgList::owned_ptr _arguments,
    Terminal::owned_ptr _paren_r,
    Terminal::owned_ptr _semicolon
)
  : SyntaxNode("file_statement_function_declaration", this)
  , access_modifier(std::move(_access_modifier))
  , unsafe_modifier(std::move(_unsafe_modifier))
  , type_specifier(std::move(_type_specifier))
  , name(std::move(_name))
  , paren_l(std::move(_paren_l))
  , arguments(std::move(_arguments))
  , paren_r(std::move(_paren_r))
  , semicolon(std::move(_semicolon))
{
  add_child(access_modifier.get());
  add_child(unsafe_modifier.get());
  add_child(type_specifier.get());
  add_child(name.get());
  add_child(paren_l.get());
  add_child(arguments.get());
  add_child(paren_r.get());
  add_child(semicolon.get());
}
FileStatementFunctionDeclaration::~FileStatementFunctionDeclaration()
{}
const AccessModifier &
FileStatementFunctionDeclaration::get_access_modifier() const
{
  return *access_modifier;
}
const UnsafeModifier &
FileStatementFunctionDeclaration::get_unsafe_modifier() const
{
  return *unsafe_modifier;
}
const TypeSpecifier &
FileStatementFunctionDeclaration::get_type_specifier() const
{
  return *type_specifier;
}
const Terminal &
FileStatementFunctionDeclaration::get_name() const { return *name; }
 
const FunctionDefinitionArgList &
FileStatementFunctionDeclaration::get_arguments() const
{ return *arguments; }

///////////////////////////////////////////////////
ScopeBody::ScopeBody()
  : SyntaxNode("scope_body", this)
{}
ScopeBody::~ScopeBody()
{}
///////////////////////////////////////////////////
FileStatementFunctionDefinition::FileStatementFunctionDefinition(
    AccessModifier::owned_ptr _access_modifier,
    UnsafeModifier::owned_ptr _unsafe_modifier,
    TypeSpecifier::owned_ptr _type_specifier,
    Terminal::owned_ptr _name,
    Terminal::owned_ptr _paren_l,
    FunctionDefinitionArgList::owned_ptr _arguments,
    Terminal::owned_ptr _paren_r,
    ScopeBody::owned_ptr _scope_body
)
  : SyntaxNode("file_statement_function_declaration", this)
  , access_modifier(std::move(_access_modifier))
  , unsafe_modifier(std::move(_unsafe_modifier))
  , type_specifier(std::move(_type_specifier))
  , name(std::move(_name))
  , paren_l(std::move(_paren_l))
  , arguments(std::move(_arguments))
  , paren_r(std::move(_paren_r))
  , scope_body(std::move(_scope_body))
{
  add_child(access_modifier.get());
  add_child(unsafe_modifier.get());
  add_child(type_specifier.get());
  add_child(name.get());
  add_child(paren_l.get());
  add_child(arguments.get());
  add_child(paren_r.get());
  add_child(scope_body.get());
}
FileStatementFunctionDefinition::~FileStatementFunctionDefinition()
{}
const AccessModifier &
FileStatementFunctionDefinition::get_access_modifier() const
{
  return *access_modifier;
}
const UnsafeModifier &
FileStatementFunctionDefinition::get_unsafe_modifier() const
{
  return *unsafe_modifier;
}
const TypeSpecifier &
FileStatementFunctionDefinition::get_type_specifier() const
{
  return *type_specifier;
}
const Terminal &
FileStatementFunctionDefinition::get_name() const { return *name; }
 
const FunctionDefinitionArgList &
FileStatementFunctionDefinition::get_arguments() const
{ return *arguments; }

const ScopeBody &
FileStatementFunctionDefinition::get_scope_body() const
{ return *scope_body; }

///////////////////////////////////////////////////

ArrayLength::ArrayLength()
  : SyntaxNode("array_length", this)
{}
ArrayLength::~ArrayLength()
{}
GlobalInitializer::GlobalInitializer()
  : SyntaxNode("global_initializer", this)
{}
GlobalInitializer::~GlobalInitializer()
{}


///////////////////////////////////////////////////
FileStatementGlobalDefinition::FileStatementGlobalDefinition(
                                                             AccessModifier::owned_ptr _access_modifier,
                                                             UnsafeModifier::owned_ptr _unsafe_modifier,
                                                             TypeSpecifier::owned_ptr _type_specifier,
                                                             Terminal::owned_ptr _name,
                                                             ArrayLength::owned_ptr _array_length,
                                                             GlobalInitializer::owned_ptr _global_initializer,
                                                             Terminal::owned_ptr _semicolon
                                                             )
: SyntaxNode("file_statement_global_definition", this)
, access_modifier(std::move(_access_modifier))
, unsafe_modifier(std::move(_unsafe_modifier))
, type_specifier(std::move(_type_specifier))
, name(std::move(_name))
, array_length(std::move(_array_length))
, global_initializer(std::move(_global_initializer))
, semicolon(std::move(_semicolon))
{
  add_child(access_modifier.get());
  add_child(unsafe_modifier.get());
  add_child(type_specifier.get());
  add_child(name.get());
  add_child(array_length.get());
  add_child(global_initializer.get());
  add_child(semicolon.get());
}

FileStatementGlobalDefinition::~FileStatementGlobalDefinition()
{}

const AccessModifier &
FileStatementGlobalDefinition::get_access_modifier() const
{ return *access_modifier; }
const UnsafeModifier &
FileStatementGlobalDefinition::get_unsafe_modifier() const
{ return *unsafe_modifier; }
const TypeSpecifier &
FileStatementGlobalDefinition::get_type_specifier() const
{ return *type_specifier; }
const Terminal &
FileStatementGlobalDefinition::get_name() const
{ return *name; }
const ArrayLength &
FileStatementGlobalDefinition::get_array_length() const
{ return *array_length; }
const GlobalInitializer &
FileStatementGlobalDefinition::get_global_initializer() const
{ return *global_initializer; }


FileStatement::FileStatement(FileStatementType _statement)
  : SyntaxNode("file_statement", this)
  , statement(std::move(_statement))
{}
FileStatement::~FileStatement()
{}
const FileStatement::FileStatementType &
FileStatement::get_statement() const
{
  return statement;
}


FileStatementList::FileStatementList()
  : SyntaxNode("file_statement_list", this)
{}
FileStatementList::~FileStatementList()
{}
const std::vector<FileStatement::owned_ptr> &
FileStatementList::get_statements() const
{ return statements; }
void
FileStatementList::add_statement(FileStatement::owned_ptr statement)
{
  statements.push_back(std::move(statement));
}

TranslationUnit::TranslationUnit(
                                 FileStatementList::owned_ptr _file_statement_list,
                                 Terminal::owned_ptr _yyeof_token)
  : SyntaxNode("translation_unit", this)
  , file_statement_list(std::move(_file_statement_list))
  , yyeof_token(std::move(_yyeof_token))
{
  add_child(file_statement_list.get());
  add_child(yyeof_token.get());
}
TranslationUnit::~TranslationUnit()
{}

FileStatementList::raw_ptr
TranslationUnit::get_statements()
{
  return file_statement_list.get();
}
