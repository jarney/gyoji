#include <gyoji-frontend/ns2.hpp>
#include <gyoji-misc/jstring.hpp>

const std::string Gyoji::frontend::namespaces::NS2Context::NAMESPACE_DELIMITER("::");

using namespace Gyoji::frontend::namespaces;


///////////////////////////////////////////////////
// NS2Entity
///////////////////////////////////////////////////
NS2Entity::NS2Entity(
    std::string _name,
    EntityType _type,
    NS2Entity* _parent,
    const Gyoji::context::SourceReference & _source_ref
    )
    : name(_name)
    , type(_type)
    , parent(_parent)
    , source_ref(_source_ref)
{}

NS2Entity::~NS2Entity()
{}

const std::string &
NS2Entity::get_name() const
{ return name;}

std::string
NS2Entity::get_fully_qualified_name() const
{
    std::string ret_string;
    if (parent == nullptr) {
	// If our parent is null, we are the root
	// namespace, so our name is empty.
	return std::string("");
    }
    if (parent->parent != nullptr) {
	// If our parent is not the root, we can ask for its name also.
	return parent->get_fully_qualified_name() + NS2Context::NAMESPACE_DELIMITER + get_name();
    }
    else {
	return get_name();
    }
}


const NS2Entity::EntityType &
NS2Entity::get_type() const
{ return type; }

const Gyoji::context::SourceReference &
NS2Entity::get_source_ref() const
{ return source_ref; }

NS2Entity*
NS2Entity::add_identifier(
    std::string _name,
    const Gyoji::context::SourceReference & _source_ref
    )
{
    const auto & it = elements.find(_name);
    if (it != elements.end()) {
	fprintf(stderr, "Identifier already defined in namespace %s\n", _name.c_str());
	return nullptr;
    }
    
    Gyoji::owned<NS2Entity> entity = std::make_unique<NS2Entity>(_name, NS2Entity::ENTITY_TYPE_IDENTIFIER, this, _source_ref);
    NS2Entity *ret = entity.get();
    elements.insert(std::pair(_name, std::move(entity)));
    return ret;
}

NS2Entity*
NS2Entity::add_type(
    std::string _name,
    const Gyoji::context::SourceReference & _source_ref
    )
{
    const auto & it = elements.find(_name);
    if (it != elements.end()) {
	fprintf(stderr, "Identifier already defined in namespace %s\n", _name.c_str());
	return nullptr;
    }

    Gyoji::owned<NS2Entity> entity = std::make_unique<NS2Entity>(_name, NS2Entity::ENTITY_TYPE_TYPE, this, _source_ref);
    NS2Entity *ret = entity.get();
    elements.insert(std::pair(_name, std::move(entity)));
    return ret;
}

NS2Entity*
NS2Entity::add_class(
    std::string _name,
    const Gyoji::context::SourceReference & _source_ref
    )
{
    const auto & it = elements.find(_name);
    if (it != elements.end()) {
	fprintf(stderr, "Identifier already defined in namespace %s\n", _name.c_str());
	return nullptr;
    }
    Gyoji::owned<NS2Entity> entity = std::make_unique<NS2Entity>(_name, NS2Entity::ENTITY_TYPE_CLASS, this, _source_ref);
    NS2Entity *ret = entity.get();
    elements.insert(std::pair(_name, std::move(entity)));
    return ret;
}

NS2Entity*
NS2Entity::add_namespace(
    std::string _namespace,
    const Gyoji::context::SourceReference & _source_ref
    )
{
    Gyoji::owned<NS2Entity> entity = std::make_unique<NS2Entity>(_namespace, NS2Entity::ENTITY_TYPE_NAMESPACE, this, _source_ref);
    NS2Entity *ret = entity.get();
    elements.insert(std::pair(_namespace, std::move(entity)));
    return ret;
}

NS2Entity *
NS2Entity::get_entity(std::string _name) const
{
    const auto & it = elements.find(_name);
    if (it == elements.end()) {
	return nullptr;
    }
    return it->second.get();
}

NS2Entity*
NS2Entity::add_entity(std::string _name, Gyoji::owned<NS2Entity> _entity)
{
    NS2Entity *ret = _entity.get();
    elements.insert(std::pair(_name, std::move(_entity)));
    return ret;
}

NS2Entity *
NS2Entity::get_parent() const
{ return  parent; }

void
NS2Entity::dump(int indent) const
{
    std::string pad(indent*8, ' ');
    std::string pad2((indent+1)*8, ' ');
    std::string start = pad + std::string("{");
    std::string end = pad + std::string("}");
    fprintf(stderr, "%s\n", start.c_str());
    for (const auto & el : elements) {
	std::string name = el.first;
	const NS2Entity *entity = el.second.get();

	std::string typestr;
	switch (entity->get_type()) {
	case NS2Entity::ENTITY_TYPE_IDENTIFIER:
	    typestr = " identifier";
	    break;
	case NS2Entity::ENTITY_TYPE_LABEL:
	    typestr = " label";
	    break;
	case NS2Entity::ENTITY_TYPE_TYPE:
	    typestr = " type";
	    break;
	case NS2Entity::ENTITY_TYPE_CLASS:
	    typestr = " class";
	    break;
	case NS2Entity::ENTITY_TYPE_NAMESPACE:
	    typestr = " namespace";
	    break;
	}
	std::string entity_desc = pad2 + name + typestr;
	fprintf(stderr, "%s\n", entity_desc.c_str());
	if (entity->get_type() == NS2Entity::ENTITY_TYPE_CLASS ||
	    entity->get_type() == NS2Entity::ENTITY_TYPE_NAMESPACE) {
	    entity->dump(indent+1);
	}
    }
    fprintf(stderr, "%s\n", end.c_str());
}

///////////////////////////////////////////////////
// NS2Context
///////////////////////////////////////////////////
NS2SearchPaths::NS2SearchPaths()
{}

NS2SearchPaths::~NS2SearchPaths()
{}

void
NS2SearchPaths::add_using(std::string name, NS2Entity *alias)
{
    aliases.push_back(std::pair(name, alias));
    alias_map.insert(std::pair(name, alias));
}

NS2Entity *
NS2SearchPaths::get_name(std::string name)
{
    const auto & it = alias_map.find(name);
    if (it == alias_map.end()) {
	return nullptr;
    }
    return it->second;
}

///////////////////////////////////////////////////
// NS2Context
///////////////////////////////////////////////////

static std::string internal_filename("builtin");
static const Gyoji::context::SourceReference zero_source_ref(internal_filename, 1, 0, 0);

NS2Context::NS2Context()
    : root(std::make_unique<NS2Entity>(
	       "root",
	       NS2Entity::ENTITY_TYPE_NAMESPACE,
	       nullptr,
	       zero_source_ref
	       )
	)
{
    Gyoji::owned<NS2SearchPaths> search_paths = std::make_unique<NS2SearchPaths>();
    stack.push_back(std::pair(root.get(), std::move(search_paths)));

    root->add_type("i8", zero_source_ref);
    root->add_type("i16", zero_source_ref);
    root->add_type("i32", zero_source_ref);
    root->add_type("i64", zero_source_ref);
    
    root->add_type("u8", zero_source_ref);
    root->add_type("u16", zero_source_ref);
    root->add_type("u32", zero_source_ref);
    root->add_type("u64", zero_source_ref);
    
    root->add_type("f32", zero_source_ref);
    root->add_type("f64", zero_source_ref);
    
    root->add_type("void", zero_source_ref);
    root->add_type("bool", zero_source_ref);
}

NS2Context::~NS2Context()
{}

// We're going to search for an entity in
// the current namespace context.
//void
//NS2Context::find_entity(std::string _name)
//{
//}
//
// We are going to define a type or identifier
// in the current namespace.  This would be like 'class Foo' or 'typedef u8 bar'.
//NS2Context::new_entity();

// We are now adding a namespace to the list of
// namespaces we want to search.
void
NS2Context::namespace_using(std::string name, NS2Entity* alias)
{
    const auto & b = stack.back();
    b.second->add_using(name, alias);
}

NS2Entity*
NS2Context::namespace_find(std::string _name) const
{
    std::vector<std::string> name_components = Gyoji::misc::string_split(_name, NAMESPACE_DELIMITER);
    // This is the point where we might insert
    // our aliases to search for them under different
    // names.
    return namespace_find(name_components);
}

NS2Entity *
NS2Context::namespace_find_in(NS2Entity* current, std::vector<std::string> names) const
{
    NS2Entity *entity_found = nullptr;

    //dump();
    
    // First, look in the current namespace
    // for the first component of the name.
    size_t i = 0;
    while (true) {
	entity_found = current->get_entity(names.at(i));
	if (i == names.size()-1) {
	    break;
	}
	NS2Entity *next_current = current->get_parent();
	if (next_current == nullptr) {
	    // We've reached the end of our search
	    // and we didn't find the leaf node
	    // of the name;
	    return nullptr;
	}
	// We have another parent to search, so try that.
	current = next_current;
	i++;
    }
    return entity_found;
}

NS2Entity*
NS2Context::namespace_find(std::vector<std::string> names) const
{
    // Try resolving in the current namespace
    // and then try resolving at every point up the stack.
    //for (const auto & s : stack) {
    //    fprintf(stderr, "Stack is %s\n", s.first->get_name().c_str());
    //}
    
    NS2Entity *current = stack.back().first;
    while (current) {
	NS2Entity *found = namespace_find_in(current, names);
	if (found != nullptr) {
	    return found;
	}
	current = current->get_parent();
    }
    return nullptr;
}

NS2Entity *
NS2Context::get_current() const
{ return stack.back().first; }

// Enter the given namespace
// so that new declarations will
// appear in this namespace
void
NS2Context::namespace_push(NS2Entity *ns)
{
    // We are now in the context of this
    // namespace with our own search path.
    Gyoji::owned<NS2SearchPaths> search_paths = std::make_unique<NS2SearchPaths>();
    stack.push_back(
	std::pair(
	    ns,
	    std::move(search_paths)
	    )
	);
}

void
NS2Context::namespace_pop()
{
    stack.pop_back();
}

void
NS2Context::dump() const
{
    root->dump(0);
}



// Scenario:
// namespace foo::bar {
//
// In this scenario, we create foo and bar as namespaces
// inside the current namespace.
// We also push the namespace resolution context to resolve
// first from this new namespace 'bar' and then
// from the parents of that namespace.
// Any 'using namespace' statements add to the current resolution
// context.

// Scenario:
// }
//
// In this scenario, we pop the namespace 'bar' we just pushed
// and we also pop the 'using' statements.  Future declarations
// will go back into the root namespace.  Lookups will happen
// in the context of whatever 'using' were in effect before
// the push.  Qualified resolutions will happen relative to
// the current namespace or 'root' if not found there.

// Scenario:
// void foo();
//
// We look for an existing declaration of 'foo' in the current namespace.
// If it already exists, we throw an error.

// Scenario:
// void foo() {
//
// 1) We look for an existing declaration 'foo' in the
// current namespace resolution context.  If found, we resolve
// 'foo' to that name.  If more than one found, we
// throw an error.  If none found, we declare in current namespace.
//
// Scenario
// class Foo {
//     void foo();
// }
//
// We define 'Foo' as a class in the current namespace.
// We enter that namespace context.
// We define 'foo' inside the 'Foo' namespace.

// Scenario:
// Foo::foo();
// We look for 'Foo' in the current namespace
// resolution context.  If found as a NS, if it is a
// namespace, we look for 'Foo::foo()' declared
// inside it.  If found as a class, we look for 'foo'
// declared inside it.

