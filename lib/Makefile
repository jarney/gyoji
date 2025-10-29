
JCC=../build/src/cmdline/jcc

all: strcpy pointers

pointers: pointers.o
	clang -o pointers pointers.o

strcpy: strcpy.o
	clang -o strcpy strcpy.o

clean:
	rm -f pointers
	rm -f *.o
	rm -f *~

%.o: %.g
	$(JCC) --output-mir --output-llvm-ir -I . $< -o $@

