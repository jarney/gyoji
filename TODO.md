# TODO list

## Semantic processing
* Error Handling
  * Syntax Errors, we should consume more context
    so we don't cut off the error line as soon as the
    parse halts.

  * Basic MIR lowering for functions/expressions.

  * More MIR for types : Enums and such.

* Build some tests that verify compatibility with
  most of the expectations of the C-style
  expressions we know and love by using the preprocessor
  to execute the same expression in C and Jlang
  to make sure we get the same results in both cases.


## Compilation
* Figure out how to handle header files and declarations/imports.
  Should we require a pre-processor, or can we do without?
  
* Do we need keywords for 'super' and 'this'.

* What's our feeling about inheritance?

* What about traits and interfaces?

* What about generics and type parameters?

* Is this just going to end up being a messy bloat like C++?

* Where's the cutoff point between simple syntax and complete functionality?

## Code generation
* Start building basic LLVM code generation capabilities.

## Syntax verifications
* Build some unit-tests to verify that the syntax is what we expect and
  the operator precedence rules work correctly.

## Start writing the standard library in JLang
* Basic string manipulation functions
* Containers: map, list, string, vector, set

---------------------------------------------------------------

Semantics:

shared_ptr(type) is the default pointer type.
  * Constructor takes the result of a malloc and ref-counts it.
  * Cannot assign into it, that could invalidate it.
  * Lives as long as whatever container contains it.
  * This means that any member functions can effectively
    use it as long as the container lives.
  * Can cast to naked pointer (safely) as long as container is in scope.

class Container {
    shared_ptr(int) value;
};

void do_something(int *v)
{
    *v = 10;
}

// This is safe ONLY because
// Container is in scope when it is used.
void foo() {
    Container c;
    do_something(c.value);

    // This is also valid because
    // d is in scope.
    shared_ptr(Container) d;
    do_something(d->value);
}

// This is safe, but the down-side here
// is that you can't de-allocate the value
// unless you go all the way back to the root,
// so sharing memory across call-stacks can't
// really be done.  i.e. you can't allocate in
// one function and de-allocate in another one.

int & read_data() {
    return new int;
}

process_data(int * data) {
    *data = 10;
}

int &do_processing()
{
    // Initialize a reference to heap.
    int &v(read_data());

    // De-reference it.
    process_data(&v);

    return v;    // De-allocation doesn't happen because we're returning it so someone else can own it.
}

void main()
{
        int &v(do_processing());
} // De-allocation happens here because nobody else has it.
  // No pointers go onto the heap.

class Container {
    int &v;
};

void main() {
    Container & c(new Container(v=do_processing())); // Places c on the heap and 
};
-------------------------------------------------------------------------
Lists are where the rubber meets the road.  If we allow pushing and popping
values from a list, then each element may or may not still be active,
but when we go get one of them, we need to make sure it's not deleted out
from under us.

class List(T) {
    push(T v);       // Puts an element onto the list.
    pop();             // Removes it from the list.
    T & get(int i);  // Returns a pointer that's guaranteed to exist.
};

main() {
    List(int) list;
    list.push(10);
    int & v = list.get(0); // This returns a copy of the value?
    list.pop();

    do_something(v);
}; // v is decref at end of scope.


// Pointer stacks.

int main()
{
    <list state : ok>
    list_t list;
    list.append(p);

    <list state : Borrowed>
    // rule: list.get() moves list into borrowed (q) state.
    // At compile time, we accept a list in the 'ok' or 'borrowed' state
    // and append a reference to the list's 'borrowed' sate.
    list_el_t q = list.get(0);    // q points back to list
    list_el_t p = list.get(0);    // Another pointer is borrowed, so list goes into borrowed state (again)

    <list state : ok>
    // At compile time, if we give an instance back to the list, we're
    // no longer in the borrow state.  The function need have no implementation,
    // but will have the (compile-time) function of marking this
    // as having been returned to the collection.
    list.return(q)           // List removes q from borrow list.

    <!NO: we still have an outstanding borrow, so we can't do this.>
    list.pop();              // The pointer that points to the element
    
    list.return(p)           // List removes p from borrow list.

    <Requires that we be in the 'ok' state>
    list.iterate();

    <we can do this now because our borrows are empty>
    list.pop();              // The pointer that points to the element
    [p = 0x0111000, q = 0x0111000]

    p = 0;                   // Null this pointer.
    [q = 0x0111000]

    *q = 10;                 // This is still safe because q is still live.
} // Any borrows in current state are automatically restored at end of scope.
  // Here, we say p and q are owned by list, so must be returned

Rules:
list_t list;           // Constructor: set list to 'ok' state.
list.push();           // Do nothing, we're still ok.
list_el_t list.get();  // list.state = Borrowed
                       // list.borrows = [returned-variable-pointer]
                       // list_el_t.list.borrows.push_back(list_el_t)

~list_el_t             // list_el_t.list.borrows.erase(list_el_t)

list.pop();            // if (list.state != Borrowed) compile-error constraint violation.

list.return(p); // list.borrows remove [returned-variable-pointer]
               // if list.borrows.empty() list.state = ok;

class list_t(T) {
    T* elements;
    push(T *el) { T[i] = el; }
    get();
}

class list_el_t(list_t(T))
{
}
-------------------------------------------------------

Algorithm:
For each statement, process it in order.

compute_while : evaluate state after termination condition.
                                     start
                             |                  |
                        if_not_started       if_ended

compute_ifelse : Evaluate state after each branch.  Bifurcate state
                 as a binary-tree for each
                              start
                             |      |
                            if_a   if_b

So what results is that in order to process
a program, we start making an observation about
state.  Next, we make an observation about
each of the possible states that result.

This ends up exploding, but the simplification we can make
is that not all observations are relevant for all code paths.

             0_var_a                   0_var_b
                 |                       /\
                 |                      /  \
             1_var_a               1_b_1    1_b_2
               /\                    |        |
              /  \                   |        |
         2_a_1     2_a_2          2_b_1    2_b_2


So for the next stage of execution(3), we need to consider
all of the possible states (2_a_1,2_b_1) and (2_a_2,2_b_1) and ...
for all 2^n possible states.

But in some flows, for example, not all variables will be
relevant, so only the ones relevant (touched) during that flow
need be considered and all others will just "carry forward" to the
next flow.  For example, if variable 'x' isn't used anywhere in this
part of the flow, its value can simply be carried forward without
contributing to the explosion.

At the end of each flow, observations of two branches can be
combined.  For example, if the "is null" observation is made
and both branches make the same "is null" observation, this branch
can be combined since it's invariant of the branch.

--------------------------------------------------------------

In our linked-list get example, we have a list_get() operation.
It will have two outcomes.  If we observe "ret",
we can make some observations:

* is-null
* value-initialization


          list_get()
rc = null            rc != null
value-undef          value = valid-pointer

The null observation is easy because we
have everything inside the "get" to tell us that.
The "value" isn't so clear since it depends on the
list that was passed and how "value" got populated
which isn't available in the 'get', so we need
to consider how it was populated.  Also, it's on the
heap, so we may not know.


list_append()

list_get()

