#pragma once

#include <jlang-mir.hpp>
#include <jlang-frontend.hpp>
#include <jlang-frontend/function-scope.hpp>

namespace JLang::frontend {
    class TypeResolver;
    
    class FunctionDefinitionResolver {
    public:
	FunctionDefinitionResolver(
	    JLang::context::CompilerContext & _compiler_context,
	    const JLang::frontend::tree::FileStatementFunctionDefinition & _function_definition,
	    JLang::mir::MIR & _mir,
	    JLang::frontend::TypeResolver & _type_resolver
	    );
	~FunctionDefinitionResolver();
	bool resolve();

    private:
	JLang::context::CompilerContext & compiler_context;
	const JLang::frontend::tree::FileStatementFunctionDefinition & function_definition;
	JLang::mir::MIR & mir;
	TypeResolver & type_resolver;
        ScopeTracker scope_tracker;
	
	JLang::owned<JLang::mir::Function> function;
	size_t current_block;
	
	bool numeric_widen(
	    const JLang::context::SourceReference & _src_ref,
	    size_t & _widen_var,
	    const JLang::mir::Type *widen_to
	    );

	
	bool create_constant_integer(
	    const JLang::frontend::integers::ParseLiteralIntResult & parse_result,
	    size_t & returned_tmpvar,
	    const JLang::context::SourceReference & _src_ref
	    );
	
	bool create_constant_integer_one(
	    const JLang::mir::Type *type,
	    size_t & returned_tmpvar,
	    const JLang::context::SourceReference & _src_ref
	    );
	
	bool create_incdec_operation(
	    const JLang::context::SourceReference & src_ref,
	    size_t & returned_tmpvar,
	    const size_t & operand_tmpvar,
	    bool is_increment,
	    bool is_postfix
	    );
	
	bool local_declare_or_error(
	    const JLang::mir::Type *mir_type,
	    const std::string & name,
	    const JLang::context::SourceReference & source_ref
	    );

	// Widen the shorter
	// of the two operands to be the
	// larger one.
	bool numeric_widen_binary_operation(
	    const JLang::context::SourceReference & _src_ref,
	    size_t & a_tmpvar,
	    size_t & b_tmpvar,
	    const JLang::mir::Type *atype,
	    const JLang::mir::Type *btype,
	    const JLang::mir::Type **widened
	    );
	
	bool handle_binary_operation_arithmetic(
	    const JLang::context::SourceReference & _src_ref,
	    JLang::mir::Operation::OperationType type,
	    size_t & returned_tmpvar,
	    size_t a_tmpvar,
	    size_t b_tmpvar
	    );
	
	bool handle_binary_operation_logical(
	    const JLang::context::SourceReference & _src_ref,
	    JLang::mir::Operation::OperationType type,
	    size_t & returned_tmpvar,
	    size_t a_tmpvar,
	    size_t b_tmpvar
	    );
	
	bool handle_binary_operation_bitwise(
	    const JLang::context::SourceReference & _src_ref,
	    JLang::mir::Operation::OperationType type,
	    size_t & returned_tmpvar,
	    size_t a_tmpvar,
	    size_t b_tmpvar
	    );

	bool handle_binary_operation_shift(
	    const JLang::context::SourceReference & _src_ref,
	    JLang::mir::Operation::OperationType type,
	    size_t & returned_tmpvar,
	    size_t a_tmpvar,
	    size_t b_tmpvar
	    );

	bool handle_binary_operation_compare(
	    const JLang::context::SourceReference & _src_ref,
	    JLang::mir::Operation::OperationType type,
	    size_t & returned_tmpvar,
	    size_t a_tmpvar,
	    size_t b_tmpvar
	    );

	bool handle_binary_operation_assignment(
	    const JLang::context::SourceReference & _src_ref,
	    JLang::mir::Operation::OperationType type,
	    size_t & returned_tmpvar,
	    size_t a_tmpvar,
	    size_t b_tmpvar
	    );

	bool extract_from_expression_primary_identifier(
	    size_t & returned_tmpvar,
	    const JLang::frontend::tree::ExpressionPrimaryIdentifier & expression);
	
	bool extract_from_expression_primary_nested(
	    size_t & returned_tmpvar,
	    const JLang::frontend::tree::ExpressionPrimaryNested & expression);
	
	bool extract_from_expression_primary_literal_char(
	    size_t & returned_tmpvar,
	    const JLang::frontend::tree::ExpressionPrimaryLiteralChar & expression);
	
	bool extract_from_expression_primary_literal_string(
	    size_t & returned_tmpvar,
	    const JLang::frontend::tree::ExpressionPrimaryLiteralString & expression);
	
	bool extract_from_expression_primary_literal_int(
	    size_t & returned_tmpvar,
	    const JLang::frontend::tree::ExpressionPrimaryLiteralInt & expression);
	
	bool extract_from_expression_primary_literal_float(
	    size_t & returned_tmpvar,
	    const JLang::frontend::tree::ExpressionPrimaryLiteralFloat & expression);
	
	bool extract_from_expression_postfix_array_index(
	    size_t & returned_tmpvar,
	    const JLang::frontend::tree::ExpressionPostfixArrayIndex & expression);
	
	bool extract_from_expression_postfix_function_call(
	    size_t & returned_tmpvar,
	    const JLang::frontend::tree::ExpressionPostfixFunctionCall & expression);
	
	bool extract_from_expression_postfix_dot(
	    size_t & returned_tmpvar,
	    const JLang::frontend::tree::ExpressionPostfixDot & expression);
	
	bool extract_from_expression_postfix_arrow(
	    size_t & returned_tmpvar,
	    const JLang::frontend::tree::ExpressionPostfixArrow & expression);
	
	bool extract_from_expression_postfix_incdec(
	    size_t & returned_tmpvar,
	    const JLang::frontend::tree::ExpressionPostfixIncDec & expression);
	
	bool extract_from_expression_unary_prefix(
	    size_t & returned_tmpvar,
	    const JLang::frontend::tree::ExpressionUnaryPrefix & expression);
	
	bool extract_from_expression_unary_sizeof_type(
	    size_t & returned_tmpvar,
	    const JLang::frontend::tree::ExpressionUnarySizeofType & expression);
	
	bool extract_from_expression_binary(
	    size_t & returned_tmpvar,
	    const JLang::frontend::tree::ExpressionBinary & expression);
	
	bool extract_from_expression_trinary(
	    size_t & returned_tmpvar,
	    const JLang::frontend::tree::ExpressionTrinary & expression);
	
	bool extract_from_expression_cast(
	    size_t & returned_tmpvar,
	    const JLang::frontend::tree::ExpressionCast & expression);
	
	bool extract_from_expression(
	    size_t & returned_tmpvar,
	    const JLang::frontend::tree::Expression & expression
	    );
	
	bool extract_from_statement_variable_declaration(
	    const JLang::frontend::tree::StatementVariableDeclaration & statement
	    );
	
	bool extract_from_statement_ifelse(
	    std::map<std::string, JLang::mir::FunctionLabel> & labels,
	    const JLang::frontend::tree::StatementIfElse & statement
	    );
	
	bool extract_from_statement_while(
	    std::map<std::string, JLang::mir::FunctionLabel> & labels,
	    const JLang::frontend::tree::StatementWhile & statement
	    );
	
	bool extract_from_statement_for(
	    std::map<std::string, JLang::mir::FunctionLabel> & labels,
	    const JLang::frontend::tree::StatementFor & statement
	    );
	
	bool extract_from_statement_label(
	    std::map<std::string, JLang::mir::FunctionLabel> & labels,
	    const JLang::frontend::tree::StatementLabel & statement
	    );
	
	bool extract_from_statement_break(
	    std::map<std::string, JLang::mir::FunctionLabel> & labels,
	    const JLang::frontend::tree::StatementBreak & statement
	    );
	
	bool extract_from_statement_continue(
	    std::map<std::string, JLang::mir::FunctionLabel> & labels,
	    const JLang::frontend::tree::StatementContinue & statement
	    );
	
	bool extract_from_statement_goto(
	    std::map<std::string, JLang::mir::FunctionLabel> & labels,
	    const JLang::frontend::tree::StatementGoto & statement
	    );
	
	bool extract_from_statement_return(
	    const JLang::frontend::tree::StatementReturn & statement
	    );
	
	void
	leave_scope(
	    const JLang::context::SourceReference & src_ref,
	    std::vector<std::string> & unwind);
	
	bool extract_from_statement_list(
	    std::map<std::string, JLang::mir::FunctionLabel> & labels,
	    const JLang::frontend::tree::StatementList & statement_list
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
	    JLang::context::CompilerContext & _compiler_context,
	    const JLang::frontend::ParseResult & _parse_result,
	    JLang::mir::MIR & _mir,
	    JLang::frontend::TypeResolver & _type_resolver
	    );
	~FunctionResolver();
	bool resolve();
    private:
	JLang::context::CompilerContext & compiler_context;
	const JLang::frontend::ParseResult & parse_result;
	JLang::mir::MIR & mir;
	TypeResolver & type_resolver;
	
	bool extract_from_class_definition(const JLang::frontend::tree::ClassDefinition & definition);
	bool extract_from_namespace(
	    const JLang::frontend::tree::FileStatementNamespace & namespace_declaration
	    );
	bool extract_functions(const std::vector<JLang::owned<JLang::frontend::tree::FileStatement>> & statements);
	
    };
    
};
