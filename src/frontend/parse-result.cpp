#include <gyoji-frontend.hpp>
#include <gyoji.l.hpp>
#include <gyoji.y.hpp>
#include <gyoji-misc/jstring.hpp>

using namespace Gyoji::context;
using namespace Gyoji::frontend;
using namespace Gyoji::frontend::namespaces;
using namespace Gyoji::frontend::tree;

ParseResult::ParseResult(
    Gyoji::context::CompilerContext & _compiler_context,
    Gyoji::owned<NS2Context>  _ns2_context
    )
    : compiler_context(_compiler_context)
    , ns2_context(std::move(_ns2_context))
    , translation_unit(nullptr)
{}
ParseResult::~ParseResult()
{}

const NS2Context &
ParseResult::get_ns2_context() const
{
    return *ns2_context;
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
const Gyoji::context::CompilerContext &
ParseResult::get_compiler_context() const
{ return compiler_context; }

void
ParseResult::set_translation_unit(Gyoji::owned<TranslationUnit> _translation_unit)
{
    translation_unit = std::move(_translation_unit);
}

void
ParseResult::symbol_define(std::string _symbol, const SourceReference &src_ref)
{
    Symbol symbol(_symbol, src_ref);
    symbol_table.insert(std::pair(_symbol, symbol));
}

const Symbol *
ParseResult::symbol_get(std::string name) const
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
ParseResult::symbol_get_or_create(std::string symbol_name, const SourceReference & src_ref)
{
    const Symbol *found = symbol_get(symbol_name);
    if (found) return found;
    
    symbol_define(symbol_name, src_ref);
    return symbol_get(symbol_name);

}

NS2Entity *
ParseResult::identifier_get_or_create(
    std::string name,
    bool allow_placement_in_namespace,
    const SourceReference & _src_ref
    )
{
    NS2Entity *entity = ns2_context->namespace_find(name);
    if (entity != nullptr) {
	if (entity->get_type() == NS2Entity::ENTITY_TYPE_IDENTIFIER) {
	    return entity;
	}
	auto error = std::make_unique<Gyoji::context::Error>(std::string("Identifier ") + name + std::string(" is ambiguous"));
	error->add_message(_src_ref,
			   std::string("Identifier ") + name + std::string(" was declared as a different type of identifier."));
	error->add_message(entity->get_source_ref(),
			   std::string("First declared here"));
	compiler_context.get_errors().add_error(std::move(error));
	return nullptr;
    }

    // If the identifier is not
    // a simple identifier, look for the
    // namespace by the prefix
    std::vector<std::string> name_components = Gyoji::misc::string_split(name, NS2Context::NAMESPACE_DELIMITER);
    if (name_components.size() != 1) {
	if (!allow_placement_in_namespace) {
	    auto error = std::make_unique<Gyoji::context::Error>(std::string("Invalid identifier") + name + std::string("."));
	    error->add_message(_src_ref, std::string("Identifier must be a simple identifier ") + name + std::string(" and must not contain '::'."));
	    compiler_context.get_errors().add_error(std::move(error));
	    return nullptr;
	}
	else {
	    std::string simple_name = name_components.at(name_components.size()-1);
	    name_components.pop_back();
	    std::string namespace_part = Gyoji::misc::join(name_components, NS2Context::NAMESPACE_DELIMITER);
	    NS2Entity *namespace_entity = ns2_context->namespace_find(namespace_part);
	    if (namespace_entity == nullptr) {
		namespace_entity = namespace_get_or_create(namespace_part, _src_ref);
		if (namespace_entity == nullptr) {
		    return nullptr;
		}
	    }
	    if (namespace_entity->get_type() == NS2Entity::ENTITY_TYPE_NAMESPACE ||
		namespace_entity->get_type() == NS2Entity::ENTITY_TYPE_CLASS) {
		return namespace_entity->add_identifier(simple_name, _src_ref);
	    }
	    else {
		auto error = std::make_unique<Gyoji::context::Error>(std::string("Invalid identifier") + name + std::string("."));
		error->add_message(_src_ref, std::string("Identifier must ") + name + std::string(" must be placed inside a namespace or class."));
		error->add_message(namespace_entity->get_source_ref(), std::string("Namespace is ") + namespace_part + std::string(" is not a namespace or class"));
		compiler_context.get_errors().add_error(std::move(error));
		return nullptr;
	    }
	}
    }
    entity = ns2_context->get_current()->add_identifier(
	name,
	_src_ref
	);
    return entity;
}

NS2Entity *
ParseResult::namespace_get_or_create(
    std::string _name,
    const Gyoji::context::SourceReference & _source_ref
    )
{
    // The _name might be a composite name, so we need to split it and handle
    // each component separately as its own namespace.
    std::vector<std::string> name_components = Gyoji::misc::string_split(_name, NS2Context::NAMESPACE_DELIMITER);

    // For each component, first try to find it
    // as an entity or namespace name.
    NS2Entity *current = ns2_context->get_current();
    for (const std::string & name : name_components) {
	// So what can happen is we have an enitity that is like a class
	// that is an entity but also a namespace of the same name
	// containing other things.  But if that happens, we can't define
	// it this way through a 'namespace' statement, so it's correct
	// to discard it this way.
	
	NS2Entity *entity = current->get_entity(name);
	if (entity != nullptr) {
	    if (entity->get_type() != NS2Entity::ENTITY_TYPE_NAMESPACE) {
		auto error = std::make_unique<Gyoji::context::Error>(std::string("Invalid identifier") + name + std::string("."));
		error->add_message(_source_ref, std::string("Namespace ") + name + std::string(" must be placed inside another namespace or at the root."));
		error->add_message(entity->get_source_ref(), std::string("Identifier ") + name + std::string(" is not a namespace"));
		compiler_context.get_errors().add_error(std::move(error));
		return nullptr;
	    }
	    current = entity;
	}
	else {
	    NS2Entity* newcurrent = current->add_namespace(name, _source_ref);
	    current = newcurrent;
	}
    }
    return current;
    
}

NS2Entity*
ParseResult::type_get_or_create(
    std::string _name,
    const Gyoji::context::SourceReference & _source_ref
    )
{
    NS2Entity *current = ns2_context->get_current();
    
    Gyoji::owned<NS2Entity> new_class = std::make_unique<NS2Entity>(_name, NS2Entity::ENTITY_TYPE_TYPE, current, _source_ref);
    NS2Entity *ret_class = new_class.get();
    current->add_entity(_name, std::move(new_class));
    return ret_class;
}

NS2Entity*
ParseResult::class_get_or_create(
    std::string _name,
    const Gyoji::context::SourceReference & _source_ref
    )
{
    NS2Entity *current = ns2_context->get_current();
    
    Gyoji::owned<NS2Entity> new_class = std::make_unique<NS2Entity>(_name, NS2Entity::ENTITY_TYPE_CLASS, current, _source_ref);
    NS2Entity *ret_class = new_class.get();
    current->add_entity(_name, std::move(new_class));
    return ret_class;
}


///////////////////////////////////////////////////
// Symbol
///////////////////////////////////////////////////

Symbol::Symbol(std::string _name, const SourceReference & _src_ref)
    : name(_name)
    , src_ref(_src_ref)
{}
Symbol::~Symbol()
{}
Symbol::Symbol(const Symbol & _other)
    : name(_other.name)
    , src_ref(_other.src_ref)
{}
