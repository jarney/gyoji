
JCC=../build/src/cmdline/jcc

all: strcpy.o pointers.o

clean:
	rm -f *.o
	rm -f *~

%.o: %.g
	$(JCC) -I . $< -o $@

