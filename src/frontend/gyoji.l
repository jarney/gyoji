%{
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
#include <cstdlib>
#include <memory>
#include <gyoji-frontend.hpp>
#include <gyoji.y.hpp>

using namespace Gyoji::context;
using namespace Gyoji::frontend;
using namespace Gyoji::frontend::tree;
using namespace Gyoji::frontend::namespaces;
using namespace Gyoji::frontend::yacc;


std::vector<Gyoji::owned<TerminalNonSyntax>> non_syntax_data;

#define DEBUG_TERMINALS 0
#if DEBUG_TERMINALS
#define PRINT_TERMINALS(s,t)                                                  \
    printf("%s : %s%s\n", s, t,                                               \
                          (node->get_fully_qualified_name().size() > 0) ?           \
                          (std::string(" : ") + node->get_fully_qualified_name()).c_str() : \
                          std::string().c_str());
#else
#define PRINT_TERMINALS(s,t) /**/
#endif

#define YY_INPUT(buf,result,max_size)                                          \
    ((LexContext*)yyget_extra(yyscanner))->input_source.read(buf, result, max_size)


void move_array(
                std::vector<Gyoji::owned<TerminalNonSyntax>> & dst,
                std::vector<Gyoji::owned<TerminalNonSyntax>> & src
                )
{
    dst.clear();
    for (auto & srcitem : src) {
        dst.push_back(std::move(srcitem));
    }
    src.clear();
}

#define TOKEN_APPEND()                                               \
{                                                                    \
    LexContext *lc = (LexContext*)yyget_extra(yyscanner);            \
    lc->compiler_context.get_token_stream()                          \
        .append_token(std::string(yytext));                          \
}

#define TOKEN_ADD(nodetype)                                          \
    LexContext *lc = (LexContext*)yyget_extra(yyscanner);            \
    const Token &tok =                                               \
        lc->compiler_context.get_token_stream()                      \
            .add_token(                                              \
                Gyoji::frontend::tree::TERMINAL_ ##nodetype,         \
                std::string(yytext),                                 \
                lc->compiler_context.get_filename(),                 \
                lc->line,                                            \
                lc->column                                           \
            );                                                       \
    lc->column += strlen(yytext);                                    \

#define START_NODE(nodetype)                                         \
    TOKEN_ADD(nodetype);                                             \
    Terminal* node = new Terminal(tok);                              \
    move_array(node->non_syntax, non_syntax_data);                   \
    yylval->emplace<Gyoji::owned<Terminal>>(node);

#define RETURN_NODE(nodetype)                                        \
    return YaccParser::token::nodetype;

#define PROCESS_NODE(nodetype)                                       \
    START_NODE(nodetype)                                             \
    PRINT_TERMINALS(#nodetype, yytext)                               \
    RETURN_NODE(nodetype);

#define PROCESS_IDENTIFIER(nodetype, entity)                         \
    START_NODE(nodetype)                                             \
    node->set_ns2_entity(entity);                                    \
    PRINT_TERMINALS(#nodetype, yytext)                               \
    RETURN_NODE(nodetype);

// \&\& { PROCESS_NODE(AND_OP);}
//|"0b"|"0")
//radix               ("0x"|"0b"|"0o")
//opt_radix           ({radix}?)
%}
 
%option reentrant interactive noyywrap nodefault

%x COMMENT

opt_sign            (\-?)

bin_digit           [01]
oct_digit           ({bin_digit}|[234567])
dec_digit           ({oct_digit}|[89])
hex_digit           ({dec_digit}|[aAbBcCdDeEfF])

bin_dseq            ("0b"{bin_digit}({dec_digit}|\_)*)
oct_dseq            ("0o"{oct_digit}({dec_digit}|\_)*)
dec_dseq            ({dec_digit}({dec_digit}|\_)*)
hex_dseq            ("0x"{hex_digit}({hex_digit}|\_)*)

int_dseq            ({bin_dseq}|{oct_dseq}|{dec_dseq}|{hex_dseq})

opt_dec_dseq        ({dec_dseq}?)

frac                (({opt_dec_dseq}"."{dec_dseq})|{dec_dseq}".")
exp                 ([eE][+-]?{dec_dseq})
exp_opt             ({exp}?)

integer             ({opt_sign}{int_dseq}{opt_integer_size})
integer_size        ("u64"|"u32"|"u16"|"u8"|"i64"|"i32"|"i16"|"i8")
opt_integer_size    ({integer_size}?)

float               ({opt_sign}(({frac}{exp_opt})|({dec_dseq}{exp}))({opt_float_size}))
float_size          ("f32"|"f64")
opt_float_size      ({float_size}?)

identifier          ([a-zA-Z_][a-zA-Z_0-9]*)
whitespace          ([[:space:]])
%%

namespace {PROCESS_NODE(NAMESPACE);}
using {PROCESS_NODE(USING);}
as {PROCESS_NODE(AS);}
typedef {PROCESS_NODE(TYPEDEF);}
class {PROCESS_NODE(CLASS);}
public {PROCESS_NODE(PUBLIC);}
enum {PROCESS_NODE(ENUM);}

private {PROCESS_NODE(PRIVATE);}
protected {PROCESS_NODE(PROTECTED);}

struct {PROCESS_NODE(STRUCT);}
union {PROCESS_NODE(UNION);}

if {PROCESS_NODE(IF);}
else {PROCESS_NODE(ELSE);}
while {PROCESS_NODE(WHILE);}
for {PROCESS_NODE(FOR);}
switch {PROCESS_NODE(SWITCH);}
return {PROCESS_NODE(RETURN);}
break {PROCESS_NODE(BREAK);}
continue {PROCESS_NODE(CONTINUE);}
label {PROCESS_NODE(LABEL);}
goto {PROCESS_NODE(GOTO);}
case {PROCESS_NODE(CASE);}
default {PROCESS_NODE(DEFAULT);}

sizeof {PROCESS_NODE(SIZEOF);}
cast {PROCESS_NODE(CAST);}
typeof {PROCESS_NODE(TYPEOF);}

const {PROCESS_NODE(CONST);}
volatile {PROCESS_NODE(VOLATILE);}
unsafe {PROCESS_NODE(UNSAFE);} 

static {PROCESS_NODE(STATIC);}

; {PROCESS_NODE(SEMICOLON);}
-> {PROCESS_NODE(PTR_OP);}
>> {PROCESS_NODE(RIGHT_OP);}
\+\+ {PROCESS_NODE(INC_OP);}
\-\- {PROCESS_NODE(DEC_OP);}
\<\< {PROCESS_NODE(LEFT_OP);}
\< { PROCESS_NODE(COMPARE_LESS); }
\> { PROCESS_NODE(COMPARE_GREATER); }
\<= { PROCESS_NODE(COMPARE_LESS_EQUAL); }
\>= { PROCESS_NODE(COMPARE_GREATER_EQUAL); }
== { PROCESS_NODE(COMPARE_EQUAL); }
!= { PROCESS_NODE(COMPARE_NOT_EQUAL); }
\^ { PROCESS_NODE(XOR_OP);}

\|\| { PROCESS_NODE(OR_OP);}
\*\= { PROCESS_NODE(MUL_ASSIGNMENT); }
\/\= { PROCESS_NODE(DIV_ASSIGNMENT); }
\+\= { PROCESS_NODE(ADD_ASSIGNMENT); }
\-\= { PROCESS_NODE(SUB_ASSIGNMENT); }
\<\<\= { PROCESS_NODE(LEFT_ASSIGNMENT); }
\>\>\= { PROCESS_NODE(RIGHT_ASSIGNMENT); }
\&\= { PROCESS_NODE(AND_ASSIGNMENT); }
\^\= { PROCESS_NODE(XOR_ASSIGNMENT); }
\|\= { PROCESS_NODE(OR_ASSIGNMENT); }

\( {PROCESS_NODE(PAREN_L);}
\) {PROCESS_NODE(PAREN_R);}
\[ {PROCESS_NODE(BRACKET_L);}
\] {PROCESS_NODE(BRACKET_R);}
\{ {PROCESS_NODE(BRACE_L);}
\} {PROCESS_NODE(BRACE_R);}

\. {PROCESS_NODE(DOT);}
\? {PROCESS_NODE(QUESTIONMARK);}
\: {PROCESS_NODE(COLON);}
\, {PROCESS_NODE(COMMA);}
\! {PROCESS_NODE(BANG);}
\& {PROCESS_NODE(ANDPERSAND);}
\| {PROCESS_NODE(PIPE);}
\+ {PROCESS_NODE(PLUS);}
\- {PROCESS_NODE(MINUS);}
\* {PROCESS_NODE(STAR);}
\/ {PROCESS_NODE(SLASH);}
\%           {PROCESS_NODE(PERCENT);}
\=           {PROCESS_NODE(ASSIGNMENT);}

\"[^\"]*\"   {PROCESS_NODE(LITERAL_STRING);}
\'([^'\\\n]|\\[abefnrt\'])\' {
	     PROCESS_NODE(LITERAL_CHAR);
}
(true|false) {PROCESS_NODE(LITERAL_BOOL);}
null         {PROCESS_NODE(LITERAL_NULL);}

{float} {PROCESS_NODE(LITERAL_FLOAT);}
{integer} { PROCESS_NODE(LITERAL_INT);}
(::)?{identifier}({whitespace}*:{whitespace}*:{whitespace}*(\~{whitespace}*)?{identifier})* {
// The identifier regex is pretty intense, so here's the breakdown:
// It can start with "::" for root-qualified searches.
// Then it can contain an identifier [a-zA-z0-9]*
// After that, you can put any number of '::' delimiters (with whitespace)
// and optionally a ~ to denote a destructor.
//
// The fact that the ~ must appear AFTER the :: is what allows us
// to disambiguate it from a regular ~ as a bitwise not even at the lexical
// layer.

    LexContext *lex_context = (LexContext*)yyget_extra(yyscanner);
    NS2Context & ns2_context = lex_context->ns2_context;

    //fprintf(stderr, "Looking up in namespace context %s\n", yytext);
    NS2Entity *entity = ns2_context.namespace_find(std::string(yytext));
    if (entity == nullptr) {
        // Not yet known.  We expect the syntax layer to
        // find a place to put this identifier in a namespace.
        //fprintf(stderr, "Name is an unknown identifier %s\n", yytext);
        PROCESS_IDENTIFIER(IDENTIFIER, nullptr);
    }
    else if (entity->get_type() == NS2Entity::ENTITY_TYPE_NAMESPACE) {
        //fprintf(stderr, "Name is a namespace %s\n", yytext);
        PROCESS_IDENTIFIER(NAMESPACE_NAME, entity);
    }
    else if (entity->get_type() == NS2Entity::ENTITY_TYPE_TYPE || entity->get_type() == NS2Entity::ENTITY_TYPE_CLASS) {
        //fprintf(stderr, "Name is a type %s\n", yytext);
        PROCESS_IDENTIFIER(TYPE_NAME, entity);
    }
    else {
        //fprintf(stderr, "Name is an identifier we've seen before %s\n", yytext);
        // An identifier we've seen before.  It should
        // already be assigned to a namespace location.
        PROCESS_IDENTIFIER(IDENTIFIER, entity);
    }
}
\~ {
    // Needs to be after identifier because
    // identifiers (for destructors) can contain a tilde.
    PROCESS_NODE(TILDE);
}

"/*"        {
  BEGIN(COMMENT);
  TOKEN_ADD(comment);
  Gyoji::owned<TerminalNonSyntax> nsd = Gyoji::owned_new<TerminalNonSyntax>(
        TerminalNonSyntax::TerminalNonSyntax::Type::EXTRA_COMMENT_MULTI_LINE,
        tok
        );
  non_syntax_data.push_back(std::move(nsd));
}
<COMMENT>"*/" {
  TOKEN_APPEND()
  BEGIN(INITIAL);
}
<COMMENT>[^*]* {
  TOKEN_APPEND()
}
<COMMENT>\* {
  TOKEN_APPEND()
}

\/\/.* {
// Single-line comment:
    TOKEN_ADD(single_line_comment);
    Gyoji::owned<TerminalNonSyntax> nsd = Gyoji::owned_new<TerminalNonSyntax>(
        TerminalNonSyntax::Type::EXTRA_COMMENT_SINGLE_LINE,
        tok
    );
    non_syntax_data.push_back(std::move(nsd));
}
[ \t]+ {
    TOKEN_ADD(whitespace);
    Gyoji::owned<TerminalNonSyntax> nsd = Gyoji::owned_new<TerminalNonSyntax>(
        TerminalNonSyntax::Type::EXTRA_WHITESPACE, 
        tok
    );
    non_syntax_data.push_back(std::move(nsd));
}
\n {
    TOKEN_ADD(newline);
    Gyoji::owned<TerminalNonSyntax> nsd = Gyoji::owned_new<TerminalNonSyntax>(
        TerminalNonSyntax::Type::EXTRA_WHITESPACE, 
        tok
    );
    non_syntax_data.push_back(std::move(nsd));
    LexContext *lex_context = (LexContext*)yyget_extra(yyscanner);
    lex_context->line++;
    lex_context->column = 0;
}
\#[a-zA-Z]+\ [[:digit:]]+\ \".*\"\n {
    // XXX TODO : Parse the source file and line number to
    // mark the current position of compilation in terms of
    // an original source file that generated this block of code.
    // This is useful, for example, when working with a YACC file
    // that generates some Gyoji code and you want to trace
    // the error to the correct line of YACC code and not necessarily
    // to the source file being compiled.
    TOKEN_ADD(file_metadata)
    Gyoji::owned<TerminalNonSyntax> nsd = Gyoji::owned_new<TerminalNonSyntax>(
        TerminalNonSyntax::Type::EXTRA_FILE_METADATA,
        tok
    );
    non_syntax_data.push_back(std::move(nsd));
}
\#.*\n {
    TOKEN_ADD(file_metadata)
    Gyoji::owned<TerminalNonSyntax> nsd = Gyoji::owned_new<TerminalNonSyntax>(
        TerminalNonSyntax::Type::EXTRA_FILE_METADATA,
        tok
    );
    non_syntax_data.push_back(std::move(nsd));
}
. {
    return YaccParser::token::INVALID_INPUT;
}
<<EOF>> {PROCESS_NODE(YYEOF)}

%%
