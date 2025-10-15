
namespace jlang {
    class Foo {
        u32 a;
        u8 b;
        u8* s;
        void set_something(u32 a);
    };
    void foo_function();
};

u32 print_value(u32 number);
using namespace jlang as asdf;

void
Foo::set_something(u32 a)
{
	return;
}

//void foo_function()
//{
//    u32 a;
//    return;
//}

u32 main(u32 a, u8** b);

u32 main(u32 argc, u8**argv)
{
	Foo cl;
	Foo* pcl;
	pcl = &cl;
	
	u32 b;
	cl.a = 10u32;
	b = cl.a;
	b = b + 192;

	cl.a = b;
	cl.a += 100;
	
	print_value(cl.a);
	print_value(pcl->a);
	return 0u32;
}
