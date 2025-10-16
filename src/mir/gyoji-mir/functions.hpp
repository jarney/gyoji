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

#include <gyoji-misc/pointers.hpp>
#include <gyoji-mir/types.hpp>
#include <gyoji-mir/operations.hpp>

#include <string>
#include <map>
#include <vector>

namespace Gyoji::mir {
    class Function;
    class SimpleStatement;
    class BasicBlock;

    /**
     * @brief
     * Container for functions
     *
     * @details
     * This class is a container for the functions defined
     * in a translation unit.
     */
    class Functions {
    public:
	/**
	 * @brief Construct an empty function table.
	 *
	 * @details
	 * This constructs an empty function table.
	 */
	Functions();

	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	~Functions();

	/**
	 * @brief Add a function to the MIR.
	 *
	 * @details
	 * Adds a function to the MIR representation.  This
	 * function takes ownership of the provided
	 * function, so you must std::move it when calling.
	 */
	void add_function(Gyoji::owned<Function> _function);

	/**
	 * @brief Returns the list of functions defined.
	 *
	 * @details
	 * This method returns the list of functions
	 * defined in this function table.  The list is
	 * returned as an immutable reference to owned
	 * pointers.  The caller may not aquire ownership
	 * through this method, but may get access to
	 * the functions by de-referencing the pointer.
	 * The pointer is guaranteed to be valid because
	 * this container owns them and they cannot
	 * be removed once created.  The returned reference
	 * must not live longer than this container.
	 */
	const std::vector<Gyoji::owned<Function>> & get_functions() const;

	/**
	 * @brief Dump the function table to a file handle.
	 *
	 * @details
	 * This method dumps the "Functions" portion of the MIR
	 * to the given file handle.
	 */
	void dump(FILE *out) const;
	
    private:
	std::vector<Gyoji::owned<Function>> functions;
    };
    
    /**
     * @brief Basic block of a function.
     *
     * @details
     * A basic block is an ordered list of operations
     * that contain no flow-control except at the end
     * which may return from the function, or
     * transfer control to another basic block.
     *
     * The operations in each basic block
     * are primitive 'opcodes' that make
     * up the instruction-set of the MIR
     * representation.
     *
     * Basic blocks are "flat" in the sense
     * that they do not recursively contain
     * other basic blocks.  The process of
     * 'lowering' in the front-end is responsible
     * for "flattening" the program into a set
     * of basic blocks containing primitive opcodes.
     */
    class BasicBlock {
    public:
	/**
	 * @brief Create a new basic block.
	 *
	 * @details
	 * This constructs a new basic block.  This is
	 * intended to be called only through the Function
	 * object with "add_block" (see Function).  The
	 * newly created block is empty, but empty basic blocks
	 * are not permitted, so the caller is assumed to
	 * add operations to the block before
	 * MIR construction is complete.
	 */
	BasicBlock();
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	~BasicBlock();

	/**
	 * @brief Add an Operation to the block
	 *
	 * @details
	 * This adds a new operation to this block.  The operation
	 * is passed as an owned pointer and the caller
	 * gives up ownership to the block, so it must
	 * std::move it into the call.
	 */
	void add_operation(Gyoji::owned<Operation> operation);

	/**
	 * @brief Return the number of operations in this block.
	 *
	 * @details
	 * Returns the number of operations contained
	 * in this basic block.  This is used to form
	 * a 'FunctionPoint' where we can insert
	 * operations at this point.  This is used
	 * mainly by the 'goto' operation which needs
	 * to insert instructions that won't be
	 * determined until after the entire function
	 * has been processed, so we must 'fix up'
	 * the goto jump statement in order
	 * to 'unwind' variables declared in this
	 * scope that won't be available in the
	 * scope we're jumping to.
	 */
	size_t size() const;

	/**
	 * @brief Insert operations at a specific point.
	 *
	 * @details
	 * This inserts the given operation at the specific
	 * position in this block instead of appending
	 * it to the end.  Note that any other insert points
	 * will need to have their position shifted down if
	 * other operations are required to be inserted.
	 *
	 * This is mainly used to 'fix up' the 'goto' statement
	 * which needs to insert the 'undeclare' and possibly
	 * destructor calls before the goto operation in order
	 * to ensure that any variables that should be going out
	 * of scope are removed correctly.
	 *
	 * Fortunately in the case of 'goto', this will always
	 * terminate the basic block, so we don't need to worry
	 * about shifting insert points because the next goto
	 * statement will always be in a different basic block.
	 */
	void insert_operation(size_t position, Gyoji::owned<Operation> operation);
	
	/**
	 * @brief Access to list of Operation of basic block
	 *
	 * @details
	 * This method returns the list of operations
	 * defined in this basic block.  The list is
	 * returned as an immutable reference to owned
	 * pointers.  The caller may not aquire ownership
	 * through this method, but may get access to
	 * the functions by de-referencing the pointer.
	 * The pointer is guaranteed to be valid because
	 * this basic block owns them and they cannot
	 * be removed once created.  The returned reference
	 * must not live longer than this container.
	 */
	const std::vector<Gyoji::owned<Operation>> & get_operations() const;

	/**
	 * @brief Dump block to file handle.
	 *
	 * @details
	 * This method dumps the list of Operation
	 * for this basic block to the given file handle.
	 */
	void dump(FILE *out) const;

	/**
	 * This method returns true if the block
	 * already returns a 'terminating' instruction
	 * such as a branch, goto, or return.
	 */
	bool contains_terminator() const;
    private:
	std::vector<Gyoji::owned<Operation>> operations;
    };

    /**
     * @brief A single named argument to a function
     *
     * @details
     * Each function may take zero or more arguments
     * which are represented as a name and a Type.
     * The name represents the local variable that it
     * will be accessible as in the function and the
     * Type is the data-type it is interpreted as.
     */
    class FunctionArgument {
    public:
	/**
	 * @brief Create new argument from a name and a type
	 *
	 * @details
	 * This creates a new argument from a name and a type.
	 * The name is the name of the variable that is
	 * referenced in the function by load and store
	 * opcodes.  The type is a pointer to a type
	 * defined in the Types table.  It must live at least
	 * as long as the Function itself, so it must be
	 * a part of the MIR container in order to guarantee
	 * that it is always valid in this scope.
	 */
	FunctionArgument(
	    std::string & _name,
	    const Type *_type,
	    const Gyoji::context::SourceReference & _name_source_ref,
	    const Gyoji::context::SourceReference & _type_source_ref
	    );
	/**
	 * @brief copy constructor
	 *
	 * @details
	 * The copy constructor is provided so that this
	 * object may be used inside std::vector and copied
	 * when needed to manipulate the list.
	 */
	FunctionArgument(const FunctionArgument & _other);
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	~FunctionArgument();

	/**
	 * @brief Name of the argument
	 *
	 * @details
	 * Returns an immutable reference to the
	 * name of the argument.
	 */
	const std::string & get_name() const;
	
	/**
	 * @brief Type of the argument
	 *
	 * @details
	 * Returns a pointer to the immutable
	 * type of the argument.
	 */
	const Type * get_type() const;

	const Gyoji::context::SourceReference & get_type_source_ref() const;
	const Gyoji::context::SourceReference & get_name_source_ref() const;
	
    private:
	std::string name;
	const Type * type;
	const Gyoji::context::SourceReference & name_source_ref;
	const Gyoji::context::SourceReference & type_source_ref;
    };

    /**
     * @brief Function inside a translation unit.
     *
     * @details
     * A function represents an actual defined function (not just
     * a prototype).  It is defined by its name, return type,
     * and list of arguments.  Inside each function is a series
     * of basic blocks containing a set of operations.  Each of these
     * operations is an 'opcode' in the MIR virtual machine.
     * That machine operates on the tempoary variables (i.e. registers)
     * of the virtual machine.
     */
    class Function {
    public:
	/**
	 * @brief Creates a new function.
	 *
	 * @details
	 * A new function is created with the given name,
	 * return-type, and arguments.  The source-reference
	 * is the location of the beginning of the function
	 * definition.
	 *
	 * The name of the function is assumed to be already 'mangled'
	 * and at this point, it should be globally unique.
	 */
	Function(
	    std::string _name,
	    const Type *_return_type,
	    const std::vector<FunctionArgument> & _arguments,
	    const Gyoji::context::SourceReference & _source_ref
	    );
	~Function();

	/**
	 * @brief Name of the function
	 * @details
	 * Returns the name of the function.  The
	 * name should be globally unique already when
	 * it gets all the way down to the MIR, so any
	 * desired name-mangling should be done
	 * when creating the function.  The back-end
	 * code-generators may assume that this is
	 * globally unique and already has the namespace
	 * and function arguments baked into the name.
	 */
	const std::string & get_name() const;

	/**
	 * @brief Return type of the function.
	 *
	 * @details
	 * This is the type of value that this function is declared
	 * to return.
	 */
	const Type *get_return_type() const;
	
	/**
	 * @brief Arguments to the function
	 *
	 * @details
	 * This returns a list of the arguments to the function
	 * which are each a pair of name and type for the argument.
	 */
	const std::vector<FunctionArgument> & get_arguments() const;

	/**
	 * @brief Get an immutable basic block from the function by ID.
	 *
	 * @details
	 * This returns the basic block with the given ID.  Note
	 * that it is assumed that the blockid exists, and this
	 * may dump core if the blockid does not exist.
	 */
	const BasicBlock & get_basic_block(size_t blockid) const;
	/**
	 * @brief Get a mutable block by ID.
	 *
	 * @details
	 * Returns a mutable basic block from
	 * the function by its ID.  Note that
	 * it is assumed that the blockid exists, and this may
	 * dump core if the blockid does not exist.
	 */
	BasicBlock & get_basic_block(size_t blockid);

	/**
	 * @brief Creates a new basic block and returns the ID.
	 *
	 * @details
	 * Adds a new basic block to the function and returns
	 * the ID of that new block.
	 */
	size_t add_block();

	/**
	 * @brief Get the blocks of the function.
	 *
	 * @details
	 * This returns a map of all of the defined basic blocks
	 * and their associated IDs as a pair.  This list is not
	 * ordered, but each of the blocks "naturally" forms a
	 * control-flow graph because the end of each block is
	 * a flow-control decision (branch/goto) or a flow-terminator (return)
	 * which ends the control of that block, so the
	 * execution order can be derived by the code-generator
	 * based on these control-flow decisions.
	 */
	const std::map<size_t, Gyoji::owned<BasicBlock>> & get_blocks() const;

	/**
	 * @brief Dump a function to the given file handle for debugging.
	 *
	 * @details
	 * This dumps a function, all of its basic blocks,
	 * and its operations to the file handle given.
	 * This is mainly for debugging purposes.
	 */
	void dump(FILE *out) const;

	/**
	 * @brief Reference to the source location where the
	 *        function is defined.
	 *
	 * @details
	 * This returns a reference to where this function was
	 * defined.  Since functions tend to be large, this reference
	 * points to where the function name is located in the source.
	 */
	const Gyoji::context::SourceReference & get_source_ref() const;

	/**
	 * @brief Temporary values used by opcodes
	 *
	 * @details
	 * Temporary values are the 'registers' of the
	 * abstract MIR virtual machine.  They are typically
	 * used exactly once in order to be easy to reason about
	 * and ensure that the flow of the data can be reasoned
	 * about cleanly.  Each temporary value will typically
	 * have a lifetime of only a few opcodes, being produced by
	 * one opcode and consumed by another one fairly soon
	 * afterward.  Each temporary value has an ID and a type.
	 * The ID is a size_t and typically only
	 */
	const Type *tmpvar_get(size_t tmpvar_id) const;
	/**
	 * @brief Define a new temporary variable with the given type.
	 *
	 * @details
	 * Defines a new 'temporary' variable for use
	 * by opcodes as a 'register' for the MIR
	 * virtual machine.
	 */
	size_t tmpvar_define(const Type *tmpvar_type);

	/**
	 * @brief Duplicate a temporary variable/reigster.
	 *
	 * @details
	 * Duplicate a temporary variable with
	 * exactly the same type as another one.
	 * This is mainly a convenience for
	 * places where there isn't direct access
	 * to the type of a temporary variable, but
	 * we just want to duplicate the type from
	 * another one.  Note that this does NOT
	 * duplicate the value inside a variable,
	 * but only the variable itself.
	 */
	size_t tmpvar_duplicate(size_t tmpvar_id);
	
    private:
	const std::string name;
	const Type *return_type;
	std::vector<FunctionArgument> arguments;
	
	const Gyoji::context::SourceReference & source_ref;
	
	// Holds the max blockid
	// as we build them.
	size_t blockid;
	std::map<size_t, Gyoji::owned<BasicBlock>> blocks;
	std::vector<const Type*> tmpvars;
    };
};
