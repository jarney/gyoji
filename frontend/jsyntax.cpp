#include <jlang-frontend/jsyntax.hpp>

using namespace JLang::frontend::alt_imp;

///////////////////////////////////////////////////
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

///////////////////////////////////////////////////
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
void
SyntaxNode::prepend_child(const SyntaxNode *node)
{
  children.insert(children.begin(), node);
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

///////////////////////////////////////////////////
AccessModifier::AccessModifier(Terminal::owned_ptr _modifier, AccessModifier::AccessModifierType _type)
  : SyntaxNode("access_modifier", this)
  , modifier(std::move(_modifier))
  , type(_type)
{
  add_child(modifier.get());
}
AccessModifier::~AccessModifier()
{}
const AccessModifier::AccessModifierType &
AccessModifier::get_type() const
{
  return type;
}
///////////////////////////////////////////////////


UnsafeModifier::UnsafeModifier()
  : SyntaxNode("unsafe_modifier", this)
  , unsafe_token(nullptr)
{}
UnsafeModifier::UnsafeModifier(Terminal::owned_ptr _unsafe_token)
  : SyntaxNode("unsafe_modifier", this)
  , unsafe_token(std::move(_unsafe_token))
{
  add_child(unsafe_token.get());
}
UnsafeModifier::~UnsafeModifier()
{}
bool
UnsafeModifier::is_unsafe() const
{
  return unsafe_token.get() != nullptr;
}

///////////////////////////////////////////////////

TypeSpecifier::TypeSpecifier()
  : SyntaxNode("type_specifier", this)
{}
TypeSpecifier::~TypeSpecifier()
{}

///////////////////////////////////////////////////
FunctionDefinitionArg::FunctionDefinitionArg(TypeSpecifier::owned_ptr _type_specifier,
                                             Terminal::owned_ptr _identifier_token
                                             )
  : SyntaxNode("function_definition_arg", this)
  , type_specifier(std::move(_type_specifier))
  , identifier_token(std::move(_identifier_token))
{
  add_child(type_specifier.get());
  add_child(identifier_token.get());
}
FunctionDefinitionArg::~FunctionDefinitionArg()
{}
const TypeSpecifier &
FunctionDefinitionArg::get_type_specifier() const
{ return *type_specifier; }
const std::string &
FunctionDefinitionArg::get_name() const
{ return identifier_token->value; }

///////////////////////////////////////////////////
FunctionDefinitionArgList::FunctionDefinitionArgList()
  : SyntaxNode("function_definition_arg_list", this)
{}
FunctionDefinitionArgList::~FunctionDefinitionArgList()
{}
const std::vector<FunctionDefinitionArg::owned_ptr> &
FunctionDefinitionArgList::get_arguments() const
{ return arguments; }
void
FunctionDefinitionArgList::add_argument(FunctionDefinitionArg::owned_ptr _argument)
{
  arguments.push_back(std::move(_argument));
}


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
StatementVariableDeclaration::StatementVariableDeclaration(
                                   std::unique_ptr<TypeSpecifier> _type_specifier,
                                   Terminal::owned_ptr _identifier_token,
                                   std::unique_ptr<ArrayLength> _array_length,
                                   std::unique_ptr<GlobalInitializer> _global_initializer,
                                   Terminal::owned_ptr _semicolon_token
)
  : SyntaxNode("statement_variable_declaration", this)
  , type_specifier(std::move(_type_specifier))
  , identifier_token(std::move(_identifier_token))
  , array_length(std::move(_array_length))
  , global_initializer(std::move(_global_initializer))
  , semicolon_token(std::move(_semicolon_token))
{
  add_child(type_specifier.get());
  add_child(identifier_token.get());
  add_child(array_length.get());
  add_child(global_initializer.get());
  add_child(semicolon_token.get());
}
StatementVariableDeclaration::~StatementVariableDeclaration()
{}
const TypeSpecifier &
StatementVariableDeclaration::get_type_specifier() const
{ return *type_specifier;}
const std::string &
StatementVariableDeclaration::get_name() const
{ return identifier_token->value; }
const ArrayLength &
StatementVariableDeclaration::get_array_length() const
{ return *array_length; }
const GlobalInitializer &
StatementVariableDeclaration::get_initializer() const
{ return *global_initializer;}
///////////////////////////////////////////////////
StatementBlock::StatementBlock(
                     UnsafeModifier::owned_ptr _unsafe_modifier,
                     ScopeBody::owned_ptr _scope_body
                     )
  : SyntaxNode("statement_block", this)
  , unsafe_modifier(std::move(_unsafe_modifier))
  , scope_body(std::move(_scope_body))
{
  add_child(unsafe_modifier.get());
  add_child(scope_body.get());
}
StatementBlock::~StatementBlock()
{}
const UnsafeModifier &
StatementBlock::get_unsafe_modifier() const
{ return *unsafe_modifier; }
const ScopeBody &
StatementBlock::get_scope_body() const
{ return *scope_body; }
///////////////////////////////////////////////////
StatementExpression::StatementExpression(
                          std::unique_ptr<Expression> _expression,
                          Terminal::owned_ptr _semicolon_token
                          )
  : SyntaxNode("statement_expression", this)
  , expression(std::move(_expression))
  , semicolon_token(std::move(_semicolon_token))
{
  add_child(expression.get());
  add_child(semicolon_token.get());
}
StatementExpression::~StatementExpression()
{}
const Expression &
StatementExpression::get_expression() const
{ return *expression; }
///////////////////////////////////////////////////
StatementGoto::StatementGoto(
                    Terminal::owned_ptr _goto_token,
                    Terminal::owned_ptr _identifier_token,
                    Terminal::owned_ptr _semicolon_token
                    )
  : SyntaxNode("statement_goto", this)
  , goto_token(std::move(_goto_token))
  , identifier_token(std::move(_identifier_token))
  , semicolon_token(std::move(_semicolon_token))
{
  add_child(goto_token.get());
  add_child(identifier_token.get());
  add_child(semicolon_token.get());
}
StatementGoto::~StatementGoto()
{}
const std::string &
StatementGoto::get_label() const
{ return identifier_token->value; }
///////////////////////////////////////////////////
StatementIfElse::StatementIfElse(
                      Terminal::owned_ptr _if_token,
                      Terminal::owned_ptr _paren_l_token,
                      std::unique_ptr<Expression> _expression,
                      Terminal::owned_ptr _paren_r_token,
                      ScopeBody::owned_ptr _if_scope_body,
                      Terminal::owned_ptr _else_token,
                      ScopeBody::owned_ptr _else_scope_body
                                 )                                 
  : SyntaxNode("statement_ifelse", this)
  , m_has_else(true)
  , if_token(std::move(_if_token))
  , paren_l_token(std::move(_paren_l_token))
  , expression(std::move(_expression))
  , paren_r_token(std::move(_paren_r_token))
  , if_scope_body(std::move(_if_scope_body))
  , else_token(std::move(_else_token))
  , else_scope_body(std::move(_else_scope_body))
{
  add_child(if_token.get());
  add_child(paren_l_token.get());
  add_child(expression.get());
  add_child(paren_r_token.get());
  add_child(if_scope_body.get());
  add_child(else_token.get());
  add_child(else_scope_body.get());
}
StatementIfElse::StatementIfElse(
                      Terminal::owned_ptr _if_token,
                      Terminal::owned_ptr _paren_l_token,
                      std::unique_ptr<Expression> _expression,
                      Terminal::owned_ptr _paren_r_token,
                      ScopeBody::owned_ptr _if_scope_body
                                 )
  : SyntaxNode("statement_ifelse", this)
  , m_has_else(false)
  , if_token(std::move(_if_token))
  , paren_l_token(std::move(_paren_l_token))
  , expression(std::move(_expression))
  , paren_r_token(std::move(_paren_r_token))
  , if_scope_body(std::move(_if_scope_body))
  , else_token(nullptr)
  , else_scope_body(nullptr)
{
  add_child(if_token.get());
  add_child(paren_l_token.get());
  add_child(expression.get());
  add_child(paren_r_token.get());
  add_child(if_scope_body.get());
}  
StatementIfElse::~StatementIfElse()
{}
bool
StatementIfElse::has_else() const
{ return m_has_else; }
const Expression &
StatementIfElse::get_expression() const
{ return *expression; }
const ScopeBody &
StatementIfElse::get_if_scope_body() const
{ return *if_scope_body; }
const ScopeBody &
StatementIfElse::get_else_scope_body() const
{ return *else_scope_body; }

///////////////////////////////////////////////////
StatementWhile::StatementWhile(
                     Terminal::owned_ptr _while_token,
                     Terminal::owned_ptr _paren_l_token,
                     std::unique_ptr<Expression> _expression,
                     Terminal::owned_ptr _paren_r_token,
                     std::unique_ptr<ScopeBody> _scope_body
                     )
  : SyntaxNode("statement_while", this)
  , while_token(std::move(_while_token))
  , paren_l_token(std::move(_paren_l_token))
  , expression(std::move(_expression))
  , paren_r_token(std::move(_paren_r_token))
  , scope_body(std::move(_scope_body))
{
  add_child(while_token.get());
  add_child(paren_l_token.get());
  add_child(expression.get());
  add_child(paren_r_token.get());
  add_child(scope_body.get());
}
StatementWhile::~StatementWhile()
{}
const Expression &
StatementWhile::get_expression() const
{ return *expression; }
const ScopeBody &
StatementWhile::get_scope_body() const
{ return *scope_body; }
///////////////////////////////////////////////////
StatementFor::StatementFor(
                   Terminal::owned_ptr _for_token,
                   Terminal::owned_ptr _paren_l_token,
                   std::unique_ptr<Expression> _expression_initial,
                   Terminal::owned_ptr _semicolon_initial,
                   std::unique_ptr<Expression> _expression_termination,
                   Terminal::owned_ptr _semicolon_termination,
                   std::unique_ptr<Expression> _expression_increment,
                   Terminal::owned_ptr _paren_r_token,
                   std::unique_ptr<ScopeBody> _scope_body
  )
  : SyntaxNode("statement_for", this)
  , for_token(std::move(_for_token))
  , paren_l_token(std::move(_paren_l_token))
  , expression_initial(std::move(_expression_initial))
  , semicolon_initial(std::move(_semicolon_initial))
  , expression_termination(std::move(_expression_termination))
  , semicolon_termination(std::move(_semicolon_termination))
  , expression_increment(std::move(_expression_increment))
  , paren_r_token(std::move(_paren_r_token))
  , scope_body(std::move(_scope_body))
{
  add_child(for_token.get());
  add_child(paren_l_token.get());
  add_child(expression_initial.get());
  add_child(semicolon_initial.get());
  add_child(expression_termination.get());
  add_child(semicolon_termination.get());
  add_child(expression_increment.get());
  add_child(paren_r_token.get());
  add_child(scope_body.get());
}
StatementFor::~StatementFor()
{}
const Expression &
StatementFor::get_expression_initial() const
{ return *expression_initial; }
const Expression &
StatementFor::get_expression_termination() const
{ return *expression_termination; }
const Expression &
StatementFor::get_expression_increment() const
{ return *expression_increment; }
const ScopeBody &
StatementFor::get_scope_body() const
{ return *scope_body; }
///////////////////////////////////////////////////
StatementSwitchBlock::StatementSwitchBlock(
                           Terminal::owned_ptr _default_token,
                           Terminal::owned_ptr _colon_token,
                           std::unique_ptr<ScopeBody> _scope_body
                           )
  : SyntaxNode("statement_switch_block", this)
  , m_is_default(true)
  , default_token(std::move(_default_token))
  , colon_token(std::move(_colon_token))
  , scope_body(std::move(_scope_body))
{
  add_child(default_token.get());
  add_child(colon_token.get());
  add_child(scope_body.get());
}
StatementSwitchBlock::StatementSwitchBlock(
                           Terminal::owned_ptr _case_token,
                           std::unique_ptr<Expression> _expression,
                           Terminal::owned_ptr _colon_token,
                           std::unique_ptr<ScopeBody> _scope_body
                           )
  : SyntaxNode("statement_switch_block", this)
  , m_is_default(false)
  , case_token(std::move(_case_token))
  , expression(std::move(_expression))
  , colon_token(std::move(_colon_token))
  , scope_body(std::move(_scope_body))
{
  add_child(case_token.get());
  add_child(expression.get());
  add_child(colon_token.get());
  add_child(scope_body.get());
}
StatementSwitchBlock::~StatementSwitchBlock()
{}
bool
StatementSwitchBlock::is_default() const
{ return m_is_default; }
const Expression &
StatementSwitchBlock::get_expression()
{ return *expression; }
const ScopeBody &
StatementSwitchBlock::get_scope_body()
{ return *scope_body; }

///////////////////////////////////////////////////

StatementSwitchContent::StatementSwitchContent()
  : SyntaxNode("statement_switch_content", this)
{}
StatementSwitchContent::~StatementSwitchContent()
{}
const std::vector<StatementSwitchBlock::owned_ptr> &
StatementSwitchContent::get_blocks() const
{ return blocks; }
void
StatementSwitchContent::add_block(StatementSwitchBlock::owned_ptr _block)
{ blocks.push_back(std::move(_block)); }
///////////////////////////////////////////////////
StatementSwitch::StatementSwitch(
                      Terminal::owned_ptr _switch_token,
                      Terminal::owned_ptr _paren_l_token,
                      std::unique_ptr<Expression> _expression,
                      Terminal::owned_ptr _paren_r_token,
                      Terminal::owned_ptr _brace_l_token,
                      StatementSwitchContent::owned_ptr _switch_content,
                      Terminal::owned_ptr _brace_r_token
                      )
  : SyntaxNode("statement_switch", this)
  , switch_token(std::move(_switch_token))
  , paren_l_token(std::move(_paren_l_token))
  , expression(std::move(_expression))
  , paren_r_token(std::move(_paren_r_token))
  , brace_l_token(std::move(_brace_l_token))
  , switch_content(std::move(_switch_content))
  , brace_r_token(std::move(_brace_r_token))
{
  add_child(switch_token.get());
  add_child(expression.get());
  add_child(paren_l_token.get());
  add_child(expression.get());
  add_child(paren_r_token.get());
  add_child(brace_l_token.get());
  add_child(switch_content.get());
  add_child(brace_r_token.get());
}
StatementSwitch::~StatementSwitch()
{}
const Expression &
StatementSwitch::get_expression() const
{ return *expression; }
const StatementSwitchContent &
StatementSwitch::get_switch_content() const
{ return *switch_content; }
///////////////////////////////////////////////////
StatementReturn::StatementReturn()
  : SyntaxNode("statement_return", this)
{}
StatementReturn::~StatementReturn()
{}
///////////////////////////////////////////////////
StatementContinue::StatementContinue()
  : SyntaxNode("statement_continue", this)
{}
StatementContinue::~StatementContinue()
{}
///////////////////////////////////////////////////
StatementBreak::StatementBreak()
  : SyntaxNode("StatementBreak();", this)
{}
StatementBreak::~StatementBreak()
{}
///////////////////////////////////////////////////
StatementLabel::StatementLabel()
  : SyntaxNode("statement_label", this)
{}
StatementLabel::~StatementLabel()
{}
///////////////////////////////////////////////////
Statement::Statement(StatementType _statement)
  : SyntaxNode("statement", this)
  , statement(std::move(_statement))
{}
Statement::~Statement()
{}
const Statement::StatementType &
Statement::get_statement() const
{ return statement; }

///////////////////////////////////////////////////
StatementList::StatementList()
  : SyntaxNode("statement_list", this)
{}
StatementList::~StatementList()
{}
void
StatementList::add_statement(Statement::owned_ptr _statement)
{
  statements.push_back(std::move(_statement));
}
const std::vector<Statement::owned_ptr> &
StatementList::get_statements() const
{ return statements; }

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
  , bracket_l_token(nullptr)
  , literal_int_token(nullptr)
  , bracket_r_token(nullptr)
{}

ArrayLength::ArrayLength(
                  Terminal::owned_ptr _bracket_l_token,
                  Terminal::owned_ptr _literal_int_token,
                  Terminal::owned_ptr _bracket_r_token
                  )
  : SyntaxNode("array_length", this)
  , bracket_l_token(std::move(_bracket_l_token))
  , literal_int_token(std::move(_literal_int_token))
  , bracket_r_token(std::move(_bracket_r_token))
{
  add_child(bracket_l_token.get());
  add_child(literal_int_token.get());
  add_child(bracket_r_token.get());
}
ArrayLength::~ArrayLength()
{}
bool
ArrayLength::is_array() const
{
  return literal_int_token.get() != nullptr;
}
size_t
ArrayLength::get_size() const
{ return (size_t)atol(literal_int_token->value.c_str());}

///////////////////////////////////////////////////
ClassDeclStart::ClassDeclStart(
                     AccessModifier::owned_ptr _access_modifier,
                     Terminal::owned_ptr _class_token,
                     Terminal::owned_ptr _identifier_token,
                     std::unique_ptr<ClassArgumentList> _class_argument_list
)
  : SyntaxNode("class_decl_start", this)
  , access_modifier(std::move(_access_modifier))
  , class_token(std::move(_class_token))
  , identifier_token(std::move(_identifier_token))
  , class_argument_list(std::move(_class_argument_list))
{
  add_child(access_modifier.get());
  add_child(class_token.get());
  add_child(identifier_token.get());
  add_child(class_argument_list.get());
}
ClassDeclStart::~ClassDeclStart()
{}
const AccessModifier &
ClassDeclStart::get_access_modifier() const
{ return *access_modifier; }
const std::string &
ClassDeclStart::get_name() const
{ return identifier_token->value; }
const ClassArgumentList &
ClassDeclStart::get_argument_list() const
{ return *class_argument_list; }
///////////////////////////////////////////////////
ClassArgumentList::ClassArgumentList(Terminal::owned_ptr _argument)
  : SyntaxNode("class_argument_list", this)
  , paren_l(nullptr)
  , paren_r(nullptr)
{
  add_child(_argument.get());
  argument_list.push_back(std::move(_argument));
}
ClassArgumentList::~ClassArgumentList()
{}
void
ClassArgumentList::add_parens(Terminal::owned_ptr _paren_l, Terminal::owned_ptr _paren_r)
{
  paren_l = std::move(_paren_l);
  prepend_child(paren_l.get());
  
  paren_r = std::move(_paren_r);
  add_child(paren_r.get());
}

void
ClassArgumentList::add_argument(Terminal::owned_ptr _comma_token, Terminal::owned_ptr _argument)
{
  comma_list.push_back(std::move(_comma_token));
  argument_list.push_back(std::move(_argument));
}
const std::vector<Terminal::owned_ptr> &
ClassArgumentList::get_arguments() const
{ return argument_list; }
///////////////////////////////////////////////////
ClassDeclarationMemberList::ClassDeclarationMemberList()
  : SyntaxNode("class_declaration_member_list", this)
{}
ClassDeclarationMemberList::~ClassDeclarationMemberList()
{}
///////////////////////////////////////////////////
ClassDefinition::ClassDefinition(
                      ClassDeclStart::owned_ptr _class_decl_start,
                      Terminal::owned_ptr _brace_l_token,
                      ClassDeclarationMemberList::owned_ptr _class_declaration_member_list,
                      Terminal::owned_ptr _brace_r_token,
                      Terminal::owned_ptr _semicolon_token
                      )
  : SyntaxNode("class_definition", this)
  , class_decl_start(std::move(_class_decl_start))
  , brace_l_token(std::move(_brace_l_token))
  , class_declaration_member_list(std::move(_class_declaration_member_list))
  , brace_r_token(std::move(_brace_r_token))
  , semicolon_token(std::move(_semicolon_token))
{
  add_child(class_decl_start.get());
  add_child(brace_l_token.get());
  add_child(class_declaration_member_list.get());
  add_child(brace_r_token.get());
  add_child(semicolon_token.get());
}
ClassDefinition::~ClassDefinition()
{}
const AccessModifier &
ClassDefinition::get_access_modifier() const
{
  return class_decl_start->get_access_modifier();
}
const std::string &
ClassDefinition::get_name() const
{
  return class_decl_start->get_name();
}
const ClassArgumentList &
ClassDefinition::get_argument_list() const
{
  return class_decl_start->get_argument_list();
}
const ClassDeclarationMemberList &
ClassDefinition::get_members() const
{ return *class_declaration_member_list; }

///////////////////////////////////////////////////
TypeDefinition::TypeDefinition(
                     AccessModifier::owned_ptr _access_modifier,
                     Terminal::owned_ptr _typedef_token,
                     TypeSpecifier::owned_ptr _type_specifier,
                     Terminal::owned_ptr _identifier_token,
                     Terminal::owned_ptr _semicolon_token
                               )
  : SyntaxNode("type_definition", this)
  , access_modifier(std::move(_access_modifier))
  , typedef_token(std::move(_typedef_token))
  , type_specifier(std::move(_type_specifier))
  , identifier_token(std::move(_identifier_token))
  , semicolon_token(std::move(semicolon_token))
{
  add_child(access_modifier.get());
  add_child(typedef_token.get());
  add_child(type_specifier.get());
  add_child(identifier_token.get());
  add_child(semicolon_token.get());
}
TypeDefinition::~TypeDefinition()
{}
const AccessModifier &
TypeDefinition::get_access_modifier() const
{ return *access_modifier; }
const std::string &
TypeDefinition::get_name() const
{ return identifier_token->value; }
const TypeSpecifier &
TypeDefinition::get_type_specifier() const
{ return *type_specifier; }

///////////////////////////////////////////////////
EnumDefinitionValue::EnumDefinitionValue(
                          Terminal::owned_ptr _identifier_token,
                          Terminal::owned_ptr _equals_token,
                          std::unique_ptr<ExpressionPrimary> _expression_primary,
                          Terminal::owned_ptr _semicolon_token
                          )
  : SyntaxNode("enum_definition_value", this)
  , identifier_token(std::move(_identifier_token))
  , equals_token(std::move(_equals_token))
  , expression_primary(std::move(_expression_primary))
  , semicolon_token(std::move(_semicolon_token))
{
  add_child(identifier_token.get());
  add_child(equals_token.get());
  add_child(expression_primary.get());
  add_child(semicolon_token.get());
}
EnumDefinitionValue::~EnumDefinitionValue()
{}
const std::string &
EnumDefinitionValue::get_name() const
{ return identifier_token->value; }

const ExpressionPrimary &
EnumDefinitionValue::get_expression() const
{ return *expression_primary; }

///////////////////////////////////////////////////
EnumDefinitionValueList::EnumDefinitionValueList()
  : SyntaxNode("enum_definition_value_list", this)
{}

EnumDefinitionValueList::~EnumDefinitionValueList()
{}
void
EnumDefinitionValueList::add_value(EnumDefinitionValue::owned_ptr _value)
{
  add_child(_value.get());
  values.push_back(std::move(_value));
}
const std::vector<EnumDefinitionValue::owned_ptr> &
EnumDefinitionValueList::get_values() const
{ return values; }

///////////////////////////////////////////////////
EnumDefinition::EnumDefinition(
                     AccessModifier::owned_ptr _access_modifier,
                     Terminal::owned_ptr _enum_token,
                     Terminal::owned_ptr _type_name_token,
                     Terminal::owned_ptr _identifier_token,
                     Terminal::owned_ptr _brace_l_token,
                     EnumDefinitionValueList::owned_ptr _enum_value_list,
                     Terminal::owned_ptr _brace_r_token,
                     Terminal::owned_ptr _semicolon_token
                               )
: SyntaxNode("enum_definition", this)
, access_modifier(std::move(_access_modifier))
, enum_token(std::move(_enum_token))
, type_name_token(std::move(_type_name_token))
, identifier_token(std::move(_identifier_token))
, brace_l_token(std::move(_brace_l_token))
, enum_value_list(std::move(_enum_value_list))
, brace_r_token(std::move(_brace_r_token))
, semicolon_token(std::move(_semicolon_token))
{
  add_child(access_modifier.get());
  add_child(enum_token.get());
  add_child(type_name_token.get());
  add_child(identifier_token.get());
  add_child(brace_l_token.get());
  add_child(enum_value_list.get());
  add_child(brace_r_token.get());
  add_child(semicolon_token.get());
}
EnumDefinition::~EnumDefinition()
{}
const AccessModifier &
EnumDefinition::get_access_modifier() const
{ return *access_modifier; }
const std::string &
EnumDefinition::type_name() const
{ return type_name_token->value; }
const std::string &
EnumDefinition::enum_name() const
{ return identifier_token->value; }
const EnumDefinitionValueList &
EnumDefinition::get_value_list() const
{ return *enum_value_list; }
///////////////////////////////////////////////////
Expression::Expression()
  : SyntaxNode("expression", this)
{}
Expression::~Expression()
{}
///////////////////////////////////////////////////

///////////////////////////////////////////////////
ExpressionPrimary::ExpressionPrimary()
  : SyntaxNode("expression_primary", this)
{}
ExpressionPrimary::~ExpressionPrimary()
{}

///////////////////////////////////////////////////

GlobalInitializerExpressionPrimary::GlobalInitializerExpressionPrimary(
                                         Terminal::owned_ptr _equals_token,
                                         ExpressionPrimary::owned_ptr _expression
                                         )
  : SyntaxNode("global_initializer_expression_primary", this)
  , equals_token(std::move(_equals_token))
  , expression(std::move(_expression))
{
  add_child(equals_token.get());
  add_child(expression.get());
}
GlobalInitializerExpressionPrimary::~GlobalInitializerExpressionPrimary()
{}
const ExpressionPrimary &
GlobalInitializerExpressionPrimary::get_expression() const
{ return *expression; };

///////////////////////////////////////////////////
StructInitializer::StructInitializer(
                        Terminal::owned_ptr _dot_token,
                        Terminal::owned_ptr _identifier_token,
                        GlobalInitializer::owned_ptr _global_initializer,
                        Terminal::owned_ptr _semicolon_token
                                     )
  : SyntaxNode("struct_initializer", this)
  , dot_token(std::move(_dot_token))
  , identifier_token(std::move(_identifier_token))
  , global_initializer(std::move(_global_initializer))
  , semicolon_token(std::move(_semicolon_token))
{
  add_child(dot_token.get());
  add_child(identifier_token.get());
  add_child(global_initializer.get());
  add_child(semicolon_token.get());
}
StructInitializer::~StructInitializer()
{}
const GlobalInitializer &
StructInitializer::get_initializer() const
{ return *global_initializer; }

///////////////////////////////////////////////////
StructInitializerList::StructInitializerList()
  : SyntaxNode("struct_initializer_list", this)
{}
StructInitializerList::~StructInitializerList()
{}
void
StructInitializerList::add_initializer(StructInitializer::owned_ptr initializer)
{
  initializers.push_back(std::move(initializer));
}
const std::vector<StructInitializer::owned_ptr> &
StructInitializerList::get_initializers() const
{
  return initializers;
}

///////////////////////////////////////////////////
GlobalInitializerAddressofExpressionPrimary::GlobalInitializerAddressofExpressionPrimary(
                                                                                         Terminal::owned_ptr _equals_token,
                                                                                         Terminal::owned_ptr _addressof_token,
                                                                                         ExpressionPrimary::owned_ptr _expression
                                                                                         )
  : SyntaxNode("global_initializer_addressof_expression_primary", this)
  , equals_token(std::move(_equals_token))
  , addressof_token(std::move(_addressof_token))
  , expression(std::move(_expression))
{
  add_child(equals_token.get());
  add_child(addressof_token.get());
  add_child(expression.get());
}
GlobalInitializerAddressofExpressionPrimary::~GlobalInitializerAddressofExpressionPrimary()
{}
const ExpressionPrimary &
GlobalInitializerAddressofExpressionPrimary::get_expression() const
{ return *expression; };


///////////////////////////////////////////////////
GlobalInitializerStructInitializerList::GlobalInitializerStructInitializerList(
                                             Terminal::owned_ptr _equals_token,
                                             Terminal::owned_ptr _brace_l_token,
                                             StructInitializerList::owned_ptr _struct_initializer,
                                             Terminal::owned_ptr _brace_r_token
)
  : SyntaxNode("global_initializer_struct_initializer_list", this)
  , equals_token(std::move(_equals_token))
  , brace_l_token(std::move(_brace_l_token))
  , struct_initializer(std::move(_struct_initializer))
  , brace_r_token(std::move(_brace_r_token))
{
  add_child(equals_token.get());
  add_child(brace_l_token.get());
  add_child(struct_initializer.get());
  add_child(brace_r_token.get());
}
GlobalInitializerStructInitializerList::~GlobalInitializerStructInitializerList()
{}
const StructInitializerList &
GlobalInitializerStructInitializerList::get_struct_initializer() const
{ return *struct_initializer; }

///////////////////////////////////////////////////
GlobalInitializer::GlobalInitializer(GlobalInitializer::GlobalInitializerType _initializer, SyntaxNode *raw_ptr)
  : SyntaxNode("global_initializer", this)
  , initializer(std::move(_initializer))
{
  add_child(raw_ptr);
}
GlobalInitializer::~GlobalInitializer()
{}
const GlobalInitializer::GlobalInitializerType &
GlobalInitializer::get_initializer() const
{ return initializer; }


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

///////////////////////////////////////////////////
NamespaceDeclaration::NamespaceDeclaration(
                           AccessModifier::owned_ptr _access_modifier,
                           Terminal::owned_ptr _namespace_token,
                           Terminal::owned_ptr _identifier_token
                           )
  : SyntaxNode("namespace_declaration", this)
  , access_modifier(std::move(_access_modifier))
  , namespace_token(std::move(_namespace_token))
  , identifier_token(std::move(_identifier_token))
{
  add_child(access_modifier.get());
  add_child(namespace_token.get());
  add_child(identifier_token.get());
}
NamespaceDeclaration::~NamespaceDeclaration()
{}
const AccessModifier &
NamespaceDeclaration::get_access_modifier() const
{ return *access_modifier; }
const Terminal &
NamespaceDeclaration::get_name() const
{ return *identifier_token; }

///////////////////////////////////////////////////
FileStatementNamespace::FileStatementNamespace(
                                               NamespaceDeclaration::owned_ptr _namespace_declaration,
                                               Terminal::owned_ptr _brace_l_token,
                                               FileStatementList::owned_ptr _file_statement_list,
                                               Terminal::owned_ptr _brace_r_token,
                                               Terminal::owned_ptr _semicolon_token
                                               )
  : SyntaxNode("file_statement_namespace", this)
  , namespace_declaration(std::move(_namespace_declaration))
  , brace_l_token(std::move(_brace_l_token))
  , file_statement_list(std::move(file_statement_list))
  , brace_r_token(std::move(_brace_r_token))
  , semicolon_token(std::move(_semicolon_token))
{
  add_child(namespace_declaration.get());
  add_child(brace_l_token.get());
  add_child(file_statement_list.get());
  add_child(brace_r_token.get());
  add_child(semicolon_token.get());
}
FileStatementNamespace::~FileStatementNamespace()
{}
const NamespaceDeclaration & FileStatementNamespace::get_declaration() const
{ return *namespace_declaration;}

const FileStatementList & FileStatementNamespace::get_statement_list() const
{ return *file_statement_list;}

///////////////////////////////////////////////////

UsingAs::UsingAs(
        Terminal::owned_ptr _as_token,
        Terminal::owned_ptr _identifier_token
        )
  : SyntaxNode("using_as", this)
  , as_token(std::move(_as_token))
  , identifier_token(std::move(_identifier_token))
{
  using_name = identifier_token->value;
  add_child(as_token.get());
  add_child(identifier_token.get());
}
UsingAs::UsingAs()
  : SyntaxNode("using_as", this)
  , as_token(nullptr)
  , identifier_token(nullptr)
{
  using_name = "";
}
UsingAs::~UsingAs()
{}
const std::string &
UsingAs::get_using_name() const
{ return using_name; }

FileStatementUsing::FileStatementUsing(AccessModifier::owned_ptr _access_modifier,
                             Terminal::owned_ptr _using,
                             Terminal::owned_ptr _namespace,
                             Terminal::owned_ptr _namespace_name,
                             UsingAs::owned_ptr _using_as,
                             Terminal::owned_ptr _semicolon)
  : SyntaxNode("file_statement_using", this)
  , using_token(std::move(_using))
  , namespace_token(std::move(_namespace))
  , namespace_name_token(std::move(_namespace_name))
  , using_as(std::move(_using_as))
  , semicolon_token(std::move(_semicolon))
{
  add_child(using_token.get());
  add_child(namespace_token.get());
  add_child(namespace_name_token.get());
  add_child(using_as.get());
  add_child(semicolon_token.get());
}
FileStatementUsing::~FileStatementUsing()
{}
const AccessModifier &
FileStatementUsing::get_access_modifier() const
{ return *access_modifier; }
std::string &
FileStatementUsing::get_namespace() const
{ return namespace_name_token->fully_qualified_name; }
const UsingAs &
FileStatementUsing::get_using_as() const
{ return *using_as; }


FileStatement::FileStatement(FileStatementType _statement, SyntaxNode *raw_ptr)
  : SyntaxNode("file_statement", this)
  , statement(std::move(_statement))
{
  add_child(raw_ptr);
}
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
