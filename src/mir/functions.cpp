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
#include <gyoji-mir/functions.hpp>
#include <variant>
#include <stdio.h>

using namespace Gyoji::mir;

/////////////////////////////////////
// Functions
/////////////////////////////////////
Functions::Functions()
{
}

Functions::~Functions()
{}

void
Functions::add_function(Gyoji::owned<Function> _function)
{
    functions.push_back(std::move(_function));
}

const std::vector<Gyoji::owned<Function>> &
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
// Function
/////////////////////////////////////
Function::Function(
    std::string _name,
    const Type *_return_type,
    const std::vector<FunctionArgument> & _arguments,
    bool _is_unsafe,
    const Gyoji::context::SourceReference & _source_ref
    )
    : name(_name)
    , return_type(_return_type)
    , arguments(_arguments)
    , m_is_unsafe(_is_unsafe)
    , source_ref(_source_ref)
    , blockid(0)
{
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

const Gyoji::context::SourceReference &
Function::get_source_ref() const
{ return source_ref; }

bool
Function::is_unsafe() const
{ return m_is_unsafe; }

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

void
Function::add_operation(size_t blockid, Gyoji::owned<Operation> operation)
{
    tmpvar_operations[operation->get_result()] = operation.get();
    get_basic_block(blockid).add_operation(std::move(operation));
}


size_t
Function::add_block()
{
    blocks[blockid] = std::make_unique<BasicBlock>();
    size_t blockid_created = blockid;
    blockid++;
    return blockid_created;
}

const std::map<size_t, Gyoji::owned<BasicBlock>> &
Function::get_blocks() const
{ return blocks; }

const Type *
Function::tmpvar_get(size_t tmpvar_id) const
{ return tmpvars.at(tmpvar_id); }

Operation *
Function::tmpvar_get_operation(size_t tmpvar)
{
    return tmpvar_operations[tmpvar];
}

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
Function::calculate_block_reachability()
{
    // This map encodes the relationship between a block
    // and the blocks it is connected to.
    std::map<size_t, std::vector<size_t>> edges;

    // This is the working list of blocks we're
    // examining.
    std::map<size_t, size_t> open_list;
    std::map<size_t, size_t> closed_list;

    // This is a map of blockid:blockid
    // for the unreachable blocks.
    std::map<size_t, size_t> unreachable;

    // Build a graph with the 'edges'.
    for (const auto & block : blocks) {
	// Everything starts out as unreachable.
	unreachable.insert(std::pair(block.first, block.first));
	std::vector connections_to = block.second->get_connections();
	edges.insert(std::pair(block.first, connections_to));
    }
    // We start with only one element on the open list,
    // that is the 'start' or 'entry' block.
    
    open_list.insert(std::pair(0, 0));
    // With each iteration, we look for
    // blocks we connect to.  Once we find them,
    // we take them off the unreachable list.
    while (open_list.size() != 0) {
	size_t current = open_list.begin()->first;
	open_list.erase(current);
	unreachable.erase(current);
	const auto & connections_to  = edges[current];
	for (size_t to : connections_to) {
	    blocks[to]->add_reachable_from(current);
	    
	    // Only add it back to the open list
	    // if it wasn't found on the closed list
	    // becuase we've reached it already.
	    if (closed_list.find(to) == closed_list.end()) {
		open_list.insert(std::pair(to, to));
	    }
	}
	closed_list.insert(std::pair(current, current));
    }
    for (const auto & unreach : unreachable) {
	// This is unreachable in the sense that the
	// basic blocks are not a connected graph.
	// If this happens, then there is a bug in the 'lowering'
	// code which should prevent this from being constructed
	// in this way.
	const BasicBlock & block = get_basic_block(unreach.first);

	// If this block is empty, we will already have
	// reported that as an error, so we don't need to
	// do that again.
	const auto & operations = block.get_operations();
	if (operations.size() == 0) {
	    // Cull it from the block list
	    // becuase it's empty and unreachable.
	    blocks.erase(unreach.first);
	}
    }

}

void
Function::iterate_operations(OperationVisitor & visitor) const
{
    const auto & blocks = get_blocks();
    for (const auto & block_it : blocks) {
	const BasicBlock & block = *block_it.second;
	size_t operation_index = 0;
	for (const auto & op_it : block.get_operations()) {
	    const Operation & operation = *op_it;
	    visitor.visit(block_it.first, block, operation_index, operation);
	    operation_index++;
	}
    }
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
BasicBlock::add_operation(Gyoji::owned<Operation> _operation)
{
    operations.push_back(std::move(_operation));
}

void
BasicBlock::dump(FILE *out) const
{
    size_t operation_index = 0;
    for (const auto & operation : operations) {
	operation->dump(out, operation_index);
	operation_index++;
    }
}
const std::vector<Gyoji::owned<Operation>> &
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

std::vector<size_t>
BasicBlock::get_connections() const
{
    for (const auto & op : operations) {
	if (op->is_terminating()) {
	    return op->get_connections();
	}
    }    

    // In theory, this should not happen
    // because we will already have checked
    // that all blocks have terminators.
    std::vector<size_t> empty_edges;
    return empty_edges;
}
const std::vector<size_t> &
BasicBlock::get_reachable_from() const
{ return reachable_from; }

void
BasicBlock::add_reachable_from(size_t other_block)
{ reachable_from.push_back(other_block); }

size_t
BasicBlock::size() const
{ return operations.size(); }

void
BasicBlock::insert_operation(size_t position, Gyoji::owned<Operation> operation)
{
    operations.insert(operations.begin() + position, std::move(operation));
}


/////////////////////////////////////
// FunctionArgument
/////////////////////////////////////
FunctionArgument::FunctionArgument(
    std::string & _name,
    const Type * _type,
    const Gyoji::context::SourceReference & _name_source_ref,
    const Gyoji::context::SourceReference & _type_source_ref
    )
    : name(_name)
    , type(_type)
    , name_source_ref(_name_source_ref)
    , type_source_ref(_type_source_ref)
{}

FunctionArgument::FunctionArgument(const FunctionArgument & _other)
    : name(_other.name)
    , type(_other.type)
    , name_source_ref(_other.name_source_ref)
    , type_source_ref(_other.type_source_ref)
{}

FunctionArgument::~FunctionArgument()
{}

const std::string &
FunctionArgument::get_name() const
{ return name; }

const Type*
FunctionArgument::get_type() const
{ return type; }

const Gyoji::context::SourceReference &
FunctionArgument::get_name_source_ref() const
{ return name_source_ref; }

const Gyoji::context::SourceReference &
FunctionArgument::get_type_source_ref() const
{ return type_source_ref; }
/////////////////////////////////////
// OperationVisitor
/////////////////////////////////////

OperationVisitor::OperationVisitor()
{}

OperationVisitor::~OperationVisitor()
{}

