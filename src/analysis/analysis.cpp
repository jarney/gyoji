#include <jlang-analysis.hpp>
#include <stdio.h>

using namespace JLang::mir;
using namespace JLang::context;
using namespace JLang::analysis;

AnalysisPass::AnalysisPass(CompilerContext & _compiler_context)
    : compiler_context(_compiler_context)
{}

AnalysisPass::~AnalysisPass()
{}

JLang::context::CompilerContext &
AnalysisPass::get_compiler_context() const
{ return compiler_context; }

