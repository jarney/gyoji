#include <jlang-frontend.hpp>
#include <jlang-frontend/function-scope.hpp>

using namespace JLang::frontend;

int main(int argc, char **argv)
{

    ScopeTracker tracker;

    tracker.add_variable("argc");
    tracker.add_variable("argv");
    tracker.scope_push();
    tracker.add_variable("foo");
    tracker.add_label("label1");
    
    tracker.scope_push();
    tracker.add_goto("label1");
    tracker.scope_pop();
    
    tracker.scope_pop();
//    tracker.add_goto("label1");

    tracker.dump();

    tracker.check();
    
    return 0;
}
