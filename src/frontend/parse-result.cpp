#include <jlang-frontend.hpp>
#include <jlang.l.hpp>
#include <jlang.y.hpp>
#include <jlang-misc/jstring.hpp>

using namespace JLang::context;
using namespace JLang::frontend;
using namespace JLang::frontend::namespaces;
using namespace JLang::frontend::tree;

ParseResult::ParseResult(
    JLang::context::CompilerContext & _compiler_context,
    JLang::owned<NamespaceContext>  _namespace_context
    )
    : namespace_context(std::move(_namespace_context))
    , compiler_context(_compiler_context)
    , translation_unit(nullptr)
{}
ParseResult::~ParseResult()
{}

const NamespaceContext &
ParseResult::get_namespace_context() const
{
    return *namespace_context;
}
Errors &
ParseResult::get_errors() const
{
    return compiler_context.get_errors();
}

const TranslationUnit & 
ParseResult::get_translation_unit() const
{
    return *translation_unit;
}
bool
ParseResult::has_translation_unit() const
{ return translation_unit.get() != nullptr; }

bool
ParseResult::has_errors() const
{
    return compiler_context.get_errors().size() != 0;
}

const TokenStream &
ParseResult::get_token_stream() const
{
    return compiler_context.get_token_stream();
}
const JLang::context::CompilerContext &
ParseResult::get_compiler_context() const
{ return compiler_context; }

void
ParseResult::set_translation_unit(JLang::owned<TranslationUnit> _translation_unit)
{
    translation_unit = std::move(_translation_unit);
}

void
ParseResult::symbol_define(std::string _symbol)
{
    Symbol symbol(_symbol);
    symbol_table.insert(std::pair(_symbol, symbol));
}

const Symbol *
ParseResult::symbol_find(std::string name) const
{
    const auto & symbol = symbol_table.find(name);
    if (symbol == symbol_table.end()) {
	return nullptr;
    }
    return &symbol->second;
}


void
ParseResult::symbol_table_dump()
{
    for (const auto & symbol : symbol_table) {
	fprintf(stderr, "Symbol table %s\n", symbol.first.c_str());
    }
}

const Symbol *
ParseResult::symbol_get_or_create(std::string symbol_name)
{
    fprintf(stderr, "Encountered symbol %s\n", symbol_name.c_str());
    
    std::vector<std::string> path = namespace_context->namespace_search_path(symbol_name);
    // Search for the symbol using the path.
    for (const auto &sp : path) {
	fprintf(stderr, "Searching for %s\n", sp.c_str());
	const Symbol* sym = symbol_find(sp);
	if (sym) {
	    fprintf(stderr, "Found symbol %s\n", sym->name.c_str());
	    return sym;
	}
    }

    std::string fqs = JLang::misc::join_nonempty(
	namespace_context->current()->fully_qualified(),
	symbol_name,
	std::string("::")
	);
    
    fprintf(stderr, "Symbol not found, defining it %s in the current namespace\n", fqs.c_str());
    symbol_define(fqs);
    return symbol_find(fqs);
}

Symbol::Symbol(std::string _name)
    : name(_name)
{}
Symbol::~Symbol()
{}
Symbol::Symbol(const Symbol & _other)
    : name(_other.name)
{}
