#ifndef _GYOJI_INTERNAL
#error "This header is intended to be used internally as a part of the Gyoji front-end.  Please include frontend.hpp instead."
#endif
#pragma once

namespace Gyoji::frontend::yacc {
    class LexContext {
    public:
	LexContext(Gyoji::frontend::namespaces::NamespaceContext &_namespace_context,
		   Gyoji::context::CompilerContext & _compiler_context,
		   Gyoji::misc::InputSource &_input_source);
	~LexContext();
	Gyoji::frontend::namespaces::NamespaceContext& namespace_context;
	Gyoji::misc::InputSource & input_source;
	Gyoji::context::CompilerContext & compiler_context;
	size_t line;
	size_t column;
    };
};
