#include <jlang-analysis.hpp>
#include <stdio.h>

using namespace JLang::mir;
using namespace JLang::context;
using namespace JLang::analysis;

void do_analysis(Types &)
{
  fprintf(stderr, "Doing some analysis...\n");
}

AnalysisPass::AnalysisPass(CompilerContext & _compiler_context)
  : compiler_context(_compiler_context)
{}

AnalysisPass::~AnalysisPass()
{}

JLang::context::CompilerContext &
AnalysisPass::get_compiler_context() const
{ return compiler_context; }

AnalysisPassTypeResolution::AnalysisPassTypeResolution(CompilerContext &_compiler_context)
  : AnalysisPass(_compiler_context)
{}
AnalysisPassTypeResolution::~AnalysisPassTypeResolution()
{}

void
AnalysisPassTypeResolution::check(Types & types) const
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
AnalysisPassTypeResolution::check_type(Type *type) const
{
  if (type->get_type() == Type::TYPE_COMPOSITE) {
    for (const auto & member : type->get_members()) {
      if (!member.second->is_complete()) {
        fprintf(stderr, "Incomplete type %s\n", member.second->get_name().c_str());
        std::unique_ptr<JLang::context::Error> error = std::make_unique<JLang::context::Error>("Class contains incomplete type");

        JLang::context::SourceReference src_ref("asdf.h", 14, 19);
        error->add_message(src_ref,
                           std::string("Incomplete type in member ") + member.first + std::string(" of type ") + type->get_name() + std::string("\n"));
        get_compiler_context().get_errors().add_error(std::move(error));
      }
      check_type(member.second);
    }
  }
}


