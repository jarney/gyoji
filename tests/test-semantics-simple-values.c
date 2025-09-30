#include <stdio.h>
#include <stdlib.h>

#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long

#define i8 char
#define i16 short
#define i32 int
#define i64 long

#define f32 float
#define f64 double

#define DECLARE_BINARY_OPERATIONS_FULL(retjtype, ajtype, bjtype, fname) \
    retjtype c_##retjtype##fname(ajtype a, bjtype b); \
    retjtype j_##retjtype##fname(ajtype a, bjtype b);
    
#define TEST_BINARY_OPERATION_FULL(retjtype, ajtype, bjtype, retfmt, afmt, bfmt, fname) \
    {									\
	ajtype a##ajtype = (ajtype)a;					\
	bjtype b##bjtype = (bjtype)b;					\
	retjtype jret##retjtype = j_##retjtype##fname(a##ajtype, b##bjtype);	\
	retjtype cret##retjtype = c_##retjtype##fname(a##ajtype, b##bjtype);	\
	if (jret##retjtype != cret##retjtype) {				\
	    fprintf(stderr,						\
		    #retjtype #fname "(" afmt "," bfmt ") : was " retfmt " expected " retfmt "\n", \
		    a##ajtype, b##bjtype, jret##retjtype, cret##retjtype); \
	    return 1;							\
	}								\
    }									\

#define TEST_BINARY_OPERATION_FULL_FLOAT(retjtype, ajtype, bjtype, retfmt, afmt, bfmt, fname) \
    {									\
	ajtype a##ajtype = (ajtype)fa;					\
	bjtype b##bjtype = (bjtype)fb;					\
	retjtype jret##retjtype = j_##retjtype##fname(a##ajtype, b##bjtype);	\
	retjtype cret##retjtype = c_##retjtype##fname(a##ajtype, b##bjtype);	\
	if (jret##retjtype != cret##retjtype) {				\
	    fprintf(stderr,						\
		    #retjtype " " afmt " " bfmt " : " retfmt " " retfmt "\n",	\
		    a##ajtype, b##bjtype, jret##retjtype, cret##retjtype);	\
	    return 1;							\
	}								\
    }									\

//////////////////////////
// Addition
//////////////////////////
DECLARE_BINARY_OPERATIONS_FULL(u8, u8, u8, _add);

DECLARE_BINARY_OPERATIONS_FULL(u16, u16, u16, _add);
DECLARE_BINARY_OPERATIONS_FULL(u16, u8, u16, _add_widen_a);
DECLARE_BINARY_OPERATIONS_FULL(u16, u16, u8, _add_widen_b);

DECLARE_BINARY_OPERATIONS_FULL(u32, u32, u32, _add);
DECLARE_BINARY_OPERATIONS_FULL(u32, u16, u32, _add_widen_a);
DECLARE_BINARY_OPERATIONS_FULL(u32, u32, u16, _add_widen_b);

DECLARE_BINARY_OPERATIONS_FULL(u64, u64, u64, _add);
DECLARE_BINARY_OPERATIONS_FULL(u64, u32, u64, _add_widen_a);
DECLARE_BINARY_OPERATIONS_FULL(u64, u64, u32, _add_widen_b);

DECLARE_BINARY_OPERATIONS_FULL(i8, i8, i8, _add);

DECLARE_BINARY_OPERATIONS_FULL(i16, i16, i16, _add);
DECLARE_BINARY_OPERATIONS_FULL(i16, i8, i16, _add_widen_a);
DECLARE_BINARY_OPERATIONS_FULL(i16, i16, i8, _add_widen_b);

DECLARE_BINARY_OPERATIONS_FULL(i32, i32, i32, _add);
DECLARE_BINARY_OPERATIONS_FULL(i32, i16, i32, _add_widen_a);
DECLARE_BINARY_OPERATIONS_FULL(i32, i32, i16, _add_widen_b);

DECLARE_BINARY_OPERATIONS_FULL(i64, i64, i64, _add);
DECLARE_BINARY_OPERATIONS_FULL(i64, i32, i64, _add_widen_a);
DECLARE_BINARY_OPERATIONS_FULL(i64, i64, i32, _add_widen_b);


DECLARE_BINARY_OPERATIONS_FULL(f32, f32, f32, _add);
DECLARE_BINARY_OPERATIONS_FULL(f64, f64, f64, _add);
DECLARE_BINARY_OPERATIONS_FULL(f64, f32, f64, _add_widen_a);
DECLARE_BINARY_OPERATIONS_FULL(f64, f64, f32, _add_widen_b);

//////////////////////////
// Subtraction
//////////////////////////
DECLARE_BINARY_OPERATIONS_FULL(u8, u8, u8, _subtract);

DECLARE_BINARY_OPERATIONS_FULL(u16, u16, u16, _subtract);
DECLARE_BINARY_OPERATIONS_FULL(u16, u8, u16, _subtract_widen_a);
DECLARE_BINARY_OPERATIONS_FULL(u16, u16, u8, _subtract_widen_b);

DECLARE_BINARY_OPERATIONS_FULL(u32, u32, u32, _subtract);
DECLARE_BINARY_OPERATIONS_FULL(u32, u16, u32, _subtract_widen_a);
DECLARE_BINARY_OPERATIONS_FULL(u32, u32, u16, _subtract_widen_b);

DECLARE_BINARY_OPERATIONS_FULL(u64, u64, u64, _subtract);
DECLARE_BINARY_OPERATIONS_FULL(u64, u32, u64, _subtract_widen_a);
DECLARE_BINARY_OPERATIONS_FULL(u64, u64, u32, _subtract_widen_b);

DECLARE_BINARY_OPERATIONS_FULL(i8, i8, i8, _subtract);

DECLARE_BINARY_OPERATIONS_FULL(i16, i16, i16, _subtract);
DECLARE_BINARY_OPERATIONS_FULL(i16, i8, i16, _subtract_widen_a);
DECLARE_BINARY_OPERATIONS_FULL(i16, i16, i8, _subtract_widen_b);

DECLARE_BINARY_OPERATIONS_FULL(i32, i32, i32, _subtract);
DECLARE_BINARY_OPERATIONS_FULL(i32, i16, i32, _subtract_widen_a);
DECLARE_BINARY_OPERATIONS_FULL(i32, i32, i16, _subtract_widen_b);

DECLARE_BINARY_OPERATIONS_FULL(i64, i64, i64, _subtract);
DECLARE_BINARY_OPERATIONS_FULL(i64, i32, i64, _subtract_widen_a);
DECLARE_BINARY_OPERATIONS_FULL(i64, i64, i32, _subtract_widen_b);

DECLARE_BINARY_OPERATIONS_FULL(f32, f32, f32, _subtract);
DECLARE_BINARY_OPERATIONS_FULL(f64, f64, f64, _subtract);
DECLARE_BINARY_OPERATIONS_FULL(f64, f32, f64, _subtract_widen_a);
DECLARE_BINARY_OPERATIONS_FULL(f64, f64, f32, _subtract_widen_b);

//////////////////////////
// Multiplication
//////////////////////////
DECLARE_BINARY_OPERATIONS_FULL(u8, u8, u8, _multiply);

DECLARE_BINARY_OPERATIONS_FULL(u16, u16, u16, _multiply);
DECLARE_BINARY_OPERATIONS_FULL(u16, u8, u16, _multiply_widen_a);
DECLARE_BINARY_OPERATIONS_FULL(u16, u16, u8, _multiply_widen_b);

DECLARE_BINARY_OPERATIONS_FULL(u32, u32, u32, _multiply);
DECLARE_BINARY_OPERATIONS_FULL(u32, u16, u32, _multiply_widen_a);
DECLARE_BINARY_OPERATIONS_FULL(u32, u32, u16, _multiply_widen_b);

DECLARE_BINARY_OPERATIONS_FULL(u64, u64, u64, _multiply);
DECLARE_BINARY_OPERATIONS_FULL(u64, u32, u64, _multiply_widen_a);
DECLARE_BINARY_OPERATIONS_FULL(u64, u64, u32, _multiply_widen_b);

DECLARE_BINARY_OPERATIONS_FULL(i8, i8, i8, _multiply);

DECLARE_BINARY_OPERATIONS_FULL(i16, i16, i16, _multiply);
DECLARE_BINARY_OPERATIONS_FULL(i16, i8, i16, _multiply_widen_a);
DECLARE_BINARY_OPERATIONS_FULL(i16, i16, i8, _multiply_widen_b);

DECLARE_BINARY_OPERATIONS_FULL(i32, i32, i32, _multiply);
DECLARE_BINARY_OPERATIONS_FULL(i32, i16, i32, _multiply_widen_a);
DECLARE_BINARY_OPERATIONS_FULL(i32, i32, i16, _multiply_widen_b);

DECLARE_BINARY_OPERATIONS_FULL(i64, i64, i64, _multiply);
DECLARE_BINARY_OPERATIONS_FULL(i64, i32, i64, _multiply_widen_a);
DECLARE_BINARY_OPERATIONS_FULL(i64, i64, i32, _multiply_widen_b);

DECLARE_BINARY_OPERATIONS_FULL(f32, f32, f32, _multiply);
DECLARE_BINARY_OPERATIONS_FULL(f64, f64, f64, _multiply);
DECLARE_BINARY_OPERATIONS_FULL(f64, f32, f64, _multiply_widen_a);
DECLARE_BINARY_OPERATIONS_FULL(f64, f64, f32, _multiply_widen_b);

//////////////////////////
// Division
//////////////////////////
DECLARE_BINARY_OPERATIONS_FULL(u8, u8, u8, _divide);

DECLARE_BINARY_OPERATIONS_FULL(u16, u16, u16, _divide);
DECLARE_BINARY_OPERATIONS_FULL(u16, u8, u16, _divide_widen_a);
DECLARE_BINARY_OPERATIONS_FULL(u16, u16, u8, _divide_widen_b);

DECLARE_BINARY_OPERATIONS_FULL(u32, u32, u32, _divide);
DECLARE_BINARY_OPERATIONS_FULL(u32, u16, u32, _divide_widen_a);
DECLARE_BINARY_OPERATIONS_FULL(u32, u32, u16, _divide_widen_b);

DECLARE_BINARY_OPERATIONS_FULL(u64, u64, u64, _divide);
DECLARE_BINARY_OPERATIONS_FULL(u64, u32, u64, _divide_widen_a);
DECLARE_BINARY_OPERATIONS_FULL(u64, u64, u32, _divide_widen_b);

DECLARE_BINARY_OPERATIONS_FULL(i8, i8, i8, _divide);

DECLARE_BINARY_OPERATIONS_FULL(i16, i16, i16, _divide);
DECLARE_BINARY_OPERATIONS_FULL(i16, i8, i16, _divide_widen_a);
DECLARE_BINARY_OPERATIONS_FULL(i16, i16, i8, _divide_widen_b);

DECLARE_BINARY_OPERATIONS_FULL(i32, i32, i32, _divide);
DECLARE_BINARY_OPERATIONS_FULL(i32, i16, i32, _divide_widen_a);
DECLARE_BINARY_OPERATIONS_FULL(i32, i32, i16, _divide_widen_b);

DECLARE_BINARY_OPERATIONS_FULL(i64, i64, i64, _divide);
DECLARE_BINARY_OPERATIONS_FULL(i64, i32, i64, _divide_widen_a);
DECLARE_BINARY_OPERATIONS_FULL(i64, i64, i32, _divide_widen_b);

DECLARE_BINARY_OPERATIONS_FULL(f32, f32, f32, _divide);
DECLARE_BINARY_OPERATIONS_FULL(f64, f64, f64, _divide);
DECLARE_BINARY_OPERATIONS_FULL(f64, f32, f64, _divide_widen_a);
DECLARE_BINARY_OPERATIONS_FULL(f64, f64, f32, _divide_widen_b);

//////////////////////////
// Modulo
//////////////////////////
// Note, floating-point types f32 and f64 are not
// defined for the modulo operation.
DECLARE_BINARY_OPERATIONS_FULL(u8, u8, u8, _modulo);

DECLARE_BINARY_OPERATIONS_FULL(u16, u16, u16, _modulo);
DECLARE_BINARY_OPERATIONS_FULL(u16, u8, u16, _modulo_widen_a);
DECLARE_BINARY_OPERATIONS_FULL(u16, u16, u8, _modulo_widen_b);

DECLARE_BINARY_OPERATIONS_FULL(u32, u32, u32, _modulo);
DECLARE_BINARY_OPERATIONS_FULL(u32, u16, u32, _modulo_widen_a);
DECLARE_BINARY_OPERATIONS_FULL(u32, u32, u16, _modulo_widen_b);

DECLARE_BINARY_OPERATIONS_FULL(u64, u64, u64, _modulo);
DECLARE_BINARY_OPERATIONS_FULL(u64, u32, u64, _modulo_widen_a);
DECLARE_BINARY_OPERATIONS_FULL(u64, u64, u32, _modulo_widen_b);

DECLARE_BINARY_OPERATIONS_FULL(i8, i8, i8, _modulo);

DECLARE_BINARY_OPERATIONS_FULL(i16, i16, i16, _modulo);
DECLARE_BINARY_OPERATIONS_FULL(i16, i8, i16, _modulo_widen_a);
DECLARE_BINARY_OPERATIONS_FULL(i16, i16, i8, _modulo_widen_b);

DECLARE_BINARY_OPERATIONS_FULL(i32, i32, i32, _modulo);
DECLARE_BINARY_OPERATIONS_FULL(i32, i16, i32, _modulo_widen_a);
DECLARE_BINARY_OPERATIONS_FULL(i32, i32, i16, _modulo_widen_b);

DECLARE_BINARY_OPERATIONS_FULL(i64, i64, i64, _modulo);
DECLARE_BINARY_OPERATIONS_FULL(i64, i32, i64, _modulo_widen_a);
DECLARE_BINARY_OPERATIONS_FULL(i64, i64, i32, _modulo_widen_b);


int main(int argc, char **argv)
{
    for (unsigned int i = 0; i < 100; i++) {
	unsigned int a = rand();
	unsigned int b = rand();

	double fa = (3.14159265)*a;
	double fb = (3.14159265)*b;

//////////////////////////
// Addition
//////////////////////////
	TEST_BINARY_OPERATION_FULL(u8, u8, u8, "%d", "%d", "%d", _add);
	
	TEST_BINARY_OPERATION_FULL(u16, u16, u16, "%d", "%d", "%d", _add);
	
	TEST_BINARY_OPERATION_FULL(u32, u32, u32, "%d", "%d", "%d", _add);
	TEST_BINARY_OPERATION_FULL(u32, u16, u32, "%d", "%d", "%d", _add_widen_a);
	TEST_BINARY_OPERATION_FULL(u32, u32, u16, "%d", "%d", "%d", _add_widen_b);

    	TEST_BINARY_OPERATION_FULL(u64, u64, u64, "%ld", "%ld", "%ld", _add);
	TEST_BINARY_OPERATION_FULL(u64, u32, u64, "%ld", "%d", "%ld", _add_widen_a);
	TEST_BINARY_OPERATION_FULL(u64, u64, u32, "%ld", "%ld", "%d", _add_widen_b);

	TEST_BINARY_OPERATION_FULL(i8, i8, i8, "%d", "%d", "%d", _add);
	
	TEST_BINARY_OPERATION_FULL(i16, i16, i16, "%d", "%d", "%d", _add);
	
	TEST_BINARY_OPERATION_FULL(i32, i32, i32, "%d", "%d", "%d", _add);
	TEST_BINARY_OPERATION_FULL(i32, i16, i32, "%d", "%d", "%d", _add_widen_a);
	TEST_BINARY_OPERATION_FULL(i32, i32, i16, "%d", "%d", "%d", _add_widen_b);

    	TEST_BINARY_OPERATION_FULL(i64, i64, i64, "%ld", "%ld", "%ld", _add);
	TEST_BINARY_OPERATION_FULL(i64, i32, i64, "%ld", "%d", "%ld", _add_widen_a);
	TEST_BINARY_OPERATION_FULL(i64, i64, i32, "%ld", "%ld", "%d", _add_widen_b);

        TEST_BINARY_OPERATION_FULL_FLOAT(f32, f32, f32, "%f", "%f", "%f", _add);
        TEST_BINARY_OPERATION_FULL_FLOAT(f64, f64, f64, "%lf", "%lf", "%lf", _add);
        TEST_BINARY_OPERATION_FULL_FLOAT(f64, f32, f64, "%lf", "%f", "%lf", _add_widen_a);
        TEST_BINARY_OPERATION_FULL_FLOAT(f64, f64, f32, "%lf", "%lf", "%f", _add_widen_b);
//////////////////////////
// Subtraction
//////////////////////////
	TEST_BINARY_OPERATION_FULL(u8, u8, u8, "%d", "%d", "%d", _subtract);
	
	TEST_BINARY_OPERATION_FULL(u16, u16, u16, "%d", "%d", "%d", _subtract);
	
	TEST_BINARY_OPERATION_FULL(u32, u32, u32, "%d", "%d", "%d", _subtract);
	TEST_BINARY_OPERATION_FULL(u32, u16, u32, "%d", "%d", "%d", _subtract_widen_a);
	TEST_BINARY_OPERATION_FULL(u32, u32, u16, "%d", "%d", "%d", _subtract_widen_b);

    	TEST_BINARY_OPERATION_FULL(u64, u64, u64, "%ld", "%ld", "%ld", _subtract);
	TEST_BINARY_OPERATION_FULL(u64, u32, u64, "%ld", "%d", "%ld", _subtract_widen_a);
	TEST_BINARY_OPERATION_FULL(u64, u64, u32, "%ld", "%ld", "%d", _subtract_widen_b);

	TEST_BINARY_OPERATION_FULL(i8, i8, i8, "%d", "%d", "%d", _subtract);
	
	TEST_BINARY_OPERATION_FULL(i16, i16, i16, "%d", "%d", "%d", _subtract);
	
	TEST_BINARY_OPERATION_FULL(i32, i32, i32, "%d", "%d", "%d", _subtract);
	TEST_BINARY_OPERATION_FULL(i32, i16, i32, "%d", "%d", "%d", _subtract_widen_a);
	TEST_BINARY_OPERATION_FULL(i32, i32, i16, "%d", "%d", "%d", _subtract_widen_b);

    	TEST_BINARY_OPERATION_FULL(i64, i64, i64, "%ld", "%ld", "%ld", _subtract);
	TEST_BINARY_OPERATION_FULL(i64, i32, i64, "%ld", "%d", "%ld", _subtract_widen_a);
	TEST_BINARY_OPERATION_FULL(i64, i64, i32, "%ld", "%ld", "%d", _subtract_widen_b);

        TEST_BINARY_OPERATION_FULL_FLOAT(f32, f32, f32, "%f", "%f", "%f", _subtract);
        TEST_BINARY_OPERATION_FULL_FLOAT(f64, f64, f64, "%lf", "%lf", "%lf", _subtract);
        TEST_BINARY_OPERATION_FULL_FLOAT(f64, f32, f64, "%lf", "%f", "%lf", _subtract_widen_a);
        TEST_BINARY_OPERATION_FULL_FLOAT(f64, f64, f32, "%lf", "%lf", "%f", _subtract_widen_b);
//////////////////////////
// Multiply
//////////////////////////
	TEST_BINARY_OPERATION_FULL(u8, u8, u8, "%d", "%d", "%d", _multiply);
	
	TEST_BINARY_OPERATION_FULL(u16, u16, u16, "%d", "%d", "%d", _multiply);
	
	TEST_BINARY_OPERATION_FULL(u32, u32, u32, "%d", "%d", "%d", _multiply);
	TEST_BINARY_OPERATION_FULL(u32, u16, u32, "%d", "%d", "%d", _multiply_widen_a);
	TEST_BINARY_OPERATION_FULL(u32, u32, u16, "%d", "%d", "%d", _multiply_widen_b);

    	TEST_BINARY_OPERATION_FULL(u64, u64, u64, "%ld", "%ld", "%ld", _multiply);
	TEST_BINARY_OPERATION_FULL(u64, u32, u64, "%ld", "%d", "%ld", _multiply_widen_a);
	TEST_BINARY_OPERATION_FULL(u64, u64, u32, "%ld", "%ld", "%d", _multiply_widen_b);

	TEST_BINARY_OPERATION_FULL(i8, i8, i8, "%d", "%d", "%d", _multiply);
	
	TEST_BINARY_OPERATION_FULL(i16, i16, i16, "%d", "%d", "%d", _multiply);
	
	TEST_BINARY_OPERATION_FULL(i32, i32, i32, "%d", "%d", "%d", _multiply);
	TEST_BINARY_OPERATION_FULL(i32, i16, i32, "%d", "%d", "%d", _multiply_widen_a);
	TEST_BINARY_OPERATION_FULL(i32, i32, i16, "%d", "%d", "%d", _multiply_widen_b);

    	TEST_BINARY_OPERATION_FULL(i64, i64, i64, "%ld", "%ld", "%ld", _multiply);
	TEST_BINARY_OPERATION_FULL(i64, i32, i64, "%ld", "%d", "%ld", _multiply_widen_a);
	TEST_BINARY_OPERATION_FULL(i64, i64, i32, "%ld", "%ld", "%d", _multiply_widen_b);

        TEST_BINARY_OPERATION_FULL_FLOAT(f32, f32, f32, "%f", "%f", "%f", _multiply);
        TEST_BINARY_OPERATION_FULL_FLOAT(f64, f64, f64, "%lf", "%lf", "%lf", _multiply);
        TEST_BINARY_OPERATION_FULL_FLOAT(f64, f32, f64, "%lf", "%f", "%lf", _multiply_widen_a);
        TEST_BINARY_OPERATION_FULL_FLOAT(f64, f64, f32, "%lf", "%lf", "%f", _multiply_widen_b);
//////////////////////////
// Divide
//////////////////////////

	// Protect divide by zero.
	if ((u8)a == 0) continue;
	if ((u8)b == 0) continue;
	TEST_BINARY_OPERATION_FULL(u8, u8, u8, "%d", "%d", "%d", _divide);

	TEST_BINARY_OPERATION_FULL(u16, u16, u16, "%d", "%d", "%d", _divide);
	
	TEST_BINARY_OPERATION_FULL(u32, u32, u32, "%d", "%d", "%d", _divide);
	TEST_BINARY_OPERATION_FULL(u32, u16, u32, "%d", "%d", "%d", _divide_widen_a);
	TEST_BINARY_OPERATION_FULL(u32, u32, u16, "%d", "%d", "%d", _divide_widen_b);

    	TEST_BINARY_OPERATION_FULL(u64, u64, u64, "%ld", "%ld", "%ld", _divide);
	TEST_BINARY_OPERATION_FULL(u64, u32, u64, "%ld", "%d", "%ld", _divide_widen_a);
	TEST_BINARY_OPERATION_FULL(u64, u64, u32, "%ld", "%ld", "%d", _divide_widen_b);

	TEST_BINARY_OPERATION_FULL(i8, i8, i8, "%d", "%d", "%d", _divide);
	
	TEST_BINARY_OPERATION_FULL(i16, i16, i16, "%d", "%d", "%d", _divide);
	
	TEST_BINARY_OPERATION_FULL(i32, i32, i32, "%d", "%d", "%d", _divide);
	TEST_BINARY_OPERATION_FULL(i32, i16, i32, "%d", "%d", "%d", _divide_widen_a);
	TEST_BINARY_OPERATION_FULL(i32, i32, i16, "%d", "%d", "%d", _divide_widen_b);

    	TEST_BINARY_OPERATION_FULL(i64, i64, i64, "%ld", "%ld", "%ld", _divide);
	TEST_BINARY_OPERATION_FULL(i64, i32, i64, "%ld", "%d", "%ld", _divide_widen_a);
	TEST_BINARY_OPERATION_FULL(i64, i64, i32, "%ld", "%ld", "%d", _divide_widen_b);

        TEST_BINARY_OPERATION_FULL_FLOAT(f32, f32, f32, "%f", "%f", "%f", _divide);
        TEST_BINARY_OPERATION_FULL_FLOAT(f64, f64, f64, "%lf", "%lf", "%lf", _divide);
        TEST_BINARY_OPERATION_FULL_FLOAT(f64, f32, f64, "%lf", "%f", "%lf", _divide_widen_a);
        TEST_BINARY_OPERATION_FULL_FLOAT(f64, f64, f32, "%lf", "%lf", "%f", _divide_widen_b);
//////////////////////////
// Modulo
//////////////////////////
	TEST_BINARY_OPERATION_FULL(u8, u8, u8, "%d", "%d", "%d", _modulo);

	TEST_BINARY_OPERATION_FULL(u16, u16, u16, "%d", "%d", "%d", _modulo);
	
	TEST_BINARY_OPERATION_FULL(u32, u32, u32, "%d", "%d", "%d", _modulo);
	TEST_BINARY_OPERATION_FULL(u32, u16, u32, "%d", "%d", "%d", _modulo_widen_a);
	TEST_BINARY_OPERATION_FULL(u32, u32, u16, "%d", "%d", "%d", _modulo_widen_b);

    	TEST_BINARY_OPERATION_FULL(u64, u64, u64, "%ld", "%ld", "%ld", _modulo);
	TEST_BINARY_OPERATION_FULL(u64, u32, u64, "%ld", "%d", "%ld", _modulo_widen_a);
	TEST_BINARY_OPERATION_FULL(u64, u64, u32, "%ld", "%ld", "%d", _modulo_widen_b);

	TEST_BINARY_OPERATION_FULL(i8, i8, i8, "%d", "%d", "%d", _modulo);
	
	TEST_BINARY_OPERATION_FULL(i16, i16, i16, "%d", "%d", "%d", _modulo);
	
	TEST_BINARY_OPERATION_FULL(i32, i32, i32, "%d", "%d", "%d", _modulo);
	TEST_BINARY_OPERATION_FULL(i32, i16, i32, "%d", "%d", "%d", _modulo_widen_a);
	TEST_BINARY_OPERATION_FULL(i32, i32, i16, "%d", "%d", "%d", _modulo_widen_b);

    	TEST_BINARY_OPERATION_FULL(i64, i64, i64, "%ld", "%ld", "%ld", _modulo);
	TEST_BINARY_OPERATION_FULL(i64, i32, i64, "%ld", "%d", "%ld", _modulo_widen_a);
	TEST_BINARY_OPERATION_FULL(i64, i64, i32, "%ld", "%ld", "%d", _modulo_widen_b);

    }
    return 0;
}
