
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
       set_something(_a, _b, _c);
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
	Foo cl;

// Disallowed member calls in static context.
//	Foo::set_something(&cl);

// Should we allow this?  It seems this is disallowed by
// the 'is_method' rule, but it seems like we should be able
// to extract this as a simple method name if we want.
//	void (*)(Foo*) fptr = Foo::set_something;
// But if that's the case, what stops us from doing
// Foo::method(x)?  Maybe that should be allowed also?



// Destructors cannot be called directly.
//	Foo::~Foo(&cl);

// This is syntactially prevented because ~ isn't valid in this context,
// so it will never resolve.
//	cl.~Foo();
	
// TODO: Factories instead of constructors!!!
// But we'll keep destructors.  Kind-of an odd mix,
// but it lets us still do scope-guards.
//	cl = Foo::construct();
	
	cl.set_something(10u32, 42u32, 99u32);
	foo_function();
	return 0u32;
}
