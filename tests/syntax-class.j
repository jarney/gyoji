
// We can define a class without
// a forward-declaration.
class WithoutForwardDeclaration {
};

class FooClass;

// We have forward-declared the class.
class FooClass {
    u32 something;
};

// Another forward declaration
// should not matter.
class FooClass;
