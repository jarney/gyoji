#pragma once

#include <jlang-mir.hpp>
#include <jlang-frontend.hpp>

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
	void resolve();
	size_t get_new_tmpvar();

    private:
	JLang::context::CompilerContext & compiler_context;
	const JLang::frontend::tree::FileStatementFunctionDefinition & function_definition;
	JLang::mir::MIR & mir;
	TypeResolver & type_resolver;
	size_t new_tmpvar_id;
	
	void extract_from_expression_primary_identifier(
	    JLang::mir::Function & function,
	    size_t & current_block,
	    size_t & returned_tmpvar,
	    const JLang::frontend::tree::ExpressionPrimaryIdentifier & expression);
	
	void extract_from_expression_primary_nested(
	    JLang::mir::Function & function,
	    size_t & current_block,
	    size_t & returned_tmpvar,
	    const JLang::frontend::tree::ExpressionPrimaryNested & expression);
	
	void extract_from_expression_primary_literal_char(
	    JLang::mir::Function & function,
	    size_t & current_block,
	    size_t & returned_tmpvar,
	    const JLang::frontend::tree::ExpressionPrimaryLiteralChar & expression);
	
	void extract_from_expression_primary_literal_string(
	    JLang::mir::Function & function,
	    size_t & current_block,
	    size_t & returned_tmpvar,
	    const JLang::frontend::tree::ExpressionPrimaryLiteralString & expression);
	
	void extract_from_expression_primary_literal_int(
	    JLang::mir::Function & function,
	    size_t & current_block,
	    size_t & returned_tmpvar,
	    const JLang::frontend::tree::ExpressionPrimaryLiteralInt & expression);
	
	void extract_from_expression_primary_literal_float(
	    JLang::mir::Function & function,
	    size_t & current_block,
	    size_t & returned_tmpvar,
	    const JLang::frontend::tree::ExpressionPrimaryLiteralFloat & expression);
	
	void extract_from_expression_postfix_array_index(
	    JLang::mir::Function & function,
	    size_t & current_block,
	    size_t & returned_tmpvar,
	    const JLang::frontend::tree::ExpressionPostfixArrayIndex & expression);
	
	void extract_from_expression_postfix_function_call(
	    JLang::mir::Function & function,
	    size_t & current_block,
	    size_t & returned_tmpvar,
	    const JLang::frontend::tree::ExpressionPostfixFunctionCall & expression);
	
	void extract_from_expression_postfix_dot(
	    JLang::mir::Function & function,
	    size_t & current_block,
	    size_t & returned_tmpvar,
	    const JLang::frontend::tree::ExpressionPostfixDot & expression);
	
	void extract_from_expression_postfix_arrow(
	    JLang::mir::Function & function,
	    size_t & current_block,
	    size_t & returned_tmpvar,
	    const JLang::frontend::tree::ExpressionPostfixArrow & expression);
	
	void extract_from_expression_postfix_incdec(
	    JLang::mir::Function & function,
	    size_t & current_block,
	    size_t & returned_tmpvar,
	    const JLang::frontend::tree::ExpressionPostfixIncDec & expression);
	
	void extract_from_expression_unary_prefix(
	    JLang::mir::Function & function,
	    size_t & current_block,
	    size_t & returned_tmpvar,
	    const JLang::frontend::tree::ExpressionUnaryPrefix & expression);
	
	void extract_from_expression_unary_sizeof_type(
	    JLang::mir::Function & function,
	    size_t & current_block,
	    size_t & returned_tmpvar,
	    const JLang::frontend::tree::ExpressionUnarySizeofType & expression);
	
	void extract_from_expression_binary(
	    JLang::mir::Function & function,
	    size_t & current_block,
	    size_t & returned_tmpvar,
	    const JLang::frontend::tree::ExpressionBinary & expression);
	
	void extract_from_expression_trinary(
	    JLang::mir::Function & function,
	    size_t & current_block,
	    size_t & returned_tmpvar,
	    const JLang::frontend::tree::ExpressionTrinary & expression);
	
	void extract_from_expression_cast(
	    JLang::mir::Function & function,
	    size_t & current_block,
	    size_t & returned_tmpvar,
	    const JLang::frontend::tree::ExpressionCast & expression);
	
	void extract_from_expression(
	    JLang::mir::Function & function,
	    size_t & current_block,
	    size_t & returned_tmpvar,
	    const JLang::frontend::tree::Expression & expression
	    );
	
	void extract_from_statement_return(
	    JLang::mir::Function & function,
	    size_t & current_block,
	    const JLang::frontend::tree::StatementReturn & statement
	    );
	
	void extract_from_statement_ifelse(
	    JLang::mir::Function & function,
	    size_t & current_block,
	    const JLang::frontend::tree::StatementIfElse & statement
	    );
	
	void extract_from_statement_list(
	    JLang::mir::Function & function,
	    size_t & current_block,
	    const JLang::frontend::tree::StatementList & statement_list
	    );
	
	void extract_from_function_definition(const JLang::frontend::tree::FileStatementFunctionDefinition & function_definition);
	
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
	void resolve();
    private:
	JLang::context::CompilerContext & compiler_context;
	const JLang::frontend::ParseResult & parse_result;
	JLang::mir::MIR & mir;
	TypeResolver & type_resolver;
	
	void extract_from_class_definition(const JLang::frontend::tree::ClassDefinition & definition);
	void extract_from_namespace(
	    const JLang::frontend::tree::FileStatementNamespace & namespace_declaration
	    );
	void extract_types(const std::vector<JLang::owned<JLang::frontend::tree::FileStatement>> & statements);
	
    };
    
};
