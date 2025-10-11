#pragma once

#include <gyoji-mir.hpp>
#include <gyoji-frontend.hpp>
#include <gyoji-frontend/function-scope.hpp>


/*!
 *  \addtogroup Frontend
 *  @{
 */

/**
 * @brief Converts the strongly-typed syntax tree into the MIR representation.
 * @details
 * This namespace covers the 'lowering' process for the compiler.  This is
 * the process of coverting the strongly-typed syntax tree into an intermediate
 * representation (MIR) for the language.  This stage applies most of the 'language'
 * rules and ultimately creates a machine in the MIR representation that will
 * safely carry out the intent as described in the syntax given for this
 * particular program.
 */
namespace Gyoji::frontend::lowering {
    class TypeResolver;

    /**
     * @brief Lowering for a function
     * @details
     * This class is responsible for converting a FileStatementFunctionDefinition
     * into an equivalent program in the MIR representation.  This iterates the
     * statements inside the function definition and emits MIR code that matches
     * the intend.  In addition, rules for the validity of the program are
     * evaluated so that grossly invalid programs never make it to the MIR level.
     * Semantic errors are reported through the CompilerContext in the form
     * of context-aware messages that highlight where the error took place.
     */
    class FunctionDefinitionResolver {
    public:
	FunctionDefinitionResolver(
	    Gyoji::context::CompilerContext & _compiler_context,
	    const Gyoji::frontend::tree::FileStatementFunctionDefinition & _function_definition,
	    Gyoji::mir::MIR & _mir,
	    TypeResolver & _type_resolver
	    );
	~FunctionDefinitionResolver();

	/**
	 * @brief Resolve a single function definition.
	 * @details
	 * Performs the bulk of the logic for reading a function definition and
	 * emitting equivalent MIR for it.  The MIR will be in a form that can
	 * be used in the code-generation phase.
	 *
	 * This method returns false if the MIR is unsuitable for code-generation
	 * due to semantic errors that were found.  Note that not all errors will
	 * result in an invalid MIR, however, some constructs will make the
	 * resulting MIR unsuitible for code generation.
	 */
	bool resolve();

    private:
	// Private members
	Gyoji::context::CompilerContext & compiler_context;
	const Gyoji::frontend::tree::FileStatementFunctionDefinition & function_definition;
	Gyoji::mir::MIR & mir;
	TypeResolver & type_resolver;
        ScopeTracker scope_tracker;
	
	Gyoji::owned<Gyoji::mir::Function> function;
	size_t current_block;

	// Private Methods

	// Returns the current point in the current block
	// where the 'unwindings' a 'goto' statement
	// will be placed.
	FunctionPoint get_current_point() const;
	
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

    /**
     * @brief Resolves all functions found in the MIR.
     * @details
     * This is the 'lowering' process for functions.
     * It reads the parse result syntax tree and constructs
     * functions and the "BasicBlock" control-flow graph
     * in order to produce the MIR which can later drive
     * semantic analysis like borrow checking and then
     * code-generation.
     *
     * This process is mainly performed by using one
     * FunctionDefinitionResolver for each function found in
     * the translation unit.
     */
    class FunctionResolver {
    public:
	/**
	 * @brief Construct a definition resolver.
	 * Constructs a function resolver using the
	 * compiler context, parse result, and an MIR
	 * to act as the destination of the parse.
	 * This makes use of the TypeResolver which will
	 * already have been used to extract type information
	 * from the translation unit.
	 */
	FunctionResolver(
	    Gyoji::context::CompilerContext & _compiler_context,
	    const Gyoji::frontend::ParseResult & _parse_result,
	    Gyoji::mir::MIR & _mir,
	    TypeResolver & _type_resolver
	    );
	
	/**
	 * @brief Move along, nothing to see here.
	 *
	 * @details
	 * Move along, nothing to see here.
	 */
	~FunctionResolver();

	/**
	 * Iterates all of the functions in the translation unit
	 * given by the parse result and lowers each one of them,
	 * inserting the results into the MIR.
	 */
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

/*! @} End of Doxygen Groups*/
