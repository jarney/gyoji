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

AnalysisPassTypeResolution::AnalysisPassTypeResolution(CompilerContext & _compiler_context)
  : AnalysisPass(_compiler_context)
{}
AnalysisPassTypeResolution::~AnalysisPassTypeResolution()
{}

void
AnalysisPassTypeResolution::check(const Types & types) const
{
  // TODO: Go back through all of the types and make sure that every type
  // that is used in a structure 'inline' is actually complete.
  // If not, produce a compile error to that effect.
  // This needs to be recursive!
  for (const auto & type : types.type_map) {
    check_type(type.second.get());
  }
}

void
AnalysisPassTypeResolution::check_type(const Type *type) const
{
  if (type->get_type() == Type::TYPE_COMPOSITE) {
    for (const auto & member : type->get_members()) {
      if (!member.get_type()->is_complete()) {

        //auto error = std::make_unique<JLang::context::Error>("Incomplete Type: Use of type that has been forward declard, but a full definition of the type was not found.");
        // XXX Need to put declared source ref into Type.
        //error->add_message(member.second->get_declared_source_ref(), "Declared here");
        // XXX Need to put used source ref into member.
        //error->add_message(member, "Used here");
        //get_compiler_context()
        //.get_errors()
        //.add_error(std::move(error));
        
        std::unique_ptr<JLang::context::Error> error = std::make_unique<JLang::context::Error>("Class contains incomplete type");

        JLang::context::SourceReference src_ref("asdf.h", 14, 19);
        error->add_message(member.get_source_ref(),
                           std::string("Incomplete type in member ") + member.get_name() + std::string(" of type ") + type->get_name());
        error->add_message(member.get_type()->get_declared_source_ref(),
                           "Forward declaration is here.");
        get_compiler_context().get_errors().add_error(std::move(error));
      }
      check_type(member.get_type());
    }
  }
}


