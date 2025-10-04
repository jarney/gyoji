#pragma once
#include <jlang-mir.hpp>
#include <jlang-context.hpp>

namespace JLang::analysis {

    class AnalysisPass {
    public:
	AnalysisPass(JLang::context::CompilerContext & _compiler_context);
	~AnalysisPass();
	virtual void check(const JLang::mir::MIR & mir) const = 0;
	
	JLang::context::CompilerContext & get_compiler_context() const;
    private:
	JLang::context::CompilerContext & compiler_context;
    };
    
    class AnalysisPassTypeResolution : public AnalysisPass {
    public:
	AnalysisPassTypeResolution(JLang::context::CompilerContext & _compiler_context);
	~AnalysisPassTypeResolution();
	
	virtual void check(const JLang::mir::MIR & mir) const;
    private:
	void check_type(const JLang::mir::Type & type) const;
    };

    /**
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
	AnalysisPassUnreachable(JLang::context::CompilerContext & _compiler_context);
	~AnalysisPassUnreachable();
	
	virtual void check(const JLang::mir::MIR & mir) const;
    private:
	void check(const JLang::mir::Function & function) const;
    };

    class AnalysisPassBorrowChecker : public AnalysisPass {
    public:
	AnalysisPassBorrowChecker(JLang::context::CompilerContext & _compiler_context);
	~AnalysisPassBorrowChecker();
	virtual void check(const JLang::mir::MIR & mir) const;
    private:
	void check(const JLang::mir::Function & function) const;
    };
    
};

