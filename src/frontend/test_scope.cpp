/* Copyright 2025 Jonathan S. Arney
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      https://github.com/jarney/gyoji/blob/master/LICENSE
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#include <gyoji-frontend.hpp>
#include <gyoji-frontend/function-scope.hpp>

using namespace Gyoji::frontend::lowering;

static const std::string zero_source_filename = "internal";
static const Gyoji::context::SourceReference zero_source_ref(zero_source_filename, 1, 0, 0);

int test_jump_backward_ok();
int test_jump_forward_ok();
int test_jump_backward_skip_initialization();

int main(int argc, char **argv)
{
    {
	int rc = test_jump_backward_skip_initialization();
	if (rc != 0) {
	    // Test failure
	    return rc;
	}
    }
    {
	
	int rc = test_jump_backward_ok();
	if (rc != 0) {
	    // Test failure
	    return rc;
	}
    }
    {
	
	int rc = test_jump_forward_ok();
	if (rc != 0) {
	    // Test failure
	    return rc;
	}
    }
    printf("PASSED\n");
    return 0;
}

/**
 * This is a case where the 'goto' is in an
 * earlier scope than the label and would jump
 * into that later scope without skipping
 * any initializations.
 */

/**
 * This is a case where the 'goto' is in a later
 * scope than the label and would jump into the earlier
 * scope without skipping any initializations.
 */
int test_jump_backward_ok()
{
    Gyoji::context::CompilerContext context("Some name");
    
    ScopeTracker tracker(context);

    tracker.add_variable("argc", nullptr, zero_source_ref);
    tracker.add_variable("argv", nullptr, zero_source_ref);

    tracker.scope_push(zero_source_ref);
    tracker.label_define("label1", 2, zero_source_ref);
    tracker.add_variable("foo", nullptr, zero_source_ref);
    tracker.scope_pop();
    
    tracker.scope_push(zero_source_ref);
    tracker.add_goto("label1", std::make_unique<FunctionPoint>(0, 1), zero_source_ref);
    tracker.scope_pop();

    tracker.add_goto("label1", std::make_unique<FunctionPoint>(1, 3), zero_source_ref);

    tracker.dump();

    std::vector<std::pair<const ScopeOperation*, std::vector<const ScopeOperation*>>> goto_fixups;
    tracker.check(goto_fixups);
    if (context.has_errors()) {
	context.get_errors().print();
	// This is an error which is expected,
	// so the test passes.
	return -1;
    }

    // This would be a test failure if we don't
    // raise an error for this.
    return 0;
}
int test_jump_forward_ok()
{
    Gyoji::context::CompilerContext context("Some name");
    
    ScopeTracker tracker(context);

    tracker.add_variable("argc", nullptr, zero_source_ref);
    tracker.add_variable("argv", nullptr, zero_source_ref);

    tracker.scope_push(zero_source_ref);
    {
        // Note that the goto and declare must happen together.
	// I don't like that these aren't atomic, but the caller
	// is responsible for getting a new basic block, but the
	// tracker is responsible for consuming it.
        tracker.add_goto("label1", std::make_unique<FunctionPoint>(0, 1), zero_source_ref);
        tracker.label_declare("label1", 2);
    }
    tracker.scope_pop();

    tracker.scope_push(zero_source_ref);
    {
	// This one doesn't need a basic block ID
	// because it was forward declared with the goto above.
        tracker.label_define("label1", zero_source_ref);
    }
    tracker.add_variable("foo", nullptr, zero_source_ref);
    tracker.scope_pop();

    tracker.dump();

    std::vector<std::pair<const ScopeOperation*, std::vector<const ScopeOperation*>>> goto_fixups;
    tracker.check(goto_fixups);
    if (context.has_errors()) {
	context.get_errors().print();
	// This is an error which is expected,
	// so the test passes.
	return -1;
    }

    // This would be a test failure if we don't
    // raise an error for this.
    return 0;
}

/**
 * This is a case where the 'goto' is in a later
 * scope and would jump back inside a scope
 * where some variables are declared.
 */
int test_jump_backward_skip_initialization()
{
    Gyoji::context::CompilerContext context("Some name");
    
    ScopeTracker tracker(context);

    tracker.add_variable("argc", nullptr, zero_source_ref);
    tracker.add_variable("argv", nullptr, zero_source_ref);

    tracker.scope_push(zero_source_ref);
    tracker.add_variable("one", nullptr, zero_source_ref);
    
    tracker.scope_push(zero_source_ref);
    tracker.add_variable("two", nullptr, zero_source_ref);
    tracker.label_define("label1", 2, zero_source_ref);
    
    tracker.scope_pop();
    tracker.scope_pop();
    tracker.add_variable("three", nullptr, zero_source_ref);    
    tracker.add_variable("three_1", nullptr, zero_source_ref);    
    tracker.add_goto("label1", std::make_unique<FunctionPoint>(0, 1), zero_source_ref);
    tracker.add_goto("label2", std::make_unique<FunctionPoint>(1, 1), zero_source_ref);
    tracker.add_variable("five", nullptr, zero_source_ref);

    tracker.scope_push(zero_source_ref);
    tracker.add_variable("six", nullptr, zero_source_ref);
    
    tracker.scope_push(zero_source_ref);
    tracker.add_variable("four", nullptr, zero_source_ref);
    tracker.label_define("label2", 2, zero_source_ref);
    tracker.scope_pop();
    
    tracker.add_variable("argc", nullptr, zero_source_ref);

    tracker.scope_pop();

    std::vector<std::pair<const ScopeOperation*, std::vector<const ScopeOperation*>>> goto_fixups;
    tracker.check(goto_fixups);
    tracker.dump();

    if (context.has_errors()) {
	context.get_errors().print();
	// This is an error which is expected,
	// so the test passes.
	return 0;
    }
    // This would be a test failure if we don't
    // raise an error for this.
    return -1;
}
