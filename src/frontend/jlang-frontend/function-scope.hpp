#pragma once
#include <jlang-frontend.hpp>
#include <jlang-mir.hpp>

namespace JLang::frontend {

    class Scope;
    class ScopeOperation;
    class ScopeTracker;
    class LocalVariable;

    class ScopeOperation {
    public:

	typedef enum {
	    VAR_DECL,
	    LABEL_DEFINITION,
	    GOTO_DEFINITION,
	    CHILD_SCOPE
	} ScopeOperationType;
	~ScopeOperation();

	static JLang::owned<ScopeOperation> create_variable(std::string variable_name, const JLang::mir::Type *variable_type);
	static JLang::owned<ScopeOperation> create_label(std::string label_name, size_t label_block_id);
	static JLang::owned<ScopeOperation> create_goto(std::string goto_label);
	static JLang::owned<ScopeOperation> create_child(JLang::owned<Scope> child);
	
	void dump(int indent) const;
	const ScopeOperationType & get_type() const;
	const std::string & get_label_name() const;
	const std::string & get_goto_label() const;
	const std::string & get_variable_name() const;
	const Scope *get_child() const;
    private:
	ScopeOperation();
	ScopeOperationType type;

	LocalVariable *local_variable;
	const JLang::mir::Type *variable_type;
	std::string variable_name;
	
	std::string label_name;
	std::string label_blockid;
	
	std::string goto_label;
	JLang::owned<Scope> child;
    };

    class LocalVariable {
    public:
	LocalVariable(
	    std::string _name,
	    const JLang::mir::Type *_type,
	    const JLang::context::SourceReference & _source_ref
	    );
	~LocalVariable();
	const std::string & get_name() const;
	const JLang::mir::Type *get_type() const;
	const JLang::context::SourceReference & get_source_ref() const;
    private:
	std::string name;
	const JLang::mir::Type *type;
	const JLang::context::SourceReference & source_ref;
    };
    
    class Scope {
    public:
	Scope();
	Scope(bool _is_loop,
	      size_t _loop_break_blockid,
	      size_t _loop_continue_blockid
	    );
	~Scope();
	void add_operation(JLang::owned<ScopeOperation> op);
	void dump(int indent) const;
	bool skips_initialization(std::string label) const;

	bool is_ancestor(const Scope *other) const;

	/**
	 * Adds a variable to the current scope.  It is assumed
	 * that the caller has previously verified that
	 * the variable is not already defined in scope using the
	 * 'get_variable' to look for it.
	 */
	void add_variable(std::string name, const JLang::mir::Type *type, const JLang::context::SourceReference & source_ref);
	
	/**
	 * If the given variable is defined in this scope, return
	 * the type of it.  If no such variable is defined, returns
	 * nullptr.
	 */
	const LocalVariable *get_variable(std::string name) const;

	bool is_loop() const;
	size_t get_loop_break_blockid() const;
	size_t get_loop_continue_blockid() const;

	const std::map<std::string, JLang::owned<LocalVariable>> & get_variables() const;
    private:
	friend ScopeTracker;
	Scope *parent;

	// If this scope is a loop,
	// what is the block id to jump
	// to for a 'break' or 'continue' operation?
	bool scope_is_loop;
	size_t loop_break_blockid;
	size_t loop_continue_blockid;
	
	std::vector<JLang::owned<ScopeOperation>> operations;
	std::map<std::string, JLang::owned<LocalVariable>> variables;
    };

    class ScopeTracker {
    public:
	ScopeTracker(const JLang::context::CompilerContext & _compiler_context);
	~ScopeTracker();

	/**
	 * Enter a new (un-named) scope.
	 */
	void scope_push();

	/**
	 * Pushes a 'loop' scope which knows where
	 * we would jump to in case of 'break' or 'continue'.
	 */
	void scope_push_loop(size_t _loop_break_blockid, size_t _loop_continue_blockid);

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
	void add_label(std::string label_name, size_t label_block_id);
	void add_goto(std::string goto_label);

	/**
	 * Defines a variable in the current scope.
	 */
	bool add_variable(std::string variable_name, const JLang::mir::Type *mir_type, const JLang::context::SourceReference & source_ref);
	
	void dump() const;

	// Evaluate the rules
	// to make sure all jumps are legal.
	bool check() const;
	
	Scope * label_find(std::string label_name) const;

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
	
    private:
	bool check_scope(const Scope *s) const;
	void add_operation(JLang::owned<ScopeOperation> op);
	JLang::owned<Scope> root;
	Scope *current;
	const JLang::context::CompilerContext & compiler_context;
	
	// Labels that actually have a definition.
	std::map<std::string, Scope*> labels;

	// Labels that have been referenced in a 'goto'
	// but not yet defined in a scope.
	std::map<std::string, size_t> notfound_labels;
	
    };
    
    
	// We want a 'minimal' mir to represent what's
	// going on here.
	// Scope {
	//     std::vector<ScopeOperation>
	//     Scope *parent;
	//     std::vector<JLang::owned<Scope>> children;
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
