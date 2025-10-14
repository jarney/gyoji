
u32 something_unqualified()
{
}

namespace std {
  u32 foo();
  u32 bar();
  u32 baz();
};
/*
  typedef u32 some_t;

using namespace std;

u32 foo()
{

}

u32 std::bar()
{
}

namespace std {
    u32 baz()
    {
    }
};

using namespace std as giz::mo;

u32 giz::mo::fooart()
{
}

namespace fart {
    u32 baz();
};

//using namespace fart;

u32 fart::baz()
{}
*/
