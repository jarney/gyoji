%{
#include <cstdlib>
#include <memory>
#include <jlang-frontend/jsyntax.hpp>
#include <jlang.y.hpp>
using namespace JLang::frontend::tree;
using namespace JLang::frontend::namespaces;
using namespace JLang::frontend::yacc;


std::string override_file;
size_t lineno = 1;
std::vector<TerminalNonSyntax_owned_ptr> non_syntax_data;

#define DEBUG_TERMINALS 0
#if DEBUG_TERMINALS
#define PRINT_TERMINALS(s,t)                                                  \
    printf("%s : %s%s\n", s, t,                                               \
                          (node->fully_qualified_name.size() > 0) ?           \
                          (std::string(" : ") + node->fully_qualified_name).c_str() : \
                          std::string().c_str());
#else
#define PRINT_TERMINALS(s,t) /**/
#endif

void move_array(
                std::vector<TerminalNonSyntax_owned_ptr> & dst,
                std::vector<TerminalNonSyntax_owned_ptr> & src
                )
{
    dst.clear();
    for (auto & srcitem : src) {
        dst.push_back(std::move(srcitem));
    }
    src.clear();
}

#define START_NODE(nodetype)                 \
    Terminal* node = new Terminal(); \
    node->type = YaccParser::symbol_kind_type::S_ ## nodetype; \
    node->typestr = std::string(#nodetype);  \
    node->value = std::string(yytext);       \
    node->lineno = lineno;                   \
    move_array(node->non_syntax, non_syntax_data);      \
    yylval->emplace<Terminal_owned_ptr>(node);

#define RETURN_NODE(nodetype)                \
    return YaccParser::token::nodetype;

#define PROCESS_NODE(nodetype)               \
    START_NODE(nodetype)                     \
    PRINT_TERMINALS(#nodetype, yytext)       \
    RETURN_NODE(nodetype);

#define PROCESS_IDENTIFIER(nodetype, ns_ptr) \
    START_NODE(nodetype)                     \
    node->fully_qualified_name = ns_ptr->fully_qualified(); \
    PRINT_TERMINALS(#nodetype, yytext)       \
    RETURN_NODE(nodetype);

%}
 
%option reentrant interactive noyywrap noyylineno nodefault

%x COMMENT

dseq            ([[:digit:]]+)
dseq_opt        ({dseq}?)
frac            (({dseq_opt}"."{dseq})|{dseq}".")
exp             ([eE][+-]?{dseq})
exp_opt         ({exp}?)
integer         ({dseq})
float           (({frac}{exp_opt})|({dseq}{exp}))
identifier      ([a-zA-Z_][a-zA-Z_0-9]*)
whitespace      ([[:space:]])
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
\&\& { PROCESS_NODE(AND_OP);}
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
    NamespaceContext *context = (NamespaceContext*)yyget_extra(yyscanner);
    NamespaceFoundReason::ptr found = context->namespace_lookup(std::string(yytext));
    if (found->reason == NamespaceFoundReason::REASON_FOUND) {
        if (found->location->type == Namespace::TYPE_NAMESPACE) {
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
        PROCESS_IDENTIFIER(IDENTIFIER, context->current());
    }
    else {
        PROCESS_NODE(YYEOF);
    }
}

"/*"        {
  BEGIN(COMMENT);
  TerminalNonSyntax_owned_ptr nsd = std::make_unique<TerminalNonSyntax>(
        TerminalNonSyntax::TerminalNonSyntax::Type::EXTRA_COMMENT_MULTI_LINE,
        std::string()
        );
  non_syntax_data.push_back(std::move(nsd));
}
<COMMENT>"*/" {
  BEGIN(INITIAL);
}
<COMMENT>[^*]* {
  TerminalNonSyntax* current = non_syntax_data.back().get();
  current->append(std::string(yytext));
}
<COMMENT>\* {
  TerminalNonSyntax *current = non_syntax_data.back().get();
  current->append(std::string(yytext));
}

\/\/.* {
// Single-line comment:
    TerminalNonSyntax_owned_ptr nsd = std::make_unique<TerminalNonSyntax>(
        TerminalNonSyntax::Type::EXTRA_COMMENT_SINGLE_LINE,
        std::string(yytext+2)
    );
    non_syntax_data.push_back(std::move(nsd));
}
[ \t]+ {
    TerminalNonSyntax_owned_ptr nsd = std::make_unique<TerminalNonSyntax>(
        TerminalNonSyntax::Type::EXTRA_WHITESPACE, 
        std::string(yytext)
    );
    non_syntax_data.push_back(std::move(nsd));
}
\n {
    TerminalNonSyntax_owned_ptr nsd = std::make_unique<TerminalNonSyntax>(
        TerminalNonSyntax::Type::EXTRA_WHITESPACE, 
        std::string(yytext)
    );
    non_syntax_data.push_back(std::move(nsd));
    lineno++;
}
\#[a-zA-Z]+\ [[:digit:]]+\ \".*\"\n {
    //printf("Preprocessor directive %s\n", yytext);
    TerminalNonSyntax_owned_ptr nsd = std::make_unique<TerminalNonSyntax>(
        TerminalNonSyntax::Type::EXTRA_FILE_METADATA,
        std::string(yytext)
    );
    non_syntax_data.push_back(std::move(nsd));
}
. {
    return YaccParser::token::INVALID_INPUT;
}
<<EOF>> {PROCESS_NODE(YYEOF)}

%%
