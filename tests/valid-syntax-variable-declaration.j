public volatile i32*[32] p;

//private const f64[19] q;

typedef u8 my_type_t;

enum u8 Choice {
    OK = 'o';
    ERROR = 2;
    INVALID = 3;
};

public class q {
    typedef u8 my_inner_type_t;

    //my_type_t[32] x;
    private my_inner_type_t zzz;
    public void foo(i32 a, i32 b);
    public q(i32 a, i32 b);
    public ~q(i32 a, i32 b);

    private enum i32 ErrorState {
        VALID = 0;
        PROBABLY_FINE = 1;
        ON_FIRE = 3;
    };

    private class ErrorValue {
        public ErrorState state;
    };

};

public i32 othername;
public q(i32, u8) name;

i32 do_something(q first, i32 second, u8 **third);

// The type from hell.
// It is a function poi32er which has type arguments
// as its return-value and arguments.

typedef i32(i64) (*fptr)(i32(u8) a,i32 b) foo;

using namespace q;

i32 do_something(u8 a)
{
      my_inner_type_t qqqq;
//        u8[32] d;
}
