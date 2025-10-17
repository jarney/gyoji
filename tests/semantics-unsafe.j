
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

// Not allowed because we need
// to do this inside an unsafe block.
//	u32 & xref = &xval;

	// Dereference of pointer must
	// be inside an unsafe block.

	// This should be legal
	// because &xval is a pointer type, but
	// we can also extract a lifetime from it,
	// so it should be a valid reference at this point.
	// In order to make that work, we need
	// to capture enough lifetime information
	// to satisfy the borrow checker.
	// u32 &xref = &xval;

	u32 &xref;
	unsafe {
		*xptr = 10;
		print_value(xval);
                xref = &xval;
	}
	
	// Assigning a reference
	// should be safe outside
	// of unsafe blocks because
	// references (should) have
	// lifetimes associated with them
	// that the borrow checker can check are
	// still valid

	*xref = 12;
	print_value(xval);
	print_value(*xref);

	xptr = xref;
	unsafe {
	    *xptr = 19;
	}
	print_value(xval);

    }

    return 0;
}
