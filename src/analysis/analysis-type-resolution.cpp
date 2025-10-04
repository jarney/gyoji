#include <jlang-analysis.hpp>
#include <stdio.h>

using namespace JLang::mir;
using namespace JLang::context;
using namespace JLang::analysis;

AnalysisPassTypeResolution::AnalysisPassTypeResolution(CompilerContext & _compiler_context)
    : AnalysisPass(_compiler_context, "type resolution")
{}
AnalysisPassTypeResolution::~AnalysisPassTypeResolution()
{}

void
AnalysisPassTypeResolution::check(const MIR & mir) const
{
    // TODO: Go back through all of the types and make sure that every type
    // that is used in a structure 'inline' is actually complete.
    // If not, produce a compile error to that effect.
    // This needs to be recursive!
    for (const auto & type_it : mir.get_types().get_types()) {
	const Type & type = *type_it.second;
	check_type(type);
    }
}

void
AnalysisPassTypeResolution::check_type(const Type & type) const
{
    if (type.get_type() == Type::TYPE_COMPOSITE) {
	for (const auto & member : type.get_members()) {
	    if (!member.get_type()->is_complete()) {
		std::unique_ptr<JLang::context::Error> error = std::make_unique<JLang::context::Error>("Class contains incomplete type");
		error->add_message(member.get_source_ref(),
				   std::string("Incomplete type in member ") + member.get_name() + std::string(" of type ") + type.get_name());
		error->add_message(member.get_type()->get_declared_source_ref(),
				   "Forward declaration is here.");
		get_compiler_context().get_errors().add_error(std::move(error));
	    }
	    check_type(*member.get_type());
	}
    }
}


