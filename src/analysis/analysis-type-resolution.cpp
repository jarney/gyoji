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
#include <gyoji-analysis.hpp>
#include <stdio.h>

using namespace Gyoji::mir;
using namespace Gyoji::context;
using namespace Gyoji::analysis;

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
		std::unique_ptr<Gyoji::context::Error> error = std::make_unique<Gyoji::context::Error>("Class contains incomplete type");
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


