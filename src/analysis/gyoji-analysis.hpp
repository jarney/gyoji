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
#include <gyoji-mir.hpp>
#include <gyoji-context.hpp>

/**
 * @brief Analysis pass performs checks to ensure semantic consistency.
 *
 * @details
 * The analysis passes each guarantee an aspect of semantics consistency.
 * The borrow checker is one of these analysis passes.
 *
 * Things that should be checked in the analysis passes:
 * * Initialization: Every variable is written (initialized) BEFORE it is read
 *    from.
 *
 * * Type Resolution: Ensures that each variable has a type that has been
 *   fully resolved (i.e. that there are no forward-declared classes that
 *   are being used before being fully declared).
 *
 * * Borrow Semantics: That access to variables through references always point
 *   to a valid location and have unique access to it at any point in time.
 *
 * * Pairing of const/dest: Every local variable constructor is paired
 *   with a destructor.  No path through the code allows a variable
 *   to be declared and constructed without a corresponding destructor
 *   and de-allocation.
 *
 * * Unreachable code: There is no code after the end of any basic block
 *   which would become unreachable because a branch has occurred before
 *   that code is executed.
 *
 * * Return semantics: Every path through the code returns a value that
 *   matches the return-value of the function (or void).
 *
 */
namespace Gyoji::analysis {

    /**
     * @brief Abstract interface to analysis passes.
     *
     * @details
     * An analysis pass is constructed with access to the compiler context
     * so that it can report errors.  The 'check' method performs
     * the specific analysis needed.  The 'check' method is a virtual
     * abstract method implemented by each concrete class that
     * inherits from it to perform the specific analysis needed by that
     * pass.
     */
    class AnalysisPass {
    public:
	AnalysisPass(Gyoji::context::CompilerContext & _compiler_context, std::string _name);
	virtual ~AnalysisPass();
	virtual void check(const Gyoji::mir::MIR & mir) const = 0;
	
	Gyoji::context::CompilerContext & get_compiler_context() const;
	const std::string & get_name() const;
    private:
	Gyoji::context::CompilerContext & compiler_context;
	std::string name;
    };

    /**
     * @brief Check that all types have been fully declared before use.
     * 
     * @details
     * This analysis ensures that all types have been fully declared
     * before they are used in code.  Essentially, this looks for
     * the existence of 'forward-declared' variables that are in use
     * by code which doesn't yet know the full definition/size of the
     * object.
     */
    class AnalysisPassTypeResolution : public AnalysisPass {
    public:
	AnalysisPassTypeResolution(Gyoji::context::CompilerContext & _compiler_context);
	virtual ~AnalysisPassTypeResolution();
	
	virtual void check(const Gyoji::mir::MIR & mir) const;
    private:
	void check_type(const Gyoji::mir::Type & type) const;
    };

    /**
     * @brief Checks for the existence of unreachable code.
     *
     * @details
     * This pass is intended to uncover basic blocks which
     * contain instructions after the terminating instruction
     * (most commonly after a jump, or return statement).
     *
     * These indicate that the code below those terminating
     * statements is systematically and definitely unreachable.
     * In this case, the program is invalid because it
     * contains such constructs.
     *
     * The other thing we look for here are basic blocks
     * which are disconnected from the graph, so they
     * cannot be reached in any way.
     */
    class AnalysisPassUnreachable : public AnalysisPass {
    public:
	AnalysisPassUnreachable(Gyoji::context::CompilerContext & _compiler_context);
	virtual ~AnalysisPassUnreachable();
	
	virtual void check(const Gyoji::mir::MIR & mir) const;
    private:
	void check(const Gyoji::mir::Function & function) const;
	void check_all_blocks_reachable(
	    const Gyoji::mir::Function & function
	    ) const;
    };

    /**
     * @brief Performs the borrow-checker algorithm
     *
     * @details
     * The borrow-checker is modelled after the 'polonius'
     * borrow checker from Rust.
     */
    class AnalysisPassBorrowChecker : public AnalysisPass {
    public:
	AnalysisPassBorrowChecker(Gyoji::context::CompilerContext & _compiler_context);
	virtual ~AnalysisPassBorrowChecker();
	virtual void check(const Gyoji::mir::MIR & mir) const;
    private:
	void check(const Gyoji::mir::Function & function) const;
    };

    /**
     * @brief Performs checks for return-value consistency
     *
     * @details
     * This analysis pass checks that every code path eventually
     * leads to a 'return' statement that ends the function.  It also
     * checks that all return statements return values that are
     * consistent with the return type of that function.
     */
    class AnalysisPassReturnValues : public AnalysisPass {
    public:
	AnalysisPassReturnValues(Gyoji::context::CompilerContext & _compiler_context);
	virtual ~AnalysisPassReturnValues();
	virtual void check(const Gyoji::mir::MIR & mir) const;
    private:
	void check(const Gyoji::mir::Function & function) const;
    };
    
    /**
     * @brief This pass ensures that every variable that has been declared is also paired with an un-declare.
     *
     * @details
     * This analysis pass checks to make sure that for every path through
     * the code, if a variable is declared, it is also un-declared and
     * also guarantees that each of the constructors called for local variables
     * is matched with a destructor, guaranteeing that it is safe to use
     * constructor/destructor pairs in a way that matches the lexical scopes.
     */
    class AnalysisPassVariableScopePairing : public AnalysisPass {
    public:
	AnalysisPassVariableScopePairing(Gyoji::context::CompilerContext & _compiler_context);
	virtual ~AnalysisPassVariableScopePairing();
	virtual void check(const Gyoji::mir::MIR & mir) const;
    private:
	void check(const Gyoji::mir::Function & function) const;
    };

    /**
     * @brief This pass ensures that all values are assigned before they are used.
     *
     * @details
     * In order to prove that a program has safe, deterministic behavior,
     * it is important that no value be used prior to assigning a definite
     * value to it.  We don't really require bounds on that value (i.e. it could come
     * from user input from a function).  Whatever it is, however, must be assigned
     * and not just some random leftover value from the stack or heap due to
     * uninitialized data.
     */
    class AnalysisPassUseBeforeAssignment : public AnalysisPass {
    public:
	AnalysisPassUseBeforeAssignment(Gyoji::context::CompilerContext & _compiler_context);
	virtual ~AnalysisPassUseBeforeAssignment();
	virtual void check(const Gyoji::mir::MIR & mir) const;
    private:
	void check(const Gyoji::mir::Function & function) const;
    };
    
};

