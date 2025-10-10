
#include <gyoji-frontend.hpp>
#include <gyoji-frontend/function-scope.hpp>
#include <sstream>

using namespace Gyoji::frontend::lowering;

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

const std::string &
ScopeOperation::get_variable_name() const
{ return variable_name; }

const Scope *
ScopeOperation::get_child() const
{ return child.get(); }

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
    const Gyoji::context::SourceReference & _source_ref
    )
{
    auto op = Gyoji::owned<ScopeOperation>(new ScopeOperation(ScopeOperation::GOTO_DEFINITION, _source_ref));
    op->goto_label = _goto_label;
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

Scope::Scope()
    : parent(nullptr)
    , scope_is_loop(false)
    , loop_break_blockid(0)
    , loop_continue_blockid(0)
{}

Scope::Scope(
    bool _is_loop,
    size_t _loop_break_blockid,
    size_t _loop_continue_blockid
    )
    : parent(nullptr)
    , scope_is_loop(_is_loop)
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

void
Scope::add_variable(std::string name, const Gyoji::mir::Type *type, const Gyoji::context::SourceReference & source_ref)
{
    Gyoji::owned<LocalVariable> local_variable = std::make_unique<LocalVariable>(name, type, source_ref);
    variables.insert(std::pair(name, std::move(local_variable)));
    auto local_var_op = ScopeOperation::create_variable(name, type, source_ref);
    operations.push_back(std::move(local_var_op));
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
size_t
Scope::get_loop_break_blockid() const
{ return loop_break_blockid; }

size_t
Scope::get_loop_continue_blockid() const
{ return loop_continue_blockid; }

const std::map<std::string, Gyoji::owned<LocalVariable>> &
Scope::get_variables() const
{ return variables; }

ScopeTracker::ScopeTracker(const Gyoji::context::CompilerContext & _compiler_context)
    : root(std::make_unique<Scope>())
    , compiler_context(_compiler_context)
{
    current = root.get();
}

ScopeTracker::~ScopeTracker()
{
}


void
ScopeTracker::scope_push(const Gyoji::context::SourceReference & _source_ref)
{
    auto child_scope = std::make_unique<Scope>();
    Scope *new_current = child_scope.get();
    child_scope->parent = current;
    auto child_op = ScopeOperation::create_child(std::move(child_scope), _source_ref);
    current->add_operation(std::move(child_op));
    current = new_current;
}
void
ScopeTracker::scope_push_loop(const Gyoji::context::SourceReference & _source_ref, size_t _loop_break_blockid, size_t _loop_continue_blockid)
{
    auto child_scope = std::make_unique<Scope>(true, _loop_break_blockid, _loop_continue_blockid);
    Scope *new_current = child_scope.get();
    child_scope->parent = current;
    auto child_op = ScopeOperation::create_child(std::move(child_scope), _source_ref);
    current->add_operation(std::move(child_op));
    current = new_current;    
}


void
ScopeTracker::scope_pop()
{
    current = current->parent;
}


const FunctionLabel *
ScopeTracker::get_label(std::string name) const
{
    const auto & it_notfound = notfound_labels.find(name);
    if (it_notfound != notfound_labels.end()) {
	return it_notfound->second.get();
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
    const auto & it_notfound = notfound_labels.find(label_name);
    if (it_notfound == notfound_labels.end()) {
	fprintf(stderr, "This should not happen\n");
	exit(1);
	return;
    }
    it_notfound->second->set_scope(current, _source_ref);
    labels.insert(std::pair(label_name, std::move(it_notfound->second)));
    notfound_labels.erase(it_notfound);
    add_operation(ScopeOperation::create_label(label_name, _source_ref));
}

void
ScopeTracker::label_define(
    std::string label_name,
    size_t label_blockid,
    const Gyoji::context::SourceReference & _source_ref
    )
{
    Gyoji::owned<FunctionLabel> new_label = std::make_unique<FunctionLabel>(label_name, label_blockid);
    new_label->set_scope(current, _source_ref);
    labels.insert(std::pair(label_name, std::move(new_label)));
    add_operation(ScopeOperation::create_label(label_name, _source_ref));
}
    
// Use this for 'goto' to say we want a label, but we
// don't yet know where it will live, so put it on the
// notfound labels list.
void
ScopeTracker::label_declare(std::string label_name, size_t label_blockid)
{
    Gyoji::owned<FunctionLabel> new_label = std::make_unique<FunctionLabel>(label_name, label_blockid);
    notfound_labels.insert(std::pair(label_name, std::move(new_label)));
}


void
ScopeTracker::add_goto(
    std::string goto_label,
    const Gyoji::context::SourceReference & _source_ref
    )
{
    // TODO:
    // Check if this label exists and jump to that
    // label if it does.  If it doesn't, forward declare it.
    // and put it in the 'forward declared' labels list.
    add_operation(ScopeOperation::create_goto(goto_label, _source_ref));
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
	s = s->parent;
    }
    return nullptr;
}

std::vector<std::string>
ScopeTracker::get_variables_to_unwind_for_root() const
{
    std::vector<std::string> variables_to_unwind;
    const Scope *s = current;
    while (s) {
	for (const auto & it : s->get_variables()) {
	    variables_to_unwind.push_back(it.first);
	}
	s = s->parent;
    }
    return variables_to_unwind;
}

std::vector<std::string>
ScopeTracker::get_variables_to_unwind_for_scope() const
{
    std::vector<std::string> variables_to_unwind;
    const Scope *s = current;
    for (const auto & it : s->get_variables()) {
	variables_to_unwind.push_back(it.first);
    }
    return variables_to_unwind;
}

std::vector<std::string>
ScopeTracker::get_variables_to_unwind_for_break() const
{
    std::vector<std::string> variables_to_unwind;
    const Scope *s = current;
    while (s && s->is_loop()) {
	for (const auto & it : s->get_variables()) {
	    variables_to_unwind.push_back(it.first);
	}
	s = s->parent;
    }
    return variables_to_unwind;

}

const Scope *
ScopeTracker::get_current() const
{ return current; }

bool
ScopeTracker::add_variable(std::string variable_name, const Gyoji::mir::Type *mir_type, const Gyoji::context::SourceReference & source_ref)
{
    // Walk up from the current scope up to the root and
    // see if this variable is defined anywhere.
    const Scope *s = current;
    while (s) {
	const LocalVariable *existing_local_variable = s->get_variable(variable_name);
	if (existing_local_variable != nullptr) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    existing_local_variable->get_source_ref(),
		    "Already declared",
		    "Already declared here");
	    return false;
	}
	s = s->parent;
    }
    // Variable was not declared earlier, so we add it to
    // the current scope.
    current->add_variable(variable_name, mir_type, source_ref);
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
}

void
Scope::dump_flat(
    std::vector<ScopeFlatElement> & flat,
    std::map<std::string, size_t> & label_locations,
    std::map<size_t, std::string> & goto_labels_at,
    size_t prior_point,
    std::map<size_t, size_t> & edges
    ) const
{
    const Scope *s = this;
    std::string prefix;

    std::vector<const Scope*> scopes;

    while (s) {
	std::stringstream ss;
	ss << std::hex << ((unsigned long)(void*)s) << std::string(" ");
	prefix = ss.str() + prefix;
	scopes.push_back(s);
	s = s->parent;
    }
    for (const auto & op : operations) {
	switch (op->get_type()) {
	case ScopeOperation::VAR_DECL:	    
	{
	    std::string s = prefix + std::string("var ") + op->get_variable_name();
	    fprintf(stderr, "%s\n", s.c_str());

	    size_t point = flat.size();
	    edges.insert(std::pair(point, prior_point));
	    prior_point = point;
	}
	break;
	case ScopeOperation::LABEL_DEFINITION:
	{
	    label_locations.insert(std::pair(op->get_label_name(), flat.size()));
	    std::string s = prefix + std::string("label ") + op->get_label_name();
	    fprintf(stderr, "%s\n", s.c_str());

	    size_t point = flat.size();
	    edges.insert(std::pair(point, prior_point));
	    prior_point = point;
	}
	break;
	case ScopeOperation::GOTO_DEFINITION:
	{
	    std::string s = prefix + std::string("goto ") + op->get_goto_label();
	    fprintf(stderr, "%s\n", s.c_str());
	    goto_labels_at.insert(std::pair(flat.size(), op->get_goto_label()));
	    
	    size_t point = flat.size();
	    edges.insert(std::pair(point, prior_point));
	    prior_point = point;
	}
	break;
	case ScopeOperation::CHILD_SCOPE:
	{
	    op->get_child()->dump_flat(flat, label_locations, goto_labels_at, prior_point, edges);
        }
	    break;
	}
	if (op->get_type() != ScopeOperation::CHILD_SCOPE) {
	    ScopeFlatElement flatel;
	    flatel.scopes = scopes;
	    flatel.operation = op.get();
	    flat.push_back(flatel);
	}
    }
}

static std::string elements_to_string(const std::vector<const Scope *> & scopes)
{
    std::string prefix;
    for (const auto & s : scopes) {
	std::stringstream ss;
	ss << std::hex << ((unsigned long)(void*)s) << std::string(" ");
	prefix = ss.str() + prefix;
    }
    return prefix;
}

#if 0
static void get_points_prior_to(const std::vector<ScopeFlatElement> & flat, size_t point, std::vector<size_t> & prior_points)
{
    while (true) {
	const ScopeFlatElement & el = flat.at(point);
	prior_points.push_back(point);
	if (point == 0) {
	    break;
	}
	point--;
    }
}
#endif

static void dump_priors(const std::map<size_t, size_t> & points)
{
    for (const auto & s : points) {
	fprintf(stderr, "   %ld\n", s);
    }
}

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
	if (it->second == -1) {
	    break;
	}
	p = it->second;
    }
}

void skipped_initializations(
    const std::vector<ScopeFlatElement> & flat,
    const std::map<size_t, size_t> & prior_to_goto,
    const std::map<size_t, size_t> & prior_to_label)
{
    for (const auto & lp : prior_to_label) {
	if (prior_to_goto.find(lp.first) == prior_to_goto.end()) {
	    const ScopeFlatElement & fp = flat.at(lp.first);
	    if (fp.operation->get_type() == ScopeOperation::VAR_DECL) {
		fprintf(stderr, "Skipped initialization at %ld : %s\n", lp.first, fp.operation->get_variable_name().c_str());
	    }
	}
    }
    for (const auto & gp : prior_to_goto) {
	if (prior_to_label.find(gp.first) == prior_to_label.end()) {
	    const ScopeFlatElement & fp = flat.at(gp.first);
	    if (fp.operation->get_type() == ScopeOperation::VAR_DECL) {
		fprintf(stderr, "Unwind variable at %ld : %s\n", gp.first, fp.operation->get_variable_name().c_str());
	    }
	}
    }

}

void
ScopeTracker::dump_flat() const
{
    std::vector<ScopeFlatElement> flat;
    std::map<std::string, size_t> label_locations;
    std::map<size_t, std::string> goto_labels_at;
    size_t prior_point = -1;

    std::map<size_t, size_t> backward_edges;
    root->dump_flat(flat, label_locations, goto_labels_at, prior_point, backward_edges);

    for (const auto & e : backward_edges) {
	fprintf(stderr, "Edge %ld %ld\n", e.first, e.second);
    }
    
    fprintf(stderr, "Doing real check for skipped\n");
    for (const auto & goto_point : goto_labels_at) {
	fprintf(stderr, "Checking goto point %ld to label %s\n", goto_point.first, goto_point.second.c_str());
	std::map<size_t, size_t> prior_to_goto;
	walk_priors(backward_edges, goto_point.first, prior_to_goto);
	
	fprintf(stderr, "Points prior to goto:\n");
	dump_priors(prior_to_goto);
	
	std::map<size_t, size_t> prior_to_label;
	size_t label_point = label_locations[goto_point.second];
	walk_priors(backward_edges, label_point, prior_to_label);
	
	fprintf(stderr, "Points prior to label:\n");
	dump_priors(prior_to_label);

	skipped_initializations(flat, prior_to_goto, prior_to_label);
	
    }
    
#if 0
    for (const auto & goto_point : goto_labels_at) {
	// For each goto point, walk backward to get
	// statements prior to it.
	std::vector<size_t> prior_to_goto;
	std::vector<size_t> prior_to_label;

	get_points_prior_to(flat, goto_point.first, prior_to_goto);
	fprintf(stderr, "Points prior to goto:\n");
	dump_priors(prior_to_goto);

	size_t label_point = label_locations[goto_point.second];
	get_points_prior_to(flat, label_point, prior_to_label);
	fprintf(stderr, "Points prior to label\n");
	dump_priors(prior_to_label);

    }
#endif
    
    fprintf(stderr, "Actual flat representation\n");
    for (size_t i = 0; i < flat.size(); i++) {
	const auto & el = flat.at(i);
//	std::string scopes = elements_to_string(el.scopes);
//	std::string prefix = std::to_string(i) + std::string(" ") + scopes + std::string(" ");
	std::string prefix = std::to_string(i) + std::string(" ");
	const ScopeOperation *op = el.operation;
	switch (op->get_type()) {
	case ScopeOperation::VAR_DECL:
	{
	    std::string s = prefix + std::string("variable ") + op->get_variable_name();
	    fprintf(stderr, "%s\n", s.c_str());
	}
	    break;
	case ScopeOperation::LABEL_DEFINITION:
	{
	    std::string s = prefix + std::string("label ") + op->get_label_name();
	    fprintf(stderr, "%s\n", s.c_str());
	}
	    break;
	case ScopeOperation::GOTO_DEFINITION:
	{
	    std::string s = prefix + std::string("goto ") + op->get_goto_label();
	    fprintf(stderr, "%s\n", s.c_str());
	}
	    break;

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
	s = s->parent;
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
	s = s->parent;
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
	s = s->parent;
    }
    return 0;
}

bool
Scope::is_ancestor(const Scope *other) const
{
    const Scope *s = other;
    while (s) {
	if (s == this) {
	    return true;
	}
	s = s->parent;
    }
    return false;
}


// This needs to be much more sophisticated
// so that it can walk backward through the
// operations and up the scope until it finds
// a possible initialization that we've skipped.
bool
Scope::skips_initialization(std::string label) const
{
    bool initialization_happened = false;
    for (const auto & op : operations) {
	if (op->get_type() == ScopeOperation::VAR_DECL) {
	    initialization_happened = true;
	}
	else if (op->get_type() == ScopeOperation::LABEL_DEFINITION) {
	    if (op->get_label_name() == label) {
		if (initialization_happened) {
		    return true;
		}
	    }
	}
    }
    return false;
}


const Scope *
ScopeTracker::find_common_ancestor(const Scope *goto_scope, const Scope *label_scope) const
{
    const Scope *goto_scope_parent = goto_scope;
    while (true) {
	if (goto_scope_parent == nullptr) {
	    fprintf(stderr, "We made it up to the root\n");
	    break;
	}
	fprintf(stderr, "Checking to see if label scope\n");
	label_scope->dump(1);
	fprintf(stderr, "is an ancestor of goto scope\n");
	goto_scope_parent->dump(1);
	if (!label_scope->is_ancestor(goto_scope_parent)) {
	    break;
	}
	goto_scope_parent = goto_scope_parent->parent;
    }
    if (goto_scope_parent != nullptr) {
	fprintf(stderr, "Found common ancestor\n");
	goto_scope_parent->dump(1);
	return goto_scope_parent;
    }
    else {
	fprintf(stderr, "No common ancestor found.  This is a bug because they should be in the same tree.\n");
	return nullptr;
    }
}

/**
 * Start with the current scope (scope of the label)
 * and walk it backward looking for variable definitions.
 * When we get to the top, move to the parent scope and keep looking
 * until we arrive at the common ancestor.  Don't check
 * the common ancestor because those will already have been declared
 * by the goto itself.
 */
//check_declared_variables_before_label(function_label->get_name());

bool
ScopeTracker::check_scope(const Scope *s) const
{
    // Iterate each operation.
    bool ok = true;
    for (const auto & op : s->operations) {
	if (op->get_type() == ScopeOperation::GOTO_DEFINITION) {
	    // Check that the label exists.
	    fprintf(stderr, "Evaluting goto label %s\n", op->get_goto_label().c_str());
	    const FunctionLabel *function_label = get_label(op->get_goto_label());
	    if (function_label == nullptr || function_label->get_scope() == nullptr) {
		    std::unique_ptr<Gyoji::context::Error> error = std::make_unique<Gyoji::context::Error>("Goto for an un-defined label.");
		    error->add_message(op->get_source_ref(),
				       std::string("Goto label ") + op->get_goto_label() + " had an undefined destination.");
		    compiler_context
			.get_errors()
			.add_error(std::move(error));
		    ok = false;
		ok = false;
		continue;
	    }
	    fprintf(stderr, "Checking for common ancestor of goto and label\n");
	    if (!root->is_ancestor(function_label->get_scope())) {
		fprintf(stderr, "This is nonsense, the root is an ancestor for everyone\n");
		exit(1);
	    }
	    // Find the common scope where the 'goto' and the 'label'
	    // both derive from ultimately.
	    const Scope *common_ancestor = find_common_ancestor(s, function_label->get_scope());
	    // Find any variables declared between the common ancestor and the label.
//	    check_declared_variables_before_label(function_label->get_name());
	    
	    if (!s->is_ancestor(function_label->get_scope())) {
		if (function_label->get_scope()->skips_initialization(op->get_goto_label())) {
		    std::unique_ptr<Gyoji::context::Error> error = std::make_unique<Gyoji::context::Error>("Goto would skip initialization.");
		    error->add_message(op->get_source_ref(),
				       std::string("Goto label ") + op->get_goto_label() + " would skip initialization of variables in destination scope.");
		    error->add_message(function_label->get_source_ref(),
				       "Label declared here.");
		    compiler_context
			.get_errors()
			.add_error(std::move(error));
		    ok = false;
		}
	    }
	}
	else if (op->get_type() == ScopeOperation::CHILD_SCOPE) {
	    if (!check_scope(op->get_child())) {
		ok = false;
	    }
	}
    }    
    return ok;
}

bool
ScopeTracker::check(
    ) const
{
    Scope *s = root.get();
    return check_scope(s);
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
    std::string _name,
    const Gyoji::mir::Type *_type,
    const Gyoji::context::SourceReference & _source_ref
    )
    : name(_name)
    , type(_type)
    , source_ref(_source_ref)
{}

LocalVariable::~LocalVariable()
{}

const std::string &
LocalVariable::get_name() const
{ return name; }

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
    std::string _name,
    size_t _block_id
    )
    : name(_name)
    , resolved(false)
    , block_id(_block_id)
    , src_ref(nullptr)
    , scope(nullptr)
{}
FunctionLabel::FunctionLabel(const FunctionLabel & _other)
    : name(_other.name)
    , resolved(_other.resolved)
    , block_id(_other.block_id)
    , src_ref(_other.src_ref)
    , scope(_other.scope)
{}
FunctionLabel::~FunctionLabel()
{}

const Gyoji::context::SourceReference &
FunctionLabel::get_source_ref() const
{ return *src_ref; }

size_t
FunctionLabel::get_block() const
{ return block_id; }

const std::string &
FunctionLabel::get_name() const
{ return name; }

bool
FunctionLabel::is_resolved() const
{ return resolved; }

const Scope *
FunctionLabel::get_scope() const
{ return scope; }

void
FunctionLabel::set_scope(const Scope *_scope, const Gyoji::context::SourceReference & _src_ref)
{
    scope = _scope;
    resolved = true;
    src_ref = &_src_ref;
}
