#include <pointers.gh>

using namespace gyoji::std::pointers;

// From libc:
void free(void *pointer);
void* malloc(u64 size);
void write(u32 fd, u8* buf, u32 size);
u32 strlen(u8* str);

void do_write(u8* buf)
{
    write(0, buf, strlen(buf));
}

Owned::~Owned()
{
    unsafe {
        free(pointer);
        do_write("Freeing pointer\n");
    }
}

Owned
Owned::create(void *p)
{
    Owned pointer = {
        .pointer = p;
    };
    // Destructor called here, but we want
    // to return the un-destroyed thing, so
    // we need to figure out how to do that,
    // even when the returned value is a
    // complicated expression, not just a single
    // variable.

    return pointer;
}

Owned
Owned::do_create()
{
    // For that matter, we call destructors for any variables
    // in scope, but if we create an object as an intermediate
    // expression, how do we make sure its destructor gets called?
    // For example create returns an object, but we don't call its
    // destructor in this case because it's not 'in scope' in the
    // sense of a variable, only in scope in terms of the expression
    // evaluation.
    return create(malloc(32u64));
}
        
void *
Owned::get()
{
    return pointer;
}


u32 main(u32 argc, u8**argv)
{
    // TODO: This is a double-free because we don't have
    // move semantics here yet and there's no std::move
    // to allow it to be moved out of the function
    // and into the caller.
    //Owned ptr = Owned::create(malloc(32u64));
    // They key question here is how '=' should
    // be interpreted for classes.  Should this always
    // implicitly call the destructor of the temporary
    // class or should some kind of move semantics be
    // encoded in the type somehow?
    
    // TODO: We still need to allow template/generic
    // types so we can do this.
    // TODO: We still need some kind of heap allocator
    // for classes so we can 'new' them.
    //Owned<ClassType> = Owned<ClassType>::create(new ClassType());

    // For now, all we get is the guarantee that 'free' is called
    // for this bare pointer.
    do_write("Allocating pointer\n");
    Owned ptr = {
        .pointer = malloc(32u64);
    };
//    Owned p2 = Owned::do_create();

    Owned & pref = &ptr;

    // In rust, this would
    // invalidate the pref
    // reference so that it can't
    // be borrowed in future.  I would
    // like a way to express that 'pref'
    // does not 'consume' the reference, but
    // just drops it at the end so that
    // it's safe to use pref again later.
    //
    // The default is to pass the reference
    // and it cannot be 'consumed' (assigned from) later.
    //
    // void some_method_foo(Foo& ref, u32 val);
    // some_method_foo(pref, 1);
    //
    // This indicates that using it here
    // 'consumes' the reference so it's unsafe to use
    // another reference to it.
    // void some_method_foo(consume &Foo ref, u32 val);
    // other_method_foo(pref, 1);
    // This would lower to:
    //        consume( _pref )
    //        function-call (some_method_foo, _pref _1)
    // indicating that the 'consume' invalidates
    // pref and makes it unavailable for use.

    // After a 'consume' usage, it's
    // not possible to use it again,
    // but after a normal usage, it is
    // because our use has terminated when the
    // function terminates.
    
    Owned & another = pref;
    // This would lower to:
    //     consume ( _pref )
    //     store( _another _pref)
    // indicating that the pref reference
    // has been consumed.
    
    // std::move it since we can't copy it(?)
    //Owned p2 move ptr;
    
    //Owned ptr = {
    //    .pointer = malloc(32u64);
    //};

    //void *raw_ptr = ptr.get();
    
    return 0;
}
