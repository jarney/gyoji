
#include <jlang-frontend.hpp>
#include <jlang-frontend/function-scope.hpp>

using namespace JLang::frontend;

ScopeOperation::ScopeOperation()
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

JLang::owned<ScopeOperation>
ScopeOperation::create_variable(std::string _variable_name, const JLang::mir::Type *_variable_type)
{
    auto op = JLang::owned<ScopeOperation>(new ScopeOperation());
    op->type = ScopeOperation::VAR_DECL;
    op->variable_name = _variable_name;
    op->variable_type = _variable_type;
    return op;
}
JLang::owned<ScopeOperation>
ScopeOperation::create_label(std::string _label_name)
{
    auto op = JLang::owned<ScopeOperation>(new ScopeOperation());
    op->type = ScopeOperation::LABEL_DEFINITION;
    op->label_name = _label_name;
    return op;
}

JLang::owned<ScopeOperation>
ScopeOperation::create_goto(std::string _goto_label)
{
    auto op = JLang::owned<ScopeOperation>(new ScopeOperation());
    op->type = ScopeOperation::GOTO_DEFINITION;
    op->goto_label = _goto_label;
    return op;
}

JLang::owned<ScopeOperation>
ScopeOperation::create_child(JLang::owned<Scope> _child)
{
    auto op = JLang::owned<ScopeOperation>(new ScopeOperation());
    op->type = ScopeOperation::CHILD_SCOPE;
    op->child = std::move(_child);
    return op;
}

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
Scope::add_operation(JLang::owned<ScopeOperation> op)
{
    operations.push_back(std::move(op));
}

void
Scope::add_variable(std::string name, const JLang::mir::Type *type, const JLang::context::SourceReference & source_ref)
{
    JLang::owned<LocalVariable> local_variable = std::make_unique<LocalVariable>(name, type, source_ref);
    variables.insert(std::pair(name, std::move(local_variable)));
    auto local_var_op = ScopeOperation::create_variable(name, type);
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

const std::map<std::string, JLang::owned<LocalVariable>> &
Scope::get_variables() const
{ return variables; }

ScopeTracker::ScopeTracker(const JLang::context::CompilerContext & _compiler_context)
    : root(std::make_unique<Scope>())
    , compiler_context(_compiler_context)
{
    current = root.get();
}

ScopeTracker::~ScopeTracker()
{
}


void
ScopeTracker::scope_push()
{
    auto child_scope = std::make_unique<Scope>();
    Scope *new_current = child_scope.get();
    child_scope->parent = current;
    auto child_op = ScopeOperation::create_child(std::move(child_scope));
    current->add_operation(std::move(child_op));
    current = new_current;
}
void
ScopeTracker::scope_push_loop(size_t _loop_break_blockid, size_t _loop_continue_blockid)
{
    auto child_scope = std::make_unique<Scope>(true, _loop_break_blockid, _loop_continue_blockid);
    Scope *new_current = child_scope.get();
    child_scope->parent = current;
    auto child_op = ScopeOperation::create_child(std::move(child_scope));
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
ScopeTracker::label_define(std::string label_name)
{
    const auto & it_notfound = notfound_labels.find(label_name);
    if (it_notfound == notfound_labels.end()) {
	fprintf(stderr, "This should not happen\n");
	exit(1);
	return;
    }
    it_notfound->second->set_scope(current);
    labels.insert(std::pair(label_name, std::move(it_notfound->second)));
    notfound_labels.erase(it_notfound);
}

void
ScopeTracker::label_define(std::string label_name, size_t label_blockid)
{
    JLang::owned<FunctionLabel> new_label = std::make_unique<FunctionLabel>(label_name, label_blockid);
    new_label->set_scope(current);
    labels.insert(std::pair(label_name, std::move(new_label)));
    add_operation(ScopeOperation::create_label(label_name));
}
    
// Use this for 'goto' to say we want a label, but we
// don't yet know where it will live, so put it on the
// notfound labels list.
void
ScopeTracker::label_declare(std::string label_name, size_t label_blockid)
{
    JLang::owned<FunctionLabel> new_label = std::make_unique<FunctionLabel>(label_name, label_blockid);
    notfound_labels.insert(std::pair(label_name, std::move(new_label)));
    add_operation(ScopeOperation::create_label(label_name));
}


void
ScopeTracker::add_goto(std::string goto_label)
{
    // TODO:
    // Check if this label exists and jump to that
    // label if it does.  If it doesn't, forward declare it.
    // and put it in the 'forward declared' labels list.
    add_operation(ScopeOperation::create_goto(goto_label));
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
ScopeTracker::add_variable(std::string variable_name, const JLang::mir::Type *mir_type, const JLang::context::SourceReference & source_ref)
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
ScopeTracker::add_operation(JLang::owned<ScopeOperation> op)
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
ScopeTracker::check_scope(const Scope *s) const
{
    // Iterate each operation.
    for (const auto & op : s->operations) {
	if (op->get_type() == ScopeOperation::GOTO_DEFINITION) {
	    // Check that the label exists.
	    const FunctionLabel *function_label = get_label(op->get_goto_label());
	    if (function_label == nullptr || function_label->get_scope() == nullptr) {
		fprintf(stderr, "Label %s does not exist\n", op->get_goto_label().c_str());
		return false;
	    }
	    if (!s->is_ancestor(function_label->get_scope())) {
		if (function_label->get_scope()->skips_initialization(op->get_goto_label())) {
		    fprintf(stderr, "Label %s would skip initialization\n", op->get_goto_label().c_str());
		    return false;
		}
	    }
	    else {
		fprintf(stderr, "It's an ancestor, so it's ok\n");
	    }
	    
	}
	else if (op->get_type() == ScopeOperation::CHILD_SCOPE) {
	    return check_scope(op->get_child());
	}
    }    
    return true;
}

bool
ScopeTracker::check() const
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
    const JLang::mir::Type *_type,
    const JLang::context::SourceReference & _source_ref
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

const JLang::mir::Type *
LocalVariable::get_type() const
{ return type; }

const JLang::context::SourceReference &
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

void
FunctionLabel::set_label(const JLang::context::SourceReference & _src_ref)
{
    resolved = true;
    src_ref = &_src_ref;
}

const JLang::context::SourceReference &
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
FunctionLabel::set_scope(const Scope *_scope)
{ scope = _scope; }
