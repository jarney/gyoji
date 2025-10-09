
#include <gyoji-frontend.hpp>
#include <gyoji-frontend/function-scope.hpp>

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
    const Scope *s = this;
    while (s) {
	if (other == s) {
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

bool
ScopeTracker::check_scope(
    const Scope *s,
    const Gyoji::context::CompilerContext & compiler_context
    ) const
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
	    if (!check_scope(op->get_child(), compiler_context)) {
		ok = false;
	    }
	}
    }    
    return ok;
}

bool
ScopeTracker::check(
    const Gyoji::context::CompilerContext & compiler_context
    ) const
{
    Scope *s = root.get();
    return check_scope(s, compiler_context);
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
