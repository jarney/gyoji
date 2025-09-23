#ifndef _JLANG_INTERNAL
#error "This header is intended to be used internally as a part of the JLang front-end.  Please include frontend.hpp instead."
#endif
#pragma once

namespace JLang::frontend::yacc {
    class LexContext {
    public:
	LexContext(JLang::frontend::namespaces::NamespaceContext &_namespace_context,
		   JLang::context::CompilerContext & _compiler_context,
		   JLang::misc::InputSource &_input_source);
	~LexContext();
	JLang::frontend::namespaces::NamespaceContext& namespace_context;
	JLang::misc::InputSource & input_source;
	JLang::context::CompilerContext & compiler_context;
	size_t line;
	size_t column;
    };
};
