#pragma once

#include <jlang-misc/pointers.hpp>
#include <jlang-mir/types.hpp>

#include <string>
#include <map>
#include <vector>

namespace JLang::mir {
    class Function;
    class SimpleStatement;
    class BasicBlock;
    
    class Functions {
    public:
	Functions();
	~Functions();

	/**
	 * Adds a function to the MIR representation.
	 */
	void add_function(JLang::owned<Function> _function);

	const Function * function_get(std::string _name) const;
	
	/**
	 * Read-only representation of functions.
	 */
	const std::vector<JLang::owned<Function>> & get_functions() const;
	
    private:
	std::vector<JLang::owned<Function>> functions;
    };
    
    // A simple statement is a
    // single un-wound expression to be
    // evaluated with one or two inputs
    // and one output.  This is effectively
    // a "three argument form" of an expression.
    // For example, a single binary operation
    // possibly using temporary variables as input
    // and temporary variables as output.
    // It might also be a branch with a compare
    // and a conditional jump.  Nesting is not allowed.
    // Instead, dependent statements appear first
    // and store their results into temporary
    // variables for the next statements later.
    class SimpleStatement {
    public:
	SimpleStatement(std::string _statement_desc);
	~SimpleStatement();
	const std::string & get_statement() const;
    private:
	std::string statement_desc;
    };
    
    class BasicBlock {
    public:
	BasicBlock();
	~BasicBlock();
	void add_statement(std::string statement);
	void dump() const;
    private:
	std::vector<JLang::owned<SimpleStatement>> statements;
    };
    
    class FunctionArgument {
    public:
       FunctionArgument(
           std::string & _name,
	   const Type *_type
           );
	FunctionArgument(const FunctionArgument & _other);
	~FunctionArgument();
       const std::string & get_name() const;
       const Type* get_type() const;
    private:
	std::string name;
	const Type *type;
    };
    /**
     * Local variables are named variables defined in the
     * source-code.  Each of them carries a name and a type
     * associated with them along with the source reference
     * where it was defined in the code.
     */
    class LocalVariable {
    public:
	LocalVariable(std::string _name, std::string _type, const JLang::context::SourceReference & _src_ref);
	LocalVariable(const LocalVariable & _other);
	~LocalVariable();
	std::string name;
	std::string type;
	const JLang::context::SourceReference & src_ref;
    };

    /**
     * These are the operands for three-address-code
     * operations performed inside basic blocks.
     */
    class TmpValue {
    public:
	TmpValue(const std::string & _type);
	TmpValue(const TmpValue & _other);
	~TmpValue();
	const std::string & get_type() const;
    private:
	std::string type;
    };
    
    class Function {
    public:
	Function(
	    std::string _name,
	    const Type *_return_type,
	    const std::vector<FunctionArgument> & _arguments,
	    const JLang::context::SourceReference & _source_ref
	    );
	~Function();

	const std::string & get_name() const;
	const Type *get_return_type() const;
	const std::vector<FunctionArgument> & get_arguments() const;
	
	const BasicBlock & get_basic_block(size_t blockid) const;
	BasicBlock & get_basic_block(size_t blockid);
	size_t add_block();
	void push_block(size_t blockid);
	
	const LocalVariable * get_local(std::string local_name);
	bool add_local(const LocalVariable & local);
	void remove_local(std::string local_name);

	void dump() const;
	
	const JLang::context::SourceReference & get_source_ref() const;

	/**
	 * Operations use these temporary variables
	 * as their operands and return-values.
	 * Each temporary value has a type.
	 */
	const TmpValue *tmpvar_get(size_t tmpvar_id);
	size_t tmpvar_define(std::string type_name);
	
    private:
	const std::string name;
	const Type *return_type;
	std::vector<FunctionArgument> arguments;
	
	const JLang::context::SourceReference & source_ref;
	
	// Holds the max blockid
	// as we build them.
	size_t blockid;
	std::map<size_t, JLang::owned<BasicBlock>> blocks;
	
	// outgoing edges of basic blocks. size_t -> size_t.
	// Each basic block (key of map)
	// has an edge that it connects to, forming a directed
	// graph.
	std::map<size_t, std::vector<size_t>> edges;
	std::vector<size_t> blocks_in_order;
	
	std::map<std::string, LocalVariable> in_scope_locals;

	std::vector<TmpValue> tmpvars;
    };
};
