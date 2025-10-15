#include <gyoji-analysis.hpp>
#include <stdio.h>

using namespace Gyoji::mir;
using namespace Gyoji::context;
using namespace Gyoji::analysis;

/**
 * This class indicates an operation
 * inside a basic block.  The block
 * is the ID of the basic block
 * and operation is the index into
 * that basic block indicating
 * a specific operation.
 */
class BasicBlockOperation {
public:
    BasicBlockOperation(size_t _blockid, size_t _operationid);
    BasicBlockOperation(const BasicBlockOperation & _other);
    ~BasicBlockOperation();
    size_t blockid;
    size_t operationid;
};

/**
 * This class represents an edge in the control-flow
 * graph with edges between operations and the 'next'
 * operation in the graph.
 */
class CFGEdge {
public:
    CFGEdge(const BasicBlockOperation & start, const BasicBlockOperation & end);
    CFGEdge(const CFGEdge & _other);
    ~CFGEdge();
    BasicBlockOperation start;
    BasicBlockOperation end;
};
///////////////////////////////
// BasicBlockOperation
///////////////////////////////
BasicBlockOperation::BasicBlockOperation(size_t _blockid, size_t _operationid)
    : blockid(_blockid)
    , operationid(_operationid)
{}
BasicBlockOperation::BasicBlockOperation(const BasicBlockOperation & _other)
    : blockid(_other.blockid)
    , operationid(_other.operationid)
{}
BasicBlockOperation::~BasicBlockOperation()
{}
///////////////////////////////
// CFGEdge
///////////////////////////////
CFGEdge::CFGEdge(const BasicBlockOperation & _start, const BasicBlockOperation & _end)
    : start(_start)
    , end(_end)
{}
CFGEdge::CFGEdge(const CFGEdge & _other)
    : start(_other.start)
    , end(_other.end)
{}
CFGEdge::~CFGEdge()
{}

///////////////////////////////
// AnalysisPassBorrowChecker
///////////////////////////////
AnalysisPassBorrowChecker::AnalysisPassBorrowChecker(CompilerContext & _compiler_context)
    : AnalysisPass(_compiler_context, "borrow checker")
{}
AnalysisPassBorrowChecker::~AnalysisPassBorrowChecker()
{}

void
AnalysisPassBorrowChecker::check(const MIR & mir) const
{
    // Here, we follow the basic plan of the 'Polonius' algorithm
    // for borrow checking.

    for (const auto & function : mir.get_functions().get_functions()) {
	check(*function);
    }
    
}

void AnalysisPassBorrowChecker::check(const Function & function) const
{
    // We start with mapping the "edges" of the graph
    // from the basic-blocks of the function.
    const auto & blocks = function.get_blocks();

    std::vector<CFGEdge> cfg_edges;
    
    for (const auto & block_it : blocks) {
	const BasicBlock & block = *block_it.second;
	const std::vector<Gyoji::owned<Operation>> & operations = block.get_operations();

	// Each basic block must have at least one operation.
	if (operations.size() == 0) {
	    get_compiler_context()
		.get_errors()
		.add_simple_error(function.get_source_ref(),
				  "Compiler bug!  Please report this message(borrow)",
				  std::string("Function ") + function.get_name() + std::string(" found with empty Basic Block")
		    );
	    break;
	}

	// TODO: Finish the logic of
	// tying blocks together in a graph
	// by traversing it.
	BasicBlockOperation startblock(block_it.first, 0);
	BasicBlockOperation endblock(block_it.first, 1);
	cfg_edges.push_back(CFGEdge(startblock, endblock));
	for (const auto & operation_ptr : block.get_operations()) {
	    const Operation & operation = *operation_ptr;
	}
    }

    // TODO:
    // We need to finish the CFG above
    // and also extract the 'loans', the 'regions',
    // and the 'live/killed' status of each variable from the MIR.

    // Finally, we perform the graph-traversal
    // to solve this.  It should mainly take the form of
    // a Hornsat solver, I think.
}
