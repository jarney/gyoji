
void safe_function(u32 a);

unsafe void unsafe_function();

u32 print_value(u32 number);

unsafe void unsafe_function()
{
    u32 x;
    u32 *p = &x;

    *p = 10;
}

void safe_function(u32 a)
{
    u32 x = 10;
}

class Foo {
      u32 x;
};

u32 main(u32 argc, u8 **argv)
{
    safe_function(19);

    Foo f;
    // Creating pointers is ok, but
    // dereferencing them must be done
    // inside an unsafe context.
    Foo *ptrf = &f;

    // This would be disallowed.
    // unsafe_function();
    unsafe {
        // This is ok because we're in an unsafe context.
        unsafe_function();
	
	ptrf->x = 10;
    }

    // This would be disallowed because
    // it is de-referencing a pointer in an
    // unsafe context.
    //ptrf->x = 10;

    // Some basic pointer semantics:
    {
        u32 xval;
	u32* xptr = &xval;

// Now, we should start to
// work on reference semantics.
// This should be valid sometimes
// to initialize a reference, but
// we need to be especially careful
// when assigning references to keep track
// of where it's pointing 'to'.
//	u32 & xref = &xval;

	// Dereference of pointer must
	// be inside an unsafe block.
	unsafe {
		*xptr = 10;
	}
    }

    return 0;
}
