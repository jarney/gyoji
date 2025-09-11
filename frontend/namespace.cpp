#include "namespace.hpp"
#include <jlang-misc/jstring.hpp>

using namespace JLang::misc;
using namespace JLang::frontend;

NamespaceFoundReason::NamespaceFoundReason(int _reason)
  : reason(_reason)
  , location(nullptr)
{}

NamespaceFoundReason::NamespaceFoundReason(int _reason, Namespace::ptr _location)
  : reason(_reason)
  , location(_location)
{}

NamespaceFoundReason::~NamespaceFoundReason()
{}

Namespace::Namespace(std::string _name, int _type, int _visibility)
  : name(_name)
  , type(_type)
  , visibility(_visibility)
  , parent(nullptr)
{}
Namespace::Namespace(std::string _name, int _type, int _visibility, Namespace::ptr _parent)
  : name(_name)
  , type(_type)
  , visibility(_visibility)
  , parent(_parent)
{}

Namespace::~Namespace()
{}

// Walk up to the root and look for the 'minimum' visibility among all parents.
int Namespace::effective_visibility(void)
{
  int current_visibility = visibility;
  Namespace::ptr current = parent;
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
  Namespace::ptr current = parent;
  while (current) {
    if (current->name.size() > 0) {
      ret = "::" + current->name + ret;
    }
    current = current->parent;
  }
  
  return ret;
}

std::string Namespace::fully_qualified(void)
{
  return fully_qualified_ns() + std::string("::") + name;
}

NamespaceContext::NamespaceContext()
{
  // This is the root namespace.
  root = std::make_shared<Namespace>("", Namespace::TYPE_NAMESPACE, Namespace::VISIBILITY_PUBLIC);
  stack.push_back(root);
}

NamespaceContext::~NamespaceContext()
{}

Namespace::ptr NamespaceContext::namespace_new(std::string name, int type, int visibility)
{
  Namespace::ptr current = stack.back();
  Namespace::ptr new_namespace = std::make_shared<Namespace>(name, type, visibility, current);
  current->children[name] = new_namespace;
  return current;
}

void NamespaceContext::namespace_push(std::string ns)
{
  Namespace::ptr current = stack.back();
  auto it = current->children.find(ns);
  if (it == current->children.end()) {
    fprintf(stderr, "Error, trying to push non-existent namespace\n");
    return;
  }
  stack.push_back(it->second);
}

void NamespaceContext::namespace_pop()
{
  stack.pop_back();
}

void NamespaceContext::namespace_using(std::string name, Namespace::ptr alias)
{
  Namespace::ptr current = stack.back();
  current->aliases[name] = alias;
}

Namespace::ptr NamespaceContext::namespace_lookup_qualified(std::vector<std::string> name_qualified, Namespace::ptr root)
{
  Namespace::ptr current = root;
  auto it = name_qualified.begin();
  while (it != name_qualified.end()) {
    // Try to find it in the
    // children of current.
    auto found_child = current->children.find(*it);
    if (found_child == current->children.end()) {
      return nullptr;
    }
    current = found_child->second;
    ++it;
  }
  return current;
}

NamespaceFoundReason::ptr
NamespaceContext::namespace_lookup_visibility(std::string search_context, Namespace::ptr found)
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
    Namespace::ptr found_parent = found->parent;
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
    // If it's private, we need to make sure that
    // the full path of what we found matches the full
    // path of our current location.
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
    Namespace::ptr found = namespace_lookup_qualified(name_qualified, root);
    if (!found) {
      return std::make_shared<NamespaceFoundReason>(NamespaceFoundReason::REASON_NOT_FOUND);
    }
    return namespace_lookup_visibility(namespace_lookup_context, found);
  }

  // Otherwise, we start with the current namespace
  // and look for it there.  If we find it,
  // it's allowed to be resolved no matter the visibility.
  Namespace::ptr current = nullptr;
  
  // Finally, try looking for it in the children of each
  // of the aliases, starting with the current and walking
  // back up the stack all the way to the root looking
  // at the aliases of each parent.
  for (auto it = stack.rbegin(); it != stack.rend(); ++it) {
    current = *it;
    Namespace::ptr found = namespace_lookup_qualified(name_qualified, current);
    if (found) {
      return namespace_lookup_visibility(namespace_lookup_context, found);
    }
  
    for (auto alias_ns_it : current->aliases) {
      Namespace::ptr alias_ns = alias_ns_it.second;
      
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

Namespace::ptr NamespaceContext::current()
{
  return stack.back();
}

std::string NamespaceContext::namespace_fully_qualified()
{
  std::string ret;
  for (auto current : stack) {
    if (current->name.size() > 0) {
      ret = ret + "::" + current->name;
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
void NamespaceContext::namespace_dump_node(Namespace::ptr parent)
{
  print_indent();
  printf("<namespace name='%s'>\n", parent->name.c_str());
  indent++;
  for (auto ns : parent->children) {
    namespace_dump_node(ns.second);
  }
  for (auto alias : parent->aliases) {
    print_indent();
    printf("Context: %s name : %s\n", alias.first.c_str(), alias.second->name.c_str());
  }
  indent--;
  print_indent();
  printf("</namespace>\n");
}

void NamespaceContext::namespace_dump()
{
  if (root == nullptr) {
    printf("!!! root was null\n");
    return;
  }
  namespace_dump_node(root);
}

