#include <jlang-frontend/namespace.hpp>
#include <jlang-misc/jstring.hpp>
#include <set>

using namespace JLang::misc;
using namespace JLang::frontend::namespaces;

NamespaceFoundReason::NamespaceFoundReason(int _reason)
    : reason(_reason)
    , location(nullptr)
{}

NamespaceFoundReason::NamespaceFoundReason(int _reason, Namespace* _location)
    : reason(_reason)
    , location(_location)
{}

NamespaceFoundReason::~NamespaceFoundReason()
{}

Namespace::Namespace(std::string _name, int _type, int _visibility)
    : parent(nullptr)
    , type(_type)
    , visibility(_visibility)
    , name(_name)
{}
Namespace::Namespace(std::string _name, int _type, int _visibility, Namespace* _parent)
    : parent(_parent)
    , type(_type)
    , visibility(_visibility)
    , name(_name)
{}

Namespace::~Namespace()
{}

// Walk up to the root and look for the 'minimum' visibility among all parents.
int Namespace::effective_visibility(void)
{
    int current_visibility = visibility;
    Namespace* current = parent;
    while (current) {
	if (current->visibility > current_visibility) {
	    current_visibility = current->visibility;
	}
	current = current->parent;
    }
    return current_visibility;
}

std::string Namespace::fully_qualified_ns(void)
{
    std::string ret;
    
    if (parent == nullptr) {
	return std::string("");
    }
    Namespace* current = parent;
    while (current) {
	if (ret.size() > 0 && (current->parent != nullptr)) {
	    ret = current->name + "::" + ret;
	}
	else {
	    ret = current->name + ret;
	}
	current = current->parent;
    }
    
    return ret;
}

std::string Namespace::fully_qualified(void)
{
    std::string ns = fully_qualified_ns();
    if (ns.size() > 0) {
	return ns + std::string("::") + name;
    }
    return name;
}

int
Namespace::get_type() const
{ return type; }

int
Namespace::get_visibility() const
{ return visibility; }

NamespaceContext::NamespaceContext()
{
    // This is the root namespace.
    root = std::make_unique<Namespace>("", Namespace::TYPE_NAMESPACE, Namespace::VISIBILITY_PUBLIC);
    stack.push_back(root.get());
    
    // XXX Definitely not the place to do this, but
    // it's a good enough place for now that I don't care
    // until we have a type system we can plug into this
    namespace_new("i8", Namespace::TYPE_TYPEDEF, Namespace::VISIBILITY_PUBLIC);
    namespace_new("i16", Namespace::TYPE_TYPEDEF, Namespace::VISIBILITY_PUBLIC);
    namespace_new("i32", Namespace::TYPE_TYPEDEF, Namespace::VISIBILITY_PUBLIC);
    namespace_new("i64", Namespace::TYPE_TYPEDEF, Namespace::VISIBILITY_PUBLIC);
    
    namespace_new("u8", Namespace::TYPE_TYPEDEF, Namespace::VISIBILITY_PUBLIC);
    namespace_new("u16", Namespace::TYPE_TYPEDEF, Namespace::VISIBILITY_PUBLIC);
    namespace_new("u32", Namespace::TYPE_TYPEDEF, Namespace::VISIBILITY_PUBLIC);
    namespace_new("u64", Namespace::TYPE_TYPEDEF, Namespace::VISIBILITY_PUBLIC);
    
    namespace_new("f32", Namespace::TYPE_TYPEDEF, Namespace::VISIBILITY_PUBLIC);
    namespace_new("f64", Namespace::TYPE_TYPEDEF, Namespace::VISIBILITY_PUBLIC);
    
    namespace_new("void", Namespace::TYPE_TYPEDEF, Namespace::VISIBILITY_PUBLIC);
    namespace_new("bool", Namespace::TYPE_TYPEDEF, Namespace::VISIBILITY_PUBLIC);
}

NamespaceContext::~NamespaceContext()
{}

Namespace* NamespaceContext::namespace_new(std::string name, int type, int visibility)
{
    Namespace* current = stack.back();
    JLang::owned<Namespace> new_namespace = std::make_unique<Namespace>(name, type, visibility, current);
    current->children.insert(std::pair(name, std::move(new_namespace)));
    return new_namespace.get();
}

void NamespaceContext::namespace_push(std::string ns)
{
    Namespace* current = stack.back();
    auto it = current->children.find(ns);
    if (it == current->children.end()) {
	fprintf(stderr, "Compiler Bug! Error, trying to push non-existent namespace\n");
	exit(1);
	return;
    }
    stack.push_back(it->second.get());
}

void NamespaceContext::namespace_pop()
{
    stack.pop_back();
}

void NamespaceContext::namespace_using(std::string name, Namespace* alias)
{
    Namespace* current = stack.back();
    current->aliases.push_back(std::pair(name, alias));
}

Namespace* NamespaceContext::namespace_lookup_qualified(std::vector<std::string> name_qualified, Namespace* root)
{
    Namespace* current = root;
    auto it = name_qualified.begin();
    while (it != name_qualified.end()) {
	// Try to find it in the
	// children of current.
	auto found_child = current->children.find(*it);
	if (found_child == current->children.end()) {
	    return nullptr;
	}
	current = found_child->second.get();
	++it;
    }
    return current;
}

NamespaceFoundReason::ptr
NamespaceContext::namespace_lookup_visibility(std::string search_context, Namespace* found)
{
    if (!found) {
	return std::make_shared<NamespaceFoundReason>(NamespaceFoundReason::REASON_NOT_FOUND);
    }
    // If it's public, we can find it no matter
    // our context.
    int effective_visibility = found->effective_visibility();
    if (effective_visibility == Namespace::VISIBILITY_PUBLIC) {
	return std::make_shared<NamespaceFoundReason>(NamespaceFoundReason::REASON_FOUND, found);
    }
    
    if (effective_visibility == Namespace::VISIBILITY_PROTECTED) {
	Namespace* found_parent = found->parent;
	std::string found_context = found_parent->fully_qualified_ns();
	
	// If it's protected, we need to make sure that the full path
	// of what's found is a parent of our current location.
	// If it's protected, then found must be contained in search.
	size_t pos = search_context.find(found_context);
	if (pos == 0) {
	    return std::make_shared<NamespaceFoundReason>(NamespaceFoundReason::REASON_FOUND, found);
	}
	else {
	    return std::make_shared<NamespaceFoundReason>(NamespaceFoundReason::REASON_NOT_FOUND_PROTECTED);
	}
    }
    
    // What we should be checking is the 'minimum'
    // value of visibility as we walk up the tree.
    if (effective_visibility == Namespace::VISIBILITY_PRIVATE) {
	std::string found_context = found->fully_qualified_ns();
	// If it is private, we need to make sure that
	// the full path of what we found matches the full
	// path of our current location.
	//    return std::make_shared<NamespaceFoundReason>(NamespaceFoundReason::REASON_NOT_FOUND_PRIVATE);
	size_t pos = search_context.find(found_context);
	if (pos == 0) {
	    return std::make_shared<NamespaceFoundReason>(NamespaceFoundReason::REASON_FOUND, found);
	}
	else {
	    return std::make_shared<NamespaceFoundReason>(NamespaceFoundReason::REASON_NOT_FOUND_PRIVATE);
	}
    }
    
    // Not found because visibility type is unknown
    return std::make_shared<NamespaceFoundReason>(NamespaceFoundReason::REASON_NOT_FOUND);
}

std::vector<std::string>
NamespaceContext::namespace_search_path(std::string name)
{
    std::vector<std::string> path;
    std::set<std::string> uniq;

    // TODO: If it starts with ::,
    // Force resolution only from root.

    // Look in our "stack" of namespaces in that order.
    for (auto it = stack.rbegin(); it != stack.rend(); ++it) {
	Namespace* current = *it;
	std::string pathel = join_nonempty(current->fully_qualified(), name, "::");
	if (uniq.find(pathel) == uniq.end()) {
	    path.push_back(pathel);
	    uniq.insert(pathel);
	}
    }
    
    // Look in any namespaces that we are 'using'
    for (const auto & alias_ns_it : current()->aliases) {
	if (alias_ns_it.first.size() == 0) {
	    std::string pathel = join_nonempty(alias_ns_it.second->fully_qualified(), name, "::");
	    if (uniq.find(pathel) == uniq.end()) {
		path.push_back(pathel);
		uniq.insert(pathel);
	    }
	}
	else {
	    std::string pathel = string_replace_start(
		name,
		alias_ns_it.first + std::string("::"),
		alias_ns_it.second->fully_qualified() + std::string("::")
		);
	    if (uniq.find(pathel) == uniq.end()) {
		path.push_back(pathel);
		uniq.insert(pathel);
	    }
	}
    }
    
    return path;
}


NamespaceFoundReason::ptr NamespaceContext::namespace_lookup(std::string name)
{
    std::vector<std::string> name_qualified = string_split(name, std::string("::"));
    // If we don't have any name, it doesn't exist.
    if (name_qualified.size() == 0) {
	return std::make_shared<NamespaceFoundReason>(NamespaceFoundReason::REASON_NOT_FOUND);
    }
    
    std::string namespace_lookup_context = namespace_fully_qualified();
    
    // If it starts with '::' then it is an
    // absolute path and should be resolved
    // from the root without any 'using' qualifiers.
    if (name_qualified.begin()->size() == 0) {
	name_qualified.erase(name_qualified.begin());
	Namespace* found = namespace_lookup_qualified(name_qualified, root.get());
	if (!found) {
	    return std::make_shared<NamespaceFoundReason>(NamespaceFoundReason::REASON_NOT_FOUND);
	}
	return namespace_lookup_visibility(namespace_lookup_context, found);
    }
    
    // Otherwise, we start with the current namespace
    // and look for it there.  If we find it,
    // it's allowed to be resolved no matter the visibility.
    Namespace* current = nullptr;
    
    // Finally, try looking for it in the children of each
    // of the aliases, starting with the current and walking
    // back up the stack all the way to the root looking
    // at the aliases of each parent.
    for (auto it = stack.rbegin(); it != stack.rend(); ++it) {
	current = *it;
	Namespace* found = namespace_lookup_qualified(name_qualified, current);
	if (found) {
	    return namespace_lookup_visibility(namespace_lookup_context, found);
	}
	
	for (auto alias_ns_it : current->aliases) {
	    Namespace* alias_ns = alias_ns_it.second;
	    
	    std::string new_name = string_replace_start(name, alias_ns_it.first + std::string("::"), std::string());
	    std::vector<std::string> new_name_qualified = string_split(new_name, std::string("::"));
	    
	    found = namespace_lookup_qualified(new_name_qualified, alias_ns);
	    if (found != nullptr) {
		return namespace_lookup_visibility(namespace_lookup_context, found);
	    }
	}
    }
    
    // It's not found anywhere, so we
    // return null;
    return std::make_shared<NamespaceFoundReason>(NamespaceFoundReason::REASON_NOT_FOUND);
    
}

Namespace* NamespaceContext::current()
{
    return stack.back();
}

std::string NamespaceContext::namespace_fully_qualified()
{
    std::string ret;
    for (auto current : stack) {
	if (ret.size() > 0 && (current->parent != nullptr)) {
	    ret = ret + "::" + current->name;
	}
	else {
	    ret = ret + current->name;
	}
    }
    return ret;
}

static int indent = 0;
static void print_indent(void)
{
    for (int i = 0; i < 2*indent; i++) {
	printf(" ");
    }
}
void NamespaceContext::namespace_dump_node(Namespace* parent) const
{
    print_indent();
    std::string type;
    switch (parent->get_type()) {
    case Namespace::TYPE_NAMESPACE:
	type = "namespace";
	break;
    case Namespace::TYPE_TYPEDEF:
	type = "typedef";
	break;
    case Namespace::TYPE_CLASS:
	type = "class";
	break;
    default:
	printf("!Compiler bug.  Unknown namespace type %d\n", parent->get_type());
	exit(1);
    }
    if (parent->children.size() == 0 && parent->aliases.size() == 0) {
	printf("<namespace name='%s' type='%s' vis='%d' />\n", parent->name.c_str(), type.c_str(), parent->visibility);
    }
    else {
	printf("<namespace name='%s' type='%s' vis='%d'>\n", parent->name.c_str(), type.c_str(), parent->visibility);
	indent++;
	for (const auto & ns : parent->children) {
	    namespace_dump_node(ns.second.get());
	}
	for (auto alias : parent->aliases) {
	    print_indent();
	    printf("<using ns='%s' as='%s'/>\n", alias.second->name.c_str(), alias.first.c_str());
	}
	indent--;
	print_indent();
	printf("</namespace>\n");
    }
}

void NamespaceContext::namespace_dump() const
{
    if (root == nullptr) {
	printf("!!! root was null\n");
	return;
    }
    namespace_dump_node(root.get());
}

