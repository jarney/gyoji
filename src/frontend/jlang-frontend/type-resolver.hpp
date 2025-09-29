#pragma once

#include <jlang-mir.hpp>
#include <jlang-frontend.hpp>

namespace JLang::frontend {
    
    //! Type Resolver
    /**
     * This function reads the result of a parse and produces
     * the set of types defined or referenced by it.  This is
     * essentially the process of 'lowering' for types where
     * each user-defined type is decomposed to a set of
     * canonical primitive types specified by the language
     * so that in the end, code-generation can operate only
     * on those primitive types at the machine level.
     * At the end of this, every type should be defined
     * in terms of primitive types (u32, f32,...), composite types (flattened)
     * and pointer types (represented as a u64).
     */
    class TypeResolver {
    public:
	TypeResolver(
	    JLang::context::CompilerContext & _compiler_context,
	    const JLang::frontend::tree::TranslationUnit & _translation_unit,
	    JLang::mir::MIR & _mir);
	~TypeResolver();
	void resolve();
	
	// Note, this is non-const because
	// it may create a type on the fly,
	// for example, it may create a 'Type*' for
	// a usage of an already existing 'Type'
	// even if it was never referenced in another
	// type, it may be referenced in a function.
	const JLang::mir::Type * extract_from_type_specifier(const JLang::frontend::tree::TypeSpecifier & type_specifier);
    private:
	JLang::mir::MIR & mir;
	JLang::context::CompilerContext & compiler_context;
	const JLang::frontend::tree::TranslationUnit & translation_unit;
	
	void extract_from_class_declaration(const JLang::frontend::tree::ClassDeclaration & declaration);
	void extract_from_class_members(JLang::mir::Type & type, const JLang::frontend::tree::ClassDefinition & definition);
	void extract_from_class_definition(const JLang::frontend::tree::ClassDefinition & definition);
	void extract_from_enum(const JLang::frontend::tree::EnumDefinition & enum_definition);
	void extract_from_namespace(const JLang::frontend::tree::FileStatementNamespace & namespace_declaration);
	void extract_types(const std::vector<JLang::owned<JLang::frontend::tree::FileStatement>> & statements);
	
	void extract_from_function_specifications(
	    const JLang::frontend::tree::Terminal & name,
	    const JLang::frontend::tree::TypeSpecifier & type_specifier,
	    const JLang::frontend::tree::FunctionDefinitionArgList & arguments
	    );
	
	void extract_from_function_definition(const JLang::frontend::tree::FileStatementFunctionDefinition & function_definition);
	void extract_from_function_declaration(const JLang::frontend::tree::FileStatementFunctionDeclaration & function_declaration);
	
	JLang::mir::Type *get_or_create(
	    std::string pointer_name,
	    JLang::mir::Type::TypeType type_type,
	    bool complete,
	    const JLang::context::SourceReference & source_ref
	    );
	
	// Move to analysis
	void check_complete_type(JLang::mir::Type *type) const;
	
    };
};
