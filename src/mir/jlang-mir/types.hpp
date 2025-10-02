#pragma once

#include <jlang-misc/pointers.hpp>
#include <jlang-context.hpp>
#include <string>
#include <map>
#include <vector>

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
namespace JLang::mir {
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

	const Type * get_pointer_to(const Type *_type, const JLang::context::SourceReference & src_ref);
	
	const Type * get_reference_to(const Type *_type, const JLang::context::SourceReference & src_ref);
	
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
	void dump(FILE *out) const;
	
	const std::map<std::string, JLang::owned<Type>> & get_types() const;
    private:
	std::map<std::string, JLang::owned<Type>> type_map;
    };
    
    //! This represents a typed member variable of a class.
    /**
     * A member variable for a class consists of a name
     * and a type.  The name is the alias that is used
     * when referring to this memory and the type is
     * the type of that data.  This also contains a source
     * reference so that errors can be correctly
     * attributed to their origin.
     */
    class TypeMember {
    public:
	TypeMember(
	    std::string _member_name,
	    const Type *_member_type,
	    const JLang::context::SourceReference & _source_ref
	    );
	TypeMember(const TypeMember & other);
	TypeMember & operator=(const TypeMember & other);
	~TypeMember();
	const std::string & get_name() const;
	const Type *get_type() const;
	const JLang::context::SourceReference & get_source_ref() const;
    private:
	std::string member_name;
	const Type *member_type;
	const JLang::context::SourceReference *source_ref;
    };

    class Argument {
    public:
	Argument(
	    const Type *_argument_type,
	    const JLang::context::SourceReference & _source_ref
	    );
	// Copy constructor so we can put it inside
	// a list.
	Argument(const Argument & _other);
	~Argument();
	const Type* get_type() const;
	const JLang::context::SourceReference & get_source_ref();
    private:
	const Type *argument_type;
	const JLang::context::SourceReference *source_ref;
	
	
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
	    TYPE_PRIMITIVE_u8,
	    TYPE_PRIMITIVE_u16,
	    TYPE_PRIMITIVE_u32,
	    TYPE_PRIMITIVE_u64,

	    TYPE_PRIMITIVE_i8,
	    TYPE_PRIMITIVE_i16,
	    TYPE_PRIMITIVE_i32,
	    TYPE_PRIMITIVE_i64,

	    TYPE_PRIMITIVE_f32,
	    TYPE_PRIMITIVE_f64,

	    TYPE_PRIMITIVE_bool,
	    TYPE_PRIMITIVE_void,
	    
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
	     * This is a pointer to a function.
	     */
	    TYPE_FUNCTION_POINTER,

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
	Type(std::string _name, TypeType _type, bool _complete, const JLang::context::SourceReference & _source_ref);
	Type(std::string _name, const JLang::context::SourceReference & _source_ref, const Type & _other);
	
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
	bool is_complete() const;

	bool is_primitive() const;
	bool is_pointer() const;
	bool is_reference() const;
	bool is_numeric() const;
	bool is_integer() const;
	bool is_unsigned() const;
	bool is_signed() const;
	bool is_float() const;
	bool is_bool() const;
	bool is_void() const;
	bool is_enum() const;
	bool is_composite() const;
	bool is_function_pointer() const;
	size_t get_primitive_size() const;
	
	TypeType get_type() const;
	
	const std::vector<TypeMember> & get_members() const;
	const TypeMember *member_get(const std::string & member_name) const;
	
	const Type * get_pointer_target() const;

	const Type * get_return_type() const;
	const std::vector<Argument> & get_argument_types() const;
	
	/**
	 * Completes the definition of a composite type.
	 */
	void complete_composite_definition(std::vector<TypeMember> _members, const JLang::context::SourceReference & _source_ref);
	
	/**
	 * Completes the definition of a pointer or reference.
	 */
	void complete_pointer_definition(const Type *_type, const JLang::context::SourceReference & _source_ref);

	/**
	 * Completes the definition of a function pointer
	 * by passing the return-value type and the types
	 * of each of the arguments.
	 */
	void complete_function_pointer_definition(
	    const Type *_return_type,
	    const std::vector<Argument> & _argument_types,
	    const JLang::context::SourceReference & _source_ref
	    );
	
	/**
	 * Used for debugging purposes to dump the content
	 * of the type database.
	 */
	void dump(FILE *out) const;
	
	/**
	 * Where the type was first declared.
	 */
	const JLang::context::SourceReference & get_declared_source_ref() const;
	
	/**
	 * Where the full definition of the type appeared.
	 */
	const JLang::context::SourceReference & get_defined_source_ref() const;
	
    private:
	std::string name;
	TypeType type;
	bool complete;
	
	const JLang::context::SourceReference *declared_source_ref;
	const JLang::context::SourceReference *defined_source_ref;

	// Used only for pointer and reference types.
	const Type *pointer_or_ref;

	// Used only for function pointer types.
	const Type *return_type;
	std::vector<Argument> argument_types;

	// Used only for class/composite types.
	std::vector<TypeMember> members;
    };
};
