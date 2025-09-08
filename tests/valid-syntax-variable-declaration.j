public volatile int *p[32];

private const double q[19];

typedef char my_type_t;

enum char Choice {
    OK = 'o';
    ERROR = 2;
    INVALID = 3;
};

public class q(somename, othername) {
    typedef char my_inner_type_t;

    private somename* field_from_generic;
    somename* another_member_pointer;

    const othername *p;

    my_type_t x[32];
    private my_inner_type_t zzz;
    public void foo(int a, int b);
    public q(int a, int b);
    public ~q(int a, int b);

    private enum int ErrorState {
        VALID = 0;
        PROBABLY_FINE = 1;
        ON_FIRE = 3;
    };

    private class ErrorValue {
        public ErrorState state;
    };

};

public int othername;
public q(int, char) name;

int do_something(q first, int second, char **third);

// The type from hell.
// It is a function pointer which has type arguments
// as its return-value and arguments.

typedef int(long) (*fptr)(int(char) a,int b) foo;

using namespace q;

int do_something(char a)
{
      my_inner_type_t qqqq;
        char d[32];
}
