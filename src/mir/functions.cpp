#include <jlang-mir/functions.hpp>
#include <variant>
#include <stdio.h>

using namespace JLang::mir;

/////////////////////////////////////
// Functions
/////////////////////////////////////
Functions::Functions()
{
}

Functions::~Functions()
{}

void
Functions::add_function(JLang::owned<Function> _function)
{
    functions.push_back(std::move(_function));
}

const std::vector<JLang::owned<Function>> &
Functions::get_functions() const
{ return functions; }

void
Functions::dump(FILE *out) const
{
    for (const auto & function : functions) {
	function->dump(out);
    }
}

/////////////////////////////////////
// LocalVariable
/////////////////////////////////////
LocalVariable::LocalVariable(std::string _name, const Type *_type, const JLang::context::SourceReference & _src_ref)
    : name(_name)
    , type(_type)
    , src_ref(_src_ref)
     
{}
LocalVariable::LocalVariable(const LocalVariable & _other)
    : name(_other.name)
    , type(_other.type)
    , src_ref(_other.src_ref)
{}
LocalVariable::~LocalVariable()
{}

std::string
LocalVariable::get_name() const
{ return name; }

const Type*
LocalVariable::get_type() const
{ return type; }

const JLang::context::SourceReference &
LocalVariable::get_source_ref() const
{ return src_ref; }

/////////////////////////////////////
// Function
/////////////////////////////////////
Function::Function(
    std::string _name,
    const Type *_return_type,
    const std::vector<FunctionArgument> & _arguments,
    const JLang::context::SourceReference & _source_ref
    )
    : name(_name)
    , return_type(_return_type)
    , arguments(_arguments)
    , source_ref(_source_ref)
    , blockid(0)
{
    for (const auto & arg : arguments) {
	add_local(LocalVariable(arg.get_name(), arg.get_type(), _source_ref));
    }
}

Function::~Function()
{}

const std::string &
Function::get_name() const
{ return name; }

const Type *
Function::get_return_type() const
{ return return_type; }

const std::vector<FunctionArgument> &
Function::get_arguments() const
{ return arguments; }

const JLang::context::SourceReference &
Function::get_source_ref() const
{ return source_ref; }

const BasicBlock &
Function::get_basic_block(size_t blockid) const
{
    const auto & it = blocks.find(blockid);
    return *(it->second);
}

BasicBlock &
Function::get_basic_block(size_t blockid)
{
    const auto & it = blocks.find(blockid);
    return *(it->second);
}
size_t
Function::add_block()
{
    blocks[blockid] = std::make_unique<BasicBlock>();
    size_t blockid_created = blockid;
    blockid++;
    return blockid_created;
}

const std::map<size_t, JLang::owned<BasicBlock>> &
Function::get_blocks() const
{ return blocks; }

const LocalVariable *
Function::get_local(std::string local_name)
{
    const auto & it = in_scope_locals.find(local_name);
    if (it != in_scope_locals.end()) {
	return &it->second;
    }
    return nullptr;
}

bool
Function::add_local(const LocalVariable & local)
{
    if (get_local(local.get_name()) != nullptr) {
	return false;
    }
    in_scope_locals.insert(std::pair(local.get_name(), local));
    return true;
}

const Type *
Function::tmpvar_get(size_t tmpvar_id) const
{ return tmpvars.at(tmpvar_id); }

size_t
Function::tmpvar_define(const Type* type)
{
    tmpvars.push_back(type);
    return tmpvars.size()-1;
}
size_t
Function::tmpvar_duplicate(size_t tempvar_id)
{
    return tmpvar_define(tmpvar_get(tempvar_id));
}
void
Function::remove_local(std::string local_name)
{
    in_scope_locals.erase(local_name);
}

void
Function::dump(FILE *out) const
{
    fprintf(out, "    %s\n", get_name().c_str());
    fprintf(out, "        return-value : %s\n", return_type->get_name().c_str());
    for (const auto & arg : arguments) {
	fprintf(out, "        arg %s : %s\n", arg.get_name().c_str(), arg.get_type()->get_name().c_str());
    }
    fprintf(out, "    temporary variables\n");
    size_t varid = 0;
    for (const auto & value : tmpvars) {
	fprintf(out, "        _%ld : %s\n", varid, value->get_name().c_str());
	varid++;
    }

    fprintf(out, "    {\n");
    for (const auto & block_it : blocks) {
	fprintf(out, "        BB%ld:\n", block_it.first);
	const BasicBlock & block = *block_it.second;
	block.dump(out);
    }
    fprintf(out, "    }\n");
}
/////////////////////////////////////
// BasicBlock
/////////////////////////////////////

BasicBlock::BasicBlock()
{}

BasicBlock::~BasicBlock()
{}

void
BasicBlock::add_operation(JLang::owned<Operation> _operation)
{
    operations.push_back(std::move(_operation));
}

void
BasicBlock::dump(FILE *out) const
{
    for (const auto & operation : operations) {
	operation->dump(out);
    }
}
const std::vector<JLang::owned<Operation>> &
BasicBlock::get_operations() const
{ return operations; }

bool
BasicBlock::contains_terminator() const
{
    for (const auto & op : operations) {
	if (op->is_terminating()) return true;
    }
    return false;
}

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
{}
FunctionLabel::FunctionLabel(const FunctionLabel & _other)
    : name(_other.name)
    , resolved(_other.resolved)
    , block_id(_other.block_id)
    , src_ref(_other.src_ref)
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

/////////////////////////////////////
// FunctionArgument
/////////////////////////////////////
FunctionArgument::FunctionArgument(
    std::string & _name,
    const Type * _type
    )
    : name(_name)
    , type(_type)
{}

FunctionArgument::FunctionArgument(const FunctionArgument & _other)
    : name(_other.name)
    , type(_other.type)
{}

FunctionArgument::~FunctionArgument()
{}

const std::string &
FunctionArgument::get_name() const
{ return name; }

const Type*
FunctionArgument::get_type() const
{ return type; }
