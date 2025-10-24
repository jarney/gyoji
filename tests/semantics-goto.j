u32 print_value(u32 number);

class Foo {
    u32 x;
    ~Foo();
};

Foo::~Foo()
{
    print_value(19);
}

u32 main(u32 argc, u8**argv)
{
	u32 a;

	a = 0;

label top:
	if (a < 10) {
	    u32 b;
	    u32 c;
	    b = 1;
            print_value(a);
            a = a + b;
	    
	    Foo x = { .x = 10; };
	    Foo y = { .x = 20; };
	    goto top;
	}
	print_value(a);
{
	u32 b;
	u32 c;
}
{
	u32 c;
}
	return 0;
}