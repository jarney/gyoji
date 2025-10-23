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
#include <gyoji-analysis.hpp>
#include <stdio.h>

using namespace Gyoji::mir;
using namespace Gyoji::context;
using namespace Gyoji::analysis;

AnalysisPassUseBeforeAssignment::AnalysisPassUseBeforeAssignment(CompilerContext & _compiler_context)
    : AnalysisPass(_compiler_context, "use-before-initialization checks")
{}
AnalysisPassUseBeforeAssignment::~AnalysisPassUseBeforeAssignment()
{}

void
AnalysisPassUseBeforeAssignment::check(const Gyoji::mir::MIR & mir) const
{
    for (const auto & function : mir.get_functions().get_functions()) {
	check(*function);
    }
}

namespace Gyoji::analysis {

    // This is an access of a local variable
    // inside a function.  Here, we track the
    // name of the variable, the tmpvar it's
    // loaded into, and the 'ProgramPoint' it was loaded/declared
    // from.
    class VariableLoadPoint {
    public:
	VariableLoadPoint(
	    std::string _variable_name,
	    ProgramPoint _program_point
	    );
	VariableLoadPoint(const VariableLoadPoint & other);
	~VariableLoadPoint();
	std::string variable_name;
	size_t tmpvar;
	ProgramPoint program_point;
    };

    class TmpvarAssignedAt {
    public:
	TmpvarAssignedAt(const TmpvarAssignedAt & other);
	TmpvarAssignedAt(
	    size_t _tmpvar,
	    ProgramPoint _program_point
	    );
	~TmpvarAssignedAt();
			 
	size_t tmpvar;
	ProgramPoint program_point;
    };

    class VariableTmpvarVisitor : public OperationVisitor {
    public:
	VariableTmpvarVisitor(const std::map<std::string, std::string> & _ignore_arguments);
	~VariableTmpvarVisitor();
	void visit(
	    size_t block_id,
	    const BasicBlock & block,
	    size_t operation_index,
	    const Operation & operation
	    );
	const std::map<size_t, std::string> & get_tmpvars() const;
    private:
	std::map<size_t, std::string> tmpvars;
	const std::map<std::string, std::string> & ignore_arguments;
    };
        
    // First, we need to just build a map
    // of all the tmpvar to the variables they
    // represent (only for tmpvars associated with variables)
    class VariableUseVisitor : public OperationVisitor {
    public:
	VariableUseVisitor(
	    const std::map<size_t, std::string> & _tmpvars
	    );
	~VariableUseVisitor();
	void visit(
	    size_t block_id,
	    const BasicBlock & block,
	    size_t operation_index,
	    const Operation & operation
	    );
	const std::vector<VariableLoadPoint> & get_loads();
	const std::vector<VariableLoadPoint> & get_stores();
    private:
	const std::map<size_t, std::string> & tmpvars;
	std::vector<VariableLoadPoint> variable_loads;
	std::vector<VariableLoadPoint> variable_stores;
    };

    // Next, we need to find all of the places those tmpvars (variables)
    // are used and all of the places they are assigned.
    
};
//////////////////////////////////////////////
// ProgramPoint
//////////////////////////////////////////////

ProgramPoint::ProgramPoint(size_t _block_id, size_t _operation_index)
    : block_id(_block_id)
    , operation_index(_operation_index)
{}
ProgramPoint::ProgramPoint(const ProgramPoint & other)
    : block_id(other.block_id)
    , operation_index(other.operation_index)
{}
ProgramPoint::~ProgramPoint()
{}
//////////////////////////////////////////////
// VariableLoadPoint
//////////////////////////////////////////////
VariableLoadPoint::VariableLoadPoint(
    std::string _variable_name,
    ProgramPoint _program_point)
    : variable_name(_variable_name)
    , program_point(_program_point)
{}
VariableLoadPoint::VariableLoadPoint(const VariableLoadPoint & other)
    : variable_name(other.variable_name)
    , program_point(other.program_point)
{}

VariableLoadPoint::~VariableLoadPoint()
{}
			 
//////////////////////////////////////////////
// VariableUseVisitor
//////////////////////////////////////////////

VariableUseVisitor::VariableUseVisitor(
    const std::map<size_t, std::string> & _tmpvars
    )
    : OperationVisitor()
    , tmpvars(_tmpvars)
{
}

VariableUseVisitor::~VariableUseVisitor()
{}

const std::vector<VariableLoadPoint> &
VariableUseVisitor::get_loads()
{
    return variable_loads;
}

const std::vector<VariableLoadPoint> &
VariableUseVisitor::get_stores()
{ return variable_stores; }


void VariableUseVisitor::visit(
    size_t block_id,
    const BasicBlock & block,
    size_t operation_index,
    const Operation & operation
    )    
{
    const auto & operands = operation.get_operands();
    size_t noperands = operands.size();
    ProgramPoint program_point(block_id, operation_index);
    
    if (operation.get_type() == Operation::OP_ASSIGN) {
	const auto & it = tmpvars.find(operands.at(0));
	if (it != tmpvars.end()) {
	    VariableLoadPoint assigned_at(
		it->second,
		program_point
		);
	    variable_stores.push_back(assigned_at);
	}
	// All other operands are accesses.
	for (size_t operand_id = 1; operand_id < noperands; operand_id++) {
	    const auto & it = tmpvars.find(operands.at(operand_id));
	    if (it != tmpvars.end()) {
		VariableLoadPoint assigned_at(
		    it->second,
		    program_point
		    );
		variable_loads.push_back(assigned_at);
	    }
	}
    }
    else {
	// For other operations, all operands are 'use' of the variable.
	for (size_t operand_id = 0; operand_id < noperands; operand_id++) {
	    const auto & it = tmpvars.find(operands.at(operand_id));
	    if (it != tmpvars.end()) {
		VariableLoadPoint assigned_at(
		    it->second,
		    program_point
		    );
		variable_loads.push_back(assigned_at);
	    }
	}
    }
}
VariableTmpvarVisitor::VariableTmpvarVisitor(
    const std::map<std::string, std::string> & _ignore_arguments
    )
    : tmpvars()
    , ignore_arguments(_ignore_arguments)
{}

VariableTmpvarVisitor::~VariableTmpvarVisitor()
{}

void
VariableTmpvarVisitor::visit(
    size_t block_id,
    const BasicBlock & block,
    size_t operation_index,
    const Operation & operation
    )
{
    if (operation.get_type() != Operation::OP_LOCAL_VARIABLE) {
	return;
    }
    const OperationLocalVariable &operation_local = (const OperationLocalVariable &)operation;
    const std::string & varname = operation_local.get_symbol_name();

    // Check to see if it's an argument and we
    // can ignore it since arguments get initialized
    // when the function begins.
    const auto & it = ignore_arguments.find(varname);
    if (it != ignore_arguments.end()) {
	return;
    }

    // Just keep track of the tmpvars that map to local variables.
    tmpvars[operation_local.get_result()] = varname;
}

const std::map<size_t, std::string> &
VariableTmpvarVisitor::get_tmpvars() const
{ return tmpvars; }

bool AnalysisPassUseBeforeAssignment::true_at(
    const Function & function,
    std::map<size_t, bool> & already_checked,
    const std::vector<ProgramPoint> & true_points,
    const ProgramPoint & check_at
    ) const
{
    bool found_in_same_block = false;
    
    for (const auto & true_point : true_points) {
	if (check_at.block_id == true_point.block_id &&
	    check_at.operation_index > true_point.operation_index) {
	    found_in_same_block = true;
	}
    }
    // It was true in the current block, so it's true.
    if (found_in_same_block) {
	return true;
    }
    const std::vector<size_t> & predecessors = function.get_basic_block(check_at.block_id).get_reachable_from();
    if (predecessors.size() == 0) {
	return false;
    }
    
    // If it wasn't true in the current block, try all predecessors.
    for (const auto & predecessor : predecessors) {
	ProgramPoint pred_point(predecessor, function.get_basic_block(predecessor).get_operations().size());

	bool is_true;
	const auto & already_checked_it = already_checked.find(pred_point.block_id);
	if (already_checked_it != already_checked.end()) {
	    is_true = already_checked_it->second;
	}
	else {
	    is_true = true_at(function, already_checked, true_points, pred_point);
	    already_checked.insert(std::pair(pred_point.block_id, is_true));
	}
	if (!is_true) {
	    return false;
	}
    }
    return true;

}

void AnalysisPassUseBeforeAssignment::check(const Function & function) const
{
    std::map<std::string, std::string> ignore_arguments;
    for (const auto & arg : function.get_arguments()) {
	ignore_arguments[arg.get_name()] = arg.get_name();
    }
    
    VariableTmpvarVisitor id_visitor(ignore_arguments);
    function.iterate_operations(id_visitor);
    
    VariableUseVisitor visitor(id_visitor.get_tmpvars());
    function.iterate_operations(visitor);
    
    std::map<std::string, std::vector<ProgramPoint>> store_points_by_variable;
    for (const auto & store : visitor.get_stores()) {
	store_points_by_variable[store.variable_name].push_back(store.program_point);
    }
    
    for (const auto & load : visitor.get_loads()) {
	std::map<size_t, bool> already_checked;
	bool is_true = true_at(function, already_checked, store_points_by_variable[load.variable_name], load.program_point);
	if (!is_true) {
	    std::unique_ptr<Gyoji::context::Error> error = std::make_unique<Gyoji::context::Error>("Variable use before initialization.");
	    const SourceReference & source_ref =
		function.get_basic_block(load.program_point.block_id).get_operations().at(load.program_point.operation_index)->get_source_ref();

	    error->add_message(
		source_ref,
		std::string("Variable ")
		+ load.variable_name
		+ std::string(" is uninitialized.  This would result in undefined behavior.  Note that if this appears on a 'return' line, then it is most likely a destructor call.")
		);
	    get_compiler_context()
		.get_errors()
		.add_error(std::move(error));
	}
	already_checked.insert(std::pair(load.program_point.block_id, is_true));
    }
    

    // We now have all of the places
    // the variable was stored
    // and all of the places it was initialized.
    // Next for each load, we need to find out if
    
    // a) There exists a store for that variable
    // b) For all stores that exist, they all happen "before" the
    //    use.

    // If they're in the same BB, it's easy.
    // If they're not, we need to find all BB that are "before"
    // and ask the question on each.
    
}
//////////////////////////
//
/*
  We start with the CFG graph of basic blocks.
            0
	   / \
	  /   \
	 1     2
	/ \     \
       /   \     3
      4     5    /
      \     /   /
       \   /   /
        \ /   /
	 6----

  Then we have some property like "used-at" which is
  associated with a program-point such as (6,3) (basic block 6, 3rd instruction)

  We also have several 'initialized-at' facts like
    (4,7 ) (2,3)

  What we would like to know if whether 'initialized-before(6,3)' is true.

  The inference rule is P(b,i) = P(b,i+1) for all i.  This says if it was true at point i,
  it is also true at point i+1 in the same block.

  We also have the rule that says P(b,i) = | (for all c in Prececessors of b : P(c,end))

  So we would like to know if initialized-before(6,3) is true,
     We first check if there is a (6,k) with k<3.
     If not, we check if ALL of (4,end) and (5,end) are true.
        In this case, 4,end is true because (4,7) is true.
	But (5,end) is false, so we check (1,end).  Again no.
	But (3,end) is true because (2,end) is true.
  
 */
