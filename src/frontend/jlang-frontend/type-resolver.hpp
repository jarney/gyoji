#pragma once

#include <jlang-mir/types.hpp>
#include <jlang-frontend.hpp>

namespace JLang::frontend {

  class TypeResolver {
  public:
    TypeResolver(const JLang::frontend::ParseResult & _parse_result, JLang::mir::Types & _types);
    ~TypeResolver();
    void resolve_types();
  private:

    JLang::mir::Type * extract_from_type_specifier(const JLang::frontend::tree::TypeSpecifier & type_specifier);
    
    void extract_from_class_declaration(const JLang::frontend::tree::ClassDeclaration & declaration);
    void extract_from_class_members(JLang::mir::Type & type, const JLang::frontend::tree::ClassDefinition & definition);
    void extract_from_class_definition(const JLang::frontend::tree::ClassDefinition & definition);
    void extract_from_enum(const JLang::frontend::tree::EnumDefinition & enum_definition);
    void extract_from_namespace(const JLang::frontend::tree::FileStatementNamespace & namespace_declaration);
    void extract_types(const std::vector<JLang::owned<JLang::frontend::tree::FileStatement>> & statements);
    
    JLang::mir::Type *get_or_create(
                                    std::string pointer_name,
                                    JLang::mir::Type *pointer_target,
                                    JLang::mir::Type::TypeType type_type,
                                    const JLang::context::SourceReference & source_ref
                                    );

    // Move to analysis
    void check_complete_type(JLang::mir::Type *type) const;
    
    JLang::mir::Types & types;
    const JLang::frontend::ParseResult & parse_result;
  };
  
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
  void resolve_types(JLang::mir::Types & types, const JLang::frontend::ParseResult & parse_result);
  
};
