#include <gyoji-analysis.hpp>
#include <stdio.h>

using namespace Gyoji::mir;
using namespace Gyoji::context;
using namespace Gyoji::analysis;

AnalysisPass::AnalysisPass(CompilerContext & _compiler_context, std::string _name)
    : compiler_context(_compiler_context)
    , name(_name)
{}

AnalysisPass::~AnalysisPass()
{}

Gyoji::context::CompilerContext &
AnalysisPass::get_compiler_context() const
{ return compiler_context; }

const std::string &
AnalysisPass::get_name() const
{ return name; }
