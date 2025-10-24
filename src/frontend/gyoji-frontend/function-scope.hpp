/* Copyright 2025 Jonathan S. Arney
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      https://github.com/jarney/gyoji/blob/master/LICENSE
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#pragma once
#include <gyoji-frontend.hpp>
#include <gyoji-mir.hpp>

namespace Gyoji::frontend::lowering {

    class Scope;
    class ScopeOperation;
    class ScopeTracker;
    class LocalVariable;

    /**
     * @brief Location inside a specific basic block.
     * @details
     * This class represents a specific location inside a basic block
     * that we use to reference a location where we can insert
     * operations.  Note that this is mainly used for inserting
     * 'undeclare' and destructor calls when a 'goto' statement
     * is executed so that we can call the appropriate
     * destructors and such when a goto would change the scope
     * of what's declared.
     */
    class FunctionPoint {
    public:
	FunctionPoint(size_t _basic_block_id, size_t _location);
	~FunctionPoint();
	size_t get_basic_block_id() const;
	size_t get_location() const;
    private:
	size_t basic_block_id;
	size_t location;
    };
    
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
	    Gyoji::owned<FunctionPoint> _goto_point,
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
	const FunctionPoint & get_goto_point() const;
	
	const std::string & get_variable_name() const;
	const Gyoji::mir::Type *get_variable_type() const;
	const Scope *get_child() const;

	const Gyoji::context::SourceReference & get_source_ref() const;

	
    private:
	ScopeOperation(
	    ScopeOperationType _type,
	    const Gyoji::context::SourceReference & _source_ref
	    );
	ScopeOperationType type;

	const Gyoji::context::SourceReference & source_ref;

	const Gyoji::mir::Type *variable_type;
	std::string variable_name;
	
	std::string label_name;
	
	std::string goto_label;
	Gyoji::owned<FunctionPoint> goto_point;
	
	Gyoji::owned<Scope> child;
    };

    class LocalVariable {
    public:
	LocalVariable(
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
	const Gyoji::mir::Type *get_type() const;
	const Gyoji::context::SourceReference & get_source_ref() const;
    private:
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
	Scope(bool _is_unsafe);
	/**
	 * This is a scope that is associated with some
	 * loop like a 'for' or 'while'.  The block ID for
	 * where to jump to in case of break or continue
	 * are provided so that they can be known if they are needed.
	 */
	Scope(
	    bool _is_unsafe,
	    bool _is_loop,
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

	/**
	 * If the given variable is defined in this scope, return
	 * the type of it.  If no such variable is defined, returns
	 * nullptr.
	 */
	const LocalVariable *get_variable(std::string name) const;

	bool is_loop() const;

	/**
	 * Returns true if this scope is declared
	 * as unsafe.  Does not check ancestors, only
	 * the current scope.  If you want to know if
	 * an ancestor is unsafe, use the scope tracker
	 * instead.
	 */
	bool is_unsafe() const;
	
	size_t get_loop_break_blockid() const;
	
	size_t get_loop_continue_blockid() const;

	void add_variable(std::string name, const Gyoji::mir::Type *mir_type, const Gyoji::context::SourceReference & source_ref);
    
	const std::map<std::string, Gyoji::owned<LocalVariable>> & get_variables() const;

	const std::vector<std::string> & get_variables_in_declaration_order() const;

	Scope *get_parent() const;
	
	void set_parent(Scope *_parent);
	
    private:
	friend ScopeTracker;
	Scope *parent;

	// If this scope is a loop,
	// what is the block id to jump
	// to for a 'break' or 'continue' operation?
	bool scope_is_loop;
	bool scope_is_unsafe;
	size_t loop_break_blockid;
	size_t loop_continue_blockid;
	
	std::vector<Gyoji::owned<ScopeOperation>> operations;
	std::map<std::string, Gyoji::owned<LocalVariable>> variables;
	std::vector<std::string> variables_in_declaration_order;
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
	    size_t _block_id
	    );
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
        ~FunctionLabel();
	size_t get_block() const;
	bool is_resolved() const;
	void resolve(const Gyoji::context::SourceReference & _src_ref);
	const Gyoji::context::SourceReference & get_source_ref() const;
    private:
	bool resolved;
	size_t block_id;
	const Gyoji::context::SourceReference * src_ref;
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
	ScopeTracker(
	    bool _root_is_unsafe,
	    const Gyoji::context::CompilerContext & _compiler_context
	    );
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	~ScopeTracker();

	/**
	 * Enter a new (un-named) scope.  This will add a
	 * new scope with the 'unsafe' modifier if it is
	 * declared as unsafe.
	 */
	void scope_push(
	    bool is_unsafe,
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
	    Gyoji::owned<FunctionPoint> function_point,
	    const Gyoji::context::SourceReference & _source_ref
	    );
		      
	const FunctionLabel * get_label(std::string name) const;

	/**
	 * Defines a variable in the current scope.
	 */
	bool add_variable(std::string variable_name, const Gyoji::mir::Type *mir_type, const Gyoji::context::SourceReference & source_ref);
	
	void dump() const;

	/**
	 * @brief
	 *
	 * @details
	 * This method could definitely use a better name.  It does a few
	 * things and it's important that they be done together
	 * even though they are only somewhat related.
	 *
	 * The main thing the method does is evaluate the scope
	 * rules for 'goto' statements and make sure that
	 * the result of the 'goto' would not skip variable
	 * declarations or initializations at the target
	 * label.
	 *
	 * As a side-effect of the calculations, it also
	 * determines which variables would go out of scope
	 * as a result of the 'goto' statement and thus
	 * which ones need to be un-declared and the destructors
	 * called.
	 *
	 * The parameter given 'goto_fixups' is an 'out'
	 * parameter that fills the vector with all of the
	 * 'goto' statements along with the vector of variables that
	 * need to be un-declared before the Jump statment of
	 * the goto.  The vector of variables is in the
	 * order in which they should be destructed, so order
	 * is important when processing the fixups to the MIR.
	 */
	bool check(
	    std::vector<std::pair<const ScopeOperation*, std::vector<const ScopeOperation*>>> & goto_fixups
	    ) const;
	
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

	/**
	 * This returns true if this scope or one of its ancestors
	 * is marked as unsafe.
	 */
	bool is_unsafe() const;
    private:
	void add_flat_op(const ScopeOperation *op);

	void add_operation(Gyoji::owned<ScopeOperation> op);

	Gyoji::owned<Scope> root;
	Scope *current;
	const Gyoji::context::CompilerContext & compiler_context;
	
	// Labels that actually have a definition.
	std::map<std::string, Gyoji::owned<FunctionLabel>> labels;

	// Labels that have been referenced in a 'goto'
	// but not yet defined in a scope.
	std::map<std::string, Gyoji::owned<FunctionLabel>> labels_forward_declared;

	std::vector<size_t> tracker_prior_point;
	std::map<size_t, size_t> tracker_backward_edges;
	std::vector<const ScopeOperation*> tracker_flat;
	
	std::map<std::string, size_t> tracker_label_locations;
	std::map<size_t, std::string> tracker_goto_labels_at;
    };
};
