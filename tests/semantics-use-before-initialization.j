
u32 print_value(u32 number);

class Foo {
    u32 member;
    static Foo construct();
    u32 method();
};

u32
Foo::method()
{
    return 0;
}

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

	u32 a = 4;
	while (argc == 0) {
	      a = 10;
	}
	print_value(a);

// We should figure out how to deal
// with class initialization.
	Foo f = {
		.member = 29;
	};
	f.member = 10;

	Foo g = {
	    .member = 18;
	};
	g.method();

// We should somehow be able to get this
// in safe mode, but the unsafe rules currently
// prohibit getting a reference to an object
//      unsafe {
	      Foo x = {
	          .member = 19;
	      };
	      x = g;
	      print_value(x.member);
//	}

	{
		u32 z;
	}
	{
		u32 z = 10;
		z = z + 19;
	}

	//
// This could work for initialization,
// but this function needs to be implemented
// also, so that just pushes the problem upstream.
//      Foo f = Foo::construct();

	return 0;
}