#include <stdio.h>
#include <stdlib.h>

unsigned int c_demo_function(unsigned int a, unsigned int b);
unsigned int demo_function(unsigned int a, unsigned int b);

int main(int argc, char **argv)
{
    for (unsigned int i = 0; i < 100; i++) {
	for (unsigned int j = 0; j < 100; j++) {
	    unsigned int jret = demo_function(i, j);
	    unsigned int cret = c_demo_function(i, j);
	    if (jret != cret) {
		fprintf(stderr, "%d %d : %d %d\n", i, j, jret, cret);
		return 1;
	    }
	}
    }
    return 0;
}
