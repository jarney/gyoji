#pragma once
#include <jlang-mir/types.hpp>
#include <jlang-context.hpp>

namespace JLang::analysis {

    class AnalysisPass {
    public:
	AnalysisPass(JLang::context::CompilerContext & _compiler_context);
	~AnalysisPass();
	virtual void check(const JLang::mir::Types & types) const = 0;
	
	JLang::context::CompilerContext & get_compiler_context() const;
    private:
	JLang::context::CompilerContext & compiler_context;
    };
    
    class AnalysisPassTypeResolution : public AnalysisPass {
    public:
	AnalysisPassTypeResolution(JLang::context::CompilerContext & _compiler_context);
	~AnalysisPassTypeResolution();
	
	virtual void check(const JLang::mir::Types & types) const;
    private:
	void check_type(const JLang::mir::Type *type) const;
    };
};

