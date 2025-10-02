
all: build/Makefile
	cd build; $(MAKE)

docs: build/Makefile
	echo "Checking?"
	cd build; $(MAKE) docs

test: build/Makefile
	cd build; $(MAKE) all; $(MAKE) test
	mkdir -p build/gcov
	gcovr  --html build/gcov/report.html --html-details

clean: build/Makefile
	cd build; $(MAKE) clean
	find . -name "*~" -exec rm -f {} \; -print

install: build/Makefile
	cd build; $(MAKE) install

build/Makefile: CMakeLists.txt
	mkdir -p build
	mkdir -p install
	cmake \
		-DCMAKE_INSTALL_PREFIX=install \
		-DCMAKE_BUILD_TYPE=Debug \
		-B build -S .



