
OBJ_FILES = \
	target/main.o

INCLUDES = \
	-I. \
	-Imisc/target/include \
	-Ifrontend/target/include \
	-Ibackend/target/include \
	-Icodegen/target/include \
	-I/usr/include/llvm-18 \
	-I/usr/include/llvm-c-18

LIBRARIES = \
	-L/usr/lib/llvm-18/lib \
	-lLLVM-18 \
	-Lcodegen/target -ljlang-codegen \
	-Lbackend/target -ljlang-backend \
	-Lfrontend/target -ljlang-frontend \
	-Lmisc/target -ljlang-misc

LIBRARY_FILES=\
	frontend/target/libjlang-frontend.a \
	backend/target/libjlang-backend.a \
	codegen/target/libjlang-codegen.a

all: target/cparse

test:
	echo "Making"
	cd misc; make test
	cd frontend; make test
	cd backend; make test
	cd codegen; make test
	target/cparse format-identity tests/valid-syntax-expression-primary.j >target/valid-syntax-expression-primary.j
	diff tests/valid-syntax-expression-primary.j target/valid-syntax-expression-primary.j
	target/cparse format-identity tests/valid-syntax-variable-declaration.j >target/valid-syntax-variable-declaration.j
	diff tests/valid-syntax-variable-declaration.j target/valid-syntax-variable-declaration.j



target/cparse: $(OBJ_FILES) $(LIBRARY_FILES)
	g++ -g -o target/cparse $(OBJ_FILES) $(LIBRARIES)

target/:
	mkdir -p target/
	mkdir -p target/include/jlang-frontend

target/main.o: target/ $(LIBRARY_FILES)

misc/target/libjlang-misc.a:
	cd misc; make

frontend/target/libjlang-frontend.a: misc/target/libjlang-misc.a
	cd frontend; make

backend/target/libjlang-backend.a: frontend/target/libjlang-frontend.a
	cd backend; make

codegen/target/libjlang-codegen.a: backend/target/libjlang-backend.a
	cd codegen; make

clean:
	rm -rf target/
	rm -f *~
	rm -f tests/*~
	cd misc; make clean
	cd frontend; make clean
	cd backend; make clean
	cd codegen; make clean

include make/rules.mk

