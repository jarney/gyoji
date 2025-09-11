ROOT=..

TARGET_INCLUDES=jbackend-format-identity.hpp \
	jbackend-format-pretty.hpp \
	jbackend-format-tree.hpp \
	jbackend.hpp

# Need more work.
#jbackend-toc.hpp
#jbackend-toc.o

TARGET_LIBRARY=libjlang-backend.a

INCLUDES =                              \
    -I.                                 \
    -I$(ROOT)/test/include              \
    -I$(ROOT)/misc/target/include	\
    -I$(ROOT)/frontend/target/include   \
    -I$(ROOT)/backend/target/include

OBJ_FILES =                             \
	jbackend.o \
	jbackend-format-identity.o \
	jbackend-format-pretty.o \
	jbackend-format-tree.o

TARGET_OBJ_FILES=$(addprefix target/,$(OBJ_FILES))

all: target/$(TARGET_LIBRARY)

.target: target/

target/:
	mkdir -p target/
	mkdir -p target/include/jlang-backend
	cp $(TARGET_INCLUDES) target/include/jlang-backend

target/$(TARGET_LIBRARY): target/ $(TARGET_OBJ_FILES)
	echo $(TARGET_OBJ_FILES)
	ar rv $@ $(TARGET_OBJ_FILES)

clean:
	rm -rf target
	rm -f *~
	rm -f tests/*~

include $(ROOT)/make/rules.mk

