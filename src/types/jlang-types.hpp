#pragma once

#include <jlang-frontend.hpp>

//! The types namespace is used to extract and resolve types from the syntax tree.
/**
 * The purpose of this namespace is to process the synax tree and
 * extract the types of objects that will be used later in
 * the compilation and code generation.
 *
 * This consists of several data types representing the type
 * system as well as a set of classes and functions intended
 * to construct the type information by processing a TranslationUnit.
 *
 * Type processing may encounter errors where objects are
 * defined incorrectly or types cannot be properly resolved.
 * In these cases, errors may be reported to the
 * error handling system so that the programmer is aware
 * of why the definition or resolution did not succeed.
 */
namespace JLang::types {
  class Type;
  class Types;
  
  //! Set of types extracted from a translation unit.
  /**
   * This class represents a type extracted from the
   * source translation unit.  Types may be primitive
   * or composite structures containing other types.
   */
  class Types {
  public:
    Types();
    ~Types();
    
    /**
     * This returns the fully resolved type from the
     * fully-qualified name of the type.  By fully-qualified,
     * we mean a namespace path beginning with the root
     * namespace and identifying a specific type.
     */
    Type * get_type(std::string type) const;

    /**
     * This is used to define a fully-qualified type
     * from the definition.  Note that some types
     * may be incompletely specified when they are
     * initially declared and a final resolution step may
     * be required in order to ensure that the type is fully
     * specified.
     */
    void define_type(JLang::owned<Type> type);

    /**
     * This is used for debugging purposes to dump
     * the content of the type database.
     */
    void dump();
    
    std::map<std::string, JLang::owned<Type>> type_map;
  private:
  };

  //! This represents a type as declared in a translation unit.
  /**
   * A type consists of several aspects:
   * * Visibility : The visibility semantics indicating
   *                where in the code this type may be referenced.
   *
   * * Physical Structure : The structural and storage aspects of
   *                        the type indicating how the type is
   *                        constructed from various primitive types.
   *
   * The type as resolved should define all of the fields
   * associated in a heirarchal fashion all the way down
   * to the primitive types used to store the data in memory
   * or registers.  From this type information, it should be
   * possible to identify the specific operations that can be
   * performed to manipulate any of the data as primitive
   * machine operations.
   */
  class Type {
  public:
    typedef enum {
      /**
       * This is a primitive type such as u8, u16, u32, etc.
       */
      TYPE_PRIMITIVE,

      /**
       * This is a composite type consisting of
       * one or more other name/type pairs.
       * This may not be a leaf-node because
       * ultimately all types must be resolvable
       * down to primitive types.
       */
      TYPE_COMPOSITE,
      
      /**
       * This is a pointer to another type or to a primitive
       * and is stored internally in a u64 primitive value
       * containing the address of the data to be accessed.
       */
      TYPE_POINTER,

      /**
       * This is similar to a pointer and is also stored
       * internally as an address in a u64, but carries
       * additional semantics used by the borrow checker.
       */
      TYPE_REFERENCE,
      
      /**
       * This is an enum constant type that resolves to a u32 primitive
       * but may contain names specifying the values they map to.
       */
      TYPE_ENUM
    } TypeType;

    /**
     * This defines a primitive type of the given
     * type as a primitive type.
     */
    Type(std::string _name, TypeType _type, bool _complete);

    /**
     * Destructor, nothing special.
     */
    ~Type();
    
    /**
     * This returns the fully-qualified name of the type.
     */
    const std::string & get_name() const;
    /**
     * This returns true if the type has not yet been completely specified.
     * In particular, types may be incomplete when they are initially
     * declared in a forward-declaration, but may receive a final definition
     * only later when the type resolution occurs.
     */
    bool is_complete();
    
    /**
     * Completes the definition of a composite type.
     */
    void complete_composite_definition(std::vector<std::pair<std::string, Type*>> _members);

    /**
     * Completes the definition of a pointer or reference.
     */
    void complete_pointer_definition(Type *_type);
    
    /**
     * Used for debugging purposes to dump the content
     * of the type database.
     */
    void dump();
  private:
    bool complete;
    std::string name;
    TypeType type;
    Type *pointer_or_ref;
    std::vector<std::pair<std::string, Type*>> members;
  };

  class TypeResolver {
  public:
    TypeResolver(const JLang::frontend::ParseResult & _parse_result, Types & _types);
    ~TypeResolver();
    void resolve_types();
  private:

    Type * extract_from_type_specifier(const JLang::frontend::tree::TypeSpecifier & type_specifier);
    
    void extract_from_class_declaration(const JLang::frontend::tree::ClassDeclaration & declaration);
    void extract_from_class_members(Type & type, const JLang::frontend::tree::ClassDefinition & definition);
    void extract_from_class_definition(const JLang::frontend::tree::ClassDefinition & definition);
    void extract_from_enum(const JLang::frontend::tree::EnumDefinition & enum_definition);
    void extract_from_namespace(const JLang::frontend::tree::FileStatementNamespace & namespace_declaration);
    void extract_types(const std::vector<::JLang::owned<JLang::frontend::tree::FileStatement>> & statements);
    
    Type *get_or_create(std::string pointer_name, Type *pointer_target, Type::TypeType type_type);
    
    Types & types;
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
  JLang::owned<Types> resolve_types(const JLang::frontend::ParseResult & parse_result);
  
  int doit();
};
