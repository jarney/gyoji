#include <stdio.h>
#include <stdlib.h>

#define DECLARE_BINARY_OPERATIONS(jtype, ctype, fname)		\
    ctype c_##jtype##fname(ctype a, ctype b); \
ctype j_##jtype##fname(ctype a, ctype b);

DECLARE_BINARY_OPERATIONS(u16, unsigned short, _add);
DECLARE_BINARY_OPERATIONS(i16, short, _add);
DECLARE_BINARY_OPERATIONS(u32, unsigned int, _add);

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
    

int main(int argc, char **argv)
{
    for (unsigned int i = 0; i < 100; i++) {
	unsigned int a = rand();
	unsigned int b = rand();

	TEST_BINARY_OPERATION(i16, short, "%d", _add);
	
	TEST_BINARY_OPERATION(u16, unsigned short, "%d", _add);
	
	TEST_BINARY_OPERATION(u32, unsigned int, "%d", _add);
    }
    return 0;
}
