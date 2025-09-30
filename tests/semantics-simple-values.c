/*
 * This is a test file that runs in both 'j' and 'c'
 * for the purpose of testing various binary operations
 * to ensure that the semantics are the same between
 * the two languages.
 */
#ifdef C_LANG

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

#define FNAME(suffix)				\
    c_##suffix
#else
#define FNAME(suffix)				\
    j_##suffix
#endif

#define FUNCTION(suffix, rettype, atype, btype, operator)	\
rettype FNAME(suffix)(atype a, btype b) \
{					\
    rettype c;		                \
    c = a operator b;			\
    return c;				\
}

//////////////////////////
// Addition
//////////////////////////
FUNCTION(u8_add, u8, u8, u8, +)

FUNCTION(u16_add, u16, u16, u16, +)
FUNCTION(u16_add_widen_a, u16, u8, u16, +)
FUNCTION(u16_add_widen_b, u16, u16, u8, +)

FUNCTION(u32_add, u32, u32, u32, +)
FUNCTION(u32_add_widen_a, u32, u16, u32, +)
FUNCTION(u32_add_widen_b, u32, u32, u16, +)
    
FUNCTION(u64_add, u64, u64, u64, +)
FUNCTION(u64_add_widen_a, u64, u32, u64, +)
FUNCTION(u64_add_widen_b, u64, u64, u32, +)

FUNCTION(i8_add, i8, i8, i8, +)

FUNCTION(i16_add, i16, i16, i16, +)
FUNCTION(i16_add_widen_a, i16, i8, i16, +)
FUNCTION(i16_add_widen_b, i16, i16, i8, +)

FUNCTION(i32_add, i32, i32, i32, +)
FUNCTION(i32_add_widen_a, i32, i16, i32, +)
FUNCTION(i32_add_widen_b, i32, i32, i16, +)
    
FUNCTION(i64_add, i64, i64, i64, +)
FUNCTION(i64_add_widen_a, i64, i32, i64, +)
FUNCTION(i64_add_widen_b, i64, i64, i32, +)

FUNCTION(f32_add, f32, f32, f32, +)
FUNCTION(f64_add, f64, f64, f64, +)
FUNCTION(f64_add_widen_a, f64, f32, f64, +)
FUNCTION(f64_add_widen_b, f64, f64, f32, +)

//////////////////////////
// Subtraction
//////////////////////////
FUNCTION(u8_subtract, u8, u8, u8, -)

FUNCTION(u16_subtract, u16, u16, u16, -)
FUNCTION(u16_subtract_widen_a, u16, u8, u16, -)
FUNCTION(u16_subtract_widen_b, u16, u16, u8, -)

FUNCTION(u32_subtract, u32, u32, u32, -)
FUNCTION(u32_subtract_widen_a, u32, u16, u32, -)
FUNCTION(u32_subtract_widen_b, u32, u32, u16, -)
    
FUNCTION(u64_subtract, u64, u64, u64, -)
FUNCTION(u64_subtract_widen_a, u64, u32, u64, -)
FUNCTION(u64_subtract_widen_b, u64, u64, u32, -)

FUNCTION(i8_subtract, i8, i8, i8, -)

FUNCTION(i16_subtract, i16, i16, i16, -)
FUNCTION(i16_subtract_widen_a, i16, i8, i16, -)
FUNCTION(i16_subtract_widen_b, i16, i16, i8, -)

FUNCTION(i32_subtract, i32, i32, i32, -)
FUNCTION(i32_subtract_widen_a, i32, i16, i32, -)
FUNCTION(i32_subtract_widen_b, i32, i32, i16, -)
    
FUNCTION(i64_subtract, i64, i64, i64, -)
FUNCTION(i64_subtract_widen_a, i64, i32, i64, -)
FUNCTION(i64_subtract_widen_b, i64, i64, i32, -)

FUNCTION(f32_subtract, f32, f32, f32, -)
FUNCTION(f64_subtract, f64, f64, f64, -)
FUNCTION(f64_subtract_widen_a, f64, f32, f64, -)
FUNCTION(f64_subtract_widen_b, f64, f64, f32, -)

//////////////////////////
// Multiplication
//////////////////////////
FUNCTION(u8_multiply, u8, u8, u8, *)

FUNCTION(u16_multiply, u16, u16, u16, *)
FUNCTION(u16_multiply_widen_a, u16, u8, u16, *)
FUNCTION(u16_multiply_widen_b, u16, u16, u8, *)

FUNCTION(u32_multiply, u32, u32, u32, *)
FUNCTION(u32_multiply_widen_a, u32, u16, u32, *)
FUNCTION(u32_multiply_widen_b, u32, u32, u16, *)
    
FUNCTION(u64_multiply, u64, u64, u64, *)
FUNCTION(u64_multiply_widen_a, u64, u32, u64, *)
FUNCTION(u64_multiply_widen_b, u64, u64, u32, *)

FUNCTION(i8_multiply, i8, i8, i8, *)

FUNCTION(i16_multiply, i16, i16, i16, *)
FUNCTION(i16_multiply_widen_a, i16, i8, i16, *)
FUNCTION(i16_multiply_widen_b, i16, i16, i8, *)

FUNCTION(i32_multiply, i32, i32, i32, *)
FUNCTION(i32_multiply_widen_a, i32, i16, i32, *)
FUNCTION(i32_multiply_widen_b, i32, i32, i16, *)
    
FUNCTION(i64_multiply, i64, i64, i64, *)
FUNCTION(i64_multiply_widen_a, i64, i32, i64, *)
FUNCTION(i64_multiply_widen_b, i64, i64, i32, *)

FUNCTION(f32_multiply, f32, f32, f32, *)
FUNCTION(f64_multiply, f64, f64, f64, *)
FUNCTION(f64_multiply_widen_a, f64, f32, f64, *)
FUNCTION(f64_multiply_widen_b, f64, f64, f32, *)

//////////////////////////
// Divide
//////////////////////////
FUNCTION(u8_divide, u8, u8, u8, /)

FUNCTION(u16_divide, u16, u16, u16, /)
FUNCTION(u16_divide_widen_a, u16, u8, u16, /)
FUNCTION(u16_divide_widen_b, u16, u16, u8, /)

FUNCTION(u32_divide, u32, u32, u32, /)
FUNCTION(u32_divide_widen_a, u32, u16, u32, /)
FUNCTION(u32_divide_widen_b, u32, u32, u16, /)
    
FUNCTION(u64_divide, u64, u64, u64, /)
FUNCTION(u64_divide_widen_a, u64, u32, u64, /)
FUNCTION(u64_divide_widen_b, u64, u64, u32, /)

FUNCTION(i8_divide, i8, i8, i8, /)

FUNCTION(i16_divide, i16, i16, i16, /)
FUNCTION(i16_divide_widen_a, i16, i8, i16, /)
FUNCTION(i16_divide_widen_b, i16, i16, i8, /)

FUNCTION(i32_divide, i32, i32, i32, /)
FUNCTION(i32_divide_widen_a, i32, i16, i32, /)
FUNCTION(i32_divide_widen_b, i32, i32, i16, /)
    
FUNCTION(i64_divide, i64, i64, i64, /)
FUNCTION(i64_divide_widen_a, i64, i32, i64, /)
FUNCTION(i64_divide_widen_b, i64, i64, i32, /)

FUNCTION(f32_divide, f32, f32, f32, /)
FUNCTION(f64_divide, f64, f64, f64, /)
FUNCTION(f64_divide_widen_a, f64, f32, f64, /)
FUNCTION(f64_divide_widen_b, f64, f64, f32, /)

//////////////////////////
// Modulo
//////////////////////////
FUNCTION(u8_modulo, u8, u8, u8, %)

FUNCTION(u16_modulo, u16, u16, u16, %)
FUNCTION(u16_modulo_widen_a, u16, u8, u16, %)
FUNCTION(u16_modulo_widen_b, u16, u16, u8, %)

FUNCTION(u32_modulo, u32, u32, u32, %)
FUNCTION(u32_modulo_widen_a, u32, u16, u32, %)
FUNCTION(u32_modulo_widen_b, u32, u32, u16, %)
    
FUNCTION(u64_modulo, u64, u64, u64, %)
FUNCTION(u64_modulo_widen_a, u64, u32, u64, %)
FUNCTION(u64_modulo_widen_b, u64, u64, u32, %)

FUNCTION(i8_modulo, i8, i8, i8, %)

FUNCTION(i16_modulo, i16, i16, i16, %)
FUNCTION(i16_modulo_widen_a, i16, i8, i16, %)
FUNCTION(i16_modulo_widen_b, i16, i16, i8, %)

FUNCTION(i32_modulo, i32, i32, i32, %)
FUNCTION(i32_modulo_widen_a, i32, i16, i32, %)
FUNCTION(i32_modulo_widen_b, i32, i32, i16, %)
    
FUNCTION(i64_modulo, i64, i64, i64, %)
FUNCTION(i64_modulo_widen_a, i64, i32, i64, %)
FUNCTION(i64_modulo_widen_b, i64, i64, i32, %)

