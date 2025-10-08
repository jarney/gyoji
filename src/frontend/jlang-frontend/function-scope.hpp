#pragma once
#include <jlang-frontend.hpp>

namespace JLang::frontend {

    class Scope;
    class ScopeOperation;
    class ScopeTracker;

    class ScopeOperation {
    public:

	typedef enum {
	    VAR_DECL,
	    LABEL_DEFINITION,
	    GOTO_DEFINITION,
	    CHILD_SCOPE
	} ScopeOperationType;
	~ScopeOperation();

	static JLang::owned<ScopeOperation> createVariable(std::string variable_name);
	static JLang::owned<ScopeOperation> createLabel(std::string label_name);
	static JLang::owned<ScopeOperation> createGoto(std::string goto_label);
	static JLang::owned<ScopeOperation> createChild(JLang::owned<Scope> child);
	
	void dump(int indent) const;
	const ScopeOperationType & get_type() const;
	const std::string & get_label_name() const;
	const std::string & get_goto_label() const;
	const std::string & get_variable_name() const;
	const Scope *get_child() const;
    private:
	ScopeOperation();
	ScopeOperationType type;
	std::string variable_name;
	std::string label_name;
	std::string goto_label;
	JLang::owned<Scope> child;
    };

    class Scope {
    public:
	Scope();
	~Scope();
	void add_operation(JLang::owned<ScopeOperation> op);
	void dump(int indent) const;
	bool skips_initialization(std::string label) const;

	bool is_ancestor(const Scope *other) const;
    private:
	friend ScopeTracker;
	Scope *parent;
	std::vector<JLang::owned<ScopeOperation>> operations;
    };

    class ScopeTracker {
    public:
	ScopeTracker();
	~ScopeTracker();
	void scope_push();
	void scope_pop();
	void add_label(std::string label_name);
	void add_goto(std::string goto_label);
	void add_variable(std::string variable_name);
	void dump() const;

	// Evaluate the rules
	// to make sure all jumps are legal.
	bool check() const;
	bool check_scope(const Scope *s) const;
	
	Scope * label_find(std::string label_name) const;
	
    private:
	void add_operation(JLang::owned<ScopeOperation> op);
	JLang::owned<Scope> root;
	Scope *current;
	std::map<std::string, Scope*> labels;
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
