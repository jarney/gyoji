#pragma once

#include "jbackend.hpp"

/*!
 *  \addtogroup Backend
 *  @{
 */
//! Backend formatting the output as C syntax.
/*!
 * The purpose of this code is to provide a way
 * to compile a Gyoji source-file into a
 * C source-file, providing a quick way to
 * cross-compile to C or to debug the syntax
 * and ensure that it correctly represents the
 * intent of the source in Gyoji.
 */
namespace Gyoji::backend::toc {
    
    class Statement {
    };
    
    enum TypeType {
	TYPE_PRIMITIVE,
	TYPE_STRUCT,
	TYPE_POINTER
    };
    
    class Type;
    
    class LValue {
    public:
	int id;
	std::shared_ptr<Type> type;
    };
    
    class RValue {
	int id;
	std::shared_ptr<Type> type;
    };
    
    class Member {
    public:
	std::string name;
	std::shared_ptr<Type> type;
    };
    
    class Type {
    public:
	TypeType type;
	std::string primitive;        // PRIMITIVE TYPES
	std::list<std::shared_ptr<Member>> members;    // Member variables.
	std::shared_ptr<Type> target; // Target type of the pointer.
    };
    
// Expressions propagate types
// binary operations must all be same type.
// & operator adds an indirection to a type.
// * removes an indirection from a type.
// Functions have return types.
// Arithmetic is banned except for primitive types.
    class Expression {
    public:
	std::shared_ptr<Type> type;
    };
    
    class StatementDeclaration : public Statement {
    public:
	std::string variable_name;
	std::shared_ptr<Type> type; // Type of variable being declared.
    };
    
    class StatementAssignment : public Statement {
    public:
	std::shared_ptr<LValue> lvalue; // Target of declaration
	std::shared_ptr<Expression> rvalue; // Expression to assign value to.
    };
    
    class FunctionBody {
    public:
	std::list<Statement> statements;
    };
    
    class FunctionArgument {
    public:
	std::shared_ptr<Type> type;
	std::string name;
    };
    
// This is the definition
// of a function.
    class Function {
    public:
	std::shared_ptr<Type> return_type;
	std::string name;
	std::list<std::shared_ptr<FunctionArgument>> arguments;
	std::shared_ptr<FunctionBody> body;
    };
    
    class FunctionTable {
    public:
	std::list<std::shared_ptr<Function>> functions;
    };
    
    class TypeTable {
    public:
	std::map<std::string, std::shared_ptr<Type>> typedefs;
    };
    
/**
 * This is the identity transformation that should always
 * format the code exactly as it appeared in the input,
 * assuming that the input is a valid syntax tree.
 */
    class JBackendToC : public JBackend {
    public:
	JBackendToC();
	~JBackendToC();
	virtual int process(ASTNode::ptr file);
	
	std::string collect_comments(ASTNode::ptr node);
	std::string break_multiline_comment(std::string str);
	
	//  void print_whitespace(ASTDataNonSyntax::ptr node);
	//  void print_comments(std::vector<ASTDataNonSyntax::ptr> &non_syntax_list);
	//  void print_comment_single_line(ASTDataNonSyntax::ptr node);
	//  void print_comment_multi_line(ASTDataNonSyntax::ptr node);
	//  void print_file_metadata(ASTDataNonSyntax::ptr node);
	//  void print_non_syntax(ASTDataNonSyntax::ptr node);
	
	//  void print_node_generic(ASTNode::ptr node);
	//  void print_node_plain(ASTNode::ptr node);
	void print_scope_body(ASTNode::ptr node);
	void newline();
	void print_typedef(ASTNode::ptr node);
	void print_import(ASTNode::ptr node);
	void print_node_function_definition(ASTNode::ptr node);
	
	void process_definition(ASTNode::ptr node);
	
	void dump_types();
	void dump_functions();
	
	int indent_level;
	int depth;
	
	FunctionTable function_table;
	TypeTable type_table;
    };
    
};

/*! @} End of Doxygen Groups*/
