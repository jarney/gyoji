
all: build/Makefile
	cd build; $(MAKE)

docs: build/Makefile
	echo "Checking?"
	cd build; $(MAKE) docs

# Run the tests and report
# code-coverage.  Note that we only
# cover our source, not generated code
# or tests.
test: build/Makefile
	cd build; $(MAKE) all; $(MAKE) test
	mkdir -p build/gcov
	gcovr --exclude build/jlang.y.cpp \
		--exclude build/jlang.y.hpp \
		--exclude build/jlang.l.cpp \
		--exclude 'src/.*/test_.*.cpp' \
		--html build/gcov/report.html \
		--html-details

clean: build/Makefile
	cd build; $(MAKE) clean
	find . -name "*~" -exec rm -f {} \; -print

realclean: clean
	rm -rf build/*

install: build/Makefile
	cd build; $(MAKE) install

build/Makefile: CMakeLists.txt
	mkdir -p build
	mkdir -p install
	cmake \
		-DCMAKE_INSTALL_PREFIX=install \
		-DCMAKE_BUILD_TYPE=Debug \
		-B build -S .



