#pragma once

#include <string>
#include <memory>
#include <list>
#include <map>
#include <vector>

/*!
 *  \addtogroup Frontend
 *  @{
 */
//! Namespace Management
/*!
 * This is intended to manage namespaces.
 */
namespace JLang::frontend::namespaces {

/**
 * This is a namespace resolver used in the lexical
 * analysis phase of compilation to share information
 * between the parser and lexer.  The purpose is to
 * ensure that the parser can differentiate between
 * a TYPE_NAME, a NAMESPACE_NAME, and an IDENTIFIER.
 * This allows us to use a "C-style" syntax where
 * this would otherwise result in an ambiguous
 * grammar with shift/reduce conflicts.
 *
 * The structure of a namespace is that of a tree
 * where namespaces may embed other namespaces.  Each
 * element of the tree may be a NAMESPACE (i.e. just a container).
 * It may also be a TYPE (a type defined inside a namespace)
 * or it may be a CLASS (a type and also a nested namespace).
 *
 */
class Namespace;
class NamespaceContext;

class Namespace {
public:

  
  typedef std::shared_ptr<Namespace> ptr;
  
  Namespace(std::string _name, int _type, int _visibility);
  Namespace(std::string _name, int _type, int _visibility, Namespace::ptr _parent);
  ~Namespace();

  /////////////////////////////
  // Parent of this namespace.
  // This is used so we can establish
  // the canonical name of a type
  // and guarantee that it is uniquely
  // defined even when there are
  // multiple using directives and
  // we can't rely on the context's stack
  // to tell us what the 'true' name of the
  // type is. MAYBE WE DON'T NEED!!!!
  /////////////////////////////
  Namespace::ptr parent;                           // Parent, == NULL for root.
  
  /////////////////////////////
  // Type of the namespace entry
  /////////////////////////////
  static constexpr int TYPE_NAMESPACE = 0;             // Just a plain namespace.
  static constexpr int TYPE_TYPEDEF = 1;               // This is a typedef that appears inside a namespace
  static constexpr int TYPE_CLASS = 2;                 // This is a class which is a type, but is also a namespace specific to that type.

  int type;
  /////////////////////////////
  // Protection level of the namespace.
  /////////////////////////////
  static constexpr int VISIBILITY_PUBLIC = 0;          // Visible to anyone.
  static constexpr int VISIBILITY_PROTECTED = 1;       // Visible inside the same namespace.
  static constexpr int VISIBILITY_PRIVATE = 2;         // Visible inside the same class.
  int visibility;
  
  std::string name;                                // Name of the namespace.
  
  std::map<std::string, Namespace::ptr> children;  // This is the list of child namespaces.
  
  std::map<std::string, Namespace::ptr> aliases;   // This is a list of other namespaces to
                                                   // include when resolving from within the context of this namespace.
  std::string fully_qualified_ns(void);            // Fully qualified namespace of our parent.
  std::string fully_qualified(void);               // Fully qualified namespace of ourself.

  int effective_visibility(void);                  // Walk up to the root and look for the 'minimum' visibility among all parents.
};

/**
 * This is the result of a lookup.  We can find it or
 * not find it, but also we might find it, but it may
 * be protected or private where we found it and we
 * don't have permission to 'see' it.
 */
class NamespaceFoundReason {
public:
  typedef std::shared_ptr<NamespaceFoundReason> ptr;
  static constexpr int REASON_FOUND = 0;               // We found it, ptr is safe to deref.
  static constexpr int REASON_NOT_FOUND_PRIVATE = 1;   // Found, but it's private in the place we looked.
  static constexpr int REASON_NOT_FOUND_PROTECTED = 2; // Found, but it's protected where we looked.
  static constexpr int REASON_NOT_FOUND = 3;           // Not found anywhere.

  int reason;
  Namespace::ptr location;
  NamespaceFoundReason(int _reason);
  NamespaceFoundReason(int _reason, Namespace::ptr _location);
  ~NamespaceFoundReason();
};

/**
 * This structure represents the context of a namespace
 * processor.  The contect includes the 'current' namespace
 * which is a stack of the current nesting of the namespace
 * as the parse proceeds.  When a namespace or class is declared,
 * the context 'pushes' into that namespace so that resolution
 * with proceed in the context of that namespace and new elements
 * will be declared inside that namespace.  It also contains some 'using'
 * clauses which indicate pointers to other namespaces.
 */
class NamespaceContext {
public:
  Namespace::ptr root;
  std::list<Namespace::ptr> stack;

  NamespaceContext();
  ~NamespaceContext();
  
  // Adds a child to the current namespace.
  // This can be used to create a new namespace, type, or class.
  Namespace::ptr namespace_new(std::string name, int type, int visibility);

  // Moves the resolution context into the given
  // child.  This is used after defining
  // a namespace or class to move resolution into
  // that context.
  void namespace_push(std::string name);

  // Ends definition of a namespace.
  void namespace_pop();

  // When we define a 'using', we're really just
  // defining an alias to another namespace.
  // This 'using' will be added in the current namespace's context.
  // The 'using' will only be relevant in this (and child) contexts.
  void namespace_using(std::string name, Namespace::ptr alias);

  // This returns the type resolved by the
  // given name in the current context.
  // Resolution from the root is guaranteed only
  // when the name begins with '::' indicating that
  // it should be treated as an absolute path without
  // using any 'using' directives or searching.
  NamespaceFoundReason::ptr namespace_lookup(std::string name);
  Namespace::ptr namespace_lookup_qualified(std::vector<std::string> name_qualified, Namespace::ptr root);
  NamespaceFoundReason::ptr namespace_lookup_visibility(std::string search_context, Namespace::ptr resolved_name);

  // Returns the 'fully qualified' name of a type.
  std::string namespace_fully_qualified();
  Namespace::ptr current();

  void namespace_dump_node(Namespace::ptr parent);
  void namespace_dump();

};

};
/*! @} End of Doxygen Groups*/
