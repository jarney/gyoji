
u32 print_value(u32 number);

class Foo {
    u32 member;
    static Foo construct();
    u32 method();
};

u32 main(u32 argc, u8**argv)
{
	u32 b;
	if (argc == 0) {
		b = 10;
	}
	else {
	     b = 12;
	}
	print_value(b);

	u32 a;
	while (argc == 0) {
	      a = 10;
	}
	print_value(a);

// We should figure out how to deal
// with class initialization.
	Foo f;
	f.member = 10;

	Foo g;
	g.method();

	//
// This could work for initialization,
// but this function needs to be implemented
// also, so that just pushes the problem upstream.
//      Foo f = Foo::construct();

	return 0;
}