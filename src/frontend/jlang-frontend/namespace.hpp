#pragma once

#include <string>
#include <memory>
#include <list>
#include <map>
#include <vector>

#include <jlang-misc/pointers.hpp>
/*!
 *  \addtogroup Frontend
 *  @{
 */

//! Namespace Management
/*!
 * The namespaces namespace (HA!) is responsible for
 * maintaining the tree of namespaces found in the
 * source file.  This is important because the C-style
 * syntax is ambiguous unless the lexer provides
 * information on whether a token refers to a
 * general identifier (i.e. variable name) or a
 * type (i.e. char).  Without that disambiguation, the
 * syntax leads to shift/reduce conflicts in parse
 * generation.  Therefore, the job of the namespaces
 * in the front-end is to keep track of defined types,
 * classes, and namespaces so that they can be recognized later
 * during the parse and provided back to the lexer
 * to distinguish between types and identifiers.
 *
 * As a part of this disambiguation, the types and
 * identifiers are associated with the namespaces in which they
 * are defined so that when the parse is complete,
 * the fully-qualified type names are available.
 *
 * This is used later to inform the code-generation
 * layers because fully-qualified types and identifiers
 * will be needed in order to correctly generate
 * the symbols for the object files.
 */
namespace JLang::frontend::namespaces {
    
    class Namespace;
    class NamespaceContext;
    
    /**
     * A namespace is a container for types, identifiers,
     * and other namespaces for the purpose of organizing
     * the source files for large projects.  Each namespace
     * may contain one or more other namespaces with a type
     * of NAMESPACE, TYPEDEF, or CLASS.  In addition, namespaces
     * support visibility rules which allow them to mark their content
     * as public, protected, or private so that library authors
     * can communicate which aspects of the interface are publicly
     * available and supported and which parts should not be used
     * by external entities.  Note that the visibility rules
     * here are advisory and should not be taken as security measures
     * as they can easily be circumvented by the keen user.
     *
     * The structure of a namespace is that of a tree
     * where namespaces may embed other namespaces.  Each
     * element of the tree may be a NAMESPACE (i.e. just a container).
     * It may also be a TYPEDEF (a type defined inside a namespace)
     * or it may be a CLASS (a type and also a nested namespace).
     */
    class Namespace {
    public:
	/////////////////////////////
	// Type of the namespace entry
	/////////////////////////////
	static constexpr int TYPE_NAMESPACE = 0;             // Just a plain namespace.
	static constexpr int TYPE_TYPEDEF = 1;               // This is a typedef that appears inside a namespace
	static constexpr int TYPE_CLASS = 2;                 // This is a class which is a type, but is also a namespace specific to that type.
	
	/////////////////////////////
	// Protection level of the namespace.
	/////////////////////////////
	static constexpr int VISIBILITY_PUBLIC = 0;          // Visible to anyone.
	static constexpr int VISIBILITY_PROTECTED = 1;       // Visible inside the same namespace.
	static constexpr int VISIBILITY_PRIVATE = 2;         // Visible inside the same class.
	
	/**
	 * Returns the type of the namespace.  This will be one of:
	 *             * TYPE_NAMESPACE : A nested namespace.
	 *             * TYPE_TYPEDEF : Aliases for other types.
	 *             * TYPE_CLASS : Composite types.
	 */
	int get_type() const;
	/**
	 * Returns the visibility of the namespace.  This will be
	 * one of:
	 *             * VISIBILITY_PUBLIC : Visible to anyone.
	 *             * VISIBILITY_PROTECTED : Visible inside the parent's namespace.
	 *             * VISIBILITY_PRIVATE : Visible inside the same namespace only.
	 *
	 */
	int get_visibility() const;
	
	/**
	 * Returns the fully-qualified namespace of the
	 * parent namespace.
	 */
	std::string fully_qualified_ns(void);            // Fully qualified namespace of our parent.
	/**
	 * Returns the fully qualified namespace
	 * of this namespace node, type, or class.
	 */
	std::string fully_qualified(void);               // Fully qualified namespace of ourself.
	
	/**
	 *
	 * This returns the effective visibility
	 * of the current namespace by taking into
	 * account the visibility of all of our parents.
	 * In other words, if one of our parents is private
	 * then we are also private because in theory,
	 * the entirety of our parent lineage is marked private.
	 *
	 */
	int effective_visibility(void);                  // Walk up to the root and look for the 'minimum' visibility among all parents.
	
	Namespace(std::string _name, int _type, int _visibility);
	Namespace(std::string _name, int _type, int _visibility, Namespace* _parent);
	~Namespace();
	
	friend JLang::frontend::namespaces::NamespaceContext;
    protected:
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
	Namespace* parent;                           // Parent, == NULL for root.
	
	int type;
	
	int visibility;
	
	std::string name;                                // Name of the namespace.
	
	std::map<std::string, JLang::owned<Namespace>> children;  // This is the list of child namespaces.
	
	std::map<std::string, Namespace*> aliases;   // This is a list of other namespaces to
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
	Namespace* location;
	NamespaceFoundReason(int _reason);
	NamespaceFoundReason(int _reason, Namespace* _location);
	~NamespaceFoundReason();
    };
    
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
     
     * This structure represents the context of a namespace
     * processor.  The contect includes the 'current' namespace
     * which is a stack of the current nesting of the namespace
     * as the parse proceeds.  When a namespace or class is declared,
     * the context 'pushes' into that namespace so that resolution
     * with proceed in the context of that namespace and new elements
     * will be declared inside that namespace.  It also contains some 'using'
     * clauses which indicate pointers to other namespaces.
     *
     * The context of a namespace may also include one or more 'using' statements
     * which indicate that when resolving types, other namespaces should
     * be considered in the search path.
     */
    class NamespaceContext {
    public:
	/**
	 * This constructs a new namespace.  The initial namespace
	 * is not empty because it will contain definitions of the
	 * JLang built-in primitive types such as 'u8', 'u32', 'i32',
	 * etc for primitive data types.  These types exist in the
	 * root namespace and are public because they represent the
	 * fundamental building blocks that other types and classes
	 * are built upon.
	 */
	NamespaceContext();
	/**
	 * Destructor, nothing fancy.
	 */
	~NamespaceContext();
	
	/**
	 * Adds a child to the current namespace.
	 * This can be used to create a new namespace, type, or class.
	 * @param name The name of the new namespace to add.
	 * @param type The type of namespace may be one of:
	 *             * TYPE_NAMESPACE : A nested namespace.
	 *             * TYPE_TYPEDEF : Aliases for other types.
	 *             * TYPE_CLASS : Composite types.
	 * @param visibility The visibility scope of the namespace to add and should be one of:
	 *             * VISIBILITY_PUBLIC : Visible to anyone.
	 *             * VISIBILITY_PROTECTED : Visible inside the parent's namespace.
	 *             * VISIBILITY_PRIVATE : Visible inside the same namespace only.
	 */
	Namespace* namespace_new(std::string name, int type, int visibility);
	
	/**
	 * Moves the resolution context into the given
	 * child.  This is used after defining
	 * a namespace or class to move resolution into
	 * that context.  This means that new classes,
	 * namespaces, and types will be devined inside the
	 * child namespace.
	 */
	void namespace_push(std::string name);
	
	/**
	 * Moves the resolution context into the
	 * parent, popping it off of the stack.
	 * New namespaces will be created in the
	 * new context and any namespace lookups
	 * will also be performed at that level.
	 */
	void namespace_pop();
	
	/**
	 * Returns a pointer to the namespace
	 * that is current as of where the parse
	 * is during the resolution.
	 * For example, after "namespace foo { namespace bar",
	 * this will return a pointer to the 'bar' namespace.
	 */
	Namespace* current();
	
	/**
	 * When we define a 'using', we're really just
	 * defining an alias to another namespace.
	 * This 'using' will be added in the current namespace's context.
	 * The 'using' will only be relevant in this (and child) contexts.
	 * The effect of this is that when looking up types,
	 * in addition to searching the current namespace,
	 * any namespaces in the 'using' list will be searched
	 * as well.
	 */
	void namespace_using(std::string name, Namespace* alias);
	
	/**
	 * This returns the type resolved by the
	 * given name in the current context.
	 *
	 * If the name begins with '::' then the
	 * resolution of the name will be attempted
	 * at the root of the tree.
	 *
	 * Otherwise, resolution begins with the current namespace
	 * and proceeds upward in the namespace tree to the root.
	 *
	 * If no match is found, then the namespaces
	 * that are 'using' will be searched.
	 *
	 * Note that once a match is found, it is evaluated
	 * against the visibility rules so that it can
	 * be determined whether the match is valid
	 * under the visibility rules.
	 */
	NamespaceFoundReason::ptr namespace_lookup(std::string name);
	
	/**
	 * This returns the fully-qualified namespace name of
	 * the current point in the namespace resolution.
	 * For example, after "namespace foo { namespace bar",
	 * this will return "::foo::bar" indicating that this is
	 * the current namespace.  New namespaces
	 * and tokens will be created based on this namespace
	 * until either a new namespace is pushed or the
	 * current namespace scope ends with a "}".
	 */
	std::string namespace_fully_qualified();
	
	/**
	 * This is used for debugging namespace
	 * resolution by dumping the entire
	 * namespace tree to stdout.
	 */
	void namespace_dump() const;
    private:
	/**
	 * This is used for debugging namespace
	 * resolution by dumping the given namespace
	 * tree to stdout.
	 */
	void namespace_dump_node(Namespace* parent) const;
	
	/**
	 * This is used internally to search for namespace matches
	 * by splitting the search name on "::" delimiters and
	 * searching in a structured way through the tree starting
	 * at the given node.  This may return a pointer to the
	 * matched namespace or may return nullptr if no match
	 * is found.
	 */
	Namespace* namespace_lookup_qualified(std::vector<std::string> name_qualified, Namespace* root);
	
	/**
	 * This is used internally by namespace_lookup to search the various
	 * possible places where a namespace can be found and
	 * evaluating the matches against the visibility rules.
	 */
	NamespaceFoundReason::ptr namespace_lookup_visibility(std::string search_context, Namespace* resolved_name);
	
	JLang::owned<Namespace> root;
	std::list<Namespace*> stack;
	
	
    };
    
};
/*! @} End of Doxygen Groups*/
