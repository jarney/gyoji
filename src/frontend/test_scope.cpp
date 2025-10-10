#include <gyoji-frontend.hpp>
#include <gyoji-frontend/function-scope.hpp>

using namespace Gyoji::frontend::lowering;

static const std::string zero_source_filename = "internal";
static const Gyoji::context::SourceReference zero_source_ref(zero_source_filename, 1, 0, 0);

int main(int argc, char **argv)
{

    Gyoji::context::CompilerContext context("Some name");
    
    ScopeTracker tracker(context);

    tracker.add_variable("argc", nullptr, zero_source_ref);
    tracker.add_variable("argv", nullptr, zero_source_ref);

    tracker.scope_push(zero_source_ref);
    tracker.add_variable("foo", nullptr, zero_source_ref);
    tracker.label_define("label1", 2, zero_source_ref);
    
    tracker.scope_push(zero_source_ref);
    tracker.add_goto("label1", zero_source_ref);
    tracker.scope_pop();
    
    tracker.scope_pop();
    tracker.add_goto("label1", zero_source_ref);

    tracker.dump();

    tracker.check();
    if (context.has_errors()) {
	context.get_errors().print();
	return -1;
    }
#if 0

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
