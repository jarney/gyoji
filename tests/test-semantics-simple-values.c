#include <stdio.h>
#include <stdlib.h>

#define DECLARE_BINARY_OPERATIONS(jtype, ctype, fname)		\
    ctype c_##jtype##fname(ctype a, ctype b); \
ctype j_##jtype##fname(ctype a, ctype b);

#define DECLARE_BINARY_OPERATIONS_FULL(retjtype, retctype, actype, bctype, fname) \
    retctype c_##retjtype##fname(actype a, bctype b); \
    retctype j_##retjtype##fname(actype a, bctype b);
    

DECLARE_BINARY_OPERATIONS(u16, unsigned short, _add);
DECLARE_BINARY_OPERATIONS(i16, short, _add);
DECLARE_BINARY_OPERATIONS(u32, unsigned int, _add);
DECLARE_BINARY_OPERATIONS_FULL(u32, unsigned int, unsigned short, unsigned int, _add_widen_a);


#define TEST_BINARY_OPERATION(jtype, ctype, fmt, fname)			\
    {									\
	ctype a##jtype = (ctype)a;					\
	ctype b##jtype = (ctype)b;					\
	ctype jret##jtype = j_##jtype##fname(a##jtype, b##jtype);	\
	ctype cret##jtype = c_##jtype##fname(a##jtype, b##jtype);	\
	if (jret##jtype != cret##jtype) {				\
	    fprintf(stderr,						\
		    #jtype " " fmt " " fmt " : " fmt " " fmt "\n",	\
		    a##jtype, b##jtype, jret##jtype, cret##jtype);	\
	    return 1;							\
	}								\
    }									\

#define TEST_BINARY_OPERATION_FULL(retjtype, ajtype, bjtype, retctype, actype, bctype, retfmt, afmt, bfmt, fname) \
    {									\
	actype a##ajtype = (actype)a;					\
	bctype b##bjtype = (bctype)b;					\
	retctype jret##retjtype = j_##retjtype##fname(a##ajtype, b##bjtype);	\
	retctype cret##retjtype = c_##retjtype##fname(a##ajtype, b##bjtype);	\
	if (jret##retjtype != cret##retjtype) {				\
	    fprintf(stderr,						\
		    #retjtype " " afmt " " bfmt " : " retfmt " " retfmt "\n",	\
		    a##ajtype, b##bjtype, jret##retjtype, cret##retjtype);	\
	    return 1;							\
	}								\
    }									\




int main(int argc, char **argv)
{
    for (unsigned int i = 0; i < 100; i++) {
	unsigned int a = rand();
	unsigned int b = rand();

	TEST_BINARY_OPERATION(i16, short, "%d", _add);
	
	TEST_BINARY_OPERATION(u16, unsigned short, "%d", _add);
	
	TEST_BINARY_OPERATION(u32, unsigned int, "%d", _add);

	TEST_BINARY_OPERATION_FULL(u32, u16, u32, unsigned long, unsigned short, unsigned long, "%ld", "%d", "%ld", _add_widen_a);
    }
    return 0;
}
