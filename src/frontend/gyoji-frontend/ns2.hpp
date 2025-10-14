#include <string>
#include <memory>
#include <list>
#include <map>
#include <vector>

#include <gyoji-misc/pointers.hpp>
#include <gyoji-context/source-reference.hpp>

namespace Gyoji::frontend::namespaces {
    
    class NS2;
    class NS2Context;

    /**
     * @brief Entity living inside a namespace.
     *
     * @details
     * A namespace entity is anything that might
     * live inside a namespace.  This may
     * be an identifier, a type, a class, or
     * another namespace.  Note that classes
     * and namespaces also may contain namespaces of their own
     * so that they can be nested.
     */
    class NS2Entity {
    public:
	typedef enum {
	    /**
	     * The named element is some
	     * concrete object that is not a type.
	     */
	    ENTITY_TYPE_IDENTIFIER,
	    /**
	     * The named element is a type
	     * For example, a typedef
	     * may appear here.
	     */
	    ENTITY_TYPE_TYPE,
	    /**
	     * The named element is a class
	     * that is an entity itself, but also
	     * spawns its own namespace.
	     */
	    ENTITY_TYPE_CLASS,

	    ENTITY_TYPE_NAMESPACE,

	    /**
	     * This is a label used by a goto statement.
	     * It may be declared by either a 'goto' or
	     * 'label' statement and is a different type
	     * of identifier, but needs to still be unique in
	     * the namespace of a function.
	     */
	    ENTITY_TYPE_LABEL
	} EntityType;
	
	NS2Entity(
	    std::string _name,
	    EntityType _type,
	    NS2Entity *parent,
	    const Gyoji::context::SourceReference & _source_ref
	    );
	~NS2Entity();

	/**
	 * Returns the 'simple' name
	 * that is a leaf-node in a namespace.
	 * The namespace manages only the
	 * simple entity name and type, but carries
	 * no other semantic information.  This exists
	 * to provide a unique fully-qualified name
	 * for each entity that needs a name.  The
	 * semantics of the object being named are
	 * managed elsewhere in the context it is
	 * needed.
	 */
	const std::string & get_name() const;
	
	std::string get_fully_qualified_name() const;

	/**
	 * The type of object being named.  This
	 * may be a TYPE or an IDENTIFIER.  If it
	 * is a TYPE, the specific type information
	 * is carried elsewhere.
	 */
	const EntityType & get_type() const;

	NS2Entity* get_parent() const;

	NS2Entity* add_identifier(
	    std::string _name,
	    const Gyoji::context::SourceReference & _source_ref
	    );
			    
	NS2Entity* add_type(
	    std::string _name,
	    const Gyoji::context::SourceReference & _source_ref
	    );
	NS2Entity* add_class(
	    std::string _name,
	    const Gyoji::context::SourceReference & _source_ref
	    );
	
	NS2Entity* add_namespace(
	    std::string _namespace,
	    const Gyoji::context::SourceReference & _source_ref
	    );

	NS2Entity* add_entity(std::string _name, Gyoji::owned<NS2Entity> _entity);
	
	NS2Entity *get_entity(std::string _name) const;
	
	const Gyoji::context::SourceReference & get_source_ref() const;

	void dump(int indent) const;

	
    private:
	std::string name;
	EntityType type;
	NS2Entity *parent;
	const Gyoji::context::SourceReference & source_ref;
	std::map<std::string, Gyoji::owned<NS2Entity>> elements;
    };
    
    class NS2SearchPaths {
    public:
	NS2SearchPaths();
	~NS2SearchPaths();

	/**
	 * This adds a name to the search path.  If there is
	 * another name already existing in the search path,
	 * then an error is returned.
	 */
	void add_using(std::string name, NS2Entity *alias);

	/**
	 * This attempts to find a namespace associated
	 * with a name.  If found, that name will be returned
	 * as the first name in the aliases that matches.
	 */
	NS2Entity *get_name(std::string name);

	const std::vector<std::pair<std::string, NS2Entity*>> & get_aliases() const;
    private:
	/**
	 * This is the list of names and associated aliases
	 * to be used when resolving a name.  When the user
	 * specifies a name, that name will be looked up
	 * using the associated namespace and resolve there
	 * if it can be resolved.  This search path
	 * will be searched in the order in which the names
	 * are added.
	 */
	std::vector<std::pair<std::string, NS2Entity*>> aliases;

	/**
	 * This map is redundant with the vector and serves to
	 * help maintain uniqueness by allowing quick lookups
	 * to what aliases are currently defined.
	 */
	std::map<std::string, NS2Entity*> alias_map;
    };
    
    /**
     * This is the context used for namespace
     * resolution.  It consists of a 'stack' structure
     * which is updated as the parse progresses.  It
     * contains the names available in the namespace
     * as well as the search path that is currently
     * active in the namespace through 'using' statements
     * which operate to update our search path.
     */
    class NS2Context {
    public:
	NS2Context();
	~NS2Context();

	/**
	 * When we define a 'using', we're really just
	 * defining an alias to another namespace.
	 * This 'using' will be added in the current namespace's search path.
	 * The 'using' will only be relevant in this (and child) contexts,
	 * so when a namespace is left, all of the 'using' namespaces that
	 * were active inside that namespace will be no longer used even
	 * if we re-enter that namespace, the search paths will be forgotten.
	 */
	void namespace_using(std::string name, NS2Entity* alias);

	/**
	 * Add a new namespace to the current
	 * namespace.  The name here may be a
	 * composite name with foo::bar::baz
	 * representing 3 new namespaces to add nested.
	 *
	 * This can return an error if the name
	 * conflicts with an entity defined in 
	 * a namespace already.
	 *
	 * Returns the leaf-node namespace added.
	 */
	/**
	 * Resolve the name in the current namespace
	 * resolution context.
	 */
	NS2Entity* namespace_find_in(NS2Entity* current, std::string _name) const;

        NS2Entity* namespace_find_in(NS2Entity* current, std::vector<std::string> names) const;
	
	NS2Entity* namespace_find(std::string name) const;

	NS2Entity *get_current() const;
        /**
	 * Pushes our namespace resolution context
	 * using the given namespace.
	 */
	void namespace_push(NS2Entity *ns);
	void namespace_pop();
	
	void dump() const;
	
	static const std::string NAMESPACE_DELIMITER;
    private:

	/**
	 * This is the 'root' namespace
	 * which all other names are contained
	 * inside.
	 */
	Gyoji::owned<NS2Entity> root;

	/**
	 * This is the 'stack' of namespaces
	 * that is 'pushed' and 'popped' when
	 * we enter and leave the context
	 * of a namespace.  This stack consists
	 * of a namespace (NS2) and a set of search
	 * paths (using) that are currently active.
	 */
	std::vector<std::pair<NS2Entity*, Gyoji::owned<NS2SearchPaths>>> stack;
    };
    
};

