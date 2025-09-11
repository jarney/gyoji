ROOT=..

TARGET_INCLUDES=jsyntax.hpp ast.hpp namespace.hpp
TARGET_LIBRARY=target/libjlang-frontend.a

INCLUDES =                              \
    -I.                                 \
    -I$(ROOT)/test/include              \
    -I$(ROOT)/misc/target/include

OBJ_FILES =                             \
	target/jsyntax.o                \
        target/ast.o                    \
        target/namespace.o              \
        target/jlang.y.o                \
        target/jlang.l.o

TEST_NAMESPACE_OBJS =                  \
	target/test_namespace.o

TEST_LIBRARIES=                        \
	-lstdc++                       \
	-Ltarget -ljlang-frontend      \
	-L$(ROOT)/misc/target -ljlang-misc

all: target/test_namespace

test:	all
	target/test_namespace

.target: target/

target/:
	mkdir -p target/
	mkdir -p target/include/jlang-frontend

$(TARGET_LIBRARY): target/ $(OBJ_FILES)
	ar rv $@ $(OBJ_FILES)
	cp $(TARGET_INCLUDES) target/include/jlang-frontend

target/test_namespace: $(TARGET_LIBRARY) $(TEST_NAMESPACE_OBJS)
	gcc -g -o $@ $(TEST_NAMESPACE_OBJS) $(TEST_LIBRARIES)

clean:
	rm -rf target
	rm -f *~
	rm -f tests/*~

include $(ROOT)/make/rules.mk

