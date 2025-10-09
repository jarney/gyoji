#pragma once
#include <gyoji-mir.hpp>
#include <gyoji-context.hpp>

namespace Gyoji::analysis {

    class AnalysisPass {
    public:
	AnalysisPass(Gyoji::context::CompilerContext & _compiler_context, std::string _name);
	~AnalysisPass();
	virtual void check(const Gyoji::mir::MIR & mir) const = 0;
	
	Gyoji::context::CompilerContext & get_compiler_context() const;
	const std::string & get_name() const;
    private:
	Gyoji::context::CompilerContext & compiler_context;
	std::string name;
    };
    
    class AnalysisPassTypeResolution : public AnalysisPass {
    public:
	AnalysisPassTypeResolution(Gyoji::context::CompilerContext & _compiler_context);
	~AnalysisPassTypeResolution();
	
	virtual void check(const Gyoji::mir::MIR & mir) const;
    private:
	void check_type(const Gyoji::mir::Type & type) const;
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
	AnalysisPassUnreachable(Gyoji::context::CompilerContext & _compiler_context);
	~AnalysisPassUnreachable();
	
	virtual void check(const Gyoji::mir::MIR & mir) const;
    private:
	void check(const Gyoji::mir::Function & function) const;
    };

    class AnalysisPassBorrowChecker : public AnalysisPass {
    public:
	AnalysisPassBorrowChecker(Gyoji::context::CompilerContext & _compiler_context);
	~AnalysisPassBorrowChecker();
	virtual void check(const Gyoji::mir::MIR & mir) const;
    private:
	void check(const Gyoji::mir::Function & function) const;
    };
    
};

