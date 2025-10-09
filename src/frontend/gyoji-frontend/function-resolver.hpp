#pragma once

#include <gyoji-mir.hpp>
#include <gyoji-frontend.hpp>
#include <gyoji-frontend/function-scope.hpp>

namespace Gyoji::frontend {
    class TypeResolver;
    
    class FunctionDefinitionResolver {
    public:
	FunctionDefinitionResolver(
	    Gyoji::context::CompilerContext & _compiler_context,
	    const Gyoji::frontend::tree::FileStatementFunctionDefinition & _function_definition,
	    Gyoji::mir::MIR & _mir,
	    Gyoji::frontend::TypeResolver & _type_resolver
	    );
	~FunctionDefinitionResolver();
	bool resolve();

    private:
	Gyoji::context::CompilerContext & compiler_context;
	const Gyoji::frontend::tree::FileStatementFunctionDefinition & function_definition;
	Gyoji::mir::MIR & mir;
	TypeResolver & type_resolver;
        ScopeTracker scope_tracker;
	
	Gyoji::owned<Gyoji::mir::Function> function;
	size_t current_block;
	
	bool numeric_widen(
	    const Gyoji::context::SourceReference & _src_ref,
	    size_t & _widen_var,
	    const Gyoji::mir::Type *widen_to
	    );

	
	bool create_constant_integer(
	    const Gyoji::frontend::integers::ParseLiteralIntResult & parse_result,
	    size_t & returned_tmpvar,
	    const Gyoji::context::SourceReference & _src_ref
	    );
	
	bool create_constant_integer_one(
	    const Gyoji::mir::Type *type,
	    size_t & returned_tmpvar,
	    const Gyoji::context::SourceReference & _src_ref
	    );
	
	bool create_incdec_operation(
	    const Gyoji::context::SourceReference & src_ref,
	    size_t & returned_tmpvar,
	    const size_t & operand_tmpvar,
	    bool is_increment,
	    bool is_postfix
	    );
	
	bool local_declare_or_error(
	    const Gyoji::mir::Type *mir_type,
	    const std::string & name,
	    const Gyoji::context::SourceReference & source_ref
	    );

	// Widen the shorter
	// of the two operands to be the
	// larger one.
	bool numeric_widen_binary_operation(
	    const Gyoji::context::SourceReference & _src_ref,
	    size_t & a_tmpvar,
	    size_t & b_tmpvar,
	    const Gyoji::mir::Type *atype,
	    const Gyoji::mir::Type *btype,
	    const Gyoji::mir::Type **widened
	    );
	
	bool handle_binary_operation_arithmetic(
	    const Gyoji::context::SourceReference & _src_ref,
	    Gyoji::mir::Operation::OperationType type,
	    size_t & returned_tmpvar,
	    size_t a_tmpvar,
	    size_t b_tmpvar
	    );
	
	bool handle_binary_operation_logical(
	    const Gyoji::context::SourceReference & _src_ref,
	    Gyoji::mir::Operation::OperationType type,
	    size_t & returned_tmpvar,
	    size_t a_tmpvar,
	    size_t b_tmpvar
	    );
	
	bool handle_binary_operation_bitwise(
	    const Gyoji::context::SourceReference & _src_ref,
	    Gyoji::mir::Operation::OperationType type,
	    size_t & returned_tmpvar,
	    size_t a_tmpvar,
	    size_t b_tmpvar
	    );

	bool handle_binary_operation_shift(
	    const Gyoji::context::SourceReference & _src_ref,
	    Gyoji::mir::Operation::OperationType type,
	    size_t & returned_tmpvar,
	    size_t a_tmpvar,
	    size_t b_tmpvar
	    );

	bool handle_binary_operation_compare(
	    const Gyoji::context::SourceReference & _src_ref,
	    Gyoji::mir::Operation::OperationType type,
	    size_t & returned_tmpvar,
	    size_t a_tmpvar,
	    size_t b_tmpvar
	    );

	bool handle_binary_operation_assignment(
	    const Gyoji::context::SourceReference & _src_ref,
	    Gyoji::mir::Operation::OperationType type,
	    size_t & returned_tmpvar,
	    size_t a_tmpvar,
	    size_t b_tmpvar
	    );

	bool extract_from_expression_primary_identifier(
	    size_t & returned_tmpvar,
	    const Gyoji::frontend::tree::ExpressionPrimaryIdentifier & expression);
	
	bool extract_from_expression_primary_nested(
	    size_t & returned_tmpvar,
	    const Gyoji::frontend::tree::ExpressionPrimaryNested & expression);
	
	bool extract_from_expression_primary_literal_char(
	    size_t & returned_tmpvar,
	    const Gyoji::frontend::tree::ExpressionPrimaryLiteralChar & expression);
	
	bool extract_from_expression_primary_literal_string(
	    size_t & returned_tmpvar,
	    const Gyoji::frontend::tree::ExpressionPrimaryLiteralString & expression);
	
	bool extract_from_expression_primary_literal_int(
	    size_t & returned_tmpvar,
	    const Gyoji::frontend::tree::ExpressionPrimaryLiteralInt & expression);
	
	bool extract_from_expression_primary_literal_float(
	    size_t & returned_tmpvar,
	    const Gyoji::frontend::tree::ExpressionPrimaryLiteralFloat & expression);
	
	bool extract_from_expression_postfix_array_index(
	    size_t & returned_tmpvar,
	    const Gyoji::frontend::tree::ExpressionPostfixArrayIndex & expression);
	
	bool extract_from_expression_postfix_function_call(
	    size_t & returned_tmpvar,
	    const Gyoji::frontend::tree::ExpressionPostfixFunctionCall & expression);
	
	bool extract_from_expression_postfix_dot(
	    size_t & returned_tmpvar,
	    const Gyoji::frontend::tree::ExpressionPostfixDot & expression);
	
	bool extract_from_expression_postfix_arrow(
	    size_t & returned_tmpvar,
	    const Gyoji::frontend::tree::ExpressionPostfixArrow & expression);
	
	bool extract_from_expression_postfix_incdec(
	    size_t & returned_tmpvar,
	    const Gyoji::frontend::tree::ExpressionPostfixIncDec & expression);
	
	bool extract_from_expression_unary_prefix(
	    size_t & returned_tmpvar,
	    const Gyoji::frontend::tree::ExpressionUnaryPrefix & expression);
	
	bool extract_from_expression_unary_sizeof_type(
	    size_t & returned_tmpvar,
	    const Gyoji::frontend::tree::ExpressionUnarySizeofType & expression);
	
	bool extract_from_expression_binary(
	    size_t & returned_tmpvar,
	    const Gyoji::frontend::tree::ExpressionBinary & expression);
	
	bool extract_from_expression_trinary(
	    size_t & returned_tmpvar,
	    const Gyoji::frontend::tree::ExpressionTrinary & expression);
	
	bool extract_from_expression_cast(
	    size_t & returned_tmpvar,
	    const Gyoji::frontend::tree::ExpressionCast & expression);
	
	bool extract_from_expression(
	    size_t & returned_tmpvar,
	    const Gyoji::frontend::tree::Expression & expression
	    );
	
	bool extract_from_statement_variable_declaration(
	    const Gyoji::frontend::tree::StatementVariableDeclaration & statement
	    );
	
	bool extract_from_statement_ifelse(
	    const Gyoji::frontend::tree::StatementIfElse & statement
	    );
	
	bool extract_from_statement_while(
	    const Gyoji::frontend::tree::StatementWhile & statement
	    );
	
	bool extract_from_statement_for(
	    const Gyoji::frontend::tree::StatementFor & statement
	    );
	
	bool extract_from_statement_label(
	    const Gyoji::frontend::tree::StatementLabel & statement
	    );
	
	bool extract_from_statement_break(
	    const Gyoji::frontend::tree::StatementBreak & statement
	    );
	
	bool extract_from_statement_continue(
	    const Gyoji::frontend::tree::StatementContinue & statement
	    );
	
	bool extract_from_statement_goto(
	    const Gyoji::frontend::tree::StatementGoto & statement
	    );
	
	bool extract_from_statement_return(
	    const Gyoji::frontend::tree::StatementReturn & statement
	    );
	
	void
	leave_scope(
	    const Gyoji::context::SourceReference & src_ref,
	    std::vector<std::string> & unwind);
	
	bool extract_from_statement_list(
	    const Gyoji::frontend::tree::StatementList & statement_list
	    );
	
    };
    
    //! Function Resolver.
    /**
     * This is the 'lowering' process for functions.
     * It reads the parse result syntax tree and constructs
     * functions and the "BasicBlock" control-flow graph
     * in order to produce the MIR which can later drive
     * semantic analysis like borrow checking and then
     * code-generation.
     */
    class FunctionResolver {
    public:
	FunctionResolver(
	    Gyoji::context::CompilerContext & _compiler_context,
	    const Gyoji::frontend::ParseResult & _parse_result,
	    Gyoji::mir::MIR & _mir,
	    Gyoji::frontend::TypeResolver & _type_resolver
	    );
	~FunctionResolver();
	bool resolve();
    private:
	Gyoji::context::CompilerContext & compiler_context;
	const Gyoji::frontend::ParseResult & parse_result;
	Gyoji::mir::MIR & mir;
	TypeResolver & type_resolver;
	
	bool extract_from_class_definition(const Gyoji::frontend::tree::ClassDefinition & definition);
	bool extract_from_namespace(
	    const Gyoji::frontend::tree::FileStatementNamespace & namespace_declaration
	    );
	bool extract_functions(const std::vector<Gyoji::owned<Gyoji::frontend::tree::FileStatement>> & statements);
	
    };
    
};
