#pragma once

#include <jlang-misc/pointers.hpp>
#include <jlang-mir/types.hpp>
#include <jlang-context.hpp>

#include <string>
#include <map>
#include <vector>

namespace JLang::mir {
    /**
     * @brief Operations inside basic blocks, the virtual instruction-set of the MIR.
     *
     * @details
     * This represents a variation on the
     * "Three Address Code" form.  These
     * operations make up the virtual instruction-set
     * of the MIR virtual machine.
     *
     * Each operation such as 'add' c =  a + b is represented as
     * @code{.unparsed}
     *     _c = add _a, _b
     * @endcode
     *
     * where '_c' is the returned value and
     * '_a' and '_b' are the operands.
     *
     * There are some
     * operations (like function calls) that take
     * more than three addresses, but for most,
     * the operation performs an operation on
     * one or two operands and returns a result.
     *
     * Some operations carry additional information
     * such as branch locations (basic-block IDs)
     * or string literals (in the case of the literal-string opcode).
     *
     * Each operation has a "Type" field representing the
     * concrete operation being performed.
     * The operands and result are passed in
     * and manipulated.
     *
     * The operands are identified in terms of
     * the ID of the temporary value they operate on.
     * Type information is carried there, not inside
     * the operation although operations do have
     * strict conditions on the types of the operands
     * and the types of the returned values.  Those
     * conditions are enforced outside of this
     * structure.
     */
    class Operation {
    public:
	typedef enum {
	    // Global symbols
	    /**
	     * @brief Function call
	     * @details
	     * Directs the MIR to call a function.  The first
	     * operand is the function to call and the remaining
	     * operands are the arguments to the function.
	     *
	     * The opcode will return the same type that the
	     * function returns and the operand types will match
	     * the arguments of the function.
	     *
	     * Examples:
	     * @code{.unparsed}
	     * Symbols:
	     *    main : u8(*)(u32, u8**)
	     * Operations:
	     * _0: (u8)(*)(u32, u8**) = symbol("main")
	     * _1: u32 = literal-int(0)
	     * _2: u8** = nullptr
	     * _3: u8 = function-call (_0, _1, _2)
	     * @endcode
	     */
	    OP_FUNCTION_CALL,
	    /**
	     * @brief Load a symbol from the symbol-table.
	     * @details
	     * This opcode directs the MIR to load a symbol
	     * from the symbol table.  This will be either
	     * a function pointer (to be used by the OP_FUNCTION_CALL opcode)
	     * or by another opcode in evaluating an expression like
	     * a global variable.
	     *
	     * This opcode will return the type of the symbol found.
	     * In the case of a function, this is the function-pointer
	     * type signature and in the case of a global variable, it will
	     * be whatever type that variable is declared as.
	     *
	     * @code{.unparsed}
	     * Symbols:
	     *    main : u8(*)(u32, u8**)
	     * Operations:
	     * _0: (u8)(*)(u32, u8**) = symbol("main")
	     * _1: u32 = literal-int(0)
	     * _2: u8** = nullptr
	     * _3: u8 = function-call (_0, _1, _2)
	     * @endcode
	     */
	    OP_SYMBOL,

	    // Cast operations
	    /**
	     * @brief Widen a signed integer to a larger type.
	     * @details
	     * This opcode widens a value from a signed integer
	     * to another signed integer type by "sign-extending" the
	     * value.  For example, it can convert a signed i8 to a
	     * signed i32.  This operation takes a single
	     * operand, the variable to be extended, and a type
	     * to which it should be extended.  The type should also
	     * be a signed integer.
	     *
	     * The result will be the value of the integer, sign-extended
	     * to the type given.
	     *
	     * It is an error to pass any operand which is not a signed
	     * integer or a Type which is not a signed integer type.
	     *
	     * Examples:
	     * @code{.unparsed}
	     * _0 : i8
	     * _1: i16 = widen-signed ( _0, i16 )
	     * _2: i32 = widen-signed ( _1, i32 )
	     * _3: i64 = widen-signed ( _2, i64 )
	     * @endcode
	     */
	    OP_WIDEN_SIGNED,    // Widen a signed integer to a larger type.

	    /**
	     * @brief Widen an unsigned integer to a larger type.
	     * @details
	     * This opcode widens a value from an unsigned integer
	     * to another unsigned integer type by "zero-extending" the
	     * value.  For example, it can convert an unsigned u8
	     * to a u64 by zero-extending it.
	     *
	     * The result will be the value of the unsigned integer,
	     * zero-extended to the type given.
	     *
	     * It is an error to pass any operand which is not
	     * an unsigned integer or a Type which is not an unsigned
	     * integer type.
	     *
	     * Examples:
	     * @code{.unparsed}
	     * _0 : u8
	     * _1: u16 = widen-unsigned ( _0, u16 )
	     * _2: u32 = widen-unsigned ( _1, u32 )
	     * _3: u64 = widen-unsigned ( _2, u64 )
	     * @endcode
	     */
	    OP_WIDEN_UNSIGNED,  // Widen an unsigned integer to a larger type.

	    /**
	     * @brief Widen a floating-point integer to wider type.
	     *
	     * @details
	     * This opcode widens a value from a floating-point (f32)
	     * to a double-precision floating point (f64).
	     *
	     * The result will be the value of the floating-point,
	     * converted and widened to larger type (f64).
	     *
	     * It is an error to pass any operand which is not
	     * a floating-point number of a Type which is not a floating-point
	     * number.
	     *
	     * Examples:
	     * @code{.unparsed}
	     * _0 : f32
	     * _1: f64 = widen-float ( _0, f64 )
	     * @endcode
	     */
	    OP_WIDEN_FLOAT,     // Widen a floating-point to a larger type.

	    // Indirect access
	    OP_ARRAY_INDEX,     // Pointer types -> Pointer Target type
	    OP_DOT,             // class types -> Found member type
	    OP_ARROW,           // Pointer to class types -> Found member type

	    // Variable access
	    /**
	     * @brief Declare local variable
	     *
	     * @details
	     * This declares a local variable on the stack of a given type.  This
	     * takes no opcodes and returns no value.  Instead, it operates on the
	     * virtual machine by bringing a variable into scope so that it can
	     * be loaded and stored by other opcodes.
	     *
	     * This opcode has the name of the variable to declare and the
	     * type of variable it is so that it can be appropriately allocated.
	     *
	     * For example, this
	     * declaration:
	     * 
	     * @code{.unparsed}
	     * {
	     *    ...
	     *    u8 foo;
	     *    ...
	     * }
	     * @endcode
	     *
	     * will result in this opcode:
	     *
	     * @code{.unparsed}
	     *     local-declare (foo)
	     * @endcode
	     */
	    OP_LOCAL_DECLARE,   // N/A

	    /**
	     * @brief Undeclare local variable
	     * @details
	     * This de-allocates a local variable on the stack of a given type.  This
	     * takes no opcodes and returns no value.  Instead, it operates
	     * on the virtual machine by removing that variable from scope.  Access
	     * to this variable after it has gone out of scope is an error.
	     *
	     * This opcode carries the name of the variable to de-allocate.
	     * @code{.unparsed}
	     * {
	     *    ...
	     *    u8 foo;
	     *    ...
	     * }
	     * will result in this opcode when
	     * foo goes out of scope.
	     *
	     * @code{.unparsed}
	     *     local-undeclare (foo)
	     * @endcode
	     * @endcode
	     */
	    OP_LOCAL_UNDECLARE, // N/A
	    /**
	     * @brief Load value from variable.
	     * @details
	     * This loads the value of a variable from its storage and places
	     * the result in the return-value.
	     *
	     * For example, if the variable 'foo' is declared
	     * as a u8, the following will result
	     * when the value of 'foo' is declared.
	     *
	     * @code{.unparsed}
	     * _0: u8 = load (foo)
	     * @endcode
	     *
	     * This opcode results in an "lvalue", that is,
	     * the variable 'foo' can also be the target
	     * of an assignment.
	     *
	     * This opcode takes no operands.
	     */
	    OP_LOCAL_VARIABLE,
	    
            // Literals
	    /**
	     * @brief Loads a literal character constant.
	     *
	     * @details
	     * This loads a literal character constant into
	     * the return-value of the operation.
	     *
	     * @code{.unparsed}
	     * _0: u8 = literal-char( 'x' )
	     * @endcode
	     */
	    OP_LITERAL_CHAR,
	    /**
	     * @brief Loads a string constant.
	     *
	     * @details
	     * This loads a pointer to a u8 that points to a
	     * string literal defined in the static constant
	     * data area.
	     *
	     * @code{.unparsed}
	     * _0: u8* = literal-string( "Hello World" )
	     * @endcode
	     *
	     */
	    OP_LITERAL_STRING,
	    /**
	     * @brief Loads an integer constant.
	     *
	     * @details
	     * This loads an integer constant into the
	     * return-value of the operand.
	     *
	     * @code{.unparsed}
	     * _0: u32 = literal-float( 17 u32 )
	     * @endcode
	     *
	     */
	    OP_LITERAL_INT,
	    /**
	     * @brief Loads an integer constant.
	     *
	     * @details
	     * This loads a f32 or f64 floating-point constant into the
	     * return-value of the operand.
	     *
	     * @code{.unparsed}
	     * _0: f32 = literal-float( 17.8 f32 )
	     * @endcode
	     *
	     */
	    OP_LITERAL_FLOAT,
	    
            // Unary operations
	    /**
	     * @brief Returns a value and then increments the value of the operand.
	     * @details
	     * This opcode is the equivalent of the C++ increment
	     * suffix operation "a++" where the value returned is the
	     * value of the variable, but the value of the variable
	     * is incremented AFTER loading it.
	     *
	     * @code{.unparsed}
	     * _0: u32 = local-load( foo )
	     * _1: u32 = post-increment( _0 )
	     * @endcode
	     *
	     * The operand must be an lvalue (i.e. it must be assignable).
	     *
	     */
	    OP_POST_INCREMENT,  // Pointer types, integer types -> Operand type
	    
	    /**
	     * @brief Returns a value and then decrements the value of the operand.
	     * @details
	     * This opcode is the equivalent of the C++ increment
	     * suffix operation "a--" where the value returned is the
	     * value of the variable, but the value of the variable
	     * is incremented AFTER loading it.
	     *
	     * @code{.unparsed}
	     * _0: u32 = local-load( foo )
	     * _1: u32 = post-decrement( _0 )
	     * @endcode
	     *
	     * The operand must be an lvalue (i.e. it must be assignable).
	     *
	     */
	    OP_POST_DECREMENT,  // Pointer types, integer types -> Operand type
	    /**
	     * @brief Increments a value and then returns the incremented value.
	     * @details
	     * This opcode is the equivalent of the C++ increment
	     * prefix operation "++a" where the value returned is the
	     * value of the variable, but the value of the variable
	     * is incremented BEFORE loading it.
	     *
	     * @code{.unparsed}
	     * _0: u32 = local-load( foo )
	     * _1: u32 = pre-increment( _0 )
	     * @endcode
	     *
	     * The operand must be an lvalue (i.e. it must be assignable).
	     *
	     */
	    OP_PRE_INCREMENT,   // Pointer types, integer types -> Operand type
	    /**
	     * @brief Decrements a value and then returns the decremented value.
	     * @details
	     * This opcode is the equivalent of the C++ decrement
	     * prefix operation "--a" where the value returned is the
	     * value of the variable, but the value of the variable
	     * is decremented BEFORE loading it.
	     *
	     * @code{.unparsed}
	     * _0: u32 = local-load( foo )
	     * _1: u32 = pre-decrement( _0 )
	     * @endcode
	     *
	     * The operand must be an lvalue (i.e. it must be assignable).
	     *
	     */
	    OP_PRE_DECREMENT,   // Pointer types, integer types -> Operand type
	    /**
	     * @brief Returns the address of the given variable.
	     * @details
	     * This opcode is the equivalent of the C++ '&' operation.
	     * It returns the address of the value stored
	     * at the location of the operand.
	     * The operand must be an lvalue (i.e. it must be assignable).
	     * This is not a valid operation to perform on intermediate
	     * values, but only on variables.
	     *
	     * @code{.unparsed}
	     * _0: u32 = local-load( foo )
	     * _1: u32* = addressof( _0 )
	     * @endcode
	     *
	     * The operand must be an lvalue (i.e. it must be assignable).
	     *
	     */
	    OP_ADDRESSOF,   // Any type -> Pointer type
	    /**
	     * @brief De-references the given pointer.
	     * @details
	     * This opcode is the equivalent of the C++ '*' operation.
	     * It returns an lvalue for the value stored at the location
	     * referenced by the given pointer.
	     *
	     * @code{.unparsed}
	     * _0: u32* = local-load( foo )
	     * _1: u32 = dereference( _0 )
	     * @endcode
	     *
	     * The resulting dereferenced value will be an lvalue
	     * (i.e. it can be used in an assignment operation)
	     */
	    OP_DEREFERENCE, // Pointer types -> Pointer Target type

	    /**
	     * @brief Negates a signed integer.
	     *
	     * @details
	     * This opcode negates a signed or floating-point integer.
	     */
	    OP_NEGATE,      // Numeric types -> Numeric type

	    /**
	     * @brief Performs a bitwise not on an unsigned integer.
	     *
	     * @details
	     * This opcode performs a bitwise not of the unsigned
	     * integer operand.  It always returns a value
	     * the same type as the operand.
	     */
	    OP_BITWISE_NOT, // Integer types -> Integer Type
	    /**
	     * @brief This opcode performs a boolean NOT of the given operand.
	     *
	     * @details
	     * This opcode performs a logical (boolean) NOT of the given
	     * operand.  It always takes a single boolean operand.
	     */
	    OP_LOGICAL_NOT, // Boolean types -> Boolean
	    
	    /**
	     * @brief Returns the size of a specific type.
	     *
	     * @details
	     * This opcode returns a u64 representing the size
	     * of the operand of the given type.
	     */
	    OP_SIZEOF_TYPE, // Any type -> u64

	    // Binary operations: arithmetic
	    /**
	     * @brief Arithmetic add
	     *
	     * @details
	     * This opcode adds two nubmers together and returns the
	     * sum modulo 2^n where n is the number of bits in
	     * the operands.  Note that this opcode requires that
	     * both operands are of the same type: that they
	     * are either both signed, or both float and that the
	     * size of the values is the same.
	     *
	     * If widening or casting is needed, that must be
	     * performed first using one of the WIDEN or CAST
	     * instructions.
	     */
	    OP_ADD,         // Matching Numeric types -> Operand type
	    /**
	     * @brief Arithmetic subtract
	     *
	     * @details
	     * This opcode subtracts two nubmers together and returns the
	     * sum modulo 2^n where n is the number of bits in
	     * the operands.  Note that this opcode requires that
	     * both operands are of the same type: that they
	     * are either both signed, both unsigned, or both float and that the
	     * size of the values is the same.
	     *
	     * If widening or casting is needed, that must be
	     * performed first using one of the WIDEN or CAST
	     * instructions.
	     */
	    OP_SUBTRACT,    // Matching Numeric types -> Operand type
	    /**
	     * @brief Arithmetic multiply
	     *
	     * @details
	     * This opcode multiplies by nubmers together and returns the
	     * product modulo 2^n where n is the number of bits in
	     * the operands.  Note that this opcode requires that
	     * both operands are of the same type: that they
	     * are either both signed, both unsigned, or both float and that the
	     * size of the values is the same.
	     *
	     * If widening or casting is needed, that must be
	     * performed first using one of the WIDEN or CAST
	     * instructions.
	     */
	    OP_MULTIPLY,    // Matching Numeric types -> Operand type
	    /**
	     * @brief Arithmetic multiply
	     *
	     * @details
	     * This opcode divides two nubmers and returns the
	     * quotient modulo 2^n where n is the number of bits in
	     * the operands.  Note that this opcode requires that
	     * both operands are of the same type: that they
	     * are either both signed, both unsigned, or both float and that the
	     * size of the values is the same.
	     *
	     * If widening or casting is needed, that must be
	     * performed first using one of the WIDEN or CAST
	     * instructions.
	     *
	     * Note that divide-by-zero is not inherently protected
	     * at the MIR level.
	     */
	    OP_DIVIDE,      // Matching Integer types -> Operand type

	    /**
	     * @brief Modulo
	     *
	     * @details
	     * This opcode calculates the remainder when dividing
	     * two numbers modulo 2^n where n is the number of bits in
	     * the operands.  Note that this opcode requires that
	     * both operands are of the same type: that they
	     * are either both signed, both unsigned.  Floating-point
	     * operands are not supported.
	     *
	     * If widening or casting is needed, that must be
	     * performed first using one of the WIDEN or CAST
	     * instructions.
	     *
	     * Note that divide-by-zero is not inherently protected
	     * at the MIR level.
	     */
	    OP_MODULO,      // Matching Integer types -> Operand type

	    // Binary operations: logical
	    /**
	     * @breif Logical AND
	     *
	     * @details
	     * This opcode performs a boolean AND on the two opcodes
	     * and returns a boolean value as the result.  Both
	     * operands must be boolean values.
	     */
	    OP_LOGICAL_AND, // Boolean types -> Boolean
	    /**
	     * @breif Logical OR
	     *
	     * @details
	     * This opcode performs a boolean OR on the two opcodes
	     * and returns a boolean value as the result.  Both
	     * operands must be boolean values.
	     */
	    OP_LOGICAL_OR,  // Boolean types -> Boolean

	    // Binary operations: bitwise
	    /**
	     * @breif Bitwise AND
	     *
	     * @details
	     * This opcode performs a bitwise AND on the two opcodes.
	     * Both opcodes must be unsigned integer types
	     * and each bit in the representation of the integers
	     * is ORed together producing an unsigned integer result
	     * of the same size as the operands.
	     *
	     * Both integer operands must be of the same size.
	     */
	    OP_BITWISE_AND, // Matching integer types -> Integer type
	    /**
	     * @breif Bitwise OR
	     *
	     * @details
	     * This opcode performs a bitwise OR on the two opcodes.
	     * Both opcodes must be unsigned integer types
	     * and each bit in the representation of the integers
	     * is ORed together producing an unsigned integer result
	     * of the same size as the operands.
	     *
	     * Both integer operands must be of the same size.
	     */
	    OP_BITWISE_OR,  // Matching integer types -> Integer type
	    
	    /**
	     * @breif Bitwise XOR
	     *
	     * @details
	     * This opcode performs a bitwise XOR on the two opcodes.
	     * Both opcodes must be unsigned integer types
	     * and each bit in the representation of the integers
	     * is ORed together producing an unsigned integer result
	     * of the same size as the operands.
	     *
	     * Both integer operands must be of the same size.
	     */
	    OP_BITWISE_XOR, // Matching integer types -> Integer type
	    /**
	     * @breif Bitwise Shift left
	     *
	     * @details
	     * This opcode performs a bitwise shift left of the first
	     * operand by the number of bits in the second operand (modulo the
	     * number of bits in the first operand).
	     * Both operands must be unsigned integers.
	     *
	     * For example, if you shift left on a u8 by 9 bits,
	     * the result is the same as shifting left by 1 bit
	     * because 9 == 1 (modulo 8) and shifting a u8 by 9 bits
	     * would not be meaningful.
	     */
	    OP_SHIFT_LEFT,  // Matching integer types -> Integer type
	    /**
	     * @breif Bitwise Shift Right
	     *
	     * @details
	     * This opcode performs a bitwise shift right of the first
	     * operand by the number of bits in the second operand (modulo the
	     * number of bits in the first operand).
	     * Both operands must be unsigned integers.
	     *
	     * For example, if you shift left on a u16 by 17 bits,
	     * the result is the same as shifting right by 1 bit
	     * because 17 == 1 (modulo 16) and shifting a u8 by 17 bits
	     * would not be meaningful.
	     */
	    OP_SHIFT_RIGHT, // Matching integer types -> Integer type

	    // Binary operations: comparisons
	    /**
	     * @brief Compare numbers for less-than.
	     *
	     * @details
	     * This opcode performs a comparison of the two operands
	     * and returns a boolean true if the first operand is
	     * numerically less than the second.  Both operands
	     * must be numeric types of the same and must be both
	     * signed, unsigned, or floating-point numbers.
	     */
	    OP_COMPARE_LT,  // Primitive types -> Boolean
	    /**
	     * @brief Compare numbers for greater-than.
	     *
	     * @details
	     * This opcode performs a comparison of the two operands
	     * and returns a boolean true if the first operand is
	     * numerically greater than the second.  Both operands
	     * must be numeric types of the same and must be both
	     * signed, unsigned, or floating-point numbers.
	     */
	    OP_COMPARE_GT,  // Primitive types -> Boolean
	    /**
	     * @brief Compare numbers for less-than or equal to.
	     *
	     * @details
	     * This opcode performs a comparison of the two operands
	     * and returns a boolean true if the first operand is
	     * numerically less than or equal to the second.  Both operands
	     * must be numeric types of the same and must be both
	     * signed, unsigned, or floating-point numbers.
	     */
	    OP_COMPARE_LE,  // Primitive types -> Boolean
	    /**
	     * @brief Compare numbers for greater-than or equal to.
	     *
	     * @details
	     * This opcode performs a comparison of the two operands
	     * and returns a boolean true if the first operand is
	     * numerically greater than or equal to the second.  Both operands
	     * must be numeric types of the same and must be both
	     * signed, unsigned, or floating-point numbers.
	     */
	    OP_COMPARE_GE,  // Primitive types -> Boolean
	    /**
	     * @brief Compare numbers for not equal-to.
	     *
	     * @details
	     * This opcode performs a comparison of the two operands
	     * and returns a boolean true if the first operand is
	     * numerically not equal to the second.  Both operands
	     * must be numeric types of the same and must be both
	     * signed, unsigned, or floating-point numbers.
	     */
	    OP_COMPARE_NE,  // Pointer types, Primitive types, recursive for class types.
	    /**
	     * @brief Compare numbers for equality
	     *
	     * @details
	     * This opcode performs a comparison of the two operands
	     * and returns a boolean true if the first operand is
	     * numerically equal to the second.  Both operands
	     * must be numeric types of the same and must be both
	     * signed, unsigned, or floating-point numbers.
	     */
	    OP_COMPARE_EQ,  // Pointer types, Primitive types, recursive for class types.
	    
	    // Binary operations: assignments
	    /**
	     * @brief Assign value
	     *
	     * @details
	     * This opcode assigns the value of the second operand
	     * to the lvalue represented by the first operand.  The
	     * value being assigned must be an lvalue which means that
	     * it must be a variable or an expression that can be
	     * assigned to such as a pointer-indirection to a storage
	     * location.
	     */
	    OP_ASSIGN,      // Primitive types, pointer types, recursive for class types.

	    // Branch and flow control
	    /**
	     * @brief Conditional Jump
	     *
	     * @details
	     * This opcode performs a jump to one of two BasicBlocks
	     * based on the condition found in the first operand.
	     * The first operand must be a boolean value indicating
	     * which jump path to take.  The second operand is the
	     * location to jump to if the condition is true and the
	     * third operand is the location to jump to if the
	     * condition if false.
	     */
	    OP_JUMP_IF_EQUAL, // Boolean types
	    /**
	     * @brief Jump (Unconditional)
	     *
	     * @details
	     * This opcode performs a jump to another BasicBlock
	     * unconditionally.  The first (and only) operand is the
	     * ID of the basic block to jump to.
	     */
	    OP_JUMP,
	    /**
	     * @brief Return from function
	     *
	     * @details
	     * This opcode returns from the current function, returning
	     * the stack and control to the calling function.  A return-value
	     * is also provided in the first (and only) opcode indicating
	     * the value to return to the caller.  Note that this opcode
	     * does not provide a value because no further control-flow is
	     * possible in this basic-block or in this function at all, so
	     * providing a return-value from this opcode would be meaningless.
	     */
	    OP_RETURN
	} OperationType;

	/**
	 * Constructs an opcode of the given type, producing the given result.
	 * No operands are given.
	 */
	Operation(
	    OperationType _type,
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result
	    );
	/**
	 * Constructs an opcode of the given type, producing the given
	 * result and taking a single operand.  This is useful for
	 * constructing unary operations.
	 */
	Operation(
	    OperationType _type,
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    size_t _operand
	    );
	/**
	 * Constructs an opcode of the given type, producing the
	 * given result and taking two operands.  This is useful
	 * for constructing binary operations.
	 */
	Operation(
	    OperationType _type,
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    size_t _operand_a,
	    size_t _operand_b
	    );
	/**
	 * Constructs an opcode of the given type, producing the
	 * given result and taking three operands.  This is useful
	 * for some of the exceptional opcodes like conditional jump
	 * instructions.
	 */
	Operation(
	    OperationType _type,
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    size_t _operand_a,
	    size_t _operand_b,
	    size_t _operand_c
	    );
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	virtual ~Operation();

	/**
	 * This method prints debuginng information about
	 * the opcode to the given file handle.
	 */
	void dump(FILE *out) const;

	/**
	 * @brief Opcode of this operation.
	 *
	 * @details
	 * This method returns the opcode of the
	 * this operation.
	 */
	OperationType get_type() const;

	/**
	 * @brief Get the operands
	 *
	 * @details
	 * This method returns the operands provided
	 * when the operation was constructed.
	 */
	const std::vector<size_t> & get_operands() const;

	/**
	 * @brief Get the result of this operation.
	 *
	 * @details
	 * This returns the temporary variable ID for where
	 * the result of this operation should be placed.
	 */
	size_t get_result() const;

	/**
	 * @brief Get the reference to the source which originated this operation.
	 *
	 * @details
	 * This returns a reference to the source translation unit
	 * that originated this operation.  In most cases, this is
	 * the location of the operator found in the source-file such as a
	 * '*' or a '+' symbol.
	 */
	const JLang::context::SourceReference & get_source_ref() const;
	
    protected:
	OperationType type;
	const JLang::context::SourceReference & src_ref;
	std::vector<size_t> operands;
	size_t result;

	/**
	 * @brief Produce a description of the operation.
	 *
	 * @details
	 * This method returns a full description of the operation
	 * including the return-value, operands, and any other
	 * ancilary information about the operation.  This
	 * is a string that is constructed dynamically when
	 * needed and is used almost exclusively by the "dump"
	 * method to provide debuginng information.
	 */
	virtual std::string get_description() const;
	/**
	 * @brief Add an operand
	 *
	 * @details
	 * This method adds an operand to the
	 * operation.  It is used exclusively internally
	 * when constructing operations.
	 */
	void add_operand(size_t operand);
    };

    /**
     * @brief This subclass of Operation represents a unary operation.
     */
    class OperationUnary : public Operation {
    public:
	/**
	 * @brief Construct unary operation with a single operand.
	 * @details
	 * This constructor is used for unary operations
	 * which take a single operand and produce a single result.
	 */
	OperationUnary(
	    OperationType _type,
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    size_t _operand
	    );
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	virtual ~OperationUnary();
	/**
	 * @brief Returns the operand of the unary operation
	 */
	size_t get_a() const;
    };
    
    /**
     * @brief This subclass of OperationUnary represents a cast operation.
     */
    class OperationCast : public OperationUnary {
    public:
	/**
	 * @brief Construct cast operation with a single operand and a type
	 * @details
	 * This constructor is used for cast expressions
	 * which take a single operand and a reference to a type
	 * and produce a single result.
	 */
	OperationCast(
	    OperationType _type,
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    size_t _operand,
	    const Type *_cast_type
	    );
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	virtual ~OperationCast();
	/**
	 * @brief Get type being cast/converted to.
	 * @details
	 * This method returns the type of variable being cast or converted to.
	 */
	const Type* get_cast_type() const;
    protected:
	virtual std::string get_description() const;
    private:
	const Type *cast_type;
    };

    /**
     * @brief This subclass of Operation represents a binary operation.
     */
    class OperationBinary : public Operation {
    public:
	/**
	 * @brief Construct binary operation with a two operands operand.
	 * @details
	 * This constructor is used for binary operations
	 * which take a two operands and produce a single result.
	 */
	OperationBinary(
	    OperationType _type,
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    size_t _operand_a,
	    size_t _operand_b
	    );
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	virtual ~OperationBinary();
	/**
	 * @brief Access the first operand (a).
	 */
	size_t get_a() const;
	/**
	 * @brief Access the second operand (b).
	 */
	size_t get_b() const;
    };

    /**
     * @brief Function call (invoke) operation
     * @details
     * This subclass of Operation represents a function-call
     * taking an operand for the function and a list of operands,
     * one for each argument of the function.
     */
    class OperationFunctionCall : public Operation {
    public:
	/**
	 * @brief Create a function call operation.
	 * This constructor takes the function to be called (callee)
	 * and a list of other operands, one for each argument
	 * to the function.
	 */
	OperationFunctionCall(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    size_t _callee_tmpvar,
	    std::vector<size_t> _arg_args
	    );
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	virtual ~OperationFunctionCall();
    };

    /**
     * @brief Symbol-table lookup
     * @details
     * This subclass of Operation represents a lookup
     * of a symbol in the global symbol table for things
     * like function names and global variables.
     */
    class OperationSymbol : public Operation {
    public:
	/**
	 * @brief Construct symbol-table lookup operation.
	 * @details
	 * This constructor creates a symbol-table lookup
	 * operation to look up the symbol given.
	 */
	OperationSymbol(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    std::string _symbol_name
	    );
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	virtual ~OperationSymbol();
	/**
	 * Returns the name of the symbol to look up in the
	 * global symbol table.
	 */
	const std::string & get_symbol_name() const;
	
    protected:
	virtual std::string get_description() const;
    private:
	const std::string symbol_name;
    };

    /**
     * @brief This subclass of OperationBinary represents indexing an array.
     */
    class OperationArrayIndex : public OperationBinary {
    public:
	/**
	 * @brief Create an array index operation
	 * @details
	 * This constructor creates an array index operation
	 * from the index variable and the type of data
	 * contained in the array.
	 */
	OperationArrayIndex(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    size_t _array_tmpvar,
	    size_t _index_tmpvar
	    );
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	virtual ~OperationArrayIndex();
    private:
    };

    /**
     * @brief This subclass of Operation is used to access member variables of classes and other aggregate types by reference.
     * @details
     * This class represents accessing a class member
     * using the name of the member and the operand
     * which is a reference to the class itself.
     * The returned operation will be an lvalue if the
     * member variable is an lvalue (which is almost certainly will be).
     */
    class OperationDot : public Operation {
    public:
	/**
	 * Constructs a member-access operation.  The first
	 * operand is the class or structure to access and
	 * the member name is the name of the member that
	 * should be accessed.
	 */
	OperationDot(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    size_t _operand,
	    std::string _member_name
	    );
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	virtual ~OperationDot();
	/**
	 * Returns the name of the member varialbe to
	 * access.
	 */
	const std::string & get_member_name() const;
	
    protected:
	virtual std::string get_description() const;
    private:
	const std::string member_name;
    };

    /**
     * @brief This subclass of Operation is used to access member variables of classes and other aggregate types by pointer.
     * @details
     * This class represents accessing a class member
     * using the name of the member and the operand
     * which is a pointer to the class itself.
     * The returned operation will be an lvalue if the
     * member variable is an lvalue (which is almost certainly will be).
     */
    class OperationArrow : public Operation {
    public:
	/**
	 * Constructs an accessor for a class or structure
	 * by providing the pointer to the class as the operand
	 * and the name of the member variable to be accessed
	 * is specified also.
	 */
	OperationArrow(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    size_t _operand,
	    std::string _member_name
	    );
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	virtual ~OperationArrow();
	const std::string & get_member_name() const;
	
    protected:
	virtual std::string get_description() const;
    private:
	const std::string member_name;
    };
    /**
     * @brief Load a local variable
     * @details
     * This operation is a load of a local variable
     * into a register (see OP_LOCAL_VARIABLE).
     */
    class OperationLocalVariable : public Operation {
    public:
	/**
	 * Construct an operation to load a local
	 * variable given by symbol_name into
	 * the operation's return-value.
	 */
	OperationLocalVariable(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    std::string _symbol_name,
	    const Type * _var_type);
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	virtual ~OperationLocalVariable();
	const std::string & get_symbol_name() const;
	const Type * get_var_type() const;
	
    protected:
	virtual std::string get_description() const;
    private:
	const std::string symbol_name;
	const Type * var_type;
    };

    /**
     * @brief Literal character
     * @details
     * This operation loads a constant character (u8)
     * literal into the return-value.  See OP_LITERAL_CHAR.
     */
    class OperationLiteralChar : public Operation {
    public:
	/**
	 * Loads the literal character constant into
	 * the return-value.
	 */
	OperationLiteralChar(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    std::string _literal_char
	    );
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	virtual ~OperationLiteralChar();
	const std::string & get_literal_char() const;
    protected:
	virtual std::string get_description() const;
    private:
	const std::string literal_char;
    };
    /**
     * @brief Literal string
     * @details
     * This operation loads a constant character (u8)
     * literal into the return-value.  See OP_LITERAL_STRING.
     */
    class OperationLiteralString : public Operation {
    public:
	/**
	 * Loads the literal string (u8*) constant into
	 * the return-value.  The string literal is
	 * stored in the global variable area
	 * and a pointer to it is returned in the return-value.
	 */
	OperationLiteralString(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    std::string _literal_string
	    );
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	virtual ~OperationLiteralString();
	const std::string & get_literal_string() const;
    protected:
	virtual std::string get_description() const;
    private:
	const std::string literal_string;
    };
    /**
     * @brief Literal integer
     * @details
     * This operation loads a constant integer one of (i8,i16,i32,i64,u8,u16,u32,u64)
     * into the return-value.  See OP_LITERAL_INT
     */
    class OperationLiteralInt : public Operation {
    public:
	/**
	 * Create an operation to load the given constant
	 * literal integer into the return-value.
	 */
	OperationLiteralInt(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    std::string _literal_int,
	    const Type *_type
	    );
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	virtual ~OperationLiteralInt();
	const std::string & get_literal_int() const;
	const Type *get_literal_type() const;
    protected:
	virtual std::string get_description() const;
    private:
	const std::string literal_int;
	const Type *literal_type;
    };
    /**
     * @brief Literal float
     * @details
     * This operation loads a constant integer one of (f32,f64)
     * into the return-value.  See OP_LITERAL_FLOAT
     */
    class OperationLiteralFloat : public Operation {
    public:
	/**
	 * Create an operation to load the given constant
	 * literal float into the return-value.
	 */
	OperationLiteralFloat(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    std::string _literal_float
	    );
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	virtual ~OperationLiteralFloat();
	const std::string & get_literal_float() const;
    protected:
	virtual std::string get_description() const;
    private:
	const std::string literal_float;
    };
    
    class OperationSizeofType : public Operation {
    public:
	OperationSizeofType(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _result,
	    const Type *_type
	    );
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	virtual ~OperationSizeofType();

	const Type * get_type() const;
    private:
	const Type* type;
    };

    class OperationJumpIfEqual : public Operation {
    public:
	OperationJumpIfEqual(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _operand,
	    size_t _if_block,
	    size_t _else_block
	    );
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	virtual ~OperationJumpIfEqual();
    protected:
	virtual std::string get_description() const;
    private:
    };
    
    class OperationJump : public Operation {
    public:
	OperationJump(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _block
	    );
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	virtual ~OperationJump();
    protected:
	virtual std::string get_description() const;
    private:
    };
    
    class OperationReturn : public Operation {
    public:
	OperationReturn(
	    const JLang::context::SourceReference & _src_ref,
	    size_t _operand
	    );
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	virtual ~OperationReturn();
    protected:
	virtual std::string get_description() const;
    };

    class OperationLocalDeclare : public Operation {
    public:
	OperationLocalDeclare(
	    const JLang::context::SourceReference & _src_ref,
	    std::string _variable,
	    std::string _var_type
	    );
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	virtual ~OperationLocalDeclare();
	const std::string & get_variable() const;
	const std::string & get_var_type() const;
    protected:
	virtual std::string get_description() const;
    private:
	std::string variable;
	std::string var_type;
    };
    class OperationLocalUndeclare : public Operation {
    public:
	OperationLocalUndeclare(
	    const JLang::context::SourceReference & _src_ref,
	    std::string _variable
	    );
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	virtual ~OperationLocalUndeclare();
    protected:
	virtual std::string get_description() const;
    private:
	std::string variable;
    };

    
    void operation_static_init();
};
