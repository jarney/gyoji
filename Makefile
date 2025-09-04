
OBJ_FILES = \
	target/jsyntax.o \
	target/jbackend.o \
	target/jbackend-format-tree.o \
	target/jbackend-format-identity.o \
	target/ast.o \
	target/main.o \
	target/xml.o

INCLUDES = \
	-I. \
	-I.. \
	-I/usr/include/llvm-18 \
	-I/usr/include/llvm-c-18

TEST_XML_OBJS = \
	target/test_xml.o \
	target/xml.o

#LIBRARIES = \
#	-L/usr/lib/llvm-18/lib \
#	-lLLVM-18
LIBRARIES=

all: target/cparse
	target/cparse format-identity <tests/valid-syntax-expression-primary.j >target/valid-syntax-expression-primary.j
	diff tests/valid-syntax-expression-primary.j target/valid-syntax-expression-primary.j
	target/cparse format-tree <tests/valid-syntax-expression-primary.j

test: target/test_xml
	target/test_xml


target/test_xml: $(TEST_XML_OBJS)
	gcc -o $@ $(TEST_XML_OBJS) -lstdc++

clean:
	rm -rf target/*
	rm -f *~
	rm -f tests/*~

target/cparse: target/jlang.y.o target/jlang.l.o $(OBJ_FILES)
	g++ -o target/cparse target/jlang.y.o target/jlang.l.o $(OBJ_FILES) $(LIBRARIES)

target/%.o: %.cpp
	g++ -c $(INCLUDES) $< -o $@

target/%.l.o: target/%.l.cpp
	g++ -c $(INCLUDES) $< -o $@

target/%.y.o: target/%.y.cpp
	g++ -c $(INCLUDES) $< -o $@

target/jlang.y.o: target/jlang.y.cpp target/jlang.l.cpp
	g++ -c $(INCLUDES) $< -o $@

target/jlang.l.o: target/jlang.l.cpp target/jlang.y.cpp
	g++ -c $(INCLUDES) $< -o $@

target/jlang.l.cpp: jlang.l target/jlang.y.hpp
	flex -o target/jlang.l.cpp jlang.l

target/jlang.y.cpp: jlang.y
	bison jlang.y -Wcounterexamples --header=target/jlang.y.h --output=target/jlang.y.cpp


