#include <stdio.h>

class Owned {
    public:
    Owned();
    ~Owned();
};

Owned::Owned()
{
    fprintf(stderr, "Construct\n");
}
Owned::~Owned()
{
    fprintf(stderr, "Destruct\n");
}

Owned foo() {
    Owned ptr;


    return ptr;
}


int main(int argc, char **argv)
{
    Owned op = foo();
}
