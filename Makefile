
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
	cd build; $(MAKE) all; $(MAKE) test; $(MAKE) gcov-report

clean: build/Makefile
	cd build; $(MAKE) clean
	find . -name "*~" -exec rm -f {} \; -print

realclean: clean
	rm -rf build/*
	rm -rf install/*

.PHONY: .force

install: .force all test docs build/Makefile
	cd build; $(MAKE) install

build/Makefile: CMakeLists.txt
	mkdir -p build
	mkdir -p install
	cmake \
		-DCMAKE_INSTALL_PREFIX=install \
		-DCMAKE_BUILD_TYPE=Debug \
		-B build -S .



