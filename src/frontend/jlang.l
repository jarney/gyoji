%{
#include <cstdlib>
#include <memory>
#include <jlang-frontend.hpp>
#include <jlang.y.hpp>
#define _JLANG_INTERNAL
#include <lex-context.hpp>
#undef _JLANG_INTERNAL

using namespace JLang::context;
using namespace JLang::frontend;
using namespace JLang::frontend::tree;
using namespace JLang::frontend::namespaces;
using namespace JLang::frontend::yacc;


std::vector<JLang::owned<TerminalNonSyntax>> non_syntax_data;

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
                std::vector<JLang::owned<TerminalNonSyntax>> & dst,
                std::vector<JLang::owned<TerminalNonSyntax>> & src
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
    lc->column += strlen(yytext);                                    \
    const Token &tok =                                               \
        lc->compiler_context.get_token_stream()                      \
            .add_token(                                              \
                std::string(#nodetype),                              \
                std::string(yytext),                                 \
                lc->compiler_context.get_filename(),                 \
                lc->line,                                            \
                lc->column-1                                         \
            );                                                       \

#define START_NODE(nodetype)                                         \
    TOKEN_ADD(nodetype);                                             \
    Terminal* node = new Terminal(tok);                              \
    move_array(node->non_syntax, non_syntax_data);                   \
    yylval->emplace<JLang::owned<Terminal>>(node);

#define RETURN_NODE(nodetype)                                        \
    return YaccParser::token::nodetype;

#define PROCESS_NODE(nodetype)                                       \
    START_NODE(nodetype)                                             \
    PRINT_TERMINALS(#nodetype, yytext)                               \
    RETURN_NODE(nodetype);

#define PROCESS_IDENTIFIER(nodetype, ns_ptr)                         \
    START_NODE(nodetype)                                             \
    node->set_fully_qualified_name(ns_ptr->fully_qualified());       \
    PRINT_TERMINALS(#nodetype, yytext)                               \
    RETURN_NODE(nodetype);

#define PROCESS_IDENTIFIER_ID(nodetype, ns_ptr)                      \
    START_NODE(nodetype)                                             \
    node->set_fully_qualified_name(ns_ptr->fully_qualified());       \
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

float               ((({frac}{exp_opt})|({dec_dseq}{exp}))({opt_float_size}))
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

; {PROCESS_NODE(SEMICOLON);}
-> {PROCESS_NODE(PTR_OP);}
>> {PROCESS_NODE(RIGHT_OP);}
\+\+ {PROCESS_NODE(INC_OP);}
\-\- {PROCESS_NODE(DEC_OP);}
\<\< {PROCESS_NODE(LEFT_OP);}
\< { PROCESS_NODE(LT_OP); }
\> { PROCESS_NODE(GT_OP); }
\<= { PROCESS_NODE(LE_OP); }
\>= { PROCESS_NODE(GE_OP); }
== { PROCESS_NODE(EQ_OP); }
!= { PROCESS_NODE(NE_OP); }
\^ { PROCESS_NODE(XOR_OP);}

\|\| { PROCESS_NODE(OR_OP);}
\*\= { PROCESS_NODE(MUL_ASSIGN); }
\/\= { PROCESS_NODE(DIV_ASSIGN); }
\+\= { PROCESS_NODE(ADD_ASSIGN); }
\-\= { PROCESS_NODE(SUB_ASSIGN); }
\<\<\= { PROCESS_NODE(LEFT_ASSIGN); }
\>\>\= { PROCESS_NODE(RIGHT_ASSIGN); }
\&\= { PROCESS_NODE(AND_ASSIGN); }
\^\= { PROCESS_NODE(XOR_ASSIGN); }
\|\= { PROCESS_NODE(OR_ASSIGN); }

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
\~ {PROCESS_NODE(TILDE);}
\& {PROCESS_NODE(ANDPERSAND);}
\| {PROCESS_NODE(PIPE);}
\+ {PROCESS_NODE(PLUS);}
\- {PROCESS_NODE(MINUS);}
\* {PROCESS_NODE(STAR);}
\/ {PROCESS_NODE(SLASH);}
\%     {PROCESS_NODE(PERCENT);}
\=     { PROCESS_NODE(EQUALS);}

\"[^\"]*\" { PROCESS_NODE(LITERAL_STRING);}
\'.\' { PROCESS_NODE(LITERAL_CHAR);}
{float} {PROCESS_NODE(LITERAL_FLOAT);}
{integer} { PROCESS_NODE(LITERAL_INT);}
(::)*{identifier}({whitespace}*:{whitespace}*:{whitespace}*{identifier})* {
    LexContext *lex_context = (LexContext*)yyget_extra(yyscanner);
    NamespaceContext & context = lex_context->namespace_context;
    NamespaceFoundReason::ptr found = context.namespace_lookup(std::string(yytext));
    if (found->reason == NamespaceFoundReason::REASON_FOUND) {
        if (found->location->get_type() == Namespace::TYPE_NAMESPACE) {
            // When processing a namespace, the NAMESPACE_NAME
            // gives the fully-qualified name of the namespace.
            PROCESS_IDENTIFIER(NAMESPACE_NAME, found->location);
        }
        else {
            // When processing a TYPE_NAME, the second argument
            // tells us the actual fully-qualified name of the type.
            PROCESS_IDENTIFIER(TYPE_NAME, found->location);
        }
    }
    else if (found->reason == NamespaceFoundReason::REASON_NOT_FOUND) {
        // When declaring an identifier, the second
        // argument tells us what namespace the identifier
        // should go into.
        PROCESS_IDENTIFIER_ID(IDENTIFIER, context.current());
    }
    else {
        PROCESS_NODE(YYEOF);
    }
}

"/*"        {
  BEGIN(COMMENT);
  TOKEN_ADD(comment);
  JLang::owned<TerminalNonSyntax> nsd = std::make_unique<TerminalNonSyntax>(
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
    JLang::owned<TerminalNonSyntax> nsd = std::make_unique<TerminalNonSyntax>(
        TerminalNonSyntax::Type::EXTRA_COMMENT_SINGLE_LINE,
        tok
    );
    non_syntax_data.push_back(std::move(nsd));
}
[ \t]+ {
    TOKEN_ADD(whitespace);
    JLang::owned<TerminalNonSyntax> nsd = std::make_unique<TerminalNonSyntax>(
        TerminalNonSyntax::Type::EXTRA_WHITESPACE, 
        tok
    );
    non_syntax_data.push_back(std::move(nsd));
}
\n {
    TOKEN_ADD(newline);
    JLang::owned<TerminalNonSyntax> nsd = std::make_unique<TerminalNonSyntax>(
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
    // that generates some JLang code and you want to trace
    // the error to the correct line of YACC code and not necessarily
    // to the source file being compiled.
    TOKEN_ADD(file_metadata)
    JLang::owned<TerminalNonSyntax> nsd = std::make_unique<TerminalNonSyntax>(
        TerminalNonSyntax::Type::EXTRA_FILE_METADATA,
        tok
    );
    non_syntax_data.push_back(std::move(nsd));
}
\#.*\n {
    TOKEN_ADD(file_metadata)
    JLang::owned<TerminalNonSyntax> nsd = std::make_unique<TerminalNonSyntax>(
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
