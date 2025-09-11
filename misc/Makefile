ROOT=..

TARGET_INCLUDES=xml.hpp jstring.hpp test.hpp
TARGET_LIBRARY=target/libjlang-misc.a

INCLUDES =                              \
    -Itarget/include

OBJ_FILES =                             \
	target/xml.o                    \
        target/jstring.o                

TEST_LIBRARIES=-lstdc++ \
        -Ltarget -ljlang-misc

TEST_XML_OBJS =                        \
	target/test_xml.o

TEST_JSTRING_OBJS =                    \
	target/test_xml.o

all: target/test_xml target/test_jstring

test: all
	target/test_xml
	target/test_jstring

.target: target/
	touch .target

target/:
	mkdir -p target/
	mkdir -p target/include/jlang-misc
	cp $(TARGET_INCLUDES) target/include/jlang-misc

$(TARGET_LIBRARY): .target $(OBJ_FILES)
	ar rv $@ $(OBJ_FILES)

target/test_jstring: $(TARGET_LIBRARY) $(TEST_JSTRING_OBJS)
	gcc -g -o $@ $(TEST_JSTRING_OBJS) $(TEST_LIBRARIES)

target/test_xml: $(TARGET_LIBRARY) $(TEST_XML_OBJS)
	gcc -g -o $@ $(TEST_XML_OBJS) $(TEST_LIBRARIES)

clean:
	rm -rf target/

include $(ROOT)/make/rules.mk
