#include <jlang-frontend.hpp>

using namespace JLang::frontend;
using namespace JLang::frontend::ast;
using namespace JLang::frontend::tree;

///////////////////////////////////////////////////
Terminal::Terminal(const Token & _token)
  : SyntaxNode("terminal", this)
  , token(_token)
{}
Terminal::~Terminal()
{}
const std::string &
Terminal::get_type() const
{ return token.get_type(); }
const std::string &
Terminal::get_value() const
{ return token.get_value(); }
const size_t
Terminal::get_line() const
{ return token.get_line(); }
const size_t
Terminal::get_column() const
{ return token.get_column(); }

const std::string &
Terminal::get_fully_qualified_name() const
{ return fully_qualified_name; }
void
Terminal::set_fully_qualified_name(std::string _name)
{ fully_qualified_name = _name; }

///////////////////////////////////////////////////


TerminalNonSyntax::TerminalNonSyntax(TerminalNonSyntax::Type _type, std::string _data)
  : type(_type)
  , data(_data)
{}
TerminalNonSyntax::~TerminalNonSyntax()
{}
const TerminalNonSyntax::Type &
TerminalNonSyntax::get_type() const
{
  return type;
}
const std::string & TerminalNonSyntax::get_data() const
{
  return data;
}
void TerminalNonSyntax::append(std::string _data)
{
  data = data + _data;
}

///////////////////////////////////////////////////
AccessQualifier::AccessQualifier(AccessQualifier::AccessQualifierType _type)
  : SyntaxNode("access_qualifier", this)
  , type(_type)
  , qualifier(nullptr)
{}
AccessQualifier::AccessQualifier(Terminal_owned_ptr _qualifier, AccessQualifierType _type)
  : SyntaxNode("access_qualifier", this)
  , type(_type)
  , qualifier(std::move(_qualifier))
{
  add_child(*qualifier);
}
AccessQualifier::~AccessQualifier()
{}
const AccessQualifier::AccessQualifierType &
AccessQualifier::get_type() const
{ return type; }

///////////////////////////////////////////////////
AccessModifier::AccessModifier(Terminal_owned_ptr _modifier, AccessModifier::AccessModifierType _type)
  : SyntaxNode("access_modifier", this)
  , modifier(std::move(_modifier))
  , type(_type)
{
  add_child(*modifier);
}
AccessModifier::AccessModifier(AccessModifier::AccessModifierType _type)
  : SyntaxNode("access_modifier", this)
  , modifier(nullptr)
  , type(_type)
{}
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
UnsafeModifier::UnsafeModifier(Terminal_owned_ptr _unsafe_token)
  : SyntaxNode("unsafe_modifier", this)
  , unsafe_token(std::move(_unsafe_token))
{
  add_child(*unsafe_token);
}
UnsafeModifier::~UnsafeModifier()
{}
bool
UnsafeModifier::is_unsafe() const
{
  return unsafe_token.get() != nullptr;
}

///////////////////////////////////////////////////
TypeName::TypeName(Terminal_owned_ptr _type_name)
  : SyntaxNode("type_name", this)
  , m_is_expression(false)
  , type_name(std::move(_type_name))
  , typeof_token(nullptr)
  , paren_l_token(nullptr)
  , expression(nullptr)
  , paren_r_token(nullptr)
{
  add_child(*type_name);
}
TypeName::TypeName(Terminal_owned_ptr _typeof_token,
               Terminal_owned_ptr _paren_l_token,
               Expression_owned_ptr _expression,
               Terminal_owned_ptr _paren_r_token
               )
  : SyntaxNode("type_name", this)
  , m_is_expression(true)
  , type_name(nullptr)
  , typeof_token(std::move(_typeof_token))
  , paren_l_token(std::move(_paren_l_token))
  , expression(std::move(_expression))
  , paren_r_token(std::move(_paren_r_token))
{
  add_child(*typeof_token);
  add_child(*paren_l_token);
  add_child(*expression);
  add_child(*paren_r_token);
}
TypeName::~TypeName()
{}
bool
TypeName::is_expression() const
{ return m_is_expression; }
const std::string &
TypeName::get_name() const
{ return type_name->get_fully_qualified_name(); }
const Expression &
TypeName::get_expression() const
{ return *expression; }
///////////////////////////////////////////////////
TypeSpecifierCallArgs::TypeSpecifierCallArgs()
  : SyntaxNode("type_specifier_call_args", this)
{}
TypeSpecifierCallArgs::~TypeSpecifierCallArgs()
{}
const std::vector<TypeSpecifier_owned_ptr> &
TypeSpecifierCallArgs::get_arguments() const
{ return arguments; }
void
TypeSpecifierCallArgs::add_argument(TypeSpecifier_owned_ptr _argument)
{
  add_child(*_argument);
  arguments.push_back(std::move(_argument));
}
void
TypeSpecifierCallArgs::add_argument(Terminal_owned_ptr _comma_token, TypeSpecifier_owned_ptr _argument)
{
  add_child(*_comma_token);
  add_child(*_argument);
  comma_list.push_back(std::move(_comma_token));
  arguments.push_back(std::move(_argument));
}

///////////////////////////////////////////////////
TypeSpecifierSimple::TypeSpecifierSimple(
                                         AccessQualifier_owned_ptr _access_qualifier,
                                         TypeName_owned_ptr _type_name
                                         )
  : SyntaxNode("type_specifier_simple", this)
  , access_qualifier(std::move(_access_qualifier))
  , type_name(std::move(_type_name))
{
  add_child(*access_qualifier);
  add_child(*type_name);
}
TypeSpecifierSimple::~TypeSpecifierSimple()
{}
const AccessQualifier &
TypeSpecifierSimple::get_access_qualifier() const
{ return *access_qualifier; }
const TypeName &
TypeSpecifierSimple::get_type_name() const
{ return *type_name; }
///////////////////////////////////////////////////
TypeSpecifierTemplate::TypeSpecifierTemplate(
                                             TypeSpecifier_owned_ptr _type_specifier,
                                             Terminal_owned_ptr _paren_l_token,
                                             TypeSpecifierCallArgs_owned_ptr _type_specifier_call_args,
                                             Terminal_owned_ptr _paren_r_token
                                             )
  : SyntaxNode("type_specifier_template", this)
  , type_specifier(std::move(_type_specifier))
  , paren_l_token(std::move(_paren_l_token))
  , type_specifier_call_args(std::move(_type_specifier_call_args))
  , paren_r_token(std::move(_paren_r_token))
{
  add_child(*type_specifier);
  add_child(*paren_l_token);
  add_child(*type_specifier_call_args);
  add_child(*paren_r_token);
}
TypeSpecifierTemplate::~TypeSpecifierTemplate()
{}
const TypeSpecifier &
TypeSpecifierTemplate::get_type() const
{ return *type_specifier; }
const TypeSpecifierCallArgs &
TypeSpecifierTemplate::get_args() const
{ return *type_specifier_call_args; }
///////////////////////////////////////////////////
TypeSpecifierFunctionPointer::TypeSpecifierFunctionPointer(
                                   TypeSpecifier_owned_ptr _type_specifier,
                                   Terminal_owned_ptr _paren_l1_token,
                                   Terminal_owned_ptr _star_token,
                                   Terminal_owned_ptr _identifier_token,
                                   Terminal_owned_ptr _paren_r1_token,
                                   Terminal_owned_ptr _paren_l2_token,
                                   FunctionDefinitionArgList_owned_ptr _function_definition_arg_list,
                                   Terminal_owned_ptr _paren_r2_token
                                   )
  : SyntaxNode("type_specifier_function_pointer", this)
  , type_specifier(std::move(_type_specifier))
  , paren_l1_token(std::move(_paren_l1_token))
  , star_token(std::move(_star_token))
  , identifier_token(std::move(_identifier_token))
  , paren_r1_token(std::move(_paren_r1_token))
  , paren_l2_token(std::move(_paren_l2_token))
  , function_definition_arg_list(std::move(_function_definition_arg_list))
  , paren_r2_token(std::move(_paren_r2_token))
{}
TypeSpecifierFunctionPointer::~TypeSpecifierFunctionPointer()
{}
const TypeSpecifier & TypeSpecifierFunctionPointer::get_return_type() const
{ return *type_specifier; }
const std::string &
TypeSpecifierFunctionPointer::get_name() const
{ return identifier_token->get_value(); }
const FunctionDefinitionArgList &
TypeSpecifierFunctionPointer::get_args() const
{ return *function_definition_arg_list; }
///////////////////////////////////////////////////
TypeSpecifierPointerTo::TypeSpecifierPointerTo(
                             TypeSpecifier_owned_ptr _type_specifier,
                             Terminal_owned_ptr _star_token,
                             AccessQualifier_owned_ptr _access_qualifier
                             )
  : SyntaxNode("type_specifier_pointer_to", this)
  , type_specifier(std::move(_type_specifier))
  , star_token(std::move(_star_token))
  , access_qualifier(std::move(_access_qualifier))
{
  add_child(*type_specifier);
  add_child(*star_token);
  add_child(*access_qualifier);
}
TypeSpecifierPointerTo::~TypeSpecifierPointerTo()
{}
const TypeSpecifier &
TypeSpecifierPointerTo::get_type_specifier() const
{ return *type_specifier; }
const AccessQualifier & 
TypeSpecifierPointerTo::get_access_qualifier() const
{ return *access_qualifier; }
///////////////////////////////////////////////////
TypeSpecifierReferenceTo::TypeSpecifierReferenceTo(
                                                   TypeSpecifier_owned_ptr _type_specifier,
                                                   Terminal_owned_ptr _andpersand_token,
                                                   AccessQualifier_owned_ptr _access_qualifier
                                                   )
  : SyntaxNode("type_specifier_reference_to", this)
  , type_specifier(std::move(_type_specifier))
  , andpersand_token(std::move(_andpersand_token))
  , access_qualifier(std::move(_access_qualifier))
{
  add_child(*type_specifier);
  add_child(*andpersand_token);
  add_child(*access_qualifier);
}
TypeSpecifierReferenceTo::~TypeSpecifierReferenceTo()
{}
const TypeSpecifier &
TypeSpecifierReferenceTo::get_type_specifier() const
{ return *type_specifier; }
const AccessQualifier & 
TypeSpecifierReferenceTo::get_access_qualifier() const
{ return *access_qualifier; }
///////////////////////////////////////////////////
TypeSpecifier::TypeSpecifier(TypeSpecifier::TypeSpecifierType _type, const SyntaxNode & _sn)
  : SyntaxNode("type_specifier", this)
  , type(std::move(_type))
{
  add_child(_sn);
}
TypeSpecifier::~TypeSpecifier()
{}
const TypeSpecifier::TypeSpecifierType &
TypeSpecifier::get_type() const
{ return type; }

///////////////////////////////////////////////////
FunctionDefinitionArg::FunctionDefinitionArg(TypeSpecifier_owned_ptr _type_specifier,
                                             Terminal_owned_ptr _identifier_token
                                             )
  : SyntaxNode("function_definition_arg", this)
  , type_specifier(std::move(_type_specifier))
  , identifier_token(std::move(_identifier_token))
{
  add_child(*type_specifier);
  add_child(*identifier_token);
}
FunctionDefinitionArg::~FunctionDefinitionArg()
{}
const TypeSpecifier &
FunctionDefinitionArg::get_type_specifier() const
{ return *type_specifier; }
const std::string &
FunctionDefinitionArg::get_name() const
{ return identifier_token->get_value(); }

///////////////////////////////////////////////////
FunctionDefinitionArgList::FunctionDefinitionArgList()
  : SyntaxNode("function_definition_arg_list", this)
{}
FunctionDefinitionArgList::~FunctionDefinitionArgList()
{}
const std::vector<FunctionDefinitionArg_owned_ptr> &
FunctionDefinitionArgList::get_arguments() const
{ return arguments; }
void
FunctionDefinitionArgList::add_argument(FunctionDefinitionArg_owned_ptr _argument)
{
  add_child(*_argument);
  arguments.push_back(std::move(_argument));
}
void
FunctionDefinitionArgList::add_comma(Terminal_owned_ptr _comma)
{
  add_child(*_comma);
  commas.push_back(std::move(_comma));
}


///////////////////////////////////////////////////

FileStatementFunctionDeclaration::FileStatementFunctionDeclaration(
    AccessModifier_owned_ptr _access_modifier,
    UnsafeModifier_owned_ptr _unsafe_modifier,
    TypeSpecifier_owned_ptr _type_specifier,
    Terminal_owned_ptr _name,
    Terminal_owned_ptr _paren_l,
    FunctionDefinitionArgList_owned_ptr _arguments,
    Terminal_owned_ptr _paren_r,
    Terminal_owned_ptr _semicolon
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
  add_child(*access_modifier);
  add_child(*unsafe_modifier);
  add_child(*type_specifier);
  add_child(*name);
  add_child(*paren_l);
  add_child(*arguments);
  add_child(*paren_r);
  add_child(*semicolon);
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
                                   TypeSpecifier_owned_ptr _type_specifier,
                                   Terminal_owned_ptr _identifier_token,
                                   ArrayLength_owned_ptr _array_length,
                                   GlobalInitializer_owned_ptr _global_initializer,
                                   Terminal_owned_ptr _semicolon_token
)
  : SyntaxNode("statement_variable_declaration", this)
  , type_specifier(std::move(_type_specifier))
  , identifier_token(std::move(_identifier_token))
  , array_length(std::move(_array_length))
  , global_initializer(std::move(_global_initializer))
  , semicolon_token(std::move(_semicolon_token))
{
  //  add_child(*type_specifier);
  //  add_child(*identifier_token);
  //  add_child(*array_length);
  //  add_child(*global_initializer);
  //  add_child(*semicolon_token);
}
StatementVariableDeclaration::~StatementVariableDeclaration()
{}
const TypeSpecifier &
StatementVariableDeclaration::get_type_specifier() const
{ return *type_specifier;}
const std::string &
StatementVariableDeclaration::get_name() const
{ return identifier_token->get_value(); }
const ArrayLength &
StatementVariableDeclaration::get_array_length() const
{ return *array_length; }
const GlobalInitializer &
StatementVariableDeclaration::get_initializer() const
{ return *global_initializer;}
///////////////////////////////////////////////////
StatementBlock::StatementBlock(
                     UnsafeModifier_owned_ptr _unsafe_modifier,
                     ScopeBody_owned_ptr _scope_body
                     )
  : SyntaxNode("statement_block", this)
  , unsafe_modifier(std::move(_unsafe_modifier))
  , scope_body(std::move(_scope_body))
{
  add_child(*unsafe_modifier);
  add_child(*scope_body);
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
                          Expression_owned_ptr _expression,
                          Terminal_owned_ptr _semicolon_token
                          )
  : SyntaxNode("statement_expression", this)
  , expression(std::move(_expression))
  , semicolon_token(std::move(_semicolon_token))
{
  add_child(*expression);
  add_child(*semicolon_token);
}
StatementExpression::~StatementExpression()
{}
const Expression &
StatementExpression::get_expression() const
{ return *expression; }
///////////////////////////////////////////////////
StatementIfElse::StatementIfElse(
                      Terminal_owned_ptr _if_token,
                      Terminal_owned_ptr _paren_l_token,
                      Expression_owned_ptr _expression,
                      Terminal_owned_ptr _paren_r_token,
                      ScopeBody_owned_ptr _if_scope_body,
                      Terminal_owned_ptr _else_token,
                      ScopeBody_owned_ptr _else_scope_body
                                 )                                 
  : SyntaxNode("statement_ifelse", this)
  , m_has_else(true)
  , m_has_else_if(false)
  , if_token(std::move(_if_token))
  , paren_l_token(std::move(_paren_l_token))
  , expression(std::move(_expression))
  , paren_r_token(std::move(_paren_r_token))
  , if_scope_body(std::move(_if_scope_body))
  , else_token(std::move(_else_token))
  , else_scope_body(std::move(_else_scope_body))
{
  add_child(*if_token);
  add_child(*paren_l_token);
  add_child(*expression);
  add_child(*paren_r_token);
  add_child(*if_scope_body);
  add_child(*else_token);
  add_child(*else_scope_body);
}
StatementIfElse::StatementIfElse(
                      Terminal_owned_ptr _if_token,
                      Terminal_owned_ptr _paren_l_token,
                      Expression_owned_ptr _expression,
                      Terminal_owned_ptr _paren_r_token,
                      ScopeBody_owned_ptr _if_scope_body,
                      Terminal_owned_ptr _else_token,
                      StatementIfElse_owned_ptr _else_if
                                 )
  : SyntaxNode("statement_ifelse", this)
  , m_has_else(false)
  , m_has_else_if(true)
  , if_token(std::move(_if_token))
  , paren_l_token(std::move(_paren_l_token))
  , expression(std::move(_expression))
  , paren_r_token(std::move(_paren_r_token))
  , if_scope_body(std::move(_if_scope_body))
  , else_token(std::move(_else_token))
  , else_scope_body(nullptr)
  , else_if(std::move(_else_if))
{
  add_child(*if_token);
  add_child(*paren_l_token);
  add_child(*expression);
  add_child(*paren_r_token);
  add_child(*if_scope_body);
  add_child(*else_token);
  add_child(*else_if);
}
StatementIfElse::StatementIfElse(
                      Terminal_owned_ptr _if_token,
                      Terminal_owned_ptr _paren_l_token,
                      Expression_owned_ptr _expression,
                      Terminal_owned_ptr _paren_r_token,
                      ScopeBody_owned_ptr _if_scope_body
                                 )
  : SyntaxNode("statement_ifelse", this)
  , m_has_else(false)
  , m_has_else_if(false)
  , if_token(std::move(_if_token))
  , paren_l_token(std::move(_paren_l_token))
  , expression(std::move(_expression))
  , paren_r_token(std::move(_paren_r_token))
  , if_scope_body(std::move(_if_scope_body))
  , else_token(nullptr)
  , else_scope_body(nullptr)
{
  add_child(*if_token);
  add_child(*paren_l_token);
  add_child(*expression);
  add_child(*paren_r_token);
  add_child(*if_scope_body);
}  
StatementIfElse::~StatementIfElse()
{}
bool
StatementIfElse::has_else() const
{ return m_has_else; }
bool
StatementIfElse::has_else_if() const
{ return m_has_else_if; }
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
                     Terminal_owned_ptr _while_token,
                     Terminal_owned_ptr _paren_l_token,
                     Expression_owned_ptr _expression,
                     Terminal_owned_ptr _paren_r_token,
                     ScopeBody_owned_ptr _scope_body
                     )
  : SyntaxNode("statement_while", this)
  , while_token(std::move(_while_token))
  , paren_l_token(std::move(_paren_l_token))
  , expression(std::move(_expression))
  , paren_r_token(std::move(_paren_r_token))
  , scope_body(std::move(_scope_body))
{
  add_child(*while_token);
  add_child(*paren_l_token);
  add_child(*expression);
  add_child(*paren_r_token);
  add_child(*scope_body);
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
                   Terminal_owned_ptr _for_token,
                   Terminal_owned_ptr _paren_l_token,
                   Expression_owned_ptr _expression_initial,
                   Terminal_owned_ptr _semicolon_initial,
                   Expression_owned_ptr _expression_termination,
                   Terminal_owned_ptr _semicolon_termination,
                   Expression_owned_ptr _expression_increment,
                   Terminal_owned_ptr _paren_r_token,
                   ScopeBody_owned_ptr _scope_body
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
  add_child(*for_token);
  add_child(*paren_l_token);
  add_child(*expression_initial);
  add_child(*semicolon_initial);
  add_child(*expression_termination);
  add_child(*semicolon_termination);
  add_child(*expression_increment);
  add_child(*paren_r_token);
  add_child(*scope_body);
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
                           Terminal_owned_ptr _default_token,
                           Terminal_owned_ptr _colon_token,
                           ScopeBody_owned_ptr _scope_body
                           )
  : SyntaxNode("statement_switch_block", this)
  , m_is_default(true)
  , default_token(std::move(_default_token))
  , colon_token(std::move(_colon_token))
  , scope_body(std::move(_scope_body))
{
  add_child(*default_token);
  add_child(*colon_token);
  add_child(*scope_body);
}
StatementSwitchBlock::StatementSwitchBlock(
                           Terminal_owned_ptr _case_token,
                           Expression_owned_ptr _expression,
                           Terminal_owned_ptr _colon_token,
                           ScopeBody_owned_ptr _scope_body
                           )
  : SyntaxNode("statement_switch_block", this)
  , m_is_default(false)
  , case_token(std::move(_case_token))
  , expression(std::move(_expression))
  , colon_token(std::move(_colon_token))
  , scope_body(std::move(_scope_body))
{
  add_child(*case_token);
  add_child(*expression);
  add_child(*colon_token);
  add_child(*scope_body);
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
const std::vector<StatementSwitchBlock_owned_ptr> &
StatementSwitchContent::get_blocks() const
{ return blocks; }
void
StatementSwitchContent::add_block(StatementSwitchBlock_owned_ptr _block)
{ blocks.push_back(std::move(_block)); }
///////////////////////////////////////////////////
StatementSwitch::StatementSwitch(
                      Terminal_owned_ptr _switch_token,
                      Terminal_owned_ptr _paren_l_token,
                      Expression_owned_ptr _expression,
                      Terminal_owned_ptr _paren_r_token,
                      Terminal_owned_ptr _brace_l_token,
                      StatementSwitchContent_owned_ptr _switch_content,
                      Terminal_owned_ptr _brace_r_token
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
  add_child(*switch_token);
  add_child(*expression);
  add_child(*paren_l_token);
  add_child(*expression);
  add_child(*paren_r_token);
  add_child(*brace_l_token);
  add_child(*switch_content);
  add_child(*brace_r_token);
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
StatementLabel::StatementLabel(
                               Terminal_owned_ptr _label_token,
                               Terminal_owned_ptr _identifier_token,
                               Terminal_owned_ptr _colon_token
                               )
  : SyntaxNode("statement_label", this)
  , label_token(std::move(_label_token))
  , identifier_token(std::move(_identifier_token))
  , colon_token(std::move(_colon_token))
{
  add_child(*label_token);
  add_child(*identifier_token);
  add_child(*colon_token);
}
StatementLabel::~StatementLabel()
{}
const std::string &
StatementLabel::get_name() const
{ return identifier_token->get_value(); }

///////////////////////////////////////////////////
StatementGoto::StatementGoto(
                    Terminal_owned_ptr _goto_token,
                    Terminal_owned_ptr _identifier_token,
                    Terminal_owned_ptr _semicolon_token
                    )
  : SyntaxNode("statement_goto", this)
  , goto_token(std::move(_goto_token))
  , identifier_token(std::move(_identifier_token))
  , semicolon_token(std::move(_semicolon_token))
{
  add_child(*goto_token);
  add_child(*identifier_token);
  add_child(*semicolon_token);
}
StatementGoto::~StatementGoto()
{}
const std::string &
StatementGoto::get_label() const
{ return identifier_token->get_value(); }
///////////////////////////////////////////////////
StatementBreak::StatementBreak(
                               Terminal_owned_ptr _break_token,
                               Terminal_owned_ptr _semicolon_token
                               )
  : SyntaxNode("StatementBreak();", this)
  , break_token(std::move(_break_token))
  , semicolon_token(std::move(_semicolon_token))
{
  add_child(*break_token);
  add_child(*semicolon_token);
}
StatementBreak::~StatementBreak()
{}
///////////////////////////////////////////////////
StatementContinue::StatementContinue(
                                     Terminal_owned_ptr _continue_token,
                                     Terminal_owned_ptr _semicolon_token
                                     )
  : SyntaxNode("statement_continue", this)
  , continue_token(std::move(_continue_token))
  , semicolon_token(std::move(_semicolon_token))
{
  add_child(*continue_token);
  add_child(*semicolon_token);
}
StatementContinue::~StatementContinue()
{}
///////////////////////////////////////////////////
StatementReturn::StatementReturn(
                                 Terminal_owned_ptr _return_token,
                                 Expression_owned_ptr _expression,
                                 Terminal_owned_ptr _semicolon_token
                                 )
  : SyntaxNode("statement_return", this)
  , return_token(std::move(_return_token))
  , expression(std::move(_expression))
  , semicolon_token(std::move(_semicolon_token))
{
  add_child(*return_token);
  add_child(*expression);
  add_child(*semicolon_token);
}
StatementReturn::~StatementReturn()
{}
const Expression &
StatementReturn::get_expression() const
{ return *expression; }
///////////////////////////////////////////////////
Statement::Statement(StatementType _statement, const SyntaxNode & _sn)
  : SyntaxNode("statement", this)
  , statement(std::move(_statement))
{
  add_child(_sn);
}
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
StatementList::add_statement(Statement_owned_ptr _statement)
{
  add_child(*_statement);
  statements.push_back(std::move(_statement));
}
const std::vector<Statement_owned_ptr> &
StatementList::get_statements() const
{ return statements; }

///////////////////////////////////////////////////
ScopeBody::ScopeBody(
                     Terminal_owned_ptr _brace_l_token,
                     StatementList_owned_ptr _statement_list,
                     Terminal_owned_ptr _brace_r_token
                )
  : SyntaxNode("scope_body", this)
  , brace_l_token(std::move(_brace_l_token))
  , statement_list(std::move(_statement_list))
  , brace_r_token(std::move(_brace_r_token))
{
  add_child(*brace_l_token);
  add_child(*statement_list);
  add_child(*brace_r_token);
}
ScopeBody::~ScopeBody()
{}
const StatementList &
ScopeBody::get_statements() const
{ return *statement_list; }
///////////////////////////////////////////////////
FileStatementFunctionDefinition::FileStatementFunctionDefinition(
    AccessModifier_owned_ptr _access_modifier,
    UnsafeModifier_owned_ptr _unsafe_modifier,
    TypeSpecifier_owned_ptr _type_specifier,
    Terminal_owned_ptr _name,
    Terminal_owned_ptr _paren_l,
    FunctionDefinitionArgList_owned_ptr _arguments,
    Terminal_owned_ptr _paren_r,
    ScopeBody_owned_ptr _scope_body
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
  add_child(*access_modifier);
  add_child(*unsafe_modifier);
  add_child(*type_specifier);
  add_child(*name);
  add_child(*paren_l);
  add_child(*arguments);
  add_child(*paren_r);
  add_child(*scope_body);
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
                  Terminal_owned_ptr _bracket_l_token,
                  Terminal_owned_ptr _literal_int_token,
                  Terminal_owned_ptr _bracket_r_token
                  )
  : SyntaxNode("array_length", this)
  , bracket_l_token(std::move(_bracket_l_token))
  , literal_int_token(std::move(_literal_int_token))
  , bracket_r_token(std::move(_bracket_r_token))
{
  add_child(*bracket_l_token);
  add_child(*literal_int_token);
  add_child(*bracket_r_token);
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
{ return (size_t)atol(literal_int_token->get_value().c_str());}

///////////////////////////////////////////////////
ClassDeclStart::ClassDeclStart(
                     AccessModifier_owned_ptr _access_modifier,
                     Terminal_owned_ptr _class_token,
                     Terminal_owned_ptr _identifier_token,
                     ClassArgumentList_owned_ptr _class_argument_list
)
  : SyntaxNode("class_decl_start", this)
  , access_modifier(std::move(_access_modifier))
  , class_token(std::move(_class_token))
  , identifier_token(std::move(_identifier_token))
  , class_argument_list(std::move(_class_argument_list))
{
  add_child(*access_modifier);
  add_child(*class_token);
  add_child(*identifier_token);
  add_child(*class_argument_list);
}
ClassDeclStart::~ClassDeclStart()
{}
const AccessModifier &
ClassDeclStart::get_access_modifier() const
{ return *access_modifier; }
const std::string &
ClassDeclStart::get_name() const
{ return identifier_token->get_value(); }
const ClassArgumentList &
ClassDeclStart::get_argument_list() const
{ return *class_argument_list; }
///////////////////////////////////////////////////
ClassArgumentList::ClassArgumentList(Terminal_owned_ptr _argument)
  : SyntaxNode("class_argument_list", this)
  , paren_l(nullptr)
  , paren_r(nullptr)
{
  add_child(*_argument);
  argument_list.push_back(std::move(_argument));
}
ClassArgumentList::ClassArgumentList()
  : SyntaxNode("class_argument_list", this)
  , paren_l(nullptr)
  , paren_r(nullptr)
{
}
ClassArgumentList::~ClassArgumentList()
{}
void
ClassArgumentList::add_parens(Terminal_owned_ptr _paren_l, Terminal_owned_ptr _paren_r)
{
  paren_l = std::move(_paren_l);
  prepend_child(*paren_l);
  
  paren_r = std::move(_paren_r);
  add_child(*paren_r);
}

void
ClassArgumentList::add_argument(Terminal_owned_ptr _comma_token, Terminal_owned_ptr _argument)
{
  comma_list.push_back(std::move(_comma_token));
  argument_list.push_back(std::move(_argument));
}
const std::vector<Terminal_owned_ptr> &
ClassArgumentList::get_arguments() const
{ return argument_list; }
///////////////////////////////////////////////////
ClassMemberDeclarationVariable::ClassMemberDeclarationVariable(
                                     AccessModifier_owned_ptr _access_modifier,
                                     TypeSpecifier_owned_ptr _type_specifier,
                                     Terminal_owned_ptr _identifier_token,
                                     ArrayLength_owned_ptr _array_length,
                                     Terminal_owned_ptr _semicolon_token
                                     )
  : SyntaxNode("class_member_declaration_variable", this)
  , access_modifier(std::move(_access_modifier))
  , type_specifier(std::move(_type_specifier))
  , identifier_token(std::move(_identifier_token))
  , array_length(std::move(_array_length))
  , semicolon_token(std::move(_semicolon_token))
{
  add_child(*access_modifier);
  add_child(*type_specifier);
  add_child(*identifier_token);
  add_child(*array_length);
  add_child(*semicolon_token);
}
ClassMemberDeclarationVariable::~ClassMemberDeclarationVariable()
{}
const AccessModifier &
ClassMemberDeclarationVariable::get_access_modifier() const
{ return *access_modifier; }
const TypeSpecifier &
ClassMemberDeclarationVariable::get_type_specifier() const
{ return *type_specifier; }
const std::string &
ClassMemberDeclarationVariable::get_name() const
{ return identifier_token->get_value(); }
const ArrayLength &
ClassMemberDeclarationVariable::get_array_length() const
{ return *array_length; }
///////////////////////////////////////////////////
ClassMemberDeclarationMethod::ClassMemberDeclarationMethod(
                                                           AccessModifier_owned_ptr _access_modifier,
                                                           TypeSpecifier_owned_ptr _type_specifier,
                                                           Terminal_owned_ptr _identifier_token,
                                                           Terminal_owned_ptr _paren_l_token,
                                                           FunctionDefinitionArgList_owned_ptr _function_definition_arg_list,
                                                           Terminal_owned_ptr _paren_r_token,
                                                           Terminal_owned_ptr _semicolon_token
                                                           )
 : SyntaxNode("class_member_declaration_method", this)
 , access_modifier(std::move(_access_modifier))
 , type_specifier(std::move(_type_specifier))
 , identifier_token(std::move(_identifier_token))
 , paren_l_token(std::move(_paren_l_token))
 , function_definition_arg_list(std::move(_function_definition_arg_list))
 , paren_r_token(std::move(_paren_r_token))
 , semicolon_token(std::move(_semicolon_token))
{
  add_child(*access_modifier);
  add_child(*type_specifier);
  add_child(*identifier_token);
  add_child(*paren_l_token);
  add_child(*function_definition_arg_list);
  add_child(*paren_r_token);
  add_child(*semicolon_token);
}
ClassMemberDeclarationMethod::~ClassMemberDeclarationMethod()
{}
const AccessModifier &
ClassMemberDeclarationMethod::get_access_modifier() const
{ return *access_modifier; }
const TypeSpecifier &
ClassMemberDeclarationMethod::get_type_specifier() const
{ return *type_specifier; }
const std::string &
ClassMemberDeclarationMethod::get_name() const
{ return identifier_token->get_value(); }
const FunctionDefinitionArgList &
ClassMemberDeclarationMethod::get_arguments() const
{ return *function_definition_arg_list; }
///////////////////////////////////////////////////
ClassMemberDeclarationConstructor::ClassMemberDeclarationConstructor(
                                                           AccessModifier_owned_ptr _access_modifier,
                                                           TypeSpecifier_owned_ptr _type_specifier,
                                                           Terminal_owned_ptr _paren_l_token,
                                                           FunctionDefinitionArgList_owned_ptr _function_definition_arg_list,
                                                           Terminal_owned_ptr _paren_r_token,
                                                           Terminal_owned_ptr _semicolon_token
                                                           )
 : SyntaxNode("class_member_declaration_method", this)
 , access_modifier(std::move(_access_modifier))
 , type_specifier(std::move(_type_specifier))
 , paren_l_token(std::move(_paren_l_token))
 , function_definition_arg_list(std::move(_function_definition_arg_list))
 , paren_r_token(std::move(_paren_r_token))
 , semicolon_token(std::move(_semicolon_token))
{
  add_child(*access_modifier);
  add_child(*type_specifier);
  add_child(*paren_l_token);
  add_child(*function_definition_arg_list);
  add_child(*paren_r_token);
  add_child(*semicolon_token);
}
ClassMemberDeclarationConstructor::~ClassMemberDeclarationConstructor()
{}
const AccessModifier &
ClassMemberDeclarationConstructor::get_access_modifier() const
{ return *access_modifier; }
const TypeSpecifier &
ClassMemberDeclarationConstructor::get_type_specifier() const
{ return *type_specifier; }
const FunctionDefinitionArgList &
ClassMemberDeclarationConstructor::get_arguments() const
{ return *function_definition_arg_list; }
///////////////////////////////////////////////////
ClassMemberDeclarationDestructor::ClassMemberDeclarationDestructor(
                                                                   AccessModifier_owned_ptr _access_modifier,
                                                                   Terminal_owned_ptr _tilde_token,
                                                                   TypeSpecifier_owned_ptr _type_specifier,
                                                                   Terminal_owned_ptr _paren_l_token,
                                                                   FunctionDefinitionArgList_owned_ptr _function_definition_arg_list,
                                                                   Terminal_owned_ptr _paren_r_token,
                                                                   Terminal_owned_ptr _semicolon_token
                                                           )
 : SyntaxNode("class_member_declaration_method", this)
 , access_modifier(std::move(_access_modifier))
 , tilde_token(std::move(_tilde_token))
 , type_specifier(std::move(_type_specifier))
 , paren_l_token(std::move(_paren_l_token))
 , function_definition_arg_list(std::move(_function_definition_arg_list))
 , paren_r_token(std::move(_paren_r_token))
 , semicolon_token(std::move(_semicolon_token))
{
  add_child(*access_modifier);
  add_child(*tilde_token);
  add_child(*type_specifier);
  add_child(*paren_l_token);
  add_child(*function_definition_arg_list);
  add_child(*paren_r_token);
  add_child(*semicolon_token);
}
ClassMemberDeclarationDestructor::~ClassMemberDeclarationDestructor()
{}
const AccessModifier &
ClassMemberDeclarationDestructor::get_access_modifier() const
{ return *access_modifier; }
const TypeSpecifier &
ClassMemberDeclarationDestructor::get_type_specifier() const
{ return *type_specifier; }
const FunctionDefinitionArgList &
ClassMemberDeclarationDestructor::get_arguments() const
{ return *function_definition_arg_list; }
///////////////////////////////////////////////////
ClassMemberDeclaration::ClassMemberDeclaration(
                                               MemberType _member,
                                               const SyntaxNode & _sn
                                               )
  : SyntaxNode("class_member_declaration", this)
  , member(std::move(_member))
{
  add_child(_sn);
}
ClassMemberDeclaration::~ClassMemberDeclaration()
{}
const ClassMemberDeclaration::MemberType &
ClassMemberDeclaration::get_member()
{ return member; }
///////////////////////////////////////////////////


ClassMemberDeclarationList::ClassMemberDeclarationList()
  : SyntaxNode("class_member_declaration_list", this)
{}
ClassMemberDeclarationList::~ClassMemberDeclarationList()
{}
const std::vector<ClassMemberDeclaration_owned_ptr> &
ClassMemberDeclarationList::get_members() const
{
  return members;
}
void
ClassMemberDeclarationList::add_member(ClassMemberDeclaration_owned_ptr _member)
{
  add_child(*_member);
  members.push_back(std::move(_member));
}
///////////////////////////////////////////////////
ClassDefinition::ClassDefinition(
                      ClassDeclStart_owned_ptr _class_decl_start,
                      Terminal_owned_ptr _brace_l_token,
                      ClassMemberDeclarationList_owned_ptr _class_member_declaration_list,
                      Terminal_owned_ptr _brace_r_token,
                      Terminal_owned_ptr _semicolon_token
                      )
  : SyntaxNode("class_definition", this)
  , class_decl_start(std::move(_class_decl_start))
  , brace_l_token(std::move(_brace_l_token))
  , class_member_declaration_list(std::move(_class_member_declaration_list))
  , brace_r_token(std::move(_brace_r_token))
  , semicolon_token(std::move(_semicolon_token))
{
  add_child(*class_decl_start);
  add_child(*brace_l_token);
  add_child(*class_member_declaration_list);
  add_child(*brace_r_token);
  add_child(*semicolon_token);
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
const ClassMemberDeclarationList &
ClassDefinition::get_members() const
{ return *class_member_declaration_list; }

///////////////////////////////////////////////////
TypeDefinition::TypeDefinition(
                     AccessModifier_owned_ptr _access_modifier,
                     Terminal_owned_ptr _typedef_token,
                     TypeSpecifier_owned_ptr _type_specifier,
                     Terminal_owned_ptr _identifier_token,
                     Terminal_owned_ptr _semicolon_token
                               )
  : SyntaxNode("type_definition", this)
  , access_modifier(std::move(_access_modifier))
  , typedef_token(std::move(_typedef_token))
  , type_specifier(std::move(_type_specifier))
  , identifier_token(std::move(_identifier_token))
  , semicolon_token(std::move(_semicolon_token))
{
  add_child(*access_modifier);
  add_child(*typedef_token);
  add_child(*type_specifier);
  add_child(*identifier_token);
  add_child(*semicolon_token);
}
TypeDefinition::~TypeDefinition()
{}
const AccessModifier &
TypeDefinition::get_access_modifier() const
{ return *access_modifier; }
const std::string &
TypeDefinition::get_name() const
{ return identifier_token->get_value(); }
const TypeSpecifier &
TypeDefinition::get_type_specifier() const
{ return *type_specifier; }

///////////////////////////////////////////////////
EnumDefinitionValue::EnumDefinitionValue(
                          Terminal_owned_ptr _identifier_token,
                          Terminal_owned_ptr _equals_token,
                          ExpressionPrimary_owned_ptr _expression_primary,
                          Terminal_owned_ptr _semicolon_token
                          )
  : SyntaxNode("enum_definition_value", this)
  , identifier_token(std::move(_identifier_token))
  , equals_token(std::move(_equals_token))
  , expression_primary(std::move(_expression_primary))
  , semicolon_token(std::move(_semicolon_token))
{
  add_child(*identifier_token);
  add_child(*equals_token);
  add_child(*expression_primary);
  add_child(*semicolon_token);
}
EnumDefinitionValue::~EnumDefinitionValue()
{}
const std::string &
EnumDefinitionValue::get_name() const
{ return identifier_token->get_value(); }

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
EnumDefinitionValueList::add_value(EnumDefinitionValue_owned_ptr _value)
{
  add_child(*_value);
  values.push_back(std::move(_value));
}
const std::vector<EnumDefinitionValue_owned_ptr> &
EnumDefinitionValueList::get_values() const
{ return values; }

///////////////////////////////////////////////////
EnumDefinition::EnumDefinition(
                     AccessModifier_owned_ptr _access_modifier,
                     Terminal_owned_ptr _enum_token,
                     Terminal_owned_ptr _type_name_token,
                     Terminal_owned_ptr _identifier_token,
                     Terminal_owned_ptr _brace_l_token,
                     EnumDefinitionValueList_owned_ptr _enum_value_list,
                     Terminal_owned_ptr _brace_r_token,
                     Terminal_owned_ptr _semicolon_token
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
  add_child(*access_modifier);
  add_child(*enum_token);
  add_child(*type_name_token);
  add_child(*identifier_token);
  add_child(*brace_l_token);
  add_child(*enum_value_list);
  add_child(*brace_r_token);
  add_child(*semicolon_token);
}
EnumDefinition::~EnumDefinition()
{}
const AccessModifier &
EnumDefinition::get_access_modifier() const
{ return *access_modifier; }
const std::string &
EnumDefinition::type_name() const
{ return type_name_token->get_value(); }
const std::string &
EnumDefinition::enum_name() const
{ return identifier_token->get_value(); }
const EnumDefinitionValueList &
EnumDefinition::get_value_list() const
{ return *enum_value_list; }
///////////////////////////////////////////////////
ExpressionPrimaryIdentifier::ExpressionPrimaryIdentifier(Terminal_owned_ptr _identifier_token)
  : SyntaxNode("expression_primary_identifier", this)
  , identifier_token(std::move(_identifier_token))
{
  add_child(*identifier_token);
}
ExpressionPrimaryIdentifier::~ExpressionPrimaryIdentifier()
{}
const std::string &
ExpressionPrimaryIdentifier::get_identifier() const
{ return identifier_token->get_value(); }
///////////////////////////////////////////////////
ExpressionPrimaryNested::ExpressionPrimaryNested(
                                                 Terminal_owned_ptr _paren_l_token,
                                                 Expression_owned_ptr _expression,
                                                 Terminal_owned_ptr _paren_r_token
                                                 )
  : SyntaxNode("expression_primary_nested", this)
  , paren_l_token(std::move(_paren_l_token))
  , expression(std::move(_expression))
  , paren_r_token(std::move(_paren_r_token))
{
  add_child(*paren_l_token);
  add_child(*expression);
  add_child(*paren_r_token);
}
ExpressionPrimaryNested::~ExpressionPrimaryNested()
{}
const Expression &
ExpressionPrimaryNested::get_expression() const
{ return *expression; }

///////////////////////////////////////////////////
ExpressionPrimaryLiteralInt::ExpressionPrimaryLiteralInt(
                                                         Terminal_owned_ptr _literal_token
                                                         )
  : SyntaxNode("expression_primary_literal_int", this)
  , literal_token(std::move(_literal_token))
{
  add_child(*literal_token);
}
ExpressionPrimaryLiteralInt::~ExpressionPrimaryLiteralInt()
{}
const std::string &
ExpressionPrimaryLiteralInt::get_value() const
{ return literal_token->get_value(); }
///////////////////////////////////////////////////
ExpressionPrimaryLiteralChar::ExpressionPrimaryLiteralChar(
                                                         Terminal_owned_ptr _literal_token
                                                         )
  : SyntaxNode("expression_primary_literal_char", this)
  , literal_token(std::move(_literal_token))
{
  add_child(*literal_token);
}
ExpressionPrimaryLiteralChar::~ExpressionPrimaryLiteralChar()
{}
const std::string &
ExpressionPrimaryLiteralChar::get_value() const
{ return literal_token->get_value(); }
///////////////////////////////////////////////////
ExpressionPrimaryLiteralString::ExpressionPrimaryLiteralString(
                                                         Terminal_owned_ptr _literal_token
                                                         )
  : SyntaxNode("expression_primary_literal_string", this)
  , literal_token(std::move(_literal_token))
{
  add_child(*literal_token);
}
ExpressionPrimaryLiteralString::~ExpressionPrimaryLiteralString()
{}
const std::string &
ExpressionPrimaryLiteralString::get_value() const
{ return literal_token->get_value(); }
///////////////////////////////////////////////////
ExpressionPrimaryLiteralFloat::ExpressionPrimaryLiteralFloat(
                                                         Terminal_owned_ptr _literal_token
                                                         )
  : SyntaxNode("expression_primary_literal_float", this)
  , literal_token(std::move(_literal_token))
{
  add_child(*literal_token);
}
const std::string &
ExpressionPrimaryLiteralFloat::get_value() const
{ return literal_token->get_value(); }
ExpressionPrimaryLiteralFloat::~ExpressionPrimaryLiteralFloat()
{}

///////////////////////////////////////////////////
ExpressionPrimary::ExpressionPrimary(ExpressionPrimary::ExpressionType _expression_type, const SyntaxNode & _sn)
  : SyntaxNode("expression_primary", this)
  , expression_type(std::move(_expression_type))
{
  add_child(_sn);
}
ExpressionPrimary::~ExpressionPrimary()
{}
const ExpressionPrimary::ExpressionType &
ExpressionPrimary::get_expression() const
{ return expression_type; }

///////////////////////////////////////////////////
ExpressionPostfixArrayIndex::ExpressionPostfixArrayIndex(
                                  Expression_owned_ptr _array_expression,
                                  Terminal_owned_ptr _bracket_l_token,
                                  Expression_owned_ptr _index_expression,
                                  Terminal_owned_ptr _bracket_r_token
                                  )
  : SyntaxNode("expression_postfix_array_index", this)
  , array_expression(std::move(_array_expression))
  , bracket_l_token(std::move(_bracket_l_token))
  , index_expression(std::move(_index_expression))
  , bracket_r_token(std::move(_bracket_r_token))
{
  add_child(*array_expression);
  add_child(*bracket_l_token);
  add_child(*index_expression);
  add_child(*bracket_r_token);
}
ExpressionPostfixArrayIndex::~ExpressionPostfixArrayIndex()
{}
const Expression &
ExpressionPostfixArrayIndex::get_array() const
{ return *array_expression; }
const Expression &
ExpressionPostfixArrayIndex::get_index() const
{ return *index_expression; }
///////////////////////////////////////////////////
ArgumentExpressionList::ArgumentExpressionList()
  : SyntaxNode("argument_expression_list", this)
{}
ArgumentExpressionList::~ArgumentExpressionList()
{}
const std::vector<Expression_owned_ptr> &
ArgumentExpressionList::get_arguments() const
{ return arguments; }
void
ArgumentExpressionList::add_argument(Expression_owned_ptr _argument)
{
  add_child(*_argument);
  arguments.push_back(std::move(_argument));
}
void
ArgumentExpressionList::add_argument(Terminal_owned_ptr _comma_token, Expression_owned_ptr _argument)
{
  add_child(*_comma_token);
  add_child(*_argument);
  comma_list.push_back(std::move(_comma_token));
  arguments.push_back(std::move(_argument));
}

///////////////////////////////////////////////////
ExpressionPostfixFunctionCall::ExpressionPostfixFunctionCall(
                                    Expression_owned_ptr _function_expression,
                                    Terminal_owned_ptr _paren_l_token,
                                    ArgumentExpressionList_owned_ptr _arguments,
                                    Terminal_owned_ptr _paren_r_token
                                    )
  : SyntaxNode("expression_postfix_function_call", this)
  , function_expression(std::move(_function_expression))
  , paren_l_token(std::move(_paren_l_token))
  , arguments(std::move(_arguments))
  , paren_r_token(std::move(_paren_r_token))
{
  add_child(*function_expression);
  add_child(*paren_l_token);
  add_child(*arguments);
  add_child(*paren_r_token);
}
ExpressionPostfixFunctionCall::~ExpressionPostfixFunctionCall()
{}
const Expression &
ExpressionPostfixFunctionCall::get_function() const
{ return *function_expression; }
const ArgumentExpressionList &
ExpressionPostfixFunctionCall::get_arguments() const
{ return *arguments; }
///////////////////////////////////////////////////
ExpressionPostfixDot::ExpressionPostfixDot(
                           Expression_owned_ptr _expression,
                           Terminal_owned_ptr _dot_token,
                           Terminal_owned_ptr _identifier_token
                           )
  : SyntaxNode("expression_postfix_dot", this)
  , expression(std::move(_expression))
  , dot_token(std::move(_dot_token))
  , identifier_token(std::move(_identifier_token))
{
  add_child(*expression);
  add_child(*dot_token);
  add_child(*identifier_token);
}
ExpressionPostfixDot::~ExpressionPostfixDot()
{}
const Expression &
ExpressionPostfixDot::get_expression() const
{ return *expression; }
const std::string &
ExpressionPostfixDot::get_identifier() const
{ return identifier_token->get_value(); }

///////////////////////////////////////////////////
ExpressionPostfixArrow::ExpressionPostfixArrow(
                           Expression_owned_ptr _expression,
                           Terminal_owned_ptr _arrow_token,
                           Terminal_owned_ptr _identifier_token
                           )
  : SyntaxNode("expression_postfix_arrow", this)
  , expression(std::move(_expression))
  , arrow_token(std::move(_arrow_token))
  , identifier_token(std::move(_identifier_token))
{
  add_child(*expression);
  add_child(*arrow_token);
  add_child(*identifier_token);
}
ExpressionPostfixArrow::~ExpressionPostfixArrow()
{}
const Expression &
ExpressionPostfixArrow::get_expression() const
{ return *expression; }
const std::string &
ExpressionPostfixArrow::get_identifier() const
{ return identifier_token->get_value(); }

///////////////////////////////////////////////////
ExpressionPostfixIncDec::ExpressionPostfixIncDec(
                                                 Expression_owned_ptr _expression,
                                                 Terminal_owned_ptr _operator_token
                                                 )
  : SyntaxNode("expression_postfix_incdec", this)
  , expression(std::move(_expression))
  , operator_token(std::move(_operator_token))
    // TODO XXX derive this from the operator.
  , type(ExpressionPostfixIncDec::OperationType::INCREMENT)
{
  add_child(*expression);
  add_child(*operator_token);
}
ExpressionPostfixIncDec::~ExpressionPostfixIncDec()
{}
const ExpressionPostfixIncDec::OperationType &
ExpressionPostfixIncDec::get_type()
{ return type; }
const Expression &
ExpressionPostfixIncDec::get_expression()
{ return *expression; }
///////////////////////////////////////////////////
ExpressionUnaryPrefix::ExpressionUnaryPrefix(
                                             Terminal_owned_ptr _operator_token,
                                             Expression_owned_ptr _expression
                                             )
  : SyntaxNode("expression_unary_prefix", this)
  , operator_token(std::move(_operator_token))
  , expression(std::move(_expression))
  , type(ExpressionUnaryPrefix::OperationType::INCREMENT)
{
  // TODO: Calculate this from the operator given.
  //type(ExpressionUnaryPrefix::OperationType::INCREMENT)
  add_child(*operator_token);
  add_child(*expression);
}
ExpressionUnaryPrefix::~ExpressionUnaryPrefix()
{}
const ExpressionUnaryPrefix::OperationType &
ExpressionUnaryPrefix::get_type()
{ return type; }
const Expression &
ExpressionUnaryPrefix::get_expression()
{ return *expression; }
///////////////////////////////////////////////////
ExpressionUnarySizeofType::ExpressionUnarySizeofType(
                                Terminal_owned_ptr _sizeof_token,
                                Terminal_owned_ptr _paren_l_token,
                                TypeSpecifier_owned_ptr _type_specifier,
                                Terminal_owned_ptr _paren_r_token
                                )
  : SyntaxNode("expression_unary_sizeof_type", this)
  , sizeof_token(std::move(_sizeof_token))
  , paren_l_token(std::move(_paren_l_token))
  , type_specifier(std::move(_type_specifier))
  , paren_r_token(std::move(_paren_r_token))
{
  add_child(*sizeof_token);
  add_child(*paren_l_token);
  add_child(*type_specifier);
  add_child(*paren_r_token);
}
ExpressionUnarySizeofType::~ExpressionUnarySizeofType()
{}
const TypeSpecifier &
ExpressionUnarySizeofType::get_type_specifier() const
{ return *type_specifier; }
///////////////////////////////////////////////////
ExpressionCast::ExpressionCast(
                     Terminal_owned_ptr _cast_token,
                     Terminal_owned_ptr _paren_l_token,
                     TypeSpecifier_owned_ptr _type_specifier,
                     Terminal_owned_ptr _comma_token,
                     Expression_owned_ptr _expression,
                     Terminal_owned_ptr _paren_r_token
                     )
  : SyntaxNode("expression_cast", this)
  , cast_token(std::move(_cast_token))
  , paren_l_token(std::move(_paren_l_token))
  , type_specifier(std::move(_type_specifier))
  , comma_token(std::move(_comma_token))
  , expression(std::move(_expression))
  , paren_r_token(std::move(_paren_r_token))
{}
ExpressionCast::~ExpressionCast()
{}
const TypeSpecifier &
ExpressionCast::get_type() const
{ return *type_specifier; }
const Expression &
ExpressionCast::get_expression() const
{ return *expression; }
///////////////////////////////////////////////////
ExpressionBinary::ExpressionBinary(
                       Expression_owned_ptr _expression_a,
                       Terminal_owned_ptr _operator_token,
                       Expression_owned_ptr _expression_b
                       )
  : SyntaxNode("expression_binary", this)
  , expression_a(std::move(_expression_a))
  , operator_token(std::move(_operator_token))
  , expression_b(std::move(_expression_b))
{
  add_child(*expression_a);
  add_child(*operator_token);
  add_child(*expression_b);
  type = OperationType::LOGICAL_AND; // XXX TODO: get these from the tokens.
}
ExpressionBinary::~ExpressionBinary()
{}
const Expression &
ExpressionBinary::get_a() const
{ return *expression_b; }
const ExpressionBinary::OperationType &
ExpressionBinary::get_operator() const
{ return type; }
const Expression &
ExpressionBinary::get_b() const
{ return *expression_a; }
///////////////////////////////////////////////////
ExpressionTrinary::ExpressionTrinary(
                        Expression_owned_ptr _condition,
                        Terminal_owned_ptr _questionmark_token,
                        Expression_owned_ptr _if_expression,
                        Terminal_owned_ptr _colon_token,
                        Expression_owned_ptr _else_expression
                        )
  : SyntaxNode("expression_trinary", this)
  , condition(std::move(_condition))
  , questionmark_token(std::move(_questionmark_token))
  , if_expression(std::move(_if_expression))
  , colon_token(std::move(_colon_token))
  , else_expression(std::move(_else_expression))
{
  add_child(*condition);
  add_child(*questionmark_token);
  add_child(*if_expression);
  add_child(*colon_token);
  add_child(*else_expression);
}
ExpressionTrinary::~ExpressionTrinary()
{}
const Expression &
ExpressionTrinary::get_condition() const
{ return *condition; }
const Expression &
ExpressionTrinary::get_if() const
{ return *if_expression; }
const Expression &
ExpressionTrinary::get_else() const
{ return *else_expression; }
///////////////////////////////////////////////////
Expression::Expression(Expression::ExpressionType _expression_type, const SyntaxNode & _sn)
      : SyntaxNode("expression", this)
      , expression_type(std::move(_expression_type))
{
  add_child(_sn);
}
Expression::~Expression()
{}
const Expression::ExpressionType &
Expression::get_expression() const
{ return expression_type; }
///////////////////////////////////////////////////

GlobalInitializerExpressionPrimary::GlobalInitializerExpressionPrimary(
                                         Terminal_owned_ptr _equals_token,
                                         ExpressionPrimary_owned_ptr _expression
                                         )
  : SyntaxNode("global_initializer_expression_primary", this)
  , equals_token(std::move(_equals_token))
  , expression(std::move(_expression))
{
  add_child(*equals_token);
  add_child(*expression);
}
GlobalInitializerExpressionPrimary::~GlobalInitializerExpressionPrimary()
{}
const ExpressionPrimary &
GlobalInitializerExpressionPrimary::get_expression() const
{ return *expression; };

///////////////////////////////////////////////////
StructInitializer::StructInitializer(
                        Terminal_owned_ptr _dot_token,
                        Terminal_owned_ptr _identifier_token,
                        GlobalInitializer_owned_ptr _global_initializer,
                        Terminal_owned_ptr _semicolon_token
                                     )
  : SyntaxNode("struct_initializer", this)
  , dot_token(std::move(_dot_token))
  , identifier_token(std::move(_identifier_token))
  , global_initializer(std::move(_global_initializer))
  , semicolon_token(std::move(_semicolon_token))
{
  add_child(*dot_token);
  add_child(*identifier_token);
  add_child(*global_initializer);
  add_child(*semicolon_token);
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
StructInitializerList::add_initializer(StructInitializer_owned_ptr initializer)
{
  add_child(*initializer);
  initializers.push_back(std::move(initializer));
}
const std::vector<StructInitializer_owned_ptr> &
StructInitializerList::get_initializers() const
{
  return initializers;
}

///////////////////////////////////////////////////
GlobalInitializerAddressofExpressionPrimary::GlobalInitializerAddressofExpressionPrimary(
                                                                                         Terminal_owned_ptr _equals_token,
                                                                                         Terminal_owned_ptr _addressof_token,
                                                                                         ExpressionPrimary_owned_ptr _expression
                                                                                         )
  : SyntaxNode("global_initializer_addressof_expression_primary", this)
  , equals_token(std::move(_equals_token))
  , addressof_token(std::move(_addressof_token))
  , expression(std::move(_expression))
{
  add_child(*equals_token);
  add_child(*addressof_token);
  add_child(*expression);
}
GlobalInitializerAddressofExpressionPrimary::~GlobalInitializerAddressofExpressionPrimary()
{}
const ExpressionPrimary &
GlobalInitializerAddressofExpressionPrimary::get_expression() const
{ return *expression; };


///////////////////////////////////////////////////
GlobalInitializerStructInitializerList::GlobalInitializerStructInitializerList(
                                             Terminal_owned_ptr _equals_token,
                                             Terminal_owned_ptr _brace_l_token,
                                             StructInitializerList_owned_ptr _struct_initializer,
                                             Terminal_owned_ptr _brace_r_token
)
  : SyntaxNode("global_initializer_struct_initializer_list", this)
  , equals_token(std::move(_equals_token))
  , brace_l_token(std::move(_brace_l_token))
  , struct_initializer(std::move(_struct_initializer))
  , brace_r_token(std::move(_brace_r_token))
{
  add_child(*equals_token);
  add_child(*brace_l_token);
  add_child(*struct_initializer);
  add_child(*brace_r_token);
}
GlobalInitializerStructInitializerList::~GlobalInitializerStructInitializerList()
{}
const StructInitializerList &
GlobalInitializerStructInitializerList::get_struct_initializer() const
{ return *struct_initializer; }

///////////////////////////////////////////////////
GlobalInitializer::GlobalInitializer(GlobalInitializer::GlobalInitializerType _initializer, const SyntaxNode & _sn)
  : SyntaxNode("global_initializer", this)
  , initializer(std::move(_initializer))
{
  add_child(_sn);
}
GlobalInitializer::GlobalInitializer()
  : SyntaxNode("global_initializer", this)
  , initializer(nullptr)
{}
GlobalInitializer::~GlobalInitializer()
{}
const GlobalInitializer::GlobalInitializerType &
GlobalInitializer::get_initializer() const
{ return initializer; }


///////////////////////////////////////////////////
FileStatementGlobalDefinition::FileStatementGlobalDefinition(
                                                             AccessModifier_owned_ptr _access_modifier,
                                                             UnsafeModifier_owned_ptr _unsafe_modifier,
                                                             TypeSpecifier_owned_ptr _type_specifier,
                                                             Terminal_owned_ptr _name,
                                                             ArrayLength_owned_ptr _array_length,
                                                             GlobalInitializer_owned_ptr _global_initializer,
                                                             Terminal_owned_ptr _semicolon
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
  add_child(*access_modifier);
  add_child(*unsafe_modifier);
  add_child(*type_specifier);
  add_child(*name);
  add_child(*array_length);
  add_child(*global_initializer);
  add_child(*semicolon);
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
                           AccessModifier_owned_ptr _access_modifier,
                           Terminal_owned_ptr _namespace_token,
                           Terminal_owned_ptr _identifier_token
                           )
  : SyntaxNode("namespace_declaration", this)
  , access_modifier(std::move(_access_modifier))
  , namespace_token(std::move(_namespace_token))
  , identifier_token(std::move(_identifier_token))
{
  add_child(*access_modifier);
  add_child(*namespace_token);
  add_child(*identifier_token);
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
                                               NamespaceDeclaration_owned_ptr _namespace_declaration,
                                               Terminal_owned_ptr _brace_l_token,
                                               FileStatementList_owned_ptr _file_statement_list,
                                               Terminal_owned_ptr _brace_r_token,
                                               Terminal_owned_ptr _semicolon_token
                                               )
  : SyntaxNode("file_statement_namespace", this)
  , namespace_declaration(std::move(_namespace_declaration))
  , brace_l_token(std::move(_brace_l_token))
  , file_statement_list(std::move(_file_statement_list))
  , brace_r_token(std::move(_brace_r_token))
  , semicolon_token(std::move(_semicolon_token))
{
  add_child(*namespace_declaration);
  add_child(*brace_l_token);
  add_child(*file_statement_list);
  add_child(*brace_r_token);
  add_child(*semicolon_token);
}
FileStatementNamespace::~FileStatementNamespace()
{}
const NamespaceDeclaration & FileStatementNamespace::get_declaration() const
{ return *namespace_declaration;}

const FileStatementList & FileStatementNamespace::get_statement_list() const
{ return *file_statement_list;}

///////////////////////////////////////////////////

UsingAs::UsingAs(
        Terminal_owned_ptr _as_token,
        Terminal_owned_ptr _identifier_token
        )
  : SyntaxNode("using_as", this)
  , as_token(std::move(_as_token))
  , identifier_token(std::move(_identifier_token))
{
  using_name = identifier_token->get_value();
  add_child(*as_token);
  add_child(*identifier_token);
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

FileStatementUsing::FileStatementUsing(AccessModifier_owned_ptr _access_modifier,
                             Terminal_owned_ptr _using,
                             Terminal_owned_ptr _namespace,
                             Terminal_owned_ptr _namespace_name,
                             UsingAs_owned_ptr _using_as,
                             Terminal_owned_ptr _semicolon)
  : SyntaxNode("file_statement_using", this)
  , using_token(std::move(_using))
  , namespace_token(std::move(_namespace))
  , namespace_name_token(std::move(_namespace_name))
  , using_as(std::move(_using_as))
  , semicolon_token(std::move(_semicolon))
{
  add_child(*using_token);
  add_child(*namespace_token);
  add_child(*namespace_name_token);
  add_child(*using_as);
  add_child(*semicolon_token);
}
FileStatementUsing::~FileStatementUsing()
{}
const AccessModifier &
FileStatementUsing::get_access_modifier() const
{ return *access_modifier; }
const std::string &
FileStatementUsing::get_namespace() const
{ return namespace_name_token->get_fully_qualified_name(); }
const UsingAs &
FileStatementUsing::get_using_as() const
{ return *using_as; }


FileStatement::FileStatement(FileStatementType _statement, const SyntaxNode & _sn)
  : SyntaxNode("file_statement", this)
  , statement(std::move(_statement))
{
  add_child(_sn);
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
  , yyeof(nullptr)
{}
FileStatementList::FileStatementList(Terminal_owned_ptr _yyeof)
  : SyntaxNode("file_statement_list", this)
  , yyeof(std::move(_yyeof))
{
  add_child(*yyeof);
}
FileStatementList::~FileStatementList()
{}
const std::vector<FileStatement_owned_ptr> &
FileStatementList::get_statements() const
{ return statements; }
void
FileStatementList::add_statement(FileStatement_owned_ptr statement)
{
  add_child(*statement);
  statements.push_back(std::move(statement));
}

TranslationUnit::TranslationUnit(
                                 FileStatementList_owned_ptr _file_statement_list,
                                 Terminal_owned_ptr _yyeof_token)
  : SyntaxNode("translation_unit", this)
  , file_statement_list(std::move(_file_statement_list))
  , yyeof_token(std::move(_yyeof_token))
{
  add_child(*file_statement_list);
  add_child(*yyeof_token);
}
TranslationUnit::~TranslationUnit()
{}

const std::vector<FileStatement_owned_ptr> &
TranslationUnit::get_statements() const
{
  return file_statement_list->get_statements();
}
