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

#include <gyoji-frontend.hpp>

namespace Gyoji::cmdline {
    using namespace Gyoji::frontend::ast;
    
    /**
     * This is the identity transformation that should always
     * format the code exactly as it appeared in the input,
     * assuming that the input is a valid syntax tree.
     */
    class JFormatIdentity {
    public:
	JFormatIdentity();
	~JFormatIdentity();
	int process(const SyntaxNode & file);
    };
};
