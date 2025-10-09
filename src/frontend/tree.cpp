#include <gyoji-frontend.hpp>
#include <gyoji-misc/jstring.hpp>

using namespace Gyoji::context;
using namespace Gyoji::frontend;
using namespace Gyoji::frontend::ast;
using namespace Gyoji::frontend::tree;

///////////////////////////////////////////////////
// Terminal
///////////////////////////////////////////////////
Terminal::Terminal(const Token & _token)
    : SyntaxNode(_token.get_type(), this, _token.get_source_ref())
    , token(_token)
    , identifier_type(IDENTIFIER_UNCATEGORIZED)

{}
Terminal::~Terminal()
{}
const TokenID &
Terminal::get_type() const
{ return token.get_type(); }
const std::string &
Terminal::get_value() const
{ return token.get_value(); }
const SourceReference &
Terminal::get_terminal_source_ref() const
{ return token.get_source_ref(); }
const std::string &
Terminal::get_fully_qualified_name() const
{ return fully_qualified_name; }
void
Terminal::set_fully_qualified_name(std::string _name)
{ fully_qualified_name = _name; }

const Terminal::IdentifierType &
Terminal::get_identifier_type() const
{ return identifier_type; }

void
Terminal::set_identifier_type(IdentifierType _identifier_type)
{ identifier_type = _identifier_type; }

///////////////////////////////////////////////////
// TerminalNonSyntax
///////////////////////////////////////////////////
TerminalNonSyntax::TerminalNonSyntax(TerminalNonSyntax::Type _type, const Token & _token)
    : type(_type)
    , token(_token)
{}
TerminalNonSyntax::~TerminalNonSyntax()
{}
const TerminalNonSyntax::Type &
TerminalNonSyntax::get_type() const
{
    return type;
}
const std::string &
TerminalNonSyntax::get_data() const
{
    return token.get_value();
}

///////////////////////////////////////////////////
// AccessQualifier
///////////////////////////////////////////////////
AccessQualifier::AccessQualifier(const Gyoji::context::SourceReference & _source_ref)
    : SyntaxNode(NONTERMINAL_access_qualifier, this, _source_ref)
    , type(Gyoji::frontend::tree::AccessQualifier::AccessQualifierType::UNSPECIFIED)
    , qualifier(nullptr)
{}
AccessQualifier::AccessQualifier(Gyoji::owned<Terminal> _qualifier)
    : SyntaxNode(NONTERMINAL_access_qualifier, this, _qualifier->get_source_ref())
    , qualifier(std::move(_qualifier))
{
    if (qualifier->get_value() == "volatile") {
	type = Gyoji::frontend::tree::AccessQualifier::AccessQualifierType::VOLATILE;
    }
    else if (qualifier->get_value() == "const") {
	type = Gyoji::frontend::tree::AccessQualifier::AccessQualifierType::CONST;
    }
    else {
	type = Gyoji::frontend::tree::AccessQualifier::AccessQualifierType::UNSPECIFIED;
    }
    add_child(*qualifier);
}
AccessQualifier::~AccessQualifier()
{}
const AccessQualifier::AccessQualifierType &
AccessQualifier::get_type() const
{ return type; }

///////////////////////////////////////////////////
// AccessModifier
///////////////////////////////////////////////////
AccessModifier::AccessModifier(Gyoji::owned<Terminal> _modifier)
    : SyntaxNode(NONTERMINAL_access_modifier, this, _modifier->get_source_ref())
    , modifier(std::move(_modifier))
{
    if (modifier->get_value() == "public") {
	type = Gyoji::frontend::tree::AccessModifier::AccessModifierType::PUBLIC;
    }
    else if (modifier->get_value() == "protected") {
	type = Gyoji::frontend::tree::AccessModifier::AccessModifierType::PROTECTED;
    }
    else if (modifier->get_value() == "private") {
	type = Gyoji::frontend::tree::AccessModifier::AccessModifierType::PRIVATE;
    }
    else {
	type = Gyoji::frontend::tree::AccessModifier::AccessModifierType::PUBLIC;
    }
    add_child(*modifier);
}
AccessModifier::AccessModifier(const Gyoji::context::SourceReference & _source_ref)
    : SyntaxNode(NONTERMINAL_access_modifier, this, _source_ref)
    , type(Gyoji::frontend::tree::AccessModifier::AccessModifierType::PUBLIC)
    , modifier(nullptr)
{}
AccessModifier::~AccessModifier()
{}
const AccessModifier::AccessModifierType &
AccessModifier::get_type() const
{
    return type;
}
///////////////////////////////////////////////////
// UnsafeModifier
///////////////////////////////////////////////////
UnsafeModifier::UnsafeModifier(const Gyoji::context::SourceReference & _source_ref)
    : SyntaxNode(NONTERMINAL_unsafe_modifier, this, _source_ref)
    , unsafe_token(nullptr)
{}
UnsafeModifier::UnsafeModifier(Gyoji::owned<Terminal> _unsafe_token)
    : SyntaxNode(NONTERMINAL_unsafe_modifier, this, _unsafe_token->get_source_ref())
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
// TypeName
///////////////////////////////////////////////////
TypeName::TypeName(Gyoji::owned<Terminal> _type_name)
    : SyntaxNode(NONTERMINAL_type_name, this, _type_name->get_source_ref())
    , m_is_expression(false)
    , type_name(std::move(_type_name))
    , typeof_token(nullptr)
    , paren_l_token(nullptr)
    , expression(nullptr)
    , paren_r_token(nullptr)
{
    add_child(*type_name);
}
TypeName::TypeName(Gyoji::owned<Terminal> _typeof_token,
		   Gyoji::owned<Terminal> _paren_l_token,
		   Gyoji::owned<Expression> _expression,
		   Gyoji::owned<Terminal> _paren_r_token
    )
    : SyntaxNode(NONTERMINAL_type_name, this, _typeof_token->get_source_ref())
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
const SourceReference &
TypeName::get_name_source_ref() const
{ return type_name->get_source_ref(); }
const Expression &
TypeName::get_expression() const
{ return *expression; }
///////////////////////////////////////////////////
// TypeName
///////////////////////////////////////////////////
TypeSpecifierCallArgs::TypeSpecifierCallArgs(const Gyoji::context::SourceReference & _source_ref)
    : SyntaxNode(NONTERMINAL_type_specifier_call_args, this, _source_ref)
{}
TypeSpecifierCallArgs::~TypeSpecifierCallArgs()
{}
void
TypeSpecifierCallArgs::add_argument(Gyoji::owned<TypeSpecifier> _argument)
{
    add_child(*_argument);
    arguments.push_back(std::move(_argument));
}
void
TypeSpecifierCallArgs::add_argument(Gyoji::owned<Terminal> _comma_token, Gyoji::owned<TypeSpecifier> _argument)
{
    add_child(*_comma_token);
    add_child(*_argument);
    comma_list.push_back(std::move(_comma_token));
    arguments.push_back(std::move(_argument));
}
const std::vector<Gyoji::owned<TypeSpecifier>> &
TypeSpecifierCallArgs::get_arguments() const
{ return arguments; }

///////////////////////////////////////////////////
TypeSpecifierSimple::TypeSpecifierSimple(
    Gyoji::owned<AccessQualifier> _access_qualifier,
    Gyoji::owned<TypeName> _type_name
    )
    : SyntaxNode(NONTERMINAL_type_specifier_simple, this, _access_qualifier->get_source_ref())
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
    Gyoji::owned<TypeSpecifier> _type_specifier,
    Gyoji::owned<Terminal> _paren_l_token,
    Gyoji::owned<TypeSpecifierCallArgs> _type_specifier_call_args,
    Gyoji::owned<Terminal> _paren_r_token
    )
    : SyntaxNode(NONTERMINAL_type_specifier_template, this, _type_specifier->get_source_ref())
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
    Gyoji::owned<TypeSpecifier> _type_specifier,
    Gyoji::owned<Terminal> _paren_l1_token,
    Gyoji::owned<Terminal> _star_token,
    Gyoji::owned<Terminal> _identifier_token,
    Gyoji::owned<Terminal> _paren_r1_token,
    Gyoji::owned<Terminal> _paren_l2_token,
    Gyoji::owned<FunctionDefinitionArgList> _function_definition_arg_list,
    Gyoji::owned<Terminal> _paren_r2_token
    )
    : SyntaxNode(NONTERMINAL_type_specifier_function_pointer, this, _type_specifier->get_source_ref())
    , type_specifier(std::move(_type_specifier))
    , paren_l1_token(std::move(_paren_l1_token))
    , star_token(std::move(_star_token))
    , identifier_token(std::move(_identifier_token))
    , paren_r1_token(std::move(_paren_r1_token))
    , paren_l2_token(std::move(_paren_l2_token))
    , function_definition_arg_list(std::move(_function_definition_arg_list))
    , paren_r2_token(std::move(_paren_r2_token))
{
    add_child(*type_specifier);
    add_child(*paren_l1_token);
    add_child(*star_token);
    add_child(*identifier_token);
    add_child(*paren_r1_token);
    add_child(*paren_l2_token);
    add_child(*function_definition_arg_list);
    add_child(*paren_r2_token);
}
TypeSpecifierFunctionPointer::~TypeSpecifierFunctionPointer()
{}
const TypeSpecifier & TypeSpecifierFunctionPointer::get_return_type() const
{ return *type_specifier; }
const std::string &
TypeSpecifierFunctionPointer::get_name() const
{ return identifier_token->get_value(); }
const SourceReference &
TypeSpecifierFunctionPointer::get_name_source_ref() const
{ return identifier_token->get_source_ref(); }
const FunctionDefinitionArgList &
TypeSpecifierFunctionPointer::get_args() const
{ return *function_definition_arg_list; }
///////////////////////////////////////////////////
TypeSpecifierPointerTo::TypeSpecifierPointerTo(
    Gyoji::owned<TypeSpecifier> _type_specifier,
    Gyoji::owned<Terminal> _star_token,
    Gyoji::owned<AccessQualifier> _access_qualifier
    )
    : SyntaxNode(NONTERMINAL_type_specifier_pointer_to, this, _type_specifier->get_source_ref())
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
    Gyoji::owned<TypeSpecifier> _type_specifier,
    Gyoji::owned<Terminal> _andpersand_token,
    Gyoji::owned<AccessQualifier> _access_qualifier
    )
    : SyntaxNode(NONTERMINAL_type_specifier_reference_to, this, _type_specifier->get_source_ref())
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
TypeSpecifierArray::TypeSpecifierArray(
    Gyoji::owned<TypeSpecifier> _type_specifier,
    Gyoji::owned<Terminal> _bracket_l_token,
    Gyoji::owned<Terminal> _literal_int_token,
    Gyoji::owned<Terminal> _bracket_r_token
    )
    : SyntaxNode(NONTERMINAL_type_specifier_array, this, _type_specifier->get_source_ref())
    , type_specifier(std::move(_type_specifier))
    , bracket_l_token(std::move(_bracket_l_token))
    , literal_int_token(std::move(_literal_int_token))
    , bracket_r_token(std::move(_bracket_r_token))
{
    add_child(*type_specifier);
    add_child(*bracket_l_token);
    add_child(*literal_int_token);
    add_child(*bracket_r_token);
}
/**
 * Destructor, nothing special.
 */
TypeSpecifierArray::~TypeSpecifierArray()
{}
/**
 * Returns the type that is accessed behind this pointer.
 */
const TypeSpecifier &
TypeSpecifierArray::get_type_specifier() const
{ return *type_specifier; }

const Terminal &
TypeSpecifierArray::get_literal_int_token() const
{ return *literal_int_token; }

///////////////////////////////////////////////////
TypeSpecifier::TypeSpecifier(TypeSpecifier::TypeSpecifierType _type, const SyntaxNode & _sn)
    : SyntaxNode(NONTERMINAL_type_specifier, this, _sn.get_source_ref())
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
FunctionDefinitionArg::FunctionDefinitionArg(
    Gyoji::owned<TypeSpecifier> _type_specifier,
    Gyoji::owned<Terminal> _identifier_token
    )
    : SyntaxNode(NONTERMINAL_function_definition_arg, this, _type_specifier->get_source_ref())
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
const SourceReference &
FunctionDefinitionArg::get_name_source_ref() const
{ return identifier_token->get_source_ref(); }

///////////////////////////////////////////////////
FunctionDefinitionArgList::FunctionDefinitionArgList(const Gyoji::context::SourceReference & _source_ref)
    : SyntaxNode(NONTERMINAL_function_definition_arg_list, this, _source_ref)
{}
FunctionDefinitionArgList::~FunctionDefinitionArgList()
{}
const std::vector<Gyoji::owned<FunctionDefinitionArg>> &
FunctionDefinitionArgList::get_arguments() const
{ return arguments; }
void
FunctionDefinitionArgList::add_argument(Gyoji::owned<FunctionDefinitionArg> _argument)
{
    add_child(*_argument);
    arguments.push_back(std::move(_argument));
}
void
FunctionDefinitionArgList::add_comma(Gyoji::owned<Terminal> _comma)
{
    add_child(*_comma);
    commas.push_back(std::move(_comma));
}


///////////////////////////////////////////////////

FileStatementFunctionDeclaration::FileStatementFunctionDeclaration(
    Gyoji::owned<AccessModifier> _access_modifier,
    Gyoji::owned<UnsafeModifier> _unsafe_modifier,
    Gyoji::owned<TypeSpecifier> _type_specifier,
    Gyoji::owned<Terminal> _name,
    Gyoji::owned<Terminal> _paren_l,
    Gyoji::owned<FunctionDefinitionArgList> _arguments,
    Gyoji::owned<Terminal> _paren_r,
    Gyoji::owned<Terminal> _semicolon
    )
    : SyntaxNode(NONTERMINAL_file_statement_function_declaration, this, _access_modifier->get_source_ref())
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
{ return *access_modifier; }
const UnsafeModifier &
FileStatementFunctionDeclaration::get_unsafe_modifier() const
{ return *unsafe_modifier; }
const TypeSpecifier &
FileStatementFunctionDeclaration::get_return_type() const
{ return *type_specifier; }
const Terminal &
FileStatementFunctionDeclaration::get_name() const
{ return *name; }
const FunctionDefinitionArgList &
FileStatementFunctionDeclaration::get_arguments() const
{ return *arguments; }


///////////////////////////////////////////////////
InitializerExpression::InitializerExpression(
    const SourceReference & _src_ref
    )
    : SyntaxNode(NONTERMINAL_initializer_expression, this, _src_ref)
    , equals_token(nullptr)
    , expression(nullptr)
{}

InitializerExpression::InitializerExpression(
    Gyoji::owned<Terminal> _equals_token,
    Gyoji::owned<Expression> _expression
    )
    : SyntaxNode(NONTERMINAL_initializer_expression, this, _expression->get_source_ref())
    , equals_token(std::move(_equals_token))
    , expression(std::move(_expression))
{
    add_child(*equals_token);
    add_child(*expression);
}
InitializerExpression::~InitializerExpression()
{}

bool
InitializerExpression::has_expression() const
{ return expression != nullptr; }

const Expression &
InitializerExpression::get_expression() const
{ return *expression; }

///////////////////////////////////////////////////
StatementVariableDeclaration::StatementVariableDeclaration(
    Gyoji::owned<TypeSpecifier> _type_specifier,
    Gyoji::owned<Terminal> _identifier_token,
    Gyoji::owned<InitializerExpression> _initializer,
    Gyoji::owned<Terminal> _semicolon_token
    )
    : SyntaxNode(NONTERMINAL_statement_variable_declaration, this, _type_specifier->get_source_ref())
    , type_specifier(std::move(_type_specifier))
    , identifier_token(std::move(_identifier_token))
    , initializer(std::move(_initializer))
    , semicolon_token(std::move(_semicolon_token))
{
    identifier_token->set_fully_qualified_name("");
    add_child(*type_specifier);
    add_child(*identifier_token);
    add_child(*initializer);
    add_child(*semicolon_token);
}
StatementVariableDeclaration::~StatementVariableDeclaration()
{}
const TypeSpecifier &
StatementVariableDeclaration::get_type_specifier() const
{ return *type_specifier;}
const std::string &
StatementVariableDeclaration::get_name() const
{ return identifier_token->get_value(); }
const SourceReference &
StatementVariableDeclaration::get_name_source_ref() const
{ return identifier_token->get_source_ref(); }
const InitializerExpression &
StatementVariableDeclaration::get_initializer_expression() const
{ return *initializer;}
///////////////////////////////////////////////////
StatementBlock::StatementBlock(
    Gyoji::owned<UnsafeModifier> _unsafe_modifier,
    Gyoji::owned<ScopeBody> _scope_body
    )
    : SyntaxNode(NONTERMINAL_statement_block, this, _unsafe_modifier->get_source_ref())
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
    Gyoji::owned<Expression> _expression,
    Gyoji::owned<Terminal> _semicolon_token
    )
    : SyntaxNode(NONTERMINAL_statement_expression, this, _expression->get_source_ref())
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
    Gyoji::owned<Terminal> _if_token,
    Gyoji::owned<Terminal> _paren_l_token,
    Gyoji::owned<Expression> _expression,
    Gyoji::owned<Terminal> _paren_r_token,
    Gyoji::owned<ScopeBody> _if_scope_body,
    Gyoji::owned<Terminal> _else_token,
    Gyoji::owned<ScopeBody> _else_scope_body
    )
    : SyntaxNode(NONTERMINAL_statement_ifelse, this, _if_token->get_source_ref())
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
    Gyoji::owned<Terminal> _if_token,
    Gyoji::owned<Terminal> _paren_l_token,
    Gyoji::owned<Expression> _expression,
    Gyoji::owned<Terminal> _paren_r_token,
    Gyoji::owned<ScopeBody> _if_scope_body,
    Gyoji::owned<Terminal> _else_token,
    Gyoji::owned<StatementIfElse> _else_if
    )
    : SyntaxNode(NONTERMINAL_statement_ifelse, this, _if_token->get_source_ref())
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
    Gyoji::owned<Terminal> _if_token,
    Gyoji::owned<Terminal> _paren_l_token,
    Gyoji::owned<Expression> _expression,
    Gyoji::owned<Terminal> _paren_r_token,
    Gyoji::owned<ScopeBody> _if_scope_body
    )
    : SyntaxNode(NONTERMINAL_statement_ifelse, this, _if_token->get_source_ref())
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
const StatementIfElse &
StatementIfElse::get_else_if() const
{ return *else_if;}
///////////////////////////////////////////////////
StatementWhile::StatementWhile(
    Gyoji::owned<Terminal> _while_token,
    Gyoji::owned<Terminal> _paren_l_token,
    Gyoji::owned<Expression> _expression,
    Gyoji::owned<Terminal> _paren_r_token,
    Gyoji::owned<ScopeBody> _scope_body
    )
    : SyntaxNode(NONTERMINAL_statement_while, this, _while_token->get_source_ref())
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
    Gyoji::owned<Terminal> _for_token,
    Gyoji::owned<Terminal> _paren_l_token,
    Gyoji::owned<Expression> _expression_initial,
    Gyoji::owned<Terminal> _semicolon_initial,
    Gyoji::owned<Expression> _expression_termination,
    Gyoji::owned<Terminal> _semicolon_termination,
    Gyoji::owned<Expression> _expression_increment,
    Gyoji::owned<Terminal> _paren_r_token,
    Gyoji::owned<ScopeBody> _scope_body
    )
    : SyntaxNode(NONTERMINAL_statement_for, this, _for_token->get_source_ref())
    , is_declaration_initializer(false)
    , for_token(std::move(_for_token))
    , paren_l_token(std::move(_paren_l_token))
    , type_specifier(nullptr)
    , identifier_token(nullptr)
    , assignment_token(nullptr)
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
StatementFor::StatementFor(
    Gyoji::owned<Terminal> _for_token,
    Gyoji::owned<Terminal> _paren_l_token,
    Gyoji::owned<TypeSpecifier> _type_specifier,
    Gyoji::owned<Terminal> _identifier_token,
    Gyoji::owned<Terminal> _assignment_token,
    Gyoji::owned<Expression> _expression_initial,
    Gyoji::owned<Terminal> _semicolon_initial,
    Gyoji::owned<Expression> _expression_termination,
    Gyoji::owned<Terminal> _semicolon_termination,
    Gyoji::owned<Expression> _expression_increment,
    Gyoji::owned<Terminal> _paren_r_token,
    Gyoji::owned<ScopeBody> _scope_body
    )
    : SyntaxNode(NONTERMINAL_statement_for, this, _for_token->get_source_ref())
    , is_declaration_initializer(true)
    , for_token(std::move(_for_token))
    , paren_l_token(std::move(_paren_l_token))
    , type_specifier(std::move(_type_specifier))
    , identifier_token(std::move(_identifier_token))
    , assignment_token(std::move(_assignment_token))
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
    add_child(*type_specifier);
    add_child(*identifier_token);
    add_child(*assignment_token);
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
bool
StatementFor::is_declaration() const
{ return is_declaration_initializer; }
const TypeSpecifier &
StatementFor::get_type_specifier() const
{ return *type_specifier; }
const std::string &
StatementFor::get_identifier() const
{ return identifier_token->get_value(); }
const Gyoji::context::SourceReference &
StatementFor::get_identifier_source_ref() const
{ return identifier_token->get_source_ref(); }
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
    Gyoji::owned<Terminal> _default_token,
    Gyoji::owned<Terminal> _colon_token,
    Gyoji::owned<ScopeBody> _scope_body
    )
    : SyntaxNode(NONTERMINAL_statement_switch_block, this, _default_token->get_source_ref())
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
    Gyoji::owned<Terminal> _case_token,
    Gyoji::owned<Expression> _expression,
    Gyoji::owned<Terminal> _colon_token,
    Gyoji::owned<ScopeBody> _scope_body
    )
    : SyntaxNode(NONTERMINAL_statement_switch_block, this, _case_token->get_source_ref())
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

StatementSwitchContent::StatementSwitchContent(const Gyoji::context::SourceReference & _source_ref)
    : SyntaxNode(NONTERMINAL_statement_switch_content, this, _source_ref)
{}
StatementSwitchContent::~StatementSwitchContent()
{}
const std::vector<Gyoji::owned<StatementSwitchBlock>> &
StatementSwitchContent::get_blocks() const
{ return blocks; }
void
StatementSwitchContent::add_block(Gyoji::owned<StatementSwitchBlock> _block)
{
    add_child(*_block.get());
    blocks.push_back(std::move(_block));
}

///////////////////////////////////////////////////
StatementSwitch::StatementSwitch(
    Gyoji::owned<Terminal> _switch_token,
    Gyoji::owned<Terminal> _paren_l_token,
    Gyoji::owned<Expression> _expression,
    Gyoji::owned<Terminal> _paren_r_token,
    Gyoji::owned<Terminal> _brace_l_token,
    Gyoji::owned<StatementSwitchContent> _switch_content,
    Gyoji::owned<Terminal> _brace_r_token
    )
    : SyntaxNode(NONTERMINAL_statement_switch, this, _switch_token->get_source_ref())
    , switch_token(std::move(_switch_token))
    , paren_l_token(std::move(_paren_l_token))
    , expression(std::move(_expression))
    , paren_r_token(std::move(_paren_r_token))
    , brace_l_token(std::move(_brace_l_token))
    , switch_content(std::move(_switch_content))
    , brace_r_token(std::move(_brace_r_token))
{
    add_child(*switch_token);
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
    Gyoji::owned<Terminal> _label_token,
    Gyoji::owned<Terminal> _identifier_token,
    Gyoji::owned<Terminal> _colon_token
    )
    : SyntaxNode(NONTERMINAL_statement_label, this, _label_token->get_source_ref())
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
const SourceReference &
StatementLabel::get_name_source_ref() const
{ return identifier_token->get_source_ref(); }

///////////////////////////////////////////////////
StatementGoto::StatementGoto(
    Gyoji::owned<Terminal> _goto_token,
    Gyoji::owned<Terminal> _identifier_token,
    Gyoji::owned<Terminal> _semicolon_token
    )
    : SyntaxNode(NONTERMINAL_statement_goto, this, _goto_token->get_source_ref())
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
const SourceReference &
StatementGoto::get_label_source_ref() const
{ return identifier_token->get_source_ref(); }
///////////////////////////////////////////////////
StatementBreak::StatementBreak(
    Gyoji::owned<Terminal> _break_token,
    Gyoji::owned<Terminal> _semicolon_token
    )
    : SyntaxNode(NONTERMINAL_statement_break, this, _break_token->get_source_ref())
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
    Gyoji::owned<Terminal> _continue_token,
    Gyoji::owned<Terminal> _semicolon_token
    )
    : SyntaxNode(NONTERMINAL_statement_continue, this, _continue_token->get_source_ref())
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
    Gyoji::owned<Terminal> _return_token,
    Gyoji::owned<Expression> _expression,
    Gyoji::owned<Terminal> _semicolon_token
    )
    : SyntaxNode(NONTERMINAL_statement_return, this, _return_token->get_source_ref())
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
    : SyntaxNode(NONTERMINAL_statement, this, _sn.get_source_ref())
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
StatementList::StatementList(const Gyoji::context::SourceReference & _source_ref)
    : SyntaxNode(NONTERMINAL_statement_list, this, _source_ref)
{}
StatementList::~StatementList()
{}
void
StatementList::add_statement(Gyoji::owned<Statement> _statement)
{
    add_child(*_statement);
    statements.push_back(std::move(_statement));
}
const std::vector<Gyoji::owned<Statement>> &
StatementList::get_statements() const
{ return statements; }

///////////////////////////////////////////////////
ScopeBody::ScopeBody(
    Gyoji::owned<Terminal> _brace_l_token,
    Gyoji::owned<StatementList> _statement_list,
    Gyoji::owned<Terminal> _brace_r_token
    )
    : SyntaxNode(NONTERMINAL_scope_body, this, _brace_l_token->get_source_ref())
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
    Gyoji::owned<AccessModifier> _access_modifier,
    Gyoji::owned<UnsafeModifier> _unsafe_modifier,
    Gyoji::owned<TypeSpecifier> _type_specifier,
    Gyoji::owned<Terminal> _name,
    Gyoji::owned<Terminal> _paren_l,
    Gyoji::owned<FunctionDefinitionArgList> _arguments,
    Gyoji::owned<Terminal> _paren_r,
    Gyoji::owned<ScopeBody> _scope_body
    )
    : SyntaxNode(NONTERMINAL_file_statement_function_declaration, this, _access_modifier->get_source_ref())
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
FileStatementFunctionDefinition::get_return_type() const
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

ArrayLength::ArrayLength(const Gyoji::context::SourceReference & _source_ref)
    : SyntaxNode(NONTERMINAL_array_length, this, _source_ref)
    , bracket_l_token(nullptr)
    , literal_int_token(nullptr)
    , bracket_r_token(nullptr)
{}

ArrayLength::ArrayLength(
    Gyoji::owned<Terminal> _bracket_l_token,
    Gyoji::owned<Terminal> _literal_int_token,
    Gyoji::owned<Terminal> _bracket_r_token
    )
    : SyntaxNode(NONTERMINAL_array_length, this, _bracket_l_token->get_source_ref())
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
const Gyoji::context::SourceReference &
ArrayLength::get_size_source_ref() const
{ return literal_int_token->get_source_ref(); }

///////////////////////////////////////////////////
ClassDeclStart::ClassDeclStart(
    Gyoji::owned<AccessModifier> _access_modifier,
    Gyoji::owned<Terminal> _class_token,
    Gyoji::owned<Terminal> _identifier_token,
    Gyoji::owned<ClassArgumentList> _class_argument_list,
    bool is_identifier
    )
    : SyntaxNode(NONTERMINAL_class_decl_start, this, _access_modifier->get_source_ref())
    , access_modifier(std::move(_access_modifier))
    , class_token(std::move(_class_token))
    , identifier_token(std::move(_identifier_token))
    , class_argument_list(std::move(_class_argument_list))
{
    add_child(*access_modifier);
    add_child(*class_token);
    add_child(*identifier_token);
    add_child(*class_argument_list);
    name = identifier_token->get_fully_qualified_name();
}
ClassDeclStart::~ClassDeclStart()
{}
const AccessModifier &
ClassDeclStart::get_access_modifier() const
{ return *access_modifier; }
const std::string &
ClassDeclStart::get_name() const
{ return name; }
const SourceReference &
ClassDeclStart::get_name_source_ref() const
{ return identifier_token->get_source_ref(); }

const ClassArgumentList &
ClassDeclStart::get_argument_list() const
{ return *class_argument_list; }
///////////////////////////////////////////////////
ClassArgumentList::ClassArgumentList(Gyoji::owned<Terminal> _argument)
    : SyntaxNode(NONTERMINAL_class_argument_list, this, _argument->get_source_ref())
    , paren_l(nullptr)
    , paren_r(nullptr)
{
    add_child(*_argument);
    argument_list.push_back(std::move(_argument));
}
ClassArgumentList::ClassArgumentList(const Gyoji::context::SourceReference & _source_ref)
    : SyntaxNode(NONTERMINAL_class_argument_list, this, _source_ref)
    , paren_l(nullptr)
    , paren_r(nullptr)
{
}
ClassArgumentList::~ClassArgumentList()
{}
void
ClassArgumentList::add_parens(Gyoji::owned<Terminal> _paren_l, Gyoji::owned<Terminal> _paren_r)
{
    paren_l = std::move(_paren_l);
    prepend_child(*paren_l);
    
    paren_r = std::move(_paren_r);
    add_child(*paren_r);
}

void
ClassArgumentList::add_argument(Gyoji::owned<Terminal> _comma_token, Gyoji::owned<Terminal> _argument)
{
    comma_list.push_back(std::move(_comma_token));
    argument_list.push_back(std::move(_argument));
}
const std::vector<Gyoji::owned<Terminal>> &
ClassArgumentList::get_arguments() const
{ return argument_list; }
///////////////////////////////////////////////////
ClassMemberDeclarationVariable::ClassMemberDeclarationVariable(
    Gyoji::owned<AccessModifier> _access_modifier,
    Gyoji::owned<TypeSpecifier> _type_specifier,
    Gyoji::owned<Terminal> _identifier_token,
    Gyoji::owned<Terminal> _semicolon_token
    )
    : SyntaxNode(NONTERMINAL_class_member_declaration_variable, this, _access_modifier->get_source_ref())
    , access_modifier(std::move(_access_modifier))
    , type_specifier(std::move(_type_specifier))
    , identifier_token(std::move(_identifier_token))
    , semicolon_token(std::move(_semicolon_token))
{
    add_child(*access_modifier);
    add_child(*type_specifier);
    add_child(*identifier_token);
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
const SourceReference &
ClassMemberDeclarationVariable::get_name_source_ref() const
{ return identifier_token->get_source_ref(); }
///////////////////////////////////////////////////
ClassMemberDeclarationMethod::ClassMemberDeclarationMethod(
    Gyoji::owned<AccessModifier> _access_modifier,
    Gyoji::owned<TypeSpecifier> _type_specifier,
    Gyoji::owned<Terminal> _identifier_token,
    Gyoji::owned<Terminal> _paren_l_token,
    Gyoji::owned<FunctionDefinitionArgList> _function_definition_arg_list,
    Gyoji::owned<Terminal> _paren_r_token,
    Gyoji::owned<Terminal> _semicolon_token
    )
    : SyntaxNode(NONTERMINAL_class_member_declaration_method, this, _access_modifier->get_source_ref())
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
const SourceReference &
ClassMemberDeclarationMethod::get_name_source_ref() const
{ return identifier_token->get_source_ref(); }
const FunctionDefinitionArgList &
ClassMemberDeclarationMethod::get_arguments() const
{ return *function_definition_arg_list; }
///////////////////////////////////////////////////
ClassMemberDeclarationConstructor::ClassMemberDeclarationConstructor(
    Gyoji::owned<AccessModifier> _access_modifier,
    Gyoji::owned<TypeSpecifier> _type_specifier,
    Gyoji::owned<Terminal> _paren_l_token,
    Gyoji::owned<FunctionDefinitionArgList> _function_definition_arg_list,
    Gyoji::owned<Terminal> _paren_r_token,
    Gyoji::owned<Terminal> _semicolon_token
    )
    : SyntaxNode(NONTERMINAL_class_member_declaration_method, this, _access_modifier->get_source_ref())
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
    Gyoji::owned<AccessModifier> _access_modifier,
    Gyoji::owned<Terminal> _tilde_token,
    Gyoji::owned<TypeSpecifier> _type_specifier,
    Gyoji::owned<Terminal> _paren_l_token,
    Gyoji::owned<FunctionDefinitionArgList> _function_definition_arg_list,
    Gyoji::owned<Terminal> _paren_r_token,
    Gyoji::owned<Terminal> _semicolon_token
    )
    : SyntaxNode(NONTERMINAL_class_member_declaration_method, this, _access_modifier->get_source_ref())
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
    : SyntaxNode(NONTERMINAL_class_member_declaration, this, _sn.get_source_ref())
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


ClassMemberDeclarationList::ClassMemberDeclarationList(const Gyoji::context::SourceReference & _source_ref)
    : SyntaxNode(NONTERMINAL_class_member_declaration_list, this, _source_ref)
{}
ClassMemberDeclarationList::~ClassMemberDeclarationList()
{}
const std::vector<Gyoji::owned<ClassMemberDeclaration>> &
ClassMemberDeclarationList::get_members() const
{
    return members;
}
void
ClassMemberDeclarationList::add_member(Gyoji::owned<ClassMemberDeclaration> _member)
{
    add_child(*_member);
    members.push_back(std::move(_member));
}
///////////////////////////////////////////////////
ClassDeclaration::ClassDeclaration(
    Gyoji::owned<ClassDeclStart> _class_decl_start,
    Gyoji::owned<Terminal> _semicolon_token
    )
    : SyntaxNode(NONTERMINAL_class_declaration, this, _class_decl_start->get_source_ref())
    , class_decl_start(std::move(_class_decl_start))
    , semicolon_token(std::move(_semicolon_token))
{
    add_child(*class_decl_start);
    add_child(*semicolon_token);
}
ClassDeclaration::~ClassDeclaration()
{}
const AccessModifier &
ClassDeclaration::get_access_modifier() const
{
    return class_decl_start->get_access_modifier();
}
const std::string &
ClassDeclaration::get_name() const
{
    return class_decl_start->get_name();
}
const SourceReference &
ClassDeclaration::get_name_source_ref() const
{ return class_decl_start->get_name_source_ref(); }

const ClassArgumentList &
ClassDeclaration::get_argument_list() const
{
    return class_decl_start->get_argument_list();
}

///////////////////////////////////////////////////
ClassDefinition::ClassDefinition(
    Gyoji::owned<ClassDeclStart> _class_decl_start,
    Gyoji::owned<Terminal> _brace_l_token,
    Gyoji::owned<ClassMemberDeclarationList> _class_member_declaration_list,
    Gyoji::owned<Terminal> _brace_r_token,
    Gyoji::owned<Terminal> _semicolon_token
    )
    : SyntaxNode(NONTERMINAL_class_definition, this, _class_decl_start->get_source_ref())
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
const SourceReference &
ClassDefinition::get_name_source_ref() const
{ return class_decl_start->get_name_source_ref(); }
const ClassArgumentList &
ClassDefinition::get_argument_list() const
{
    return class_decl_start->get_argument_list();
}
const std::vector<Gyoji::owned<ClassMemberDeclaration>> &
ClassDefinition::get_members() const
{ return class_member_declaration_list->get_members(); }

///////////////////////////////////////////////////
TypeDefinition::TypeDefinition(
    Gyoji::owned<AccessModifier> _access_modifier,
    Gyoji::owned<Terminal> _typedef_token,
    Gyoji::owned<TypeSpecifier> _type_specifier,
    Gyoji::owned<Terminal> _identifier_token,
    Gyoji::owned<Terminal> _semicolon_token
    )
    : SyntaxNode(NONTERMINAL_type_definition, this, _access_modifier->get_source_ref())
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
const SourceReference &
TypeDefinition::get_name_source_ref() const
{ return identifier_token->get_source_ref(); }
const TypeSpecifier &
TypeDefinition::get_type_specifier() const
{ return *type_specifier; }

///////////////////////////////////////////////////
EnumDefinitionValue::EnumDefinitionValue(
    Gyoji::owned<Terminal> _identifier_token,
    Gyoji::owned<Terminal> _equals_token,
    Gyoji::owned<Expression> _expression_primary,
    Gyoji::owned<Terminal> _semicolon_token
    )
    : SyntaxNode(NONTERMINAL_enum_definition_value, this, _identifier_token->get_source_ref())
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
const SourceReference &
EnumDefinitionValue::get_name_source_ref() const
{ return identifier_token->get_source_ref(); }

const Expression &
EnumDefinitionValue::get_expression() const
{ return *expression_primary; }

///////////////////////////////////////////////////
EnumDefinitionValueList::EnumDefinitionValueList(const Gyoji::context::SourceReference & _source_ref)
    : SyntaxNode(NONTERMINAL_enum_definition_value_list, this, _source_ref)
{}

EnumDefinitionValueList::~EnumDefinitionValueList()
{}
void
EnumDefinitionValueList::add_value(Gyoji::owned<EnumDefinitionValue> _value)
{
    add_child(*_value);
    values.push_back(std::move(_value));
}
const std::vector<Gyoji::owned<EnumDefinitionValue>> &
EnumDefinitionValueList::get_values() const
{ return values; }

///////////////////////////////////////////////////
EnumDefinition::EnumDefinition(
    Gyoji::owned<AccessModifier> _access_modifier,
    Gyoji::owned<Terminal> _enum_token,
    Gyoji::owned<Terminal> _type_name_token,
    Gyoji::owned<Terminal> _identifier_token,
    Gyoji::owned<Terminal> _brace_l_token,
    Gyoji::owned<EnumDefinitionValueList> _enum_value_list,
    Gyoji::owned<Terminal> _brace_r_token,
    Gyoji::owned<Terminal> _semicolon_token
    )
    : SyntaxNode(NONTERMINAL_enum_definition, this, _access_modifier->get_source_ref())
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
EnumDefinition::get_type_name() const
{ return type_name_token->get_value(); }
const SourceReference &
EnumDefinition::get_type_name_source_ref() const
{ return type_name_token->get_source_ref(); }
const std::string &
EnumDefinition::get_name() const
{ return identifier_token->get_value(); }
const SourceReference &
EnumDefinition::get_name_source_ref() const
{ return identifier_token->get_source_ref(); }
const EnumDefinitionValueList &
EnumDefinition::get_value_list() const
{ return *enum_value_list; }
///////////////////////////////////////////////////
ExpressionPrimaryIdentifier::ExpressionPrimaryIdentifier(Gyoji::owned<Terminal> _identifier_token)
    : SyntaxNode(NONTERMINAL_expression_primary_identifier, this, _identifier_token->get_source_ref())
    , identifier_token(std::move(_identifier_token))
{
    add_child(*identifier_token);
}
ExpressionPrimaryIdentifier::~ExpressionPrimaryIdentifier()
{}
const Terminal &
ExpressionPrimaryIdentifier::get_identifier() const
{ return *identifier_token; }
///////////////////////////////////////////////////
ExpressionPrimaryNested::ExpressionPrimaryNested(
    Gyoji::owned<Terminal> _paren_l_token,
    Gyoji::owned<Expression> _expression,
    Gyoji::owned<Terminal> _paren_r_token
    )
    : SyntaxNode(NONTERMINAL_expression_primary_nested, this, _paren_l_token->get_source_ref())
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
    Gyoji::owned<Terminal> _literal_token
    )
    : SyntaxNode(NONTERMINAL_expression_primary_literal_int, this, _literal_token->get_source_ref())
    , literal_token(std::move(_literal_token))
{
    add_child(*literal_token);
}

ExpressionPrimaryLiteralInt::~ExpressionPrimaryLiteralInt()
{}
const std::string &
ExpressionPrimaryLiteralInt::get_value() const
{ return literal_token->get_value(); }

const Terminal &
ExpressionPrimaryLiteralInt::get_literal_int_token() const
{ return *literal_token; }

const SourceReference &
ExpressionPrimaryLiteralInt::get_value_source_ref() const
{ return literal_token->get_source_ref(); }
///////////////////////////////////////////////////
ExpressionPrimaryLiteralChar::ExpressionPrimaryLiteralChar(
    Gyoji::owned<Terminal> _literal_token
    )
    : SyntaxNode(NONTERMINAL_expression_primary_literal_char, this, _literal_token->get_source_ref())
    , literal_token(std::move(_literal_token))
{
    add_child(*literal_token);
}
ExpressionPrimaryLiteralChar::~ExpressionPrimaryLiteralChar()
{}
std::string
ExpressionPrimaryLiteralChar::get_value() const
{
    // Remove the leading and trailing single quote (')
    // before passing it down to the semantics
    // layer.
    const std::string & token_value = literal_token->get_value();
    size_t size = token_value.size();
    return token_value.substr(1, size-2);
}
const SourceReference &
ExpressionPrimaryLiteralChar::get_value_source_ref() const
{ return literal_token->get_source_ref(); }
///////////////////////////////////////////////////
ExpressionPrimaryLiteralString::ExpressionPrimaryLiteralString(
    Gyoji::owned<Terminal> _literal_token
    )
    : SyntaxNode(NONTERMINAL_expression_primary_literal_string, this, _literal_token->get_source_ref())
    , literal_token(std::move(_literal_token))
{
    add_child(*literal_token);
}
ExpressionPrimaryLiteralString::~ExpressionPrimaryLiteralString()
{}
// Instead of just returning a reference, we will
// concatenate the strings together and return the
// resulting string literal still escaped as it was
// in the source file.  The unescape of the string
// is delegated to the FunctionResolver because it
// is in a position to return errors whereas this
// class is mainly intended to be just a fairly
// transparent data container with no "real" logic.
std::string
ExpressionPrimaryLiteralString::get_value() const
{
    std::string retstring;
    // Strip the leading and trailing " from the string
    const std::string & token_value = literal_token->get_value();
    size_t size = token_value.size();
    std::string firstpart = token_value.substr(1, size-2);

    retstring = firstpart;

    // Do the same thing with the remaining
    // strings and append them to the literal.
    for (const auto & next_token : additional_strings) {
	const std::string & next_token_value = next_token->get_value();
	size = next_token_value.size();
	std::string nextpart = next_token_value.substr(1, size-2);
	retstring += nextpart;
    }
    
    return retstring;
}
const SourceReference &
ExpressionPrimaryLiteralString::get_value_source_ref() const
{ return literal_token->get_source_ref(); }
void
ExpressionPrimaryLiteralString::add_string(Gyoji::owned<Terminal> _added)
{
    add_child(*_added);
    additional_strings.push_back(std::move(_added));
}
///////////////////////////////////////////////////

static std::string f32_type("f32");
static std::string f64_type("f64");

ExpressionPrimaryLiteralFloat::ExpressionPrimaryLiteralFloat(
    Gyoji::owned<Terminal> _literal_token
    )
    : SyntaxNode(NONTERMINAL_expression_primary_literal_float, this, _literal_token->get_source_ref())
    , literal_token(std::move(_literal_token))
{
    add_child(*literal_token);

    const std::string & token_value = literal_token->get_value();
    size_t len = token_value.size();
    if (Gyoji::misc::endswith(token_value, f32_type)) {
	float_part = token_value.substr(0, len - f32_type.size());
	type_part = f32_type;
    }
    else if (Gyoji::misc::endswith(token_value, f64_type)) {
	float_part = token_value.substr(0, len - f64_type.size());
	type_part = f64_type;
    }
    else {
	// We make the default a 'double'
	// because in this day and age, there's
	// no good reason to default to a 'float'.
	float_part = token_value;
	type_part = f64_type;
    }
}
ExpressionPrimaryLiteralFloat::~ExpressionPrimaryLiteralFloat()
{}
const std::string &
ExpressionPrimaryLiteralFloat::get_value() const
{ return float_part; }
const std::string &
ExpressionPrimaryLiteralFloat::get_type() const
{ return type_part; }
const SourceReference &
ExpressionPrimaryLiteralFloat::get_value_source_ref() const
{ return literal_token->get_source_ref(); }

///////////////////////////////////////////////////
ExpressionPrimaryLiteralBool::ExpressionPrimaryLiteralBool(
    Gyoji::owned<Terminal> _literal_token
    )
    : SyntaxNode(NONTERMINAL_expression_primary_literal_bool, this, _literal_token->get_source_ref())
    , literal_token(std::move(_literal_token))
{
    add_child(*literal_token);
    value = (literal_token->get_value() == std::string("true"));
}
ExpressionPrimaryLiteralBool::~ExpressionPrimaryLiteralBool()
{}
bool
ExpressionPrimaryLiteralBool::get_value() const
{ return value; }
///////////////////////////////////////////////////

ExpressionPrimaryLiteralNull::ExpressionPrimaryLiteralNull(
    Gyoji::owned<Terminal> _literal_token
    )
    : SyntaxNode(NONTERMINAL_expression_primary_literal_null, this, _literal_token->get_source_ref())
    , literal_token(std::move(_literal_token))      
{
    add_child(*literal_token);
}
ExpressionPrimaryLiteralNull::~ExpressionPrimaryLiteralNull()
{}

///////////////////////////////////////////////////
ExpressionPostfixArrayIndex::ExpressionPostfixArrayIndex(
    Gyoji::owned<Expression> _array_expression,
    Gyoji::owned<Terminal> _bracket_l_token,
    Gyoji::owned<Expression> _index_expression,
    Gyoji::owned<Terminal> _bracket_r_token
    )
    : SyntaxNode(NONTERMINAL_expression_postfix_array_index, this, _array_expression->get_source_ref())
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
ArgumentExpressionList::ArgumentExpressionList(const Gyoji::context::SourceReference & _source_ref)
    : SyntaxNode(NONTERMINAL_argument_expression_list, this, _source_ref)
{}
ArgumentExpressionList::~ArgumentExpressionList()
{}
const std::vector<Gyoji::owned<Expression>> &
ArgumentExpressionList::get_arguments() const
{ return arguments; }
void
ArgumentExpressionList::add_argument(Gyoji::owned<Expression> _argument)
{
    add_child(*_argument);
    arguments.push_back(std::move(_argument));
}
void
ArgumentExpressionList::add_argument(Gyoji::owned<Terminal> _comma_token, Gyoji::owned<Expression> _argument)
{
    add_child(*_comma_token);
    add_child(*_argument);
    comma_list.push_back(std::move(_comma_token));
    arguments.push_back(std::move(_argument));
}

///////////////////////////////////////////////////
ExpressionPostfixFunctionCall::ExpressionPostfixFunctionCall(
    Gyoji::owned<Expression> _function_expression,
    Gyoji::owned<Terminal> _paren_l_token,
    Gyoji::owned<ArgumentExpressionList> _arguments,
    Gyoji::owned<Terminal> _paren_r_token
    )
    : SyntaxNode(NONTERMINAL_expression_postfix_function_call, this, _function_expression->get_source_ref())
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
    Gyoji::owned<Expression> _expression,
    Gyoji::owned<Terminal> _dot_token,
    Gyoji::owned<Terminal> _identifier_token
    )
    : SyntaxNode(NONTERMINAL_expression_postfix_dot, this, _expression->get_source_ref())
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
const SourceReference &
ExpressionPostfixDot::get_identifier_source_ref() const
{ return identifier_token->get_source_ref(); }

///////////////////////////////////////////////////
ExpressionPostfixArrow::ExpressionPostfixArrow(
    Gyoji::owned<Expression> _expression,
    Gyoji::owned<Terminal> _arrow_token,
    Gyoji::owned<Terminal> _identifier_token
    )
    : SyntaxNode(NONTERMINAL_expression_postfix_arrow, this, _expression->get_source_ref())
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
const SourceReference &
ExpressionPostfixArrow::get_identifier_source_ref() const
{ return identifier_token->get_source_ref(); }

///////////////////////////////////////////////////
ExpressionPostfixIncDec::ExpressionPostfixIncDec(
    Gyoji::owned<Expression> _expression,
    Gyoji::owned<Terminal> _operator_token,
    OperationType _type
    )
    : SyntaxNode(NONTERMINAL_expression_postfix_incdec, this, _expression->get_source_ref())
    , type(_type)
    , operator_token(std::move(_operator_token))
    , expression(std::move(_expression))
{
    add_child(*expression);
    add_child(*operator_token);
}
ExpressionPostfixIncDec::~ExpressionPostfixIncDec()
{}
const ExpressionPostfixIncDec::OperationType &
ExpressionPostfixIncDec::get_type() const
{ return type; }
const Expression &
ExpressionPostfixIncDec::get_expression() const
{ return *expression; }
///////////////////////////////////////////////////
ExpressionUnaryPrefix::ExpressionUnaryPrefix(
    Gyoji::owned<Terminal> _operator_token,
    Gyoji::owned<Expression> _expression,
    OperationType _type
    )
    : SyntaxNode(NONTERMINAL_expression_unary_prefix, this, _operator_token->get_source_ref())
    , type(_type)
    , operator_token(std::move(_operator_token))
    , expression(std::move(_expression))
{
    add_child(*operator_token);
    add_child(*expression);
}
ExpressionUnaryPrefix::~ExpressionUnaryPrefix()
{}
const ExpressionUnaryPrefix::OperationType &
ExpressionUnaryPrefix::get_type() const
{ return type; }
const Gyoji::context::SourceReference &
ExpressionUnaryPrefix::get_operator_source_ref() const
{ return operator_token->get_source_ref(); }
const Expression &
ExpressionUnaryPrefix::get_expression() const
{ return *expression; }
///////////////////////////////////////////////////
ExpressionUnarySizeofType::ExpressionUnarySizeofType(
    Gyoji::owned<Terminal> _sizeof_token,
    Gyoji::owned<Terminal> _paren_l_token,
    Gyoji::owned<TypeSpecifier> _type_specifier,
    Gyoji::owned<Terminal> _paren_r_token
    )
    : SyntaxNode(NONTERMINAL_expression_unary_sizeof_type, this, _sizeof_token->get_source_ref())
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
    Gyoji::owned<Terminal> _cast_token,
    Gyoji::owned<Terminal> _paren_l_token,
    Gyoji::owned<TypeSpecifier> _type_specifier,
    Gyoji::owned<Terminal> _comma_token,
    Gyoji::owned<Expression> _expression,
    Gyoji::owned<Terminal> _paren_r_token
    )
    : SyntaxNode(NONTERMINAL_expression_cast, this, _cast_token->get_source_ref())
    , cast_token(std::move(_cast_token))
    , paren_l_token(std::move(_paren_l_token))
    , type_specifier(std::move(_type_specifier))
    , comma_token(std::move(_comma_token))
    , expression(std::move(_expression))
    , paren_r_token(std::move(_paren_r_token))
{
    add_child(*cast_token);
    add_child(*paren_l_token);
    add_child(*type_specifier);
    add_child(*comma_token);
    add_child(*expression);
    add_child(*paren_r_token);
}
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
    Gyoji::owned<Expression> _expression_a,
    Gyoji::owned<Terminal> _operator_token,
    Gyoji::owned<Expression> _expression_b,
    OperationType _type
    )
    : SyntaxNode(NONTERMINAL_expression_binary, this, _expression_a->get_source_ref())
    , type(_type)
    , expression_a(std::move(_expression_a))
    , operator_token(std::move(_operator_token))
    , operator_token2(nullptr)
    , expression_b(std::move(_expression_b))
{
    add_child(*expression_a);
    add_child(*operator_token);
    add_child(*expression_b);
}
ExpressionBinary::ExpressionBinary(
    Gyoji::owned<Expression> _expression_a,
    Gyoji::owned<Terminal> _operator_token,
    Gyoji::owned<Terminal> _operator_token2,
    Gyoji::owned<Expression> _expression_b,
    OperationType _type
    )
    : SyntaxNode(NONTERMINAL_expression_binary, this, _expression_a->get_source_ref())
    , type(_type)
    , expression_a(std::move(_expression_a))
    , operator_token(std::move(_operator_token))
    , operator_token2(std::move(_operator_token2))
    , expression_b(std::move(_expression_b))
{
    add_child(*expression_a);
    add_child(*operator_token);
    add_child(*operator_token2);
    add_child(*expression_b);
}
ExpressionBinary::~ExpressionBinary()
{}
const Expression &
ExpressionBinary::get_a() const
{ return *expression_a; }
const ExpressionBinary::OperationType &
ExpressionBinary::get_operator() const
{ return type; }
const Gyoji::context::SourceReference &
ExpressionBinary::get_operator_source_ref() const
{ return operator_token->get_source_ref(); }
const Expression &
ExpressionBinary::get_b() const
{ return *expression_b; }
///////////////////////////////////////////////////
ExpressionTrinary::ExpressionTrinary(
    Gyoji::owned<Expression> _condition,
    Gyoji::owned<Terminal> _questionmark_token,
    Gyoji::owned<Expression> _if_expression,
    Gyoji::owned<Terminal> _colon_token,
    Gyoji::owned<Expression> _else_expression
    )
    : SyntaxNode(NONTERMINAL_expression_trinary, this, _condition->get_source_ref())
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
    : SyntaxNode(NONTERMINAL_expression, this, _sn.get_source_ref())
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
    Gyoji::owned<Terminal> _equals_token,
    Gyoji::owned<Expression> _expression
    )
    : SyntaxNode(NONTERMINAL_global_initializer_expression_primary, this, _equals_token->get_source_ref())
    , equals_token(std::move(_equals_token))
    , expression(std::move(_expression))
{
    add_child(*equals_token);
    add_child(*expression);
}

GlobalInitializerExpressionPrimary::~GlobalInitializerExpressionPrimary()
{}

const Expression &
GlobalInitializerExpressionPrimary::get_expression() const
{ return *expression; };

///////////////////////////////////////////////////
StructInitializer::StructInitializer(
    Gyoji::owned<Terminal> _dot_token,
    Gyoji::owned<Terminal> _identifier_token,
    Gyoji::owned<GlobalInitializer> _global_initializer,
    Gyoji::owned<Terminal> _semicolon_token
    )
    : SyntaxNode(NONTERMINAL_struct_initializer, this, _dot_token->get_source_ref())
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
StructInitializerList::StructInitializerList(const Gyoji::context::SourceReference & _source_ref)
    : SyntaxNode(NONTERMINAL_struct_initializer_list, this, _source_ref)
{}
StructInitializerList::~StructInitializerList()
{}
void
StructInitializerList::add_initializer(Gyoji::owned<StructInitializer> initializer)
{
    add_child(*initializer);
    initializers.push_back(std::move(initializer));
}
const std::vector<Gyoji::owned<StructInitializer>> &
StructInitializerList::get_initializers() const
{
    return initializers;
}

///////////////////////////////////////////////////
GlobalInitializerAddressofExpressionPrimary::GlobalInitializerAddressofExpressionPrimary(
    Gyoji::owned<Terminal> _equals_token,
    Gyoji::owned<Terminal> _addressof_token,
    Gyoji::owned<Expression> _expression
    )
    : SyntaxNode(NONTERMINAL_global_initializer_addressof_expression_primary, this, _equals_token->get_source_ref())
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
const Expression &
GlobalInitializerAddressofExpressionPrimary::get_expression() const
{ return *expression; };


///////////////////////////////////////////////////
GlobalInitializerStructInitializerList::GlobalInitializerStructInitializerList(
    Gyoji::owned<Terminal> _equals_token,
    Gyoji::owned<Terminal> _brace_l_token,
    Gyoji::owned<StructInitializerList> _struct_initializer,
    Gyoji::owned<Terminal> _brace_r_token
    )
    : SyntaxNode(NONTERMINAL_global_initializer_struct_initializer_list, this, _equals_token->get_source_ref())
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
    : SyntaxNode(NONTERMINAL_global_initializer, this, _sn.get_source_ref())
    , initializer(std::move(_initializer))
{
    add_child(_sn);
}
GlobalInitializer::GlobalInitializer(const Gyoji::context::SourceReference & _source_ref)
    : SyntaxNode(NONTERMINAL_global_initializer, this, _source_ref)
    , initializer(nullptr)
{}
GlobalInitializer::~GlobalInitializer()
{}
const GlobalInitializer::GlobalInitializerType &
GlobalInitializer::get_initializer() const
{ return initializer; }


///////////////////////////////////////////////////
FileStatementGlobalDefinition::FileStatementGlobalDefinition(
    Gyoji::owned<AccessModifier> _access_modifier,
    Gyoji::owned<UnsafeModifier> _unsafe_modifier,
    Gyoji::owned<TypeSpecifier> _type_specifier,
    Gyoji::owned<Terminal> _name,
    Gyoji::owned<GlobalInitializer> _global_initializer,
    Gyoji::owned<Terminal> _semicolon
    )
    : SyntaxNode(NONTERMINAL_file_statement_global_definition, this, _access_modifier->get_source_ref())
    , access_modifier(std::move(_access_modifier))
    , unsafe_modifier(std::move(_unsafe_modifier))
    , type_specifier(std::move(_type_specifier))
    , name(std::move(_name))
    , global_initializer(std::move(_global_initializer))
    , semicolon(std::move(_semicolon))
{
    add_child(*access_modifier);
    add_child(*unsafe_modifier);
    add_child(*type_specifier);
    add_child(*name);
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
const std::string &
FileStatementGlobalDefinition::get_name() const
{ return name->get_value(); }
const SourceReference &
FileStatementGlobalDefinition::get_name_source_ref() const
{ return name->get_source_ref(); }
const GlobalInitializer &
FileStatementGlobalDefinition::get_global_initializer() const
{ return *global_initializer; }

///////////////////////////////////////////////////
NamespaceDeclaration::NamespaceDeclaration(
    Gyoji::owned<AccessModifier> _access_modifier,
    Gyoji::owned<Terminal> _namespace_token,
    Gyoji::owned<Terminal> _identifier_token
    )
    : SyntaxNode(NONTERMINAL_namespace_declaration, this, _access_modifier->get_source_ref())
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
    Gyoji::owned<NamespaceDeclaration> _namespace_declaration,
    Gyoji::owned<Terminal> _brace_l_token,
    Gyoji::owned<FileStatementList> _file_statement_list,
    Gyoji::owned<Terminal> _brace_r_token,
    Gyoji::owned<Terminal> _semicolon_token
    )
    : SyntaxNode(NONTERMINAL_file_statement_namespace, this, _namespace_declaration->get_source_ref())
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
    Gyoji::owned<Terminal> _as_token,
    Gyoji::owned<Terminal> _identifier_token
    )
    : SyntaxNode(NONTERMINAL_using_as, this, _as_token->get_source_ref())
    , aas(true)
    , as_token(std::move(_as_token))
    , identifier_token(std::move(_identifier_token))
{
    using_name = identifier_token->get_value();
    add_child(*as_token);
    add_child(*identifier_token);
}
UsingAs::UsingAs(const Gyoji::context::SourceReference & _source_ref)
    : SyntaxNode(NONTERMINAL_using_as, this, _source_ref)
    , aas(false)
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
bool
UsingAs::is_as() const
{ return aas; }
const SourceReference &
UsingAs::get_using_name_source_ref() const
{ return identifier_token->get_source_ref(); }

FileStatementUsing::FileStatementUsing(
    Gyoji::owned<AccessModifier> _access_modifier,
    Gyoji::owned<Terminal> _using,
    Gyoji::owned<Terminal> _namespace,
    Gyoji::owned<Terminal> _namespace_name,
    Gyoji::owned<UsingAs> _using_as,
    Gyoji::owned<Terminal> _semicolon)
    : SyntaxNode(NONTERMINAL_file_statement_using, this, _access_modifier->get_source_ref())
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
    : SyntaxNode(NONTERMINAL_file_statement, this, _sn.get_source_ref())
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


FileStatementList::FileStatementList(const Gyoji::context::SourceReference & _source_ref)
    : SyntaxNode(NONTERMINAL_file_statement_list, this, _source_ref)
    , yyeof(nullptr)
{}
FileStatementList::FileStatementList(Gyoji::owned<Terminal> _yyeof)
    : SyntaxNode(NONTERMINAL_file_statement_list, this, _yyeof->get_source_ref())
    , yyeof(std::move(_yyeof))
{
    add_child(*yyeof);
}
FileStatementList::~FileStatementList()
{}
const std::vector<Gyoji::owned<FileStatement>> &
FileStatementList::get_statements() const
{ return statements; }
void
FileStatementList::add_statement(Gyoji::owned<FileStatement> statement)
{
    add_child(*statement);
    statements.push_back(std::move(statement));
}

TranslationUnit::TranslationUnit(
    Gyoji::owned<FileStatementList> _file_statement_list,
    Gyoji::owned<Terminal> _yyeof_token)
    : SyntaxNode(NONTERMINAL_translation_unit, this, _yyeof_token->get_source_ref())
    , file_statement_list(std::move(_file_statement_list))
    , yyeof_token(std::move(_yyeof_token))
{
    add_child(*file_statement_list);
    add_child(*yyeof_token);
}
TranslationUnit::~TranslationUnit()
{}

const std::vector<Gyoji::owned<FileStatement>> &
TranslationUnit::get_statements() const
{
    return file_statement_list->get_statements();
}
