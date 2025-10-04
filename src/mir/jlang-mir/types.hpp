#pragma once

#include <jlang-misc/pointers.hpp>
#include <jlang-context.hpp>
#include <string>
#include <map>
#include <vector>

/**
 * @brief The types namespace is used to extract and resolve types from the syntax tree.
 *
 * @details
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
    
    /**
     * @brief Set of types extracted from a translation unit.
     *
     * @details
     * This class represents a type extracted from the
     * source translation unit.  Types may be primitive
     * or composite structures containing other types.
     */
    class Types {
    public:
	/**
	 * @brief Creates a new types table with only the primitive types defined.
	 *
	 * @details
	 * This constructs a new table of types and defined the primitive types
	 * of the type system inside it.  Composite and user-defined types may also
	 * be added after construction.
	 */
	Types();
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	~Types();
	
	/**
	 * This returns the fully resolved type from the
	 * fully-qualified name of the type.  By fully-qualified,
	 * we mean a namespace path beginning with the root
	 * namespace and identifying a specific type.
	 */
	Type * get_type(std::string type) const;

	/**
	 * This returns a type that is a pointer to the given type.  The type
	 * returned may be a new type and if so, it is inserted
	 * into the table of types.  If the pointer type already
	 * exists, then a pointer to the already existing immutable
	 * type will be returned.
	 */
	const Type * get_pointer_to(const Type *_type, const JLang::context::SourceReference & src_ref);
	
	/**
	 * This returns a type that is a reference to the given type.  The type
	 * returned may be a new type and if so, it is inserted
	 * into the table of types.  If the pointer type already
	 * exists, then a pointer to the already existing immutable
	 * type will be returned.
	 */
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

	/**
	 * This is used to provide direct (immutable)
	 * access to the types table.
	 */
	const std::map<std::string, JLang::owned<Type>> & get_types() const;
    private:
	std::map<std::string, JLang::owned<Type>> type_map;
    };
    
    /**
     * @brief This represents a typed member variable of a class.
     *
     * @details
     * A member variable for a class consists of a name
     * and a type.  The name is the alias that is used
     * when referring to this memory and the type is
     * the type of that data.  This also contains a source
     * reference so that errors can be correctly
     * attributed to their origin.
     */
    class TypeMember {
    public:
	/**
	 * This is a named member of a class or structure.
	 * It is created with a name, the type of that member
	 * which may be another type, and a reference
	 * to the source location where it was declared.
	 */
	TypeMember(
	    std::string _member_name,
	    const Type *_member_type,
	    const JLang::context::SourceReference & _source_ref
	    );
	/**
	 * This makes a copy of the member.  It is provided
	 * so that the type class may use a map or array to
	 * store the members rather than storing them by pointer,
	 * so a copy and assignment is required.
	 */
	TypeMember(const TypeMember & other);
	/**
	 * This makes a copy of the member.  It is provided
	 * so that the type class may use a map or array to
	 * store the members rather than storing them by pointer,
	 * so a copy and assignment is required.
	 */
	TypeMember & operator=(const TypeMember & other);
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	~TypeMember();
	/**
	 * Returns the name of the member.
	 */
	const std::string & get_name() const;
	/**
	 * Returns the type fo the member.
	 */
	const Type *get_type() const;
	/**
	 * Returns a reference to the source location where
	 * the member was declared.
	 */
	const JLang::context::SourceReference & get_source_ref() const;
    private:
	std::string member_name;
	const Type *member_type;
	const JLang::context::SourceReference *source_ref;
    };

    /**
     * @brief Function argument
     *
     * @details
     * This class represents the argument to a function for
     * function-pointer types.
     */
    class Argument {
    public:
	/**
	 * Creates a new argument with the given type
	 * based on the type of the argument and a reference
	 * to the source location where it was declared.
	 */
	Argument(
	    const Type *_argument_type,
	    const JLang::context::SourceReference & _source_ref
	    );
	/**
	 * This makes a copy of the argument.  It is provided
	 * so that the type class may use a map or array to
	 * store the members rather than storing them by pointer,
	 * so a copy and assignment is required.
	 */
	Argument(const Argument & _other);
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	~Argument();
	/**
	 * Returns the type of argument.
	 */
	const Type* get_type() const;
	/**
	 * Returns a reference to the source
	 * where this argument was declared.
	 */
	const JLang::context::SourceReference & get_source_ref();
    private:
	const Type *argument_type;
	const JLang::context::SourceReference *source_ref;
	
	
    };
    
    /**
     * @brief This represents a type as declared in a translation unit.
     *
     * @details
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
	/**
	 * @brief Type of type
	 *
	 * @description
	 * Yes, you read that right.  Even types have types.  In this case,
	 * the type of the type is like a 'category' of type.  In many cases,
	 * the type is a single entity, but in cases like pointers and classes,
	 * the type is composite in some way and may reference other types.
	 * Types also have various attributes which make them unique
	 * in structure.  This enumeration gives the variations that
	 * types may have and gives a way to distinguish how the code-generator
	 * or analysis phases should reason about the various types.
	 *
	 * For example, the borrow-checker may need to reason that a reference
	 * to a member variable and a reference to the class containing that
	 * member variable are related in an interesting way so that
	 * it can reason about the validity of a borrow.  This is only
	 * possible if the borrow checker can know the attributes of
	 * types and how they relate.
	 */
	typedef enum {
	    /**
	     * @brief Primitive 8-bit unsigned integer
	     *
	     * @details
	     * This represents a single byte unsigned integer
	     * capable of storing values from 0 to 255 inclusive.
	     */
	    TYPE_PRIMITIVE_u8,
	    /**
	     * @brief Primitive 16-bit unsigned integer
	     *
	     * @details
	     * This represents a two-byte unsigned integer
	     * capable of storing values from 0 to 65535 inclusive.
	     */
	    TYPE_PRIMITIVE_u16,
	    /**
	     * @brief Primitive 32-bit unsigned integer
	     *
	     * @details
	     * This represents a four-byte unsigned integer
	     * capable of storing values from 0 to 2^32-1 inclusive.
	     */
	    TYPE_PRIMITIVE_u32,
	    /**
	     * @brief Primitive 64-bit unsigned integer
	     *
	     * @details
	     * This represents a four-byte unsigned integer
	     * capable of storing values from 0 to 2^64-1 inclusive.
	     */
	    TYPE_PRIMITIVE_u64,

	    /**
	     * @brief Primitive 8-bit signed integer
	     *
	     * @details
	     * This represents a single byte unsigned integer
	     * capable of storing values from -128 to 127 inclusive
	     * represented in two's complement notation.
	     */
	    TYPE_PRIMITIVE_i8,
	    /**
	     * @brief Primitive 16-bit signed integer
	     *
	     * @details
	     * This represents a single byte unsigned integer
	     * capable of storing values from -2^15 to 2^15-1 inclusive
	     * represented in two's complement notation.
	     */
	    TYPE_PRIMITIVE_i16,
	    /**
	     * @brief Primitive 32-bit signed integer
	     *
	     * @details
	     * This represents a single byte unsigned integer
	     * capable of storing values from -2^31 to 2^31-1 inclusive
	     * represented in two's complement notation.
	     */
	    TYPE_PRIMITIVE_i32,
	    /**
	     * @brief Primitive 64-bit signed integer
	     *
	     * @details
	     * This represents a single byte unsigned integer
	     * capable of storing values from -2^63 to 2^63-1 inclusive
	     * represented in two's complement notation.
	     */
	    TYPE_PRIMITIVE_i64,

	    /**
	     * @brief Primitive 32 bit floating-point number.
	     *
	     * @details
	     * This is a single precision IEEE floating-point number.
	     */
	    TYPE_PRIMITIVE_f32,
	    /**
	     * @brief Primitive 64 bit floating-point number.
	     *
	     * @details
	     * This is a double precision IEEE floating-point number.
	     */
	    TYPE_PRIMITIVE_f64,

	    /**
	     * @brief Primitive boolean value holding 'true' or 'false'.
	     *
	     * @details
	     * This may be represented as different physical sizes based on the
	     * compiler platform and may be anywhere between 1 bit (packed inside
	     * other storage) or as large as the 'natural' type of the platform
	     * like a 32-bit value for fast register access.  The storage details
	     * are left to the specific platform, but the 'sizeof' operation
	     * is guaranteed to return the actual storage size used.
	     */
	    TYPE_PRIMITIVE_bool,
	    /**
	     * @brief Primitive 'void' type
	     *
	     * @details
	     * This value represents the absence of a value.  It is
	     * provided so that, for example, functions can be
	     * declared not to return a value.  Depending on
	     * the specifics of the platform, a value may actually
	     * be present and returned, however, the languge will
	     * ignore that value if present and will behave as if
	     * no data was returned or exchanged.  Variables,
	     * members, and arguments may not be declared as void,
	     * but it may only be used for return-values to indicate
	     * their absence.
	     */
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
	 * type as a primitive type.  When declaring
	 * variables such as classes, the entirety of the
	 * type may not be known right away.  In this case,
	 * the type is considered 'incomplete'.  Examples of this
	 * are class forward-declarations.  The type of the class
	 * is known at the point of forward-declaration, but
	 * the members inside it are not yet known and the type
	 * is not complete until a 'full' declaration is provided.
	 *
	 * Primitive types are always defined as complete.
	 */
	Type(
	    std::string _name,
	    TypeType _type,
	    bool _complete,
	    const JLang::context::SourceReference & _source_ref
	    );
	/**
	 * This defines a type that is a copy of another type.
	 * This is used, for example, in a 'typedef' when a type
	 * is just different name for an already existing type.
	 */
	Type(
	    std::string _name,
	    const JLang::context::SourceReference & _source_ref,
	    const Type & _other
	    );
	
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
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

	/**
	 * This returns true if the type is one of
	 * the built-in primitive types.
	 */
	bool is_primitive() const;
	/**
	 * This returns true if the type is a pointer
	 * to another type.
	 */
	bool is_pointer() const;
	/**
	 * This returns true if the type is a reference
	 * to another type.
	 */
	bool is_reference() const;
	/**
	 * This returns true if the type is a
	 * signed integer (i8, i16, i32, i64),
	 * an unsigned integer (u8,u16,u32,u64),
	 * or a floating-point number (f32, f64).
	 */
	bool is_numeric() const;
	/**
	 * This returns true if the type is a
	 * signed integer (i8, i16, i32, i64),
	 * or an unsigned integer (u8,u16,u32,u64).
	 */
	bool is_integer() const;
	/**
	 * This returns true if the type is
	 * an unsigned integer (u8,u16,u32,u64).
	 */
	bool is_unsigned() const;
	/**
	 * This returns true if the type is a
	 * signed integer (i8, i16, i32, i64).
	 */
	bool is_signed() const;
	/**
	 * This returns true if the type
	 * is a floating-point number (f32, f64).
	 */
	bool is_float() const;
	/**
	 * This returns true if the type
	 * is a boolean type.
	 */
	bool is_bool() const;
	/**
	 * This returns true if the type
	 * is a void type.
	 */
	bool is_void() const;
	/**
	 * This returns true if the type
	 * is an enum type.
	 */
	bool is_enum() const;
	/**
	 * This returns true if the type
	 * is a composite type such as a class
	 * or structure.
	 */
	bool is_composite() const;
	/**
	 * This returns true if the type is
	 * a pointer to a function.
	 */
	bool is_function_pointer() const;

	/**
	 * This returns the size in bytes of the primitive.
	 * This is ONLY valid for primitive number types
	 * and it is a bug to call this for any type
	 * that returns 'false' for 'is_numeric'
	 */
	size_t get_primitive_size() const;

	/**
	 * This returns the type of type (see TypeType).
	 */
	TypeType get_type() const;

	/**
	 * This returns an immutable reference to the
	 * array of members of a class.  This is ONLY valid
	 * for types that are 'is_composite()'
	 */
	const std::vector<TypeMember> & get_members() const;

	/**
	 * This returns a pointer to the member indicated by
	 * the given name.  This is ONLY valid for types
	 * that are 'is_composite()'.  If the member with the
	 * given name does not exist, nullptr is returned.
	 */
	const TypeMember *member_get(const std::string & member_name) const;

	/**
	 * This returns a pointer to the type pointed to by
	 * this type.  It is ONLY valid for types that are 'is_pointer()' or
	 * 'is_reference()'.
	 */
	const Type * get_pointer_target() const;

	/**
	 * This returns a pointer to the type returned by
	 * the function for function pointer types.  This is ONLY
	 * valid for types that are 'is_function_pointer()'.
	 */
	const Type * get_return_type() const;

	/**
	 * This returns the list of arguments to the
	 * function for function pointer types.  This is ONLY
	 * valid for types that are 'is_function_pointer()'.
	 */
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
	 * If a type is forward-declared without a complete type,
	 * this returns the reference to the source location
	 * where the forward declaration happened.  If it
	 * was not forward declared, this will be the same source
	 * reference as where it is defined.
	 */
	const JLang::context::SourceReference & get_declared_source_ref() const;
	
	/**
	 * This is the location where the type's definition was completed.
	 * For forward-declared types, this will be the location where the
	 * type was 'completed'.  For other types, this will be the same
	 * as the place where the type was declared.
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
