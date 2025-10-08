
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
ScopeOperation::createVariable(std::string _variable_name)
{
    auto op = JLang::owned<ScopeOperation>(new ScopeOperation());
    op->type = ScopeOperation::VAR_DECL;
    op->variable_name = _variable_name;
    return op;
}
JLang::owned<ScopeOperation>
ScopeOperation::createLabel(std::string _label_name)
{
    auto op = JLang::owned<ScopeOperation>(new ScopeOperation());
    op->type = ScopeOperation::LABEL_DEFINITION;
    op->label_name = _label_name;
    return op;
}

JLang::owned<ScopeOperation>
ScopeOperation::createGoto(std::string _goto_label)
{
    auto op = JLang::owned<ScopeOperation>(new ScopeOperation());
    op->type = ScopeOperation::GOTO_DEFINITION;
    op->goto_label = _goto_label;
    return op;
}

JLang::owned<ScopeOperation>
ScopeOperation::createChild(JLang::owned<Scope> _child)
{
    auto op = JLang::owned<ScopeOperation>(new ScopeOperation());
    op->type = ScopeOperation::CHILD_SCOPE;
    op->child = std::move(_child);
    return op;
}

Scope::Scope()
{}

Scope::~Scope()
{}

void
Scope::add_operation(JLang::owned<ScopeOperation> op)
{
    operations.push_back(std::move(op));
}

ScopeTracker::ScopeTracker()
    : root(std::make_unique<Scope>())
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
    auto child_op = ScopeOperation::createChild(std::move(child_scope));
    current->add_operation(std::move(child_op));
    current = new_current;
}


void
ScopeTracker::scope_pop()
{
    current = current->parent;
}


void
ScopeTracker::add_label(std::string label_name)
{
    add_operation(ScopeOperation::createLabel(label_name));
    labels.insert(std::pair(label_name, current));
}

void
ScopeTracker::add_goto(std::string goto_label)
{
    add_operation(ScopeOperation::createGoto(goto_label));
}

void
ScopeTracker::add_variable(std::string variable_name)
{
    add_operation(ScopeOperation::createVariable(variable_name));
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

Scope *
ScopeTracker::label_find(std::string label_name) const
{
    const auto & it = labels.find(label_name);
    if (it == labels.end()) {
	// Label does not exist
	return nullptr;
    }
    return it->second;
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
	    Scope *label_scope = label_find(op->get_goto_label());
	    if (label_scope == nullptr) {
		fprintf(stderr, "Label %s does not exist\n", op->get_goto_label().c_str());
		return false;
	    }
	    if (!s->is_ancestor(label_scope)) {
		if (label_scope->skips_initialization(op->get_goto_label())) {
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

