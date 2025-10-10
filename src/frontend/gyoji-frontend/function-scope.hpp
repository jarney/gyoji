#pragma once
#include <gyoji-frontend.hpp>
#include <gyoji-mir.hpp>

namespace Gyoji::frontend::lowering {

    class Scope;
    class ScopeOperation;
    class ScopeTracker;
    class LocalVariable;

    /**
     * @brief Primitive operation in a scope
     * @details
     * The only types of operations in this scope tracker
     * are variable declarations, label definitions, goto statements,
     * and sub-scopes.  This is a HIGHLY abstract
     * representation specifically for the purpose of reasoning
     * about the sequence of operations in a 'goto' scenario.
     * Other types of operations inside scopes are deliberately
     * omitted because they are irrelevant to the problem
     * this is trying to solve.
     */
    class ScopeOperation {
    public:

	typedef enum {
	    /**
	     * Represents declaration of a variable inside a scope.
	     */
	    VAR_DECL,
	    /**
	     * Represents a label declared in the scope.
	     */
	    LABEL_DEFINITION,
	    /**
	     * Represents a goto statement inside the scope.
	     */
	    GOTO_DEFINITION,
	    /**
	     * Represents a child of this scope, which is itself
	     * another scope.
	     */
	    CHILD_SCOPE
	} ScopeOperationType;
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	~ScopeOperation();

	static Gyoji::owned<ScopeOperation> create_variable(
	    std::string _variable_name,
	    const Gyoji::mir::Type *_variable_type,
	    const Gyoji::context::SourceReference & _source_ref
	    );
	static Gyoji::owned<ScopeOperation> create_label(
	    std::string _label_name,
	    const Gyoji::context::SourceReference & _source_ref
	    );
	static Gyoji::owned<ScopeOperation> create_goto(
	    std::string _goto_label,
	    const Gyoji::context::SourceReference & _source_ref
	    );
	static Gyoji::owned<ScopeOperation> create_child(
	    Gyoji::owned<Scope> _child,
	    const Gyoji::context::SourceReference & _source_ref
	    );
	
	void dump(int indent) const;
	const ScopeOperationType & get_type() const;
	const std::string & get_label_name() const;
	const std::string & get_goto_label() const;
	const std::string & get_variable_name() const;
	const Scope *get_child() const;

	const Gyoji::context::SourceReference & get_source_ref() const;

	
    private:
	ScopeOperation(
	    ScopeOperationType _type,
	    const Gyoji::context::SourceReference & _source_ref
	    );
	ScopeOperationType type;

	const Gyoji::context::SourceReference & source_ref;

	LocalVariable *local_variable;
	const Gyoji::mir::Type *variable_type;
	std::string variable_name;
	
	std::string label_name;
	
	std::string goto_label;
	Gyoji::owned<Scope> child;
    };

    class LocalVariable {
    public:
	LocalVariable(
	    std::string _name,
	    const Gyoji::mir::Type *_type,
	    const Gyoji::context::SourceReference & _source_ref
	    );
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	~LocalVariable();
	const std::string & get_name() const;
	const Gyoji::mir::Type *get_type() const;
	const Gyoji::context::SourceReference & get_source_ref() const;
    private:
	std::string name;
	const Gyoji::mir::Type *type;
	const Gyoji::context::SourceReference & source_ref;
    };

    /**
     * @brief Represents variable declarations, labels, and goto inside a lexical scope.
     * @details
     * This class represents a highly simplified representation
     * of operations inside a lexical scope.  The only
     * operations that happen here are sub-scopes, labels,
     * goto statements, and variable declarations.  The order
     * of the operations is important because it's important
     * to reason about whether initialization has happened before
     * labels in the context of a 'goto' to a different scope.
     *
     * In addition, if the scope is a 'loop' (i.e. a while or for loop), this
     * tracks the Block ID of where a 'continue' or 'break' statment should
     * branch to.
     */
    class Scope {
    public:
	/**
	 * This is an anonymous scope that is NOT a
	 * loop (i.e. break and continue are not permitted here).
	 */
	Scope();
	/**
	 * This is a scope that is associated with some
	 * loop like a 'for' or 'while'.  The block ID for
	 * where to jump to in case of break or continue
	 * are provided so that they can be known if they are needed.
	 */
	Scope(bool _is_loop,
	      size_t _loop_break_blockid,
	      size_t _loop_continue_blockid
	    );
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	~Scope();
	void add_operation(Gyoji::owned<ScopeOperation> op);
	void dump(int indent) const;
	void dump_flat(
	    std::vector<const ScopeOperation*> & flat,
	    std::map<std::string, size_t> & label_locations,
	    std::map<size_t, std::string> & goto_labels_at,
	    size_t prior_point,
	    std::map<size_t, size_t> & edges
	    ) const;
	bool skips_initialization(std::string label) const;

	bool is_ancestor(const Scope *other) const;

	/**
	 * Adds a variable to the current scope.  It is assumed
	 * that the caller has previously verified that
	 * the variable is not already defined in scope using the
	 * 'get_variable' to look for it.
	 */
	void add_variable(std::string name, const Gyoji::mir::Type *type, const Gyoji::context::SourceReference & source_ref);
	
	/**
	 * If the given variable is defined in this scope, return
	 * the type of it.  If no such variable is defined, returns
	 * nullptr.
	 */
	const LocalVariable *get_variable(std::string name) const;

	bool is_loop() const;
	size_t get_loop_break_blockid() const;
	size_t get_loop_continue_blockid() const;

	const std::map<std::string, Gyoji::owned<LocalVariable>> & get_variables() const;
    private:
	friend ScopeTracker;
	Scope *parent;

	// If this scope is a loop,
	// what is the block id to jump
	// to for a 'break' or 'continue' operation?
	bool scope_is_loop;
	size_t loop_break_blockid;
	size_t loop_continue_blockid;
	
	std::vector<Gyoji::owned<ScopeOperation>> operations;
	std::map<std::string, Gyoji::owned<LocalVariable>> variables;
    };

    /**
     * @brief A named label inside a scope.
     *
     * @details
     * A label represents a location in a scope
     * that can be the target of a 'goto' statement.
     * Labels also have associated with them a set of
     * local variables that are currently in scope.
     * The rule for a 'goto' statement is that
     * you can only 'goto' a label that is in the
     * same scope and has the same local variables
     * defined in it.  Jumping to other scopes
     * would bring complicated behavior involving
     * dynamically de-allocating and re-allocating
     * variables in the scope that we would rather
     * avoid.
     *
     * By definition, a label is the start of a
     * basic block and a 'goto' is the end of a
     * basic block almost by definition of basic blocks.
     */
    class FunctionLabel {
    public:
        FunctionLabel(
	    std::string _name,
	    size_t _block_id
	    );
        FunctionLabel(const FunctionLabel & _other);
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
        ~FunctionLabel();
	const std::string & get_name() const;
	size_t get_block() const;
	bool is_resolved() const;
	const Scope *get_scope() const;
	void set_scope(const Scope *scope, const Gyoji::context::SourceReference & _src_ref);
	const Gyoji::context::SourceReference & get_source_ref() const;
    private:
	std::string name;
	bool resolved;
	size_t block_id;
	const Gyoji::context::SourceReference * src_ref;
	const Scope *scope;  // The scope where the label is actually defined.
    };

    /**
     * @brief Tracks variables declared in each scope along with
     *        abels and goto statements in a highly simplified
     *        intermediate representation.
     * @details
     * This class exists to represent a scope in a
     * highly simplified way that is specificatlly
     * designed to provide all of the information
     * needed to resolve labels and goto statements
     * and unwind scopes when a goto/branch
     * would take control of the program directly from
     * one scope to another.  Some of these constructs
     * violate the 'normal' scope rules about variable
     * declaration, so this is done in order to
     * first check that the jump is legal and then
     * to provide the information to the FunctionDefinitionResolver
     * about what variables would be leaving scope
     * when a goto occurs.  In addition, there is logic
     * here to deal with the fact that a 'goto' may
     * precede its corresponding 'label' definition
     * so that the code can be emitted anyway.
     *
     * This scope tracker is only to be used inside the
     * lowering process as an intermediate calculation
     * and should not be used outside of that context.
     */
    class ScopeTracker {
    public:
	ScopeTracker(const Gyoji::context::CompilerContext & _compiler_context);
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	~ScopeTracker();

	/**
	 * Enter a new (un-named) scope.
	 */
	void scope_push(
	    const Gyoji::context::SourceReference & _source_ref
	    );

	/**
	 * Pushes a 'loop' scope which knows where
	 * we would jump to in case of 'break' or 'continue'.
	 */
	void scope_push_loop(
	    const Gyoji::context::SourceReference & _source_ref,
	    size_t _loop_break_blockid,
	    size_t _loop_continue_blockid
	    );

	/**
	 * Pop out of current scope back to parent.
	 */
	void scope_pop();

	/**
	 * Adds a label to the current scope with the basic_block
	 * it knows about.  If it was seen first in a 'goto',
	 * we pull it out of that list and put it into the scope
	 * where it belongs.
	 */
	// Use this for 'label' to say we have a label and it's in the current
	// scope, but it's on the 'notfound' list, so move it over to
	// the real list because we've found it now.
	void label_define(
	    std::string label_name,
	    const Gyoji::context::SourceReference & _source_ref
	    );

	// Use this for 'label' to say we have a label, it's in the
	// current scope, but it wasn't forwar-declared on the
	// notfound list.
	void label_define(
	    std::string label_name,
	    size_t label_block_id,
	    const Gyoji::context::SourceReference & _source_ref
	    );
    
	// Use this for 'goto' to say we want a label, but we
	// don't yet know where it will live, so put it on the
	// notfound labels list.
	void label_declare(std::string label_name, size_t label_blockid);
	
	void add_goto(
	    std::string goto_label,
	    const Gyoji::context::SourceReference & _source_ref
	    );
		      

	const FunctionLabel * get_label(std::string name) const;

	/**
	 * Defines a variable in the current scope.
	 */
	bool add_variable(std::string variable_name, const Gyoji::mir::Type *mir_type, const Gyoji::context::SourceReference & source_ref);
	
	void dump() const;
	void dump_flat() const;

	// Evaluate the rules
	// to make sure all jumps are legal.
	bool check() const;
	
	/**
	 * Returns true if this or any ancestor is a 'loop'
	 * that can be 'break' or 'continue' out of.
	 */
	bool is_in_loop() const;

	size_t get_loop_break_blockid() const;
	size_t get_loop_continue_blockid() const;
	
	/**
	 * Searches the current scope upwards toward
	 * the parent and returns the type of variable
	 * declared if it is defined.  If it is not defined,
	 * returns nullptr to indicate that it's not defined
	 * anywhere in the current or parent scopes.
	 */
	const LocalVariable* get_variable(std::string variable_name) const;
	
	/**
	 * Returns the list of variables
	 * to unwind (in order) to get back
	 * to the root (like for a return).
	 */
	std::vector<std::string> get_variables_to_unwind_for_root() const;

	/**
	 * Returns the list of variables
	 * to unwind (in order) to get back
	 * to the root (like for a return).
	 */
	std::vector<std::string> get_variables_to_unwind_for_scope() const;
	
	std::vector<std::string> get_variables_to_unwind_for_break() const;

        /**
	 * Returns the list of variables
	 * to unwind (in order) to get back
	 * to a common ancestor of the label.
	 */
	std::vector<std::string> get_variables_to_unwind_for_label(std::string & label) const;

	const Scope *get_current() const;
	
    private:
	const Scope*find_common_ancestor(const Scope *goto_scope, const Scope *label_scope) const;
	
	bool check_scope(const Scope *s) const;
	void add_operation(Gyoji::owned<ScopeOperation> op);
	Gyoji::owned<Scope> root;
	Scope *current;
	const Gyoji::context::CompilerContext & compiler_context;
	
	// Labels that actually have a definition.
	std::map<std::string, Gyoji::owned<FunctionLabel>> labels;

	// Labels that have been referenced in a 'goto'
	// but not yet defined in a scope.
	std::map<std::string, Gyoji::owned<FunctionLabel>> notfound_labels;
	
    };
    
    
	// We want a 'minimal' mir to represent what's
	// going on here.
	// Scope {
	//     std::vector<ScopeOperation>
	//     Scope *parent;
	//     std::vector<Gyoji::owned<Scope>> children;
	// };
	//
	// This tells us what's going on in each scope,
	// but skips things like operations that don't impact
	// branches, just deals with 'abstractions' of scopes.
	// An 'if' statement, for example, will simply introduce
	// two child scopes.
	// ScopeOperation {
	//    enum { VARDECL, LABEL, GOTO, CHILDREN}
        // };
	// Then once we've extracted the scopes,
	// we can 'dump' the whole thing and analyze whether
	// the jumps are legal according to the rules because
	// we have all the information we need.  We can look
	// at each goto and find the corresponding
	// labels and emit errors.  All of this should
	// happen during the first pass before the MIR
	// is generated because we'll need all this info
	// to generate a valid MIR.
	// Perhaps this can live in the 'type-resolver' although
	// it's not really related to types.
	//
	// With this, we can simply analyze for each goto,
	// whether there is a corresponding label defined somewhere
	// and check that it's legal in terms of scope
	// rules.  If it's not defined anywhere, that's an error.
	// If it's defined and skips an initialization, that's also
	// an error.  We also have enough information in the goto
	// itself to figure out what to unwind to get to the target.

	// At the top-level, we'll want a map of label to scope
	// so that when we do our analysis, we can find the label
	// corresponding to a goto.
};
