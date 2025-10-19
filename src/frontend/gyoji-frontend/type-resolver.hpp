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

#include <gyoji-mir.hpp>
#include <gyoji-frontend.hpp>

namespace Gyoji::frontend::lowering {
    
    /**
     * @brief The TypeResolver is the lowering process for types.
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
	    Gyoji::context::CompilerContext & _compiler_context,
	    const Gyoji::frontend::tree::TranslationUnit & _translation_unit,
	    Gyoji::mir::MIR & _mir);
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	~TypeResolver();

	/**
	 * This iterates the translation unit and creates a suitable MIR type
	 * from the strongly-typed syntax tree.  The result of this is that
	 * the MIR representation will now have type definitions suitable
	 * for code-generation.
	 */
	void resolve();
	
	/** Note, this is non-const because
	 * it may create a type on the fly,
	 * for example, it may create a 'Type*' for
	 * a usage of an already existing 'Type'
	 * even if it was never referenced in another
	 * type, it may be referenced in a function.
	 */
	const Gyoji::mir::Type * extract_from_type_specifier(const Gyoji::frontend::tree::TypeSpecifier & type_specifier);
    private:
	Gyoji::mir::MIR & mir;
	Gyoji::context::CompilerContext & compiler_context;
	const Gyoji::frontend::tree::TranslationUnit & translation_unit;
	
	const Gyoji::mir::Type* extract_from_type_specifier_simple(const Gyoji::frontend::tree::TypeSpecifierSimple & type_specifier);
	const Gyoji::mir::Type* extract_from_type_specifier_template(const Gyoji::frontend::tree::TypeSpecifierTemplate & type_specifier);
	const Gyoji::mir::Type* extract_from_type_specifier_function_pointer(const Gyoji::frontend::tree::TypeSpecifierFunctionPointer & type_specifier);
	const Gyoji::mir::Type* extract_from_type_specifier_pointer_to(const Gyoji::frontend::tree::TypeSpecifierPointerTo & type_specifier);
	const Gyoji::mir::Type* extract_from_type_specifier_reference_to(const Gyoji::frontend::tree::TypeSpecifierReferenceTo & type_specifier);
	const Gyoji::mir::Type* extract_from_type_specifier_array(const Gyoji::frontend::tree::TypeSpecifierArray & type_specifier);
	
	void extract_from_class_declaration(const Gyoji::frontend::tree::ClassDeclaration & declaration);

	void extract_from_class_method_types(
	    Gyoji::mir::Type & class_type,
	    std::map<std::string, Gyoji::mir::TypeMethod> & methods,
	    std::string simple_name,
	    std::string fully_qualified_name,
	    const Gyoji::frontend::tree::UnsafeModifier & method_unsafe_modifier,
	    const Gyoji::mir::Type *method_return_type,
	    const Gyoji::frontend::tree::FunctionDefinitionArgList & function_definition_arg_list,
	    const Gyoji::context::SourceReference & source_ref
	    );

	void extract_from_class_members(Gyoji::mir::Type & type, const Gyoji::frontend::tree::ClassDefinition & definition);
	void extract_from_class_definition(const Gyoji::frontend::tree::ClassDefinition & definition);
	void extract_from_enum_definition(const Gyoji::frontend::tree::EnumDefinition & enum_definition);
	void extract_from_type_definition(const Gyoji::frontend::tree::TypeDefinition & type_definition);
	void extract_from_namespace(const Gyoji::frontend::tree::FileStatementNamespace & namespace_declaration);
	void extract_types(const std::vector<Gyoji::owned<Gyoji::frontend::tree::FileStatement>> & statements);
	
	void extract_from_function_specifications(
	    const Gyoji::frontend::tree::Terminal & name,
	    const Gyoji::mir::Type *return_type,
	    const Gyoji::context::SourceReference & return_type_source_ref,
	    const Gyoji::frontend::tree::FunctionDefinitionArgList & arguments,
	    const Gyoji::frontend::tree::UnsafeModifier & unsafe_modifier
	    );
	
	void extract_from_function_definition(const Gyoji::frontend::tree::FileStatementFunctionDefinition & function_definition);
	void extract_from_function_declaration(const Gyoji::frontend::tree::FileStatementFunctionDeclaration & function_declaration);
	
	Gyoji::mir::Type *get_or_create(
	    std::string pointer_name,
	    Gyoji::mir::Type::TypeType type_type,
	    bool complete,
	    const Gyoji::context::SourceReference & source_ref
	    );
	
	// Move to analysis
	void check_complete_type(Gyoji::mir::Type *type) const;
	
    };
};


