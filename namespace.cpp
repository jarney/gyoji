#include "namespace.hpp"
#include "jstring.hpp"

Namespace::Namespace(std::string _name)
  : name(_name)
{}
Namespace::~Namespace()
{}

NamespaceType::NamespaceType(std::string _name)
  : name(_name)
{}
NamespaceType::~NamespaceType()
{}

class TypeNameQualified {
public:
  std::list<std::string> path;
  TypeNameQualified();
  ~TypeNameQualified();
  TypeNameQualified & add(std::string _type);
  TypeNameQualified pop_first();
};

TypeNameQualified::TypeNameQualified()
{}

TypeNameQualified::~TypeNameQualified()
{}

TypeNameQualified & TypeNameQualified::add(std::string _type)
{
  path.push_back(_type);
  return *this;
}
TypeNameQualified TypeNameQualified::pop_first()
{
  TypeNameQualified ret;
  bool first = true;
  for (auto p : path) {
    if (!first) {
      ret.path.push_back(p);
    }
    first = false;
  }
  return ret;
}

class NamespaceParseContext {
public:
  typedef std::shared_ptr<NamespaceParseContext> ptr;
  Namespace::ptr ns;
  std::map<std::string, std::string> aliases;
};

static Namespace::ptr root;                         // This is the 'root' namespace.
static std::list<NamespaceParseContext::ptr> current_namespace; // This is the namespace we're currently defining.

void namespace_init()
{
  root = std::make_shared<Namespace>("");
  NamespaceParseContext::ptr current = std::make_shared<NamespaceParseContext>();
  current->ns = root;
  current_namespace.push_back(current);
}

// Create a new namespace and push it onto the stack,
// also adding it to the current namespace.
void namespace_begin(std::string name)
{
    Namespace::ptr new_ns = std::make_shared<Namespace>(name);
    NamespaceParseContext::ptr new_current = std::make_shared<NamespaceParseContext>();
    NamespaceParseContext::ptr current = current_namespace.back();
    current->ns->children[name] = new_ns;

    new_current->ns = new_ns;
    current_namespace.push_back(new_current);

}

void namespace_end()
{
    current_namespace.pop_back();
}

static int indent = 0;
static void print_indent(void)
{
  for (int i = 0; i < 2*indent; i++) {
    printf(" ");
  }
}

void namespace_type_define(std::string name, NamespaceType::Protection protection)
{
  NamespaceType::ptr type = std::make_shared<NamespaceType>(name);
  type->protection = protection;
  NamespaceParseContext::ptr context = current_namespace.back();
  context->ns->types[name] = type;
}

static bool namespace_exists_in_ns(Namespace::ptr ns, TypeNameQualified name, bool is_type)
{
  if (name.path.size() == 1) {
    if (is_type) {
      if (ns->types.find(name.path.back()) != ns->types.end()) {
        return true;
      }
    }
    else {
      if (ns->children.find(name.path.back()) != ns->children.end()) {
        return true;
      }
    }
    return false;
  }
  std::string find_child = name.path.front();
  if (ns->children.find(find_child) != ns->children.end()) {
    return namespace_exists_in_ns(ns->children[find_child], name.pop_first(), is_type);
  }

  return false;
}

static bool namespace_exists_qual(TypeNameQualified type, bool is_type);
static bool namespace_exists_str(std::string name, bool is_type);

bool namespace_exists_str(std::string name, bool is_type)
{
  // Remove any superfluous whitespaces.
  name = string_remove_nonidentifier(name);

  std::vector<std::string> split = string_split(name, std::string("::"));
  TypeNameQualified ns;
  for (auto s : split) {
    ns = ns.add(s);
  }
  bool rc = namespace_exists_qual(ns, is_type);
  return rc;

}

static bool namespace_exists_search(std::string name, bool is_type);

bool namespace_exists(std::string name)
{
  return namespace_exists_search(name, false);
}


bool namespace_type_exists(std::string name)
{
  return namespace_exists_search(name, true);
}

static bool namespace_exists_search(std::string name, bool is_type)
{
  bool rc = namespace_exists_str(name, is_type);
  if (rc) {
    return rc;
  }
  // Next, search each of the namespace aliases.
  for (auto alias : current_namespace.back()->aliases) {
    std::string aliased_name;
    if (alias.first.size() == 0) {
      aliased_name = alias.second + "::" + name;
    }
    else {
      aliased_name = string_replace_start(name, alias.first + std::string("::"), alias.second + std::string("::"));
    }
    rc = namespace_exists_str(aliased_name, is_type);
    if (rc) {
      return rc;
    }
  }
  return false;
}

void namespace_using(std::string name, std::string alias)
{
  NamespaceParseContext::ptr current = current_namespace.back();
  current->aliases[alias] = name;
}

// Given a simple identifier,
// find the type in the 'current' namespace.
static bool namespace_exists_qual(TypeNameQualified name, bool is_type)
{
  // First, determine if this type
  // exists in the current namespace.
  NamespaceParseContext::ptr current = current_namespace.back();
  if (namespace_exists_in_ns(current->ns, name, is_type)) {
    return true;
  }
  
  // If we can't find it in the
  // current namespace, try finding it
  // from children (or the root)
  std::string find_child = name.path.front();

  // If the first component is empty,
  // force resolution from root.
  if (find_child.size() == 0) {
    return namespace_exists_in_ns(root, name.pop_first(), is_type);
  }

  // Try the children of the current namespace
  // next.
  if (current->ns->children.find(find_child) != current->ns->children.end()) {
    if (namespace_exists_in_ns(current->ns->children[find_child], name.pop_first(), is_type)) {
      return true;
    }
  }

  // Finally, try finding it in the root
  return namespace_exists_in_ns(root, name, is_type);
}

static void namespace_dump_type(NamespaceType::ptr type)
{
  print_indent();
  printf("<type name='%s'/>\n", type->name.c_str());
}

static void namespace_dump_node(Namespace::ptr parent)
{
  print_indent();
  printf("<namespace name='%s'>\n", parent->name.c_str());
  indent++;
  for (auto t : parent->types) {
    namespace_dump_type(t.second);
  }
  for (auto ns : parent->children) {
    namespace_dump_node(ns.second);
  }
  indent--;
  print_indent();
  printf("</namespace>\n");
}

void namespace_dump()
{
  namespace_dump_node(root);
  // This is the namespace we're currently defining.
  NamespaceParseContext::ptr context = current_namespace.back();
  printf("Context is %s\n", context->ns->name.c_str());
  
  for (auto alias : current_namespace.back()->aliases) {
    printf("Context: %s %s\n", alias.first.c_str(), alias.second.c_str());
  }
}

