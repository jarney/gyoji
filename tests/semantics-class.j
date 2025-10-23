
namespace jlang {
    class Foo {
        u32 a;
        u8 b;
        u8* s;
        void set_something(u32 a, u32 b, u32 c);
	~Foo();
	static Foo construct();
    };
    void foo_function();
};

u32 print_value(u32 number);
using namespace jlang as asdf;

// Whtiespace in the middle still screws us up, so
// we should normalize this.  The parser is ok with
// it, but it should be eaten up in the lexical layers.
//
// Syntax says this is ok, but semantics doesn't resolve it Foo : : ~  Foo()
//
Foo::~Foo()
{
    print_value(777);
}

void foo_function()
{
    print_value(99999);
    return;
}

void
Foo::set_something(u32 _a, u32 _b, u32 _c)
{
    a = _c;

    print_value(_a);
    print_value(_b);
    print_value(_c);
    print_value(a);
    if (_a > 0) {
       _a -= 1;
       print_value(_a);

// This works, but it's a bit boring.
//       set_something(_a, _b, _c);

// A more exciting construct is to resolve
// the method and use it as a function pointer.

       void (*)(Foo*, u32, u32, u32) fptr_set_something = set_something;
       print_value(999999999);

// Method calls even work with function pointers
// since we automatically supply the 'this' pointer to them if
// they match the correct type.
       fptr_set_something(_a, _b, 999999);
    }

// Disallowed in order to provide
// guarantees about destructor behavior.
//  jlang::Foo::~Foo();
	return;
}

u32 main(u32 argc, u8**argv)
{
        u32 x = 8u32;
        x = 19;
//	Foo cl(537);
	Foo cl = {
	    .a = x;
	    .b = 0u8;
	    .c = "something";
	};

// We do allow function calls in static context.
//	Foo::set_something(&cl, 0, 0, 0);

// We can also assign function pointer types to functions
// and call them.
	void (*)(Foo*, u32, u32, u32) fptr;
	fptr = Foo::set_something;

// Indirect calls through function pointers work now also.
//	fptr(&cl, 57, 84, 92);
	
// Destructors cannot be called directly.
//	Foo::~Foo(&cl);

// This is syntactially prevented because ~ isn't valid in this context,
// so it will never resolve.
//	cl.~Foo();
	
// TODO: Factories instead of constructors!!!
// But we'll keep destructors.  Kind-of an odd mix,
// but it lets us still do scope-guards.
//	cl = Foo::construct();

	foo_function();
	
	cl.set_something(10u32, 42u32, 99u32);
	return 0u32;
}
