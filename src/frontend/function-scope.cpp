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
#include <sstream>

using namespace Gyoji::frontend::lowering;

///////////////////////////////////////////////////
// Graph-theoretic utilities just to walk graphs
// and find intersections, differences.
///////////////////////////////////////////////////
static void walk_priors(
    const std::map<size_t, size_t> & backward_edges,
    size_t start_point,
    std::map<size_t, size_t> & priors)
{
    size_t p = start_point;
    while (p >= 0) {
	priors.insert(std::pair(p,p));
	const auto & it = backward_edges.find(p);
	if (it == backward_edges.end()) {
	    break;
	}
	p = it->second;
    }
}

static void
evaluate_scope_changes(
    const std::vector<const ScopeOperation*> & flat,
    const std::map<size_t, size_t> & prior_to_goto,
    const std::map<size_t, size_t> & prior_to_label,
    std::vector<const ScopeOperation*> & skipped_initializations,
    std::vector<const ScopeOperation*> & unwind_variables
    )
{
    for (const auto & lp : prior_to_label) {
	if (prior_to_goto.find(lp.first) == prior_to_goto.end()) {
	    const ScopeOperation *op = flat.at(lp.first);
	    if (op->get_type() == ScopeOperation::VAR_DECL) {
		skipped_initializations.push_back(op);
	    }
	}
    }
    for (const auto & gp : prior_to_goto) {
	if (prior_to_label.find(gp.first) == prior_to_label.end()) {
	    const ScopeOperation *op = flat.at(gp.first);
	    if (op->get_type() == ScopeOperation::VAR_DECL) {
		unwind_variables.push_back(op);
	    }
	}
    }
    // We reverse the list because
    // we should unwind the variables
    // in the reverse order they were declared in.
    std::reverse(unwind_variables.begin(), unwind_variables.end());
}

///////////////////////////////////////////////////
// ScopeOperation
///////////////////////////////////////////////////
ScopeOperation::ScopeOperation(
    ScopeOperationType _type,
    const Gyoji::context::SourceReference & _source_ref
    )
    : type(_type)
    , source_ref(_source_ref)
{}

ScopeOperation::~ScopeOperation()
{}

const ScopeOperation::ScopeOperationType &
ScopeOperation::get_type() const
{ return type; }

const std::string &
ScopeOperation::get_label_name() const
{ return label_name; }

const std::string &
ScopeOperation::get_goto_label() const
{ return goto_label; }

const FunctionPoint &
ScopeOperation::get_goto_point() const
{ return *goto_point; }

const std::string &
ScopeOperation::get_variable_name() const
{ return variable_name; }

const Gyoji::mir::Type *
ScopeOperation::get_variable_type() const
{ return variable_type; }

Gyoji::owned<ScopeOperation>
ScopeOperation::create_variable(
    std::string _variable_name,
    const Gyoji::mir::Type *_variable_type,
    const Gyoji::context::SourceReference & _source_ref
    )
{
    auto op = Gyoji::owned<ScopeOperation>(new ScopeOperation(ScopeOperation::VAR_DECL, _source_ref));
    op->variable_name = _variable_name;
    op->variable_type = _variable_type;
    return op;
}
Gyoji::owned<ScopeOperation>
ScopeOperation::create_label(
    std::string _label_name,
    const Gyoji::context::SourceReference & _source_ref
    )			     
{
    auto op = Gyoji::owned<ScopeOperation>(new ScopeOperation(ScopeOperation::LABEL_DEFINITION, _source_ref));
    op->label_name = _label_name;
    return op;
}

Gyoji::owned<ScopeOperation>
ScopeOperation::create_goto(
    std::string _goto_label,
    Gyoji::owned<FunctionPoint> _goto_point,
    const Gyoji::context::SourceReference & _source_ref
    )
{
    auto op = Gyoji::owned<ScopeOperation>(new ScopeOperation(ScopeOperation::GOTO_DEFINITION, _source_ref));
    op->goto_label = _goto_label;
    op->goto_point = std::move(_goto_point);
    return op;
}

Gyoji::owned<ScopeOperation>
ScopeOperation::create_child(
    Gyoji::owned<Scope> _child,
    const Gyoji::context::SourceReference & _source_ref
    )
{
    auto op = Gyoji::owned<ScopeOperation>(new ScopeOperation(ScopeOperation::CHILD_SCOPE, _source_ref));
    op->child = std::move(_child);
    return op;
}

const Gyoji::context::SourceReference &
ScopeOperation::get_source_ref() const
{ return source_ref; }

///////////////////////////////////////////////////
// Scope
///////////////////////////////////////////////////
Scope::Scope(bool _is_unsafe)
    : parent(nullptr)
    , scope_is_loop(false)
    , scope_is_unsafe(_is_unsafe)
    , loop_break_blockid(0)
    , loop_continue_blockid(0)
{}

Scope::Scope(
    bool _is_unsafe,
    bool _is_loop,
    size_t _loop_break_blockid,
    size_t _loop_continue_blockid
    )
    : parent(nullptr)
    , scope_is_loop(_is_loop)
    , scope_is_unsafe(_is_unsafe)
    , loop_break_blockid(_loop_break_blockid)
    , loop_continue_blockid(_loop_continue_blockid)
{}

Scope::~Scope()
{}

void
Scope::add_operation(Gyoji::owned<ScopeOperation> op)
{
    operations.push_back(std::move(op));
}

const LocalVariable *
Scope::get_variable(std::string name) const
{
    const auto & it = variables.find(name);
    if (it == variables.end()) {
	return nullptr;
    }
    return it->second.get();
}

bool
Scope::is_loop() const
{
    return scope_is_loop;
}
bool
Scope::is_unsafe() const
{
    return scope_is_unsafe;
}
size_t
Scope::get_loop_break_blockid() const
{ return loop_break_blockid; }

size_t
Scope::get_loop_continue_blockid() const
{ return loop_continue_blockid; }

void
Scope::add_variable(std::string name, const Gyoji::mir::Type *mir_type, const Gyoji::context::SourceReference & source_ref)
{
    Gyoji::owned<LocalVariable> local_variable = std::make_unique<LocalVariable>(mir_type, source_ref);
    variables.insert(std::pair(name, std::move(local_variable)));
    variables_in_declaration_order.push_back(name);
}

const std::map<std::string, Gyoji::owned<LocalVariable>> &
Scope::get_variables() const
{ return variables; }

const std::vector<std::string> &
Scope::get_variables_in_declaration_order() const
{ return variables_in_declaration_order; }

Scope *
Scope::get_parent() const
{ return parent; }

void
Scope::set_parent(Scope *_parent)
{ parent = _parent; }

///////////////////////////////////////////////////
// ScopeTracker
///////////////////////////////////////////////////
ScopeTracker::ScopeTracker(
    bool _root_is_unsafe,
    const Gyoji::context::CompilerContext & _compiler_context)
    : root(std::make_unique<Scope>(_root_is_unsafe))
    , compiler_context(_compiler_context)
    , tracker_prior_point()
    , tracker_backward_edges()
    , tracker_flat()
{
    tracker_prior_point.push_back((size_t)-1);
    current = root.get();
}

ScopeTracker::~ScopeTracker()
{
}


void
ScopeTracker::scope_push(bool _is_unsafe, const Gyoji::context::SourceReference & _source_ref)
{
    auto child_scope = std::make_unique<Scope>(_is_unsafe);
    Scope *new_current = child_scope.get();
    child_scope->set_parent(current);
    auto child_op = ScopeOperation::create_child(std::move(child_scope), _source_ref);
    current->add_operation(std::move(child_op));
    current = new_current;
    tracker_prior_point.push_back(tracker_prior_point.back());
}

void
ScopeTracker::scope_push_loop(const Gyoji::context::SourceReference & _source_ref, size_t _loop_break_blockid, size_t _loop_continue_blockid)
{
    // Scopes associated with loops cannot directly
    // be declared as unsafe, so if you want that, you should
    // do it in the parent scope of the loop.
    auto child_scope = std::make_unique<Scope>(false, true, _loop_break_blockid, _loop_continue_blockid);
    Scope *new_current = child_scope.get();
    child_scope->set_parent(current);
    auto child_op = ScopeOperation::create_child(std::move(child_scope), _source_ref);
    current->add_operation(std::move(child_op));
    current = new_current;    
    tracker_prior_point.push_back(tracker_prior_point.back());
}


void
ScopeTracker::scope_pop()
{
    current = current->get_parent();
    tracker_prior_point.pop_back();
}


const FunctionLabel *
ScopeTracker::get_label(std::string name) const
{
    const auto & it_forward_declared = labels_forward_declared.find(name);
    if (it_forward_declared != labels_forward_declared.end()) {
	return it_forward_declared->second.get();
    }
    const auto & it = labels.find(name);
    if (it != labels.end()) {
	return it->second.get();
    }
    return nullptr;
}

void
ScopeTracker::label_define(
    std::string label_name,
    const Gyoji::context::SourceReference & _source_ref
    )
{
    const auto & it_forward_declared = labels_forward_declared.find(label_name);
    if (it_forward_declared == labels_forward_declared.end()) {
	fprintf(stderr, "Compiler Bug!  We are attempting to resolve the definition of a forward-declared label, but it wasn't forward-declared.\n");
	exit(1);
	return;
    }
    it_forward_declared->second->resolve(_source_ref);
    labels.insert(std::pair(label_name, std::move(it_forward_declared->second)));
    labels_forward_declared.erase(it_forward_declared);

    auto op = ScopeOperation::create_label(label_name, _source_ref);
    tracker_label_locations.insert(std::pair(op->get_label_name(), tracker_flat.size()));
    add_flat_op(op.get());
    
    add_operation(std::move(op));
}

void
ScopeTracker::label_define(
    std::string label_name,
    size_t label_blockid,
    const Gyoji::context::SourceReference & _source_ref
    )
{
    Gyoji::owned<FunctionLabel> new_label = std::make_unique<FunctionLabel>(label_blockid);
    new_label->resolve(_source_ref);
    labels.insert(std::pair(label_name, std::move(new_label)));

    auto op = ScopeOperation::create_label(label_name, _source_ref);
    tracker_label_locations.insert(std::pair(op->get_label_name(), tracker_flat.size()));
    add_flat_op(op.get());
    add_operation(std::move(op));
}
    
// Use this for 'goto' to say we want a label, but we
// don't yet know where it will live, so put it on the
// notfound labels list.
void
ScopeTracker::label_declare(std::string label_name, size_t label_blockid)
{
    Gyoji::owned<FunctionLabel> new_label = std::make_unique<FunctionLabel>(label_blockid);
    labels_forward_declared.insert(std::pair(label_name, std::move(new_label)));
}

void
ScopeTracker::add_flat_op(const ScopeOperation *op)
{
    if (tracker_prior_point.back() != (size_t)-1) {
	tracker_backward_edges.insert(std::pair(tracker_flat.size(), tracker_prior_point.back()));
    }
    tracker_prior_point.back() = tracker_flat.size();
    tracker_flat.push_back(op);
}


void
ScopeTracker::add_goto(
    std::string goto_label,
    Gyoji::owned<FunctionPoint> goto_point,
    const Gyoji::context::SourceReference & _source_ref
    )
{
    // TODO:
    // Check if this label exists and jump to that
    // label if it does.  If it doesn't, forward declare it.
    // and put it in the 'forward declared' labels list.
    auto op = ScopeOperation::create_goto(goto_label, std::move(goto_point), _source_ref);
    tracker_goto_labels_at.insert(std::pair(tracker_flat.size(), op->get_goto_label()));
    add_flat_op(op.get());
    add_operation(std::move(op));
}

const LocalVariable *
ScopeTracker::get_variable(std::string variable_name) const
{
    // Walk up from the current scope up to the root and
    // see if this variable is defined anywhere.
    const Scope *s = current;
    while (s) {
	const LocalVariable *existing_local_variable = s->get_variable(variable_name);
	if (existing_local_variable != nullptr) {
	    return existing_local_variable;
	}
	s = s->get_parent();
    }
    return nullptr;
}

std::vector<std::string>
ScopeTracker::get_variables_to_unwind_for_root() const
{
    std::vector<std::string> variables_to_unwind;
    const Scope *s = current;
    while (s) {
	// Reverse order because we want to
	// call destructors in the reverse order of declaration.
	auto & vars = s->get_variables_in_declaration_order();
	for (std::vector<std::string>::const_reverse_iterator it = vars.rbegin(); it != vars.rend(); ++it) {
	    variables_to_unwind.push_back(*it);
	}
	s = s->get_parent();
    }
    return variables_to_unwind;
}

std::vector<std::string>
ScopeTracker::get_variables_to_unwind_for_scope() const
{
    std::vector<std::string> variables_to_unwind;
    const Scope *s = current;
    const auto & vars = s->get_variables_in_declaration_order();
    for (std::vector<std::string>::const_reverse_iterator it = vars.rbegin(); it != vars.rend(); ++it) {
	variables_to_unwind.push_back(*it);
    }
    return variables_to_unwind;
}

std::vector<std::string>
ScopeTracker::get_variables_to_unwind_for_break() const
{
    std::vector<std::string> variables_to_unwind;
    const Scope *s = current;
    while (s && s->is_loop()) {
	const auto & vars = s->get_variables_in_declaration_order();
	for (std::vector<std::string>::const_reverse_iterator it = vars.rbegin(); it != vars.rend(); ++it) {
	    variables_to_unwind.push_back(*it);
	}
	s = s->get_parent();
    }
    return variables_to_unwind;

}

const Scope *
ScopeTracker::get_current() const
{ return current; }


bool
ScopeTracker::is_unsafe() const
{
    Scope* cur = current;
    while (cur) {
	if (cur->is_unsafe()) {
	    return true;
	}
	cur = cur->get_parent();
    }
    return false;
}

bool
ScopeTracker::add_variable(std::string variable_name, const Gyoji::mir::Type *mir_type, const Gyoji::context::SourceReference & source_ref)
{
    // Walk up from the current scope up to the root and
    // see if this variable is defined anywhere.
    const LocalVariable *maybe_existing = get_variable(variable_name);

    if (maybe_existing != nullptr) {
	std::unique_ptr<Gyoji::context::Error> error = std::make_unique<Gyoji::context::Error>("Duplicate Local Variable.");
	error->add_message(source_ref,
			   std::string("Variable with name ") + variable_name + " is already in scope and cannot be duplicated.");
	error->add_message(maybe_existing->get_source_ref(),
			   "First declared here.");
	
	compiler_context
	    .get_errors()
	    .add_error(std::move(error));
	return false;
    }

    
    // Variable was not declared earlier, so we add it to
    // the current scope.
    current->add_variable(variable_name, mir_type, source_ref);
    
    auto local_var_op = ScopeOperation::create_variable(variable_name, mir_type, source_ref);
    add_flat_op(local_var_op.get());
    add_operation(std::move(local_var_op));

    return true;
}


// TODO: We should give a better interface
// here so we can easily declare locals, labels, gotos.
void
ScopeTracker::add_operation(Gyoji::owned<ScopeOperation> op)
{
    current->add_operation(std::move(op));
}


// Now from this, I can evaluate
// what's legal.  We only need to add the 'SourceReference'
// stuff in order to produce good errors for this.
void
ScopeTracker::dump() const
{
    Scope *s = root.get();
    fprintf(stderr, "{\n");
    s->dump(0);
    fprintf(stderr, "}\n");
    for (size_t i = 0; i < tracker_flat.size(); i++) {
	const auto & op = tracker_flat.at(i);
	switch (op->get_type()) {
	case ScopeOperation::VAR_DECL:
	    fprintf(stderr, "%ld Var decl %s\n", i, op->get_variable_name().c_str());
	    break;
	case ScopeOperation::LABEL_DEFINITION:
	    fprintf(stderr, "%ld label %s\n", i, op->get_label_name().c_str());
	    break;
	case ScopeOperation::GOTO_DEFINITION:
	    fprintf(stderr, "%ld goto %s\n", i, op->get_goto_label().c_str());
	    break;
	case ScopeOperation::CHILD_SCOPE:
	    fprintf(stderr, "This should not be here\n");
	    exit(1);
	}
    }
}

bool
ScopeTracker::is_in_loop() const
{
    const Scope *s = current;
    while (s) {
	if (s->is_loop()) {
	    return true;
	}
	s = s->get_parent();
    }
    return false;
}

size_t
ScopeTracker::get_loop_break_blockid() const
{
    const Scope *s = current;
    while (s) {
	if (s->is_loop()) {
	    return s->get_loop_break_blockid();
	}
	s = s->get_parent();
    }
    return 0;
}

size_t
ScopeTracker::get_loop_continue_blockid() const
{
    const Scope *s = current;
    while (s) {
	if (s->is_loop()) {
	    return s->get_loop_continue_blockid();
	}
	s = s->get_parent();
    }
    return 0;
}

bool
ScopeTracker::check(
    std::vector<std::pair<const ScopeOperation*, std::vector<const ScopeOperation*>>> & goto_fixups
    ) const
{
    bool ok = true;
    for (const auto & goto_point : tracker_goto_labels_at) {
	const ScopeOperation *goto_operation = tracker_flat.at(goto_point.first);
	const FunctionLabel *function_label = get_label(goto_operation->get_goto_label());
	if (function_label == nullptr || !function_label->is_resolved()) {
	    std::unique_ptr<Gyoji::context::Error> error = std::make_unique<Gyoji::context::Error>("Goto for an un-defined label.");
	    error->add_message(goto_operation->get_source_ref(),
			       std::string("Goto label ") + goto_operation->get_goto_label() + " had an undefined destination.");
	    compiler_context
		.get_errors()
		.add_error(std::move(error));
	    ok = false;
	    continue;
	}
	    
	std::map<size_t, size_t> prior_to_goto;
	walk_priors(tracker_backward_edges, goto_point.first, prior_to_goto);
	
	std::map<size_t, size_t> prior_to_label;
	const auto it = tracker_label_locations.find(goto_point.second);
	if (it == tracker_label_locations.end()) {
	    fprintf(stderr, "This is a bug, we have a defined label, but no location for it\n");
	    exit(1);
	}
	size_t label_point = it->second;
	
	walk_priors(tracker_backward_edges, label_point, prior_to_label);
	
	std::vector<const ScopeOperation*> skipped_initializations;
	std::vector<const ScopeOperation*> unwind_variables;
	
	evaluate_scope_changes(tracker_flat, prior_to_goto, prior_to_label, skipped_initializations, unwind_variables);

	if (skipped_initializations.size() > 0) {
	    std::unique_ptr<Gyoji::context::Error> error = std::make_unique<Gyoji::context::Error>("Goto would skip initialization.");
	    error->add_message(goto_operation->get_source_ref(),
			       std::string("Goto label ") + goto_operation->get_goto_label() + std::string(" would skip initialization of variables in destination scope."));
	    error->add_message(function_label->get_source_ref(),
			       "Label declared here.");
	    error->add_message(skipped_initializations.at(0)->get_source_ref(),
			       "Skipped initialization occurs here.");
	    compiler_context
		.get_errors()
		.add_error(std::move(error));
	    ok = false;
	}
	    
	goto_fixups.push_back(std::pair(goto_operation, unwind_variables));
	for (const auto & s : unwind_variables) {
	    fprintf(stderr, "Unwind %s\n", s->get_variable_name().c_str());
	}
    }
    return ok;
}

void
ScopeOperation::dump(int indent) const
{
    std::string pad(indent*8, ' ');
    
    switch (type) {
    case ScopeOperation::VAR_DECL:
    {
	std::string var = pad + std::string("var ") + variable_name;
	fprintf(stderr, "%s\n", var.c_str());
    }
	break;
    case ScopeOperation::LABEL_DEFINITION:
    {
	std::string var = pad + std::string("label ") + label_name;
	fprintf(stderr, "%s\n", var.c_str());
    }
	break;
    case ScopeOperation::GOTO_DEFINITION:
    {
	std::string var = pad + std::string("goto ") + goto_label;
	fprintf(stderr, "%s\n", var.c_str());
    }
	break;
    case ScopeOperation::CHILD_SCOPE:
    {
	std::string open = pad + "{";
	fprintf(stderr, "%s\n", open.c_str());
	child->dump(indent+1);
	std::string close = pad + "}";
	fprintf(stderr, "%s\n", close.c_str());
    }
	break;
    }
}

void
Scope::dump(int indent) const
{
    std::string pad(indent*8, ' ');
    for (const auto & op : operations) {
	op->dump(indent+1);
    }
}

///////////////////////////////////////////
LocalVariable::LocalVariable(
    const Gyoji::mir::Type *_type,
    const Gyoji::context::SourceReference & _source_ref
    )
    : type(_type)
    , source_ref(_source_ref)
{}

LocalVariable::~LocalVariable()
{}

const Gyoji::mir::Type *
LocalVariable::get_type() const
{ return type; }

const Gyoji::context::SourceReference &
LocalVariable::get_source_ref() const
{ return source_ref; }
 
/////////////////////////////////////
// FunctionLabel
/////////////////////////////////////
FunctionLabel::FunctionLabel(
    size_t _block_id
    )
    : resolved(false)
    , block_id(_block_id)
    , src_ref(nullptr)
{}
FunctionLabel::~FunctionLabel()
{}

const Gyoji::context::SourceReference &
FunctionLabel::get_source_ref() const
{ return *src_ref; }

size_t
FunctionLabel::get_block() const
{ return block_id; }

bool
FunctionLabel::is_resolved() const
{ return resolved; }

void
FunctionLabel::resolve(const Gyoji::context::SourceReference & _src_ref)
{
    resolved = true;
    src_ref = &_src_ref;
}

////////////////////////////////////////////////
// FunctionPoint
////////////////////////////////////////////////
FunctionPoint::FunctionPoint(size_t _basic_block_id, size_t _location)
    : basic_block_id(_basic_block_id)
    , location(_location)
{}
FunctionPoint::~FunctionPoint()
{}

size_t
FunctionPoint::get_basic_block_id() const
{ return basic_block_id; }

size_t
FunctionPoint::get_location() const
{ return location; }
