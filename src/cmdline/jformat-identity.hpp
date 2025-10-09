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
