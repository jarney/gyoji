
u32 something_unqualified()
{
}


namespace std {
  u32 foo();
  u32 bar();
  u32 baz();
};

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

u32 giz::mo::foo()
{
}