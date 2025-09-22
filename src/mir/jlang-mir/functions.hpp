#pragma once

#include <jlang-misc/pointers.hpp>
#include <jlang-mir/types.hpp>

#include <string>
#include <map>
#include <vector>

namespace JLang::mir {
  class Function;
  class SimpleStatement;
  class BasicBlock;
  
  class Functions {
  public:
    Functions();
    ~Functions();

    /**
     * Adds a function to the MIR representation.
     */
    void add_function(JLang::owned<Function> _function);
    
    /**
     * Read-only representation of functions.
     */
    const std::vector<JLang::owned<Function>> & get_functions() const;
    
  private:
    std::vector<JLang::owned<Function>> functions;
  };

  // A simple statement is a
  // single un-wound expression to be
  // evaluated with one or two inputs
  // and one output.  This is effectively
  // a "three argument form" of an expression.
  // For example, a single binary operation
  // possibly using temporary variables as input
  // and temporary variables as output.
  // It might also be a branch with a compare
  // and a conditional jump.  Nesting is not allowed.
  // Instead, dependent statements appear first
  // and store their results into temporary
  // variables for the next statements later.
  class SimpleStatement {
  public:
    SimpleStatement();
    ~SimpleStatement();
  private:
  };
  
  class BasicBlock {
  public:
    BasicBlock();
    ~BasicBlock();
  private:
    std::vector<JLang::owned<SimpleStatement>> statements;
  };

  class FunctionArgument {
  public:
    FunctionArgument(
                     std::string & _name,
                     std::string & _type
                     );
    ~FunctionArgument();
    const std::string & get_name() const;
    const std::string & get_type() const;
  private:
    std::string name;
    std::string type;
  };

  class Function {
  public:
    Function(
             std::string _name,
             std::string _type,
             std::vector<FunctionArgument> _arguments
             );
    ~Function();

    const std::string & get_return_type() const;
    const std::string & get_name() const;
    const std::vector<FunctionArgument> & get_arguments() const;
  private:
    std::string name;
    std::string return_type;
    // Arguments are variables that are in scope
    // for the entire function and represent values
    // provided by the caller.
    std::vector<FunctionArgument> arguments;
    
    std::map<size_t, JLang::owned<BasicBlock>> blocks;
    
    // outgoing edges of basic blocks. size_t -> size_t.
    // Each basic block (key of map)
    // has an edge that it connects to, forming a directed
    // graph.
    std::map<size_t, std::vector<size_t>> edges;  
  };
};
