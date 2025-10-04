
class TestClass;

// We can define a class without
// a forward-declaration.
class TestClass {
    u32 a;
    u8 b;
    u8* s;
};

u32 main(u32 argc, u8**argv)
{
	TestClass cl;

	cl.a = 10u32;
//
//	cl.s = "abc\n";
    return 0u32;
}
