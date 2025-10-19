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

#include <gyoji-misc/pointers.hpp>
#include <gyoji-mir/types.hpp>
#include <gyoji-context.hpp>

#include <string>
#include <map>
#include <vector>

namespace Gyoji::mir {
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
	/**
	 * @brief Operations of the MIR virtual-machine.
	 *
	 * @details
	 * This type represents the various operations or
	 * instructions that the virtual machine of the MIR
	 * can perform.  Each operation performs some basic
	 * computation on the values of the 'registers' of the
	 * machine which are given by operands.  The operations
	 * also produce a result called the 'return-value' of the
	 * operation.  Most operations take three operands (i.e. three-address-code)
	 * however, some operations take fewer and a few operations
	 * take more.  Generally speaking, however, these operations
	 * are as primitive as it is possible to make them
	 * and most of the time, they will result in a single
	 * instruction on the target platform.
	 */
	typedef enum {
	    // Global symbols
	    /**
	     * @brief Function call
	     *
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
	     * @brief Load method call to that it can be called with OP_FUNCTION_CALL later.
	     *
	     * @details
	     * Directs the MIR to load the object to be called
	     * and the specific method to call so that it can
	     * be used by the OP_FUNCTION_CALL to make the actual
	     * function call.
	     */
	    OP_GET_METHOD,

	    OP_METHOD_GET_FUNCTION,
	    
	    OP_METHOD_GET_OBJECT,
	    
	    /**
	     * @brief Load a symbol from the symbol-table.
	     *
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
	     *
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
	     *
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

	    /**
	     * @brief Access a value inside an array
	     *
	     * @details
	     * This opcode allows access to values stored inside an array.
	     * It takes two operands, the first must be an array and the second
	     * is an index into the array.  The index into the array must
	     * be an unsigned integer type.
	     *
	     * The return-value of the opcode is of the same type as the
	     * elements of the array.  The returned value is an lvalue
	     * so it can be accessed or assigned.
	     *
	     * Examples:
	     * @code{.unparsed}
	     * _0 : local-declare ( u8[] )
	     * _1 : u32 = literal-int( 2 )
	     * _3 : u8 = array-index( _0 _1 )
	     * @endcode
	     */
	    OP_ARRAY_INDEX,     // Pointer types -> Pointer Target type

	    /**
	     * @brief Access a value inside a class
	     *
	     * @details
	     * This opcode is used to access values contained
	     * in a class structure.  The access may be for
	     * values (member variables) or functions (methods).
	     *
	     * For member variables, returned type is the same as the same as the
	     * type of the member variable and the value is an lvalue
	     * so it can be the target of an assignment.  For methods,
	     * the returned type is a function pointer whose signature
	     * is the same as the method and the value is not an lvalue
	     * meaning that it can be read, but not written to.
	     *
	     * Examples:
	     * @code{.unparsed}
	     * _0 : local-declare ( class { u8 : c } )
	     * _1 : u8 = member-access ( _0 "c" )
	     * @endcode
	     */
	    OP_DOT,             // class types -> Found member type

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
	     *
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
	     */
	    OP_LOCAL_UNDECLARE, // N/A
	    
	    /**
	     * @brief Load value from variable.
	     *
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
	     * data area.  The data carried here is the exact
	     * binary data to be placed into the static section.
	     * Any C-style escape sequences should already have been
	     * interpreted before this is used.  No assumptions about
	     * the data are made except that it is a null-terminated
	     * sequence of bytes.
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
	    /**
	     * @brief Loads a boolean literal value.
	     *
	     * @details
	     * This loads 'true' or 'false' into
	     * a literal constant.
	     * @code{.unparsed}
	     * _0: bool = literal_bool( true )
	     * @endcode
	     */
	    OP_LITERAL_BOOL,

	    /**
	     * @brief Loads a null pointer literal constant.
	     *
	     * @details
	     * This loads a literal 'null' pointer value
	     * for use in pointer comparison and assingments.
	     *
	     * @code{.unparsed}
	     * _0: bool = literal_null( )
	     * @endcode
	     */
	    OP_LITERAL_NULL,
	    
	    /**
	     * @brief Returns the address of the given variable.
	     *
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
	     * @brief Logical AND
	     *
	     * @details
	     * This opcode performs a boolean AND on the two opcodes
	     * and returns a boolean value as the result.  Both
	     * operands must be boolean values.
	     */
	    OP_LOGICAL_AND, // Boolean types -> Boolean
	    /**
	     * @brief Logical OR
	     *
	     * @details
	     * This opcode performs a boolean OR on the two opcodes
	     * and returns a boolean value as the result.  Both
	     * operands must be boolean values.
	     */
	    OP_LOGICAL_OR,  // Boolean types -> Boolean

	    // Binary operations: bitwise
	    /**
	     * @brief Bitwise AND
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
	     * @brief Bitwise OR
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
	     * @brief Bitwise XOR
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
	     * @brief Bitwise Shift left
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
	     * @brief Bitwise Shift Right
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
	    OP_COMPARE_LESS,  // Primitive types -> Boolean
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
	    OP_COMPARE_GREATER,  // Primitive types -> Boolean
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
	    OP_COMPARE_LESS_EQUAL,  // Primitive types -> Boolean
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
	    OP_COMPARE_GREATER_EQUAL,  // Primitive types -> Boolean
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
	    OP_COMPARE_NOT_EQUAL,  // Pointer types, Primitive types, recursive for class types.
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
	    OP_COMPARE_EQUAL,  // Pointer types, Primitive types, recursive for class types.
	    
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
	    OP_JUMP_CONDITIONAL, // Boolean types
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
	    OP_RETURN,
	    /**
	     * @brief Return from function without supplying a value.
	     *
	     * @details
	     * This opcode returns from the current function, returning
	     * the stack and control to the calling function.  A return-value
	     * is not supplied in this case (or returning the 'void' value).
	     */
	    OP_RETURN_VOID
	} OperationType;

	/**
	 * Constructs an opcode of the given type, producing the given result.
	 * No operands are given.
	 */
	Operation(
	    OperationType _type,
	    const Gyoji::context::SourceReference & _src_ref,
	    size_t _result
	    );
	/**
	 * Constructs an opcode of the given type, producing the given
	 * result and taking a single operand.  This is useful for
	 * constructing unary operations.
	 */
	Operation(
	    OperationType _type,
	    const Gyoji::context::SourceReference & _src_ref,
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
	    const Gyoji::context::SourceReference & _src_ref,
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
	    const Gyoji::context::SourceReference & _src_ref,
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
	 * @brief Returns true if this is a terminating operation for a block.
	 *
	 * @details
	 * This function returns true if the operation would terminate
	 * a basic block.  This happens if the operation is a jump,
	 * conditional branch, or return statement.
	 */
	bool is_terminating() const;

	/**
	 * @brief Returns the list of basic blocks we might connect to.
	 *
	 * @details
	 * This function returns the basic blocks this operation is connected
	 * to.  If it is a JUMP or JUMP_CONDITIONAL, it will return the blocks
	 * we are jumping to.  If it is any other type (including a return)
	 * then it returns nothing.
	 */
	std::vector<size_t> get_connections() const;
	/**
	 * @brief Get the reference to the source which originated this operation.
	 *
	 * @details
	 * This returns a reference to the source translation unit
	 * that originated this operation.  In most cases, this is
	 * the location of the operator found in the source-file such as a
	 * '*' or a '+' symbol.
	 */
	const Gyoji::context::SourceReference & get_source_ref() const;
	
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

#if 0
	// XXX Is this really the way to handle this?
	/**
	 * Returns true if this operation
	 * de-references or reads from the value
	 * given.  For example, if this is an 'rvalue' of
	 * an assignment operation, this will be true.
	 * This allows an 'imperfect' pass on read-before-write
	 * error checking.  In fact, this does give us the equivalent
	 * set of rules as Rust since Rust doesn't allow borrow or
	 * pointer to uninitialized data.
	 */
	bool get_readsfrom(size_t tmpvar) const;
	bool get_writesto(size_t tmpvar) const;
#endif
    protected:
	OperationType type;
	const Gyoji::context::SourceReference & src_ref;
	std::vector<size_t> operands;
	size_t result;

	/**
	 * @brief Add an operand
	 *
	 * @details
	 * This method adds an operand to the
	 * operation.  It is used exclusively internally
	 * when constructing operations.
	 */
	void add_operand(size_t operand);

#if 0
	// XXX Is this really the way to handle this?
	bool contains(size_t tmpvar) const;
#endif
    };

    /**
     * @brief This subclass of Operation represents a unary operation.
     * 
     * @details
     * This subclass of Operation is mainly a convenience container for unary operations.
     * It provides convenient methods to access the 'a' operand
     * of the operation and is used for most of the ordinary unary operation
     * opcodes such as OP_NEGATE and OP_LOGICAL_NOT, etc.  See those opcodes
     * for details of what types of values are supported and returned.
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
	    const Gyoji::context::SourceReference & _src_ref,
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
     *
     * @details
     * This class is used to represent opcodes whose first argument
     * is a value and the second argument is a type.  It is used
     * for opcodes that in some way manipulate the data to conform
     * to the given type.  In some cases, the value itself is not
     * modified and is returned exactly as it was with only new type
     * information (reinterpret casts), and in some cases, the value itself changes
     * such as sign extending integers or conversions from integers to
     * floating-point numbers.  Each individual opcode carries the
     * rules for these conversions.  This class is merely a container
     * for instances of those operations.
     */
    class OperationCast : public OperationUnary {
    public:
	/**
	 * @brief Construct cast operation with a single operand and a type
	 *
	 * @details
	 * This constructor is used for cast expressions
	 * which take a single operand and a reference to a type
	 * and produce a single result.
	 */
	OperationCast(
	    OperationType _type,
	    const Gyoji::context::SourceReference & _src_ref,
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
	 *
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
     *
     * @details
     * This subclass of Operation is mainly a convenience container for binary operations.
     * It provides convenient methods to access the 'a' and 'b' operands
     * of the operation and is used for most of the ordinary binary operation
     * opcodes such as OP_ADD and OP_MULTIPLY, etc.  See those opcodes
     * for details of what types of values are supported and returned.
     */
    class OperationBinary : public Operation {
    public:
	/**
	 * @brief Construct binary operation with a two operands operand.
	 *
	 * @details
	 * This constructor is used for binary operations
	 * which take a two operands and produce a single result.
	 */
	OperationBinary(
	    OperationType _type,
	    const Gyoji::context::SourceReference & _src_ref,
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
     * @brief Method call (invoke) preparation
     * @details
     * This subclass of Operation represents
     * obtaining the necessary information to
     * perform a method call.  This consists
     * of a pair of the specific object to call
     * the method on and the function-pointer of
     * the method to call.  This does not
     * actually perform the method call.  That is left
     * to the OperationFunctionCall which must come
     * afterward.  Technically, this isn't necessary
     * in the VM, but modelling the calls this way
     * makes the 'lowering' code simpler.
     */
    class OperationGetMethod : public Operation {
    public:
	OperationGetMethod(
	    const Gyoji::context::SourceReference & _src_ref,
	    size_t _result,
	    size_t _object_to_call,
	    std::string _method_name
	    );
	virtual ~OperationGetMethod();

	const std::string & get_method() const;
    private:
	size_t object_to_call;
	std::string method_name;
    };

    /**
     * @brief Function call (invoke) operation
     *
     * @details
     * This subclass of Operation represents a function-call
     * taking an operand for the function and a list of operands,
     * one for each argument of the function.
     */
    class OperationFunctionCall : public Operation {
    public:
	/**
	 * @brief Create a function call operation.
	 *
	 * @details
	 * This constructor takes the function to be called (callee)
	 * and a list of other operands, one for each argument
	 * to the function.
	 */
	OperationFunctionCall(
	    const Gyoji::context::SourceReference & _src_ref,
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
     *
     * @details
     * This subclass of Operation represents a lookup
     * of a symbol in the global symbol table for things
     * like function names and global variables.
     */
    class OperationSymbol : public Operation {
    public:
	/**
	 * @brief Construct symbol-table lookup operation.
	 *
	 * @details
	 * This constructor creates a symbol-table lookup
	 * operation to look up the symbol given.
	 */
	OperationSymbol(
	    const Gyoji::context::SourceReference & _src_ref,
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
     *
     * @details
     * See OP_ARRAY_INDEX for details.
     */
    class OperationArrayIndex : public OperationBinary {
    public:
	/**
	 * @brief Create an array index operation
	 *
	 * @details
	 * This constructor creates an array index operation
	 * from the index variable and the type of data
	 * contained in the array.
	 */
	OperationArrayIndex(
	    const Gyoji::context::SourceReference & _src_ref,
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
     *
     * @details
     * This class represents accessing a class member
     * using the name of the member and the operand
     * which is a reference to the class itself.
     * The returned operation will be an lvalue if the
     * member variable is an lvalue (which is almost certainly will be).
     */
    class OperationDot : public OperationUnary {
    public:
	/**
	 * @brief This class represents class/structure member access operations
	 *
	 * @details
	 * Constructs a member-access operation.  The first
	 * operand is the class or structure to access and
	 * the member name is the name of the member that
	 * should be accessed.
	 */
	OperationDot(
	    const Gyoji::context::SourceReference & _src_ref,
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
     * @brief Load a local variable
     *
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
	    const Gyoji::context::SourceReference & _src_ref,
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
     *
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
	    const Gyoji::context::SourceReference & _src_ref,
	    size_t _result,
	    char _literal_char
	    );
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	virtual ~OperationLiteralChar();
	char get_literal_char() const;
    protected:
	virtual std::string get_description() const;
    private:
	const char literal_char;
    };
    /**
     * @brief Literal string
     *
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
	    const Gyoji::context::SourceReference & _src_ref,
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
     *
     * @details
     * This operation loads a constant integer one of (i8,i16,i32,i64,u8,u16,u32,u64)
     * into the return-value.  See OP_LITERAL_INT.  The type of data
     * returned depends exactly on the type of data given
     * and no type conversions are done at this stage.  If type
     * conversions are needed, they should be performed
     * ahead of time using the OP_WIDEN_SIGNED or other cast operations
     * depending on the type of data needed.
     */
    class OperationLiteralInt : public Operation {
    public:
	/**
	 * Create an operation to load the given constant
	 * literal integer into the return-value.
	 * The type of data given here is an unsigned 8-bit
	 * value.
	 */
	OperationLiteralInt(
	    const Gyoji::context::SourceReference & _src_ref,
	    size_t _result,
	    Type::TypeType _literal_type,
	    unsigned char _literal_u8
	    );
	/**
	 * Create an operation to load the given constant
	 * literal integer into the return-value.
	 * The type of data given here is an unsigned 16-bit
	 * value.
	 */
	OperationLiteralInt(
	    const Gyoji::context::SourceReference & _src_ref,
	    size_t _result,
	    Type::TypeType _literal_type,
	    unsigned short _literal_u16
	    );
	/**
	 * Create an operation to load the given constant
	 * literal integer into the return-value.
	 * The type of data given here is an unsigned 32-bit
	 * value.
	 */
	OperationLiteralInt(
	    const Gyoji::context::SourceReference & _src_ref,
	    size_t _result,
	    Type::TypeType _literal_type,
	    unsigned int _literal_u32
	    );
	/**
	 * Create an operation to load the given constant
	 * literal integer into the return-value.
	 * The type of data given here is an unsigned 64-bit
	 * value.
	 */
	OperationLiteralInt(
	    const Gyoji::context::SourceReference & _src_ref,
	    size_t _result,
	    Type::TypeType _literal_type,
	    unsigned long _literal_u64
	    );
	/**
	 * Create an operation to load the given constant
	 * literal integer into the return-value.
	 * The type of data given here is a signed 8-bit
	 * value.
	 */
	OperationLiteralInt(
	    const Gyoji::context::SourceReference & _src_ref,
	    size_t _result,
	    Type::TypeType _literal_type,
	    char _literal_i8
	    );
	/**
	 * Create an operation to load the given constant
	 * literal integer into the return-value.
	 * The type of data given here is a signed 16-bit
	 * value.
	 */
	OperationLiteralInt(
	    const Gyoji::context::SourceReference & _src_ref,
	    size_t _result,
	    Type::TypeType _literal_type,
	    short _literal_i16
	    );
	/**
	 * Create an operation to load the given constant
	 * literal integer into the return-value.
	 * The type of data given here is a signed 32-bit
	 * value.
	 */
	OperationLiteralInt(
	    const Gyoji::context::SourceReference & _src_ref,
	    size_t _result,
	    Type::TypeType _literal_type,
	    int _literal_i32
	    );
	/**
	 * Create an operation to load the given constant
	 * literal integer into the return-value.
	 * The type of data given here is a signed 64-bit
	 * value.
	 */
	OperationLiteralInt(
	    const Gyoji::context::SourceReference & _src_ref,
	    size_t _result,
	    Type::TypeType _literal_type,
	    long _literal_i64
	    );
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	virtual ~OperationLiteralInt();
	Type::TypeType get_literal_type() const;
	unsigned char get_literal_u8() const;
	unsigned short get_literal_u16() const;
	unsigned int get_literal_u32() const;
	unsigned long get_literal_u64() const;

	char get_literal_i8() const;
	short get_literal_i16() const;
	int get_literal_i32() const;
	long get_literal_i64() const;

    protected:
	virtual std::string get_description() const;
    private:
	Type::TypeType literal_type;

	// Should these be a union?
	unsigned char literal_u8;
	unsigned short literal_u16;
	unsigned int literal_u32;
	unsigned long literal_u64;
	char literal_i8;
	short literal_i16;
	int literal_i32;
	long literal_i64;
    };
    /**
     * @brief Literal float
     *
     * @details
     * This operation loads a constant integer one of (f32,f64)
     * into the return-value.  See OP_LITERAL_FLOAT
     */
    class OperationLiteralFloat : public Operation {
    public:
	/**
	 * Create an operation to load the given constant
	 * literal float into the return-value.
	 * The value is a 32 bit floating-point number.
	 */
	OperationLiteralFloat(
	    const Gyoji::context::SourceReference & _src_ref,
	    size_t _result,
	    float _literal_float
	    );
	/**
	 * Create an operation to load the given constant
	 * literal float into the return-value.
	 * The value is a 64 bit floating-point number.
	 */
	OperationLiteralFloat(
	    const Gyoji::context::SourceReference & _src_ref,
	    size_t _result,
	    double _literal_float
	    );
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	virtual ~OperationLiteralFloat();
	float get_literal_float() const;
	double get_literal_double() const;
	Type::TypeType get_literal_type() const;
    protected:
	virtual std::string get_description() const;
    private:
	Type::TypeType literal_type;
	float literal_float_f32;
	double literal_float_f64;
	
    };

    /**
     * @brief Literal bool
     *
     * @details
     * This operation loads a literal boolean (true or false)
     * value into the return-value.
     * See OP_LITERAL_BOOL
     */
    class OperationLiteralBool : public Operation {
    public:
	/**
	 * Create an operation to load the given constant
	 * literal boolean into the return-value.
	 */
	OperationLiteralBool(
	    const Gyoji::context::SourceReference & _src_ref,
	    size_t _result,
	    bool _literal_bool
	    );
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	virtual ~OperationLiteralBool();
	/**
	 * This returns the value of the boolean
	 * constant (true or false).
	 */
	bool get_literal_bool() const;
    protected:
	virtual std::string get_description() const;
    private:
	bool literal_bool;
    };

    /**
     * @brief Literal null
     *
     * @details
     * This operation loads a literal null pointer
     * value into the return-value.
     * See OP_LITERAL_NULL
     */
    class OperationLiteralNull : public Operation {
    public:
	/**
	 * Create an operation to load the given constant
	 * literal float into the return-value.
	 */
	OperationLiteralNull(
	    const Gyoji::context::SourceReference & _src_ref,
	    size_t _result
	    );
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	virtual ~OperationLiteralNull();
    protected:
	virtual std::string get_description() const;
    private:
    };

    /**
     * @brief The operation for obtaining the storage size of a type.
     *
     * @details
     * This operation returns the size of the given type.  It
     * takes no operands and is only used by the code-generator
     * to extract the storage size of the given type.
     */
    class OperationSizeofType : public Operation {
    public:
	OperationSizeofType(
	    const Gyoji::context::SourceReference & _src_ref,
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

    /**
     * @brief This operation represents a conditional jump.
     *
     * @details
     * This operation takes three operands.  The first is
     * a condition which is expected to be a boolean
     * value.  The second operand is not a value, but
     * instead is the ID of the basic-block to jump to
     * if the condition evaluates to 'true'.  The third
     * operand is the ID of the basic block to jump to
     * if the condition evaluates to 'false'.
     *
     * This operand must be the last opcode
     * in a basic block because it terminates the
     * execution of that block and no further
     * operations will be executed in that block.
     * Any operations after a jump will be considered
     * 'unreachable' and may trigger a compile error
     * or dropped with a warning.
     */
    class OperationJumpConditional : public Operation {
    public:
	OperationJumpConditional(
	    const Gyoji::context::SourceReference & _src_ref,
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
	virtual ~OperationJumpConditional();
	size_t get_if_block() const;
	size_t get_else_block() const;
    protected:
	virtual std::string get_description() const;
	size_t if_block;
	size_t else_block;
    private:
    };

    /**
     * @brief Unconditional Jump
     *
     * @details
     * This opcode represents an unconditional jump.
     * The only operand is the ID of the basic block
     * to jump to.
     *
     * This operand must be the last opcode
     * in a basic block because it terminates the
     * execution of that block and no further
     * operations will be executed in that block.
     * Any operations after a jump will be considered
     * 'unreachable' and may trigger a compile error
     * or dropped with a warning.
     */
    class OperationJump : public Operation {
    public:
	OperationJump(
	    const Gyoji::context::SourceReference & _src_ref,
	    size_t _block
	    );
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	virtual ~OperationJump();
	size_t get_jump_block() const;
    protected:
	virtual std::string get_description() const;
	size_t jump_block;
    private:
    };

    /**
     * @brief Return from a function with a value
     *
     * @details
     * This opcode causes the virtual machine
     * to exit from the current function and
     * return to the caller, supplying the value
     * given.  The value given in the return is expected
     * to be the same type as the function's return-value.
     *     
     * This operand must be the last opcode
     * in a basic block because it terminates the
     * execution of that block and no further
     * operations will be executed in that block.
     * Any operations after a jump will be considered
     * 'unreachable' and may trigger a compile error
     * or dropped with a warning.
     */
    class OperationReturn : public Operation {
    public:
	OperationReturn(
	    const Gyoji::context::SourceReference & _src_ref,
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
    /**
     * @brief Return from a function without a value.
     *
     * This returns 'void' from a function (i.e. it
     * returns control to the caller without supplying
     * a return value to it.
     */
    class OperationReturnVoid : public Operation {
    public:
	OperationReturnVoid(
	    const Gyoji::context::SourceReference & _src_ref
	    );
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	virtual ~OperationReturnVoid();
    protected:
	virtual std::string get_description() const;
    };

    /**
     * @brief Declare a local variable in scope.
     *
     * @details
     * This class represents declaring a variable
     * inside the scope of a function.  This variable
     * may be in the main scope or may be in a sub-scope.
     * The name of the variable and the associated type
     * are given.  The variable is created with that
     * type so that it can be accessed later with an
     * access operation such as a 'load'.
     */
    class OperationLocalDeclare : public Operation {
    public:
	OperationLocalDeclare(
	    const Gyoji::context::SourceReference & _src_ref,
	    std::string _variable,
	    const Type *_variable_type
	    );
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	virtual ~OperationLocalDeclare();
	/**
	 * Name of the variable to declare.
	 */
	const std::string & get_variable() const;
	/**
	 * Returns a pointer to the immutable type
	 * of the variable.
	 */
	const Type *get_variable_type() const;
    protected:
	virtual std::string get_description() const;
    private:
	std::string variable;
	const Type *variable_type;
    };
    /**
     * @brief Un-declare a variable (remove it from scope)
     *
     * @details
     * This opcode is used to remove a variable when, for example,
     * it goes out of scope in a syntactical scope of the
     * source.  There may be other reasons for a variable to
     * be un-declared, but this is the most common.
     */
    class OperationLocalUndeclare : public Operation {
    public:
	OperationLocalUndeclare(
	    const Gyoji::context::SourceReference & _src_ref,
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
