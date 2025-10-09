#include <jlang-frontend.hpp>
#include <jlang-frontend/function-scope.hpp>

using namespace JLang::frontend;

static const JLang::context::SourceReference zero_source_ref("internal", 1, 0, 0);

int main(int argc, char **argv)
{
#if 0
    JLang::context::CompilerContext compiler_context("Some name");
    
    ScopeTracker tracker(compiler_context);

    tracker.add_variable("argc", nullptr, zero_source_ref);
    tracker.add_variable("argv", nullptr, zero_source_ref);
    tracker.scope_push();
    tracker.add_variable("foo", nullptr, zero_source_ref);
    tracker.add_label("label1", 2);
    
    tracker.scope_push();
    tracker.add_goto("label1");
    tracker.scope_pop();
    
    tracker.scope_pop();
//    tracker.add_goto("label1");

    tracker.dump();

    tracker.check();

    // We want to be able to:
    // Detect problems with goto/label
    // placement.

    // Unwind for each scope, what variables
    // need to be unwound for each goto
    // by finding the common ancestor for each
    // goto and unwinding the parents up to that
    // common ancestor.
    //
    // If the 'goto' has already been emitted,
    // find the BB and the operation of it
    // so we can 'insert' the unwind instructions.
    //
    // Question:
    // Should we track the scope beforehand
    // and then reference it when we emit the MIR
    // or should we do this during MIR generation
    // and do a "fixup" of the MIR after it's
    // written?

    // The latter seems cleaner?
#endif    
    return 0;
}
