
all:
	cd build; $(MAKE)

docs:
	echo "Checking?"
	cd build; $(MAKE) docs

test:
	cd build; $(MAKE) all; $(MAKE) test

clean:
	cd build; $(MAKE) clean
	find . -name "*~" -exec rm -f {} \; -print


build/Makefile:
	mkdir -p build
	mkdir -p install
	cmake \
		-DCMAKE_INSTALL_PREFIX=install \
		-B build -S .

