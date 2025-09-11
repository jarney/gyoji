
all:
	cd build; $(MAKE)

test:
	cd build; $(MAKE) test

clean:
	cd build; $(MAKE) clean
	find . -name "*~" -exec rm -f {} \; -print
