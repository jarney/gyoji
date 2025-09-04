typedef struct {
    public int a;
    protected char *b;
} foo_t;

namespace std {
    typedef char u8_t;
    namespace string {
        private const foo_t string;
    };
};

using namespace std::string;

private volatile int a/* = 4*/;
public const int *b/* = &a*/;

public foo_t k = {
.a = 4;
.b = &a;
};

int do_something(char a)
{
//TODO        function (void)(int,int) fptr;

        // expression_primary:

        // expression_primary_identifier
        a;
        // expression_primary_literal_int
        4;
        // expression_primary_literal_char
        'a';
        // expression_primary_literal_string
        "324";
        // expression_primary_literal_float
        19.2;
        // expression_primary_nested
        (a);        

        // expression_postfix

        // expression_postfix_increment
        a++;
        4++;
        'a'++;
        "234"++;
        19.2++;
        (a++)++;

        // expression_postfix_decrement
        a--;
        4--;
        'a'--;
        "234"--;
        19.2--;
        (a--)++;

        // expression_postfix_arrayindex
        a[2];
        4[x++];
        'a'[9];
        "234"[18];
        19.2[22.4];
        (a--)[p];

        // expression_postfix_dot
        a.b;
        (4).asdf;
        'a'.length;
        "234".length;
        19.2.size;
        (a--).str;

        // expression_postfix_arrow
        a->b;
        (4)->asdf;
        'a'->length;
        "234"->length;
        19.2->size;
        (a--)->str;

        // expression_postfix_function_call
        a();
        b(a);
        b(4);
        b('a');
        b("123");
        b(19.2);
        b(a++);
        b(c());
        b(a,23.4,"a",'a'++, k()++.fine);

        // expression_unary
        ++a;
        --b;

        // Type casts: We use our own type casting system
        // here because the (char*) c-style casts lead
        // to ambiguities in the language where we can't distinguish
        // between user-defined types and user-defined variables, so
        // we use the 'cast' keyword to make sure they are not
        // ambiguous.
        cast(char, a);
        cast(int, a);
        cast(float, a);
        cast(short, a);
        cast(MyNamespace::some::type, a);
        cast(MyNamespace::some::type, a++);
        cast(MyNamespace::some::type, --a);
        cast(MyNamespace::some::type, --'a');
        cast(MyNamespace::some::type*, --'a');
        cast(char***, --'a');

        // Const-ness in declarations
        cast(volatile char *, --'a');
        cast(volatile char *const *volatile, --'a');

        cast(volatile My::thing::Type *const *volatile, --'a');

        // Sizeof applied to types
        sizeof(MyCustomNamespace::type);
        sizeof(const char*);

        a++();

        // Sizeof applied to expressions
        sizeof(typeof(a++()));

        // Complicated multiplicative expression
        // used in sizeof/typeof
        sizeof(typeof(c()++*a));

        foo(a,b+2,c+2*k++,++g*q+4);

        // Unary operators.
        -9;
        -a;
        
        !9;
        !a;

        ~9;
        ~a;

        &9;
        &a;

        *9;
        *a;
        *"abc"*k;

        // Shift operators.
        a<<b*k;
        b*c>>q;

        // Relational algebra
        a < b;
        a > b;
        a <= b;
        a >= b;
        a == b;

        // More complicated.
        a == b < 2;
        a++ == --b >= 12;


        a == b | c;
        a == b || c;

        a == b & c;
        a == b && c;

        a == b ^ c;
        a == ~d;

        a == !b || ~d;


        a = 2;
        a += 5;
        a /= 10;
        a <<= 2;
        b >>= 1;
        a *= 2;
        a -= 4;

        *a(b)++*c;


        if (a == b) {
        }

        if (p) {
            foo();
        }
        else {
            bar();
        }

        if (k) {
            foo();
        }
        else if (p) {
            bar();
        }
        else if (z) {
            bar();
        }
        else {
            baz();
        }

        while (foo()++) {
        }

        while (*d++ = *s++) {
            if (a == 4) {
                continue;
            }
            else {
                break;
            }
        }

        switch (foo) {
            case bar: {
                return baz>>siz;
            }
            case baz: {
                return baz>>siz;
            }
            case fiz: {
                return a()++;
            }
            default: {
                return 2;
            }
        }

        sizeof(struct {
            private int a;
            protected foo b;
            public char buf[32];
            public struct {
                private bar a;
                private baz d;
            } d;
        });

        label foo:
        foo_fn();
        goto foo;

        // Typedef in function scope.
        typedef struct {
            private int a;
            private char *b;
        } foo_t;

        unsafe {
            char *a;
        }

}
