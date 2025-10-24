u32 print_value(u32 number);

u32 main(u32 argc, u8**argv)
{
        u32 x = 8u32;
	// Is 'const' a type attribute or is it an attribute
	// of the 'INSTANCE' of the type?  I think it's really an
	// attribute of the instance, not the type itself(?)
	// Should we manage this in the MIR as an attribute of the
	// type itself or should we manage this in the front-end lowering
	// code?
	const u32 y;

	y = x;

	return 0;
}