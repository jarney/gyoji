ROOT=..

TARGET_INCLUDES= \
	jbackend-llvm.hpp \
	jbackend-llvm-operators.hpp \
	jbackend-llvm-types.hpp \
	jsemantics.hpp

# Need more work.
#jbackend-toc.hpp
#jbackend-toc.o

TARGET_LIBRARY=libjlang-codegen.a

INCLUDES =                              \
	-I.					\
	-I/usr/include/llvm-18			\
	-I/usr/include/llvm-c-18		\
	-I$(ROOT)/test/include			\
	-I$(ROOT)/misc/target/include		\
	-I$(ROOT)/frontend/target/include	\
	-I$(ROOT)/backend/target/include	\
	-I$(ROOT)/codegen/target/include

OBJ_FILES =					\
	jbackend-llvm.o				\
	jbackend-llvm-operators.o		\
	jbackend-llvm-types.o			\
	jsemantics.o				\
	llvm-translation-unit-visitor.o

TARGET_OBJ_FILES=$(addprefix target/,$(OBJ_FILES))

all: target/$(TARGET_LIBRARY)

.target: target/

target/:
	mkdir -p target/
	mkdir -p target/include/jlang-codegen
	cp $(TARGET_INCLUDES) target/include/jlang-codegen

target/$(TARGET_LIBRARY): target/ $(TARGET_OBJ_FILES)
	echo $(TARGET_OBJ_FILES)
	ar rv $@ $(TARGET_OBJ_FILES)

clean:
	rm -rf target
	rm -f *~
	rm -f tests/*~

include $(ROOT)/make/rules.mk

