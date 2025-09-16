
all: build/Makefile
	cd build; $(MAKE)

docs: build/Makefile
	cd build; $(MAKE) docs

test: build/Makefile
	cd build; $(MAKE) all; $(MAKE) test

clean: build/Makefile
	cd build; $(MAKE) clean
	find . -name "*~" -exec rm -f {} \; -print


build/Makefile:
	mkdir -p build
	mkdir -p install
	cmake \
		-DCMAKE_INSTALL_PREFIX=install \
		-B build -S .

