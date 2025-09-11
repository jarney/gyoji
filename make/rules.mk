
target/%.o: %.cpp
	g++ -g -c $(INCLUDES) $< -o $@

target/%.l.o: target/%.l.cpp
	g++ -g -c $(INCLUDES) $< -o $@

target/%.y.o: target/%.y.cpp
	g++ -g -c $(INCLUDES) $< -o $@

target/jlang.y.o: target/jlang.y.cpp target/jlang.l.cpp
	g++ -g -c $(INCLUDES) $< -o $@

target/jlang.l.o: target/jlang.l.cpp target/jlang.y.cpp
	g++ -g -c $(INCLUDES) $< -o $@

target/jlang.l.cpp: jlang.l target/jlang.y.hpp
	flex -o target/jlang.l.cpp jlang.l

target/jlang.y.cpp: jlang.y
	bison jlang.y -Wcounterexamples --header=target/jlang.y.h --output=target/jlang.y.cpp


