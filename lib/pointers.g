#include <pointers.gh>

using namespace gyoji::std::pointers;

// From libc:
void free(void *pointer);
void* malloc(u64 size);

Owned::~Owned()
{
    unsafe {
        free(pointer);
    }
}

Owned
Owned::create(void *p)
{
    Owned pointer = {
        .pointer = p;
    };
    return pointer;
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

    // TODO: We still need to allow template/generic
    // types so we can do this.
    // TODO: We still need some kind of heap allocator
    // for classes so we can 'new' them.
    //Owned<ClassType> = Owned<ClassType>::create(new ClassType());

    // For now, all we get is the guarantee that 'free' is called
    // for this bare pointer.
    Owned ptr = {
        .pointer = malloc(32u64);
    };

    void *raw_ptr = ptr.get();
    
    return 0;
}
