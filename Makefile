
OBJ_FILES = \
	target/jsyntax.o \
	target/jbackend.o \
	target/jbackend-format-tree.o \
	target/jbackend-format-identity.o \
	target/jbackend-llvm.o \
        target/llvm-translation-unit-visitor.o \
        target/ast.o \
	target/main.o \
	target/xml.o \
	target/jstring.o \
	target/namespace.o \
	target/jsemantics.o

INCLUDES = \
	-I. \
	-I.. \
	-I/usr/include/llvm-18 \
	-I/usr/include/llvm-c-18

TEST_XML_OBJS = \
	target/test_xml.o \
	target/jstring.o \
	target/xml.o

TEST_NAMESPACE_OBJS = \
	target/test_namespace.o \
	target/jstring.o \
	target/namespace.o

TEST_STRING_OBJS = \
	target/test_string.o \
	target/jstring.o

LIBRARIES = \
	-L/usr/lib/llvm-18/lib \
	-lLLVM-18

all: target/cparse test
	target/cparse format-identity tests/valid-syntax-expression-primary.j >target/valid-syntax-expression-primary.j
	diff tests/valid-syntax-expression-primary.j target/valid-syntax-expression-primary.j
	target/cparse format-identity tests/valid-syntax-variable-declaration.j >target/valid-syntax-variable-declaration.j
	diff tests/valid-syntax-variable-declaration.j target/valid-syntax-variable-declaration.j
#	target/cparse format-tree tests/valid-syntax-typedefs.j


test: target/test_string target/test_xml target/test_namespace
	target/test_string
	target/test_xml
	target/test_namespace

target/test_string: $(TEST_STRING_OBJS)
	gcc -g -o $@ $(TEST_STRING_OBJS) -lstdc++

target/test_xml: $(TEST_XML_OBJS)
	gcc -g -o $@ $(TEST_XML_OBJS) -lstdc++

target/test_namespace: $(TEST_NAMESPACE_OBJS)
	gcc -g -o $@ $(TEST_NAMESPACE_OBJS) -lstdc++

target/test_namespace2: $(TEST_NAMESPACE2_OBJS)
	gcc -g -o $@ $(TEST_NAMESPACE2_OBJS) -lstdc++


clean:
	rm -rf target/*
	rm -f *~
	rm -f tests/*~

target/cparse: target/jlang.y.o target/jlang.l.o $(OBJ_FILES)
	g++ -g -o target/cparse target/jlang.y.o target/jlang.l.o $(OBJ_FILES) $(LIBRARIES)

target/%.o: %.cpp
	g++ -g -c $(INCLUDES) $< -o $@

target/%.l.o: target/%.l.cpp
	g++ -g -c $(INCLUDES) $< -o $@

target/%.y.o: target/%.y.cpp
	g++ -g -c $(INCLUDES) $< -o $@

target/jlang.y.o: target/jlang.y.cpp target/jlang.l.cpp
	g++ -g -c $(INCLUDES) $< -o $@

target/jlang.l.o: target/jlang.l.cpp target/jlang.y.cpp
	g++ -g -c $(INCLUDES) $< -o $@

target/jlang.l.cpp: jlang.l target/jlang.y.hpp
	flex -o target/jlang.l.cpp jlang.l

target/jlang.y.cpp: jlang.y
	bison jlang.y -Wcounterexamples --header=target/jlang.y.h --output=target/jlang.y.cpp


