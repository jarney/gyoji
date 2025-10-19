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
#include <gyoji-frontend/function-resolver.hpp>
#include <gyoji-misc/jstring.hpp>
#include <variant>
#include <stdio.h>

using namespace Gyoji::mir;
using namespace Gyoji::context;
using namespace Gyoji::frontend::tree;
using namespace Gyoji::frontend::lowering;
using namespace Gyoji::frontend::namespaces;

FunctionResolver::FunctionResolver(
    Gyoji::context::CompilerContext & _compiler_context,
    const Gyoji::frontend::ParseResult & _parse_result,
    Gyoji::mir::MIR & _mir,
    Gyoji::frontend::lowering::TypeResolver & _type_resolver
    )
    : compiler_context(_compiler_context)
    , parse_result(_parse_result)
    , mir(_mir)
    , type_resolver(_type_resolver)
{}

FunctionResolver::~FunctionResolver()
{}

bool FunctionResolver::resolve()
{
    return extract_functions(parse_result.get_translation_unit().get_statements());
}
bool
FunctionResolver::extract_from_namespace(
    const FileStatementNamespace & namespace_declaration)
{
    const auto & statements = namespace_declaration.get_statement_list().get_statements();
    return extract_functions(statements);
}

bool
FunctionResolver::extract_from_class_definition(const ClassDefinition & definition)
{
    return true;
}

bool
FunctionResolver::extract_functions(const std::vector<Gyoji::owned<FileStatement>> & statements)
{
    for (const auto & statement : statements) {
	const auto & file_statement = statement->get_statement();
	if (std::holds_alternative<Gyoji::owned<FileStatementFunctionDeclaration>>(file_statement)) {
	    // Nothing, no functions can exist here.
	}
	else if (std::holds_alternative<Gyoji::owned<FileStatementFunctionDefinition>>(file_statement)) {
	    // This is the only place that functions can be extracted from.
	    // We make this a separate object because we want convenient
	    // access to certain pieces of context used in resolution.
	    FunctionDefinitionResolver function_def_resolver(
		compiler_context,
		*std::get<Gyoji::owned<FileStatementFunctionDefinition>>(file_statement),
		mir,
		type_resolver
		);
	    if (!function_def_resolver.resolve()) {
		return false;
	    }
	}
	else if (std::holds_alternative<Gyoji::owned<FileStatementGlobalDefinition>>(file_statement)) {
	    // Nothing, no globals can exist here.
	    // Global declarations should already be resolved by the type_resolver earlier.
	}
	else if (std::holds_alternative<Gyoji::owned<ClassDeclaration>>(file_statement)) {
	    // Nothing, no functions can exist here.
	    // Class declarations should already be resolved by the type_resolver earlier.
	}
	else if (std::holds_alternative<Gyoji::owned<ClassDefinition>>(file_statement)) {
	    // Constructors, Destructors, and methods are special cases.
	    if (!extract_from_class_definition(*std::get<Gyoji::owned<ClassDefinition>>(file_statement))) {
		return false;
	    }
	}
	else if (std::holds_alternative<Gyoji::owned<EnumDefinition>>(file_statement)) {
	    // Nothing, no functions can exist here.
	    // Enums should already be resolved by the type_resolver earlier.
	}
	else if (std::holds_alternative<Gyoji::owned<TypeDefinition>>(file_statement)) {
	    // Nothing, no functions can exist here.
	    // Typedefs should already be resolved by the type_resolver earlier.
	}
	else if (std::holds_alternative<Gyoji::owned<FileStatementNamespace>>(file_statement)) {
	    if (!extract_from_namespace(*std::get<Gyoji::owned<FileStatementNamespace>>(file_statement))) {
		return false;
	    }
	}
	else if (std::holds_alternative<Gyoji::owned<FileStatementUsing>>(file_statement)) {
	    // Namespace using is largely handled by the parse stage, so we don't
	    // need to do any function resolution here.
	}
	else {
	    compiler_context
		.get_errors()
		.add_simple_error(statement->get_source_ref(),
				  "Compiler bug!  Please report this message(5)",
				  "Unknown statement type in variant, extracting statements from file (compiler bug)"
		    );
	    return false;
	}
    }
    return true;
}

////////////////////////////////////////////////
// FunctionDefinitionResolver
////////////////////////////////////////////////

FunctionDefinitionResolver::FunctionDefinitionResolver(
    Gyoji::context::CompilerContext & _compiler_context,
    const Gyoji::frontend::tree::FileStatementFunctionDefinition & _function_definition,
    Gyoji::mir::MIR & _mir,
    Gyoji::frontend::lowering::TypeResolver & _type_resolver
    )
    : compiler_context(_compiler_context)
    , function_definition(_function_definition)
    , mir(_mir)
    , type_resolver(_type_resolver)
    , scope_tracker(_function_definition.get_unsafe_modifier().is_unsafe(), _compiler_context)
    , class_type(nullptr)
    , method(nullptr)
{}
FunctionDefinitionResolver::~FunctionDefinitionResolver()
{}

bool
FunctionDefinitionResolver::is_method() const
{
    return class_type != nullptr;
}

bool
FunctionDefinitionResolver::resolve()
{
    std::string fully_qualified_function_name = 
	function_definition.get_name().get_fully_qualified_name();

    bool is_unsafe = function_definition.get_unsafe_modifier().is_unsafe();
    
    NS2Entity *entity = function_definition.get_name().get_ns2_entity();

    // This section tries to figure out if this is a plain function
    // or a method call on a class.
    Type *maybe_class_type = mir.get_types().get_type(entity->get_parent()->get_fully_qualified_name());
    if (maybe_class_type != nullptr) {
	const auto & method_it = maybe_class_type->get_methods().find(entity->get_name());
	if (method_it == maybe_class_type->get_methods().end()) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    function_definition.get_source_ref(),
		    "Member function not declared.",
		    std::string("Member method ")
		    + entity->get_name()
		    + std::string(" was not declared in class ")
		    + entity->get_parent()->get_fully_qualified_name()
		    );
	    return false;
	}
	else {
	    // This is a specific member function of a class.
	    // Mark the class and method here.
	    class_type = maybe_class_type;
	    method = &method_it->second;
	}
    }
    
    const TypeSpecifier & return_type_specifier = function_definition.get_return_type();
    const Type *return_type = type_resolver.extract_from_type_specifier(return_type_specifier);
    
    if (return_type == nullptr) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		function_definition.get_source_ref(),
		"Return-value type not defined",
		std::string("Return type was not declared")
		);
	return false;
	
    }
    std::vector<FunctionArgument> arguments;

    // If this is a method instead of a plain function,
    // we add the implicit '<this>' argument as the first argument
    // so that we can use it to get access to the class content.
    // Note that we do not expose 'this' or 'super' as a keyword here
    // in order to limit the damage the programmer can potentially do in
    // leaking the 'this' pointer elsewhere, particularly in a constructor.
    if (is_method()) {
	class_pointer_type = mir.get_types().get_pointer_to(class_type, function_definition.get_source_ref());
	std::string this_arg_name("<this>");
	FunctionArgument arg(this_arg_name, class_pointer_type,
			     function_definition.get_source_ref(),
			     function_definition.get_source_ref());
	arguments.push_back(arg);
    }
    
    const auto & function_argument_list = function_definition.get_arguments();
    const auto & function_definition_args = function_argument_list.get_arguments();

    bool member_conflict_errors = false;
    for (const auto & function_definition_arg : function_definition_args) {
	std::string name = function_definition_arg->get_identifier().get_name();

	// If this is a method, we want to check that the arguments
	// to the function don't conflict with member variable names.
	if (is_method()) {
	    const TypeMember *member = class_type->member_get(name);
	    if (member != nullptr) {
		std::unique_ptr<Gyoji::context::Error> error = std::make_unique<Gyoji::context::Error>("Variable Name Conflict");
		error->add_message(
		    function_definition_arg->get_identifier().get_source_ref(),
		    std::string("Method defined argument ") + name + std::string(" which would conflict with class member name.")
		    );
		error->add_message(
		    member->get_source_ref(),
		    std::string("Member variable declared here.")
		    );
		compiler_context
		    .get_errors()
		    .add_error(std::move(error));
		member_conflict_errors = true;
	    }
	}
	    
	const Gyoji::mir::Type * mir_type = type_resolver.extract_from_type_specifier(function_definition_arg->get_type_specifier());
	
	FunctionArgument arg(name, mir_type,
			     function_definition_arg->get_identifier().get_source_ref(),
			     function_definition_arg->get_type_specifier().get_source_ref());
	arguments.push_back(arg);
	if (!scope_tracker.add_variable(name, mir_type, function_definition_arg->get_identifier().get_source_ref())) {
	    return false;
	}
    }
    
    if (member_conflict_errors) {
	return false;
    }

    // Check that the arguments declared here
    // match the function (or method) declaration,
    // otherwise we'll end up with a badly defined
    // function.
    if (is_method()) {
	// Argument mismatch from method.
	if (arguments.size() != method->get_arguments().size()) {
		std::unique_ptr<Gyoji::context::Error> error = std::make_unique<Gyoji::context::Error>("Method argument mismatch");
		error->add_message(
		    function_definition.get_source_ref(),
		    std::string("Method has ") + std::to_string(arguments.size()-1) + std::string(" arguments defined")
		    );
		error->add_message(
		    method->get_source_ref(),
		    std::string("First declared here with ") + std::to_string(method->get_arguments().size()-1)
		    );
		compiler_context
		    .get_errors()
		    .add_error(std::move(error));

	    return false;
	}
	bool arg_error = false;
	if (method->get_return_type()->get_name() != return_type->get_name()) {
	    std::unique_ptr<Gyoji::context::Error> error = std::make_unique<Gyoji::context::Error>("Return-value does not match declaration");
	    error->add_message(
		function_definition.get_return_type().get_source_ref(),
		std::string("Return-value defined as ") + return_type->get_name() + std::string(".")
		);
	    error->add_message(
		method->get_source_ref(),
		std::string("Does not match declaration ") + method->get_return_type()->get_name()
		);
	    compiler_context
		.get_errors()
		.add_error(std::move(error));
	    arg_error = true;
	}
	
	for (size_t i = 0; i < arguments.size(); i++) {
	    const FunctionArgument & fa = arguments.at(i);
	    const Argument & ma = method->get_arguments().at(i);
	    if (fa.get_type()->get_name() != ma.get_type()->get_name()) {
		std::unique_ptr<Gyoji::context::Error> error = std::make_unique<Gyoji::context::Error>("Method argument mismatch");
		error->add_message(
		    fa.get_type_source_ref(),
		    std::string("Argument defined as ") + fa.get_type()->get_name() + std::string(" does not match declaration.")
		    );
		error->add_message(
		    ma.get_source_ref(),
		    std::string("First declared here as ") + ma.get_type()->get_name()
		    );
		compiler_context
		    .get_errors()
		    .add_error(std::move(error));
		arg_error = true;
	    }
	}
	if (arg_error) {
	    return false;
	}
    }
    else {
	// We should to the same check
	// against the function declaration for a 'plain' function.
	const Gyoji::mir::Symbol *symbol = mir.get_symbols().get_symbol(fully_qualified_function_name);
	if (symbol == nullptr) {
	    // This is perfectly fine.  It just
	    // means that there was no forward declaration
	    // for this function.
	}
	else {
	    // If there was a forward declaration, we need to make sure
	    // it is the correct type and matches the function signature.
	    const Type *symbol_type = symbol->get_type();
	    if (symbol_type->get_type() != Type::TYPE_FUNCTION_POINTER) {
		std::unique_ptr<Gyoji::context::Error> error = std::make_unique<Gyoji::context::Error>("Symbol is not a function");
		error->add_message(
		    function_definition.get_source_ref(),
		    std::string("Symbol ") + fully_qualified_function_name + std::string(" is not declared as a function.")
		    );
		compiler_context
		    .get_errors()
		    .add_error(std::move(error));
		return false;
	    }
	    const std::vector<Argument> & function_arguments = symbol_type->get_argument_types();

	    if (arguments.size() != function_arguments.size()) {
		std::unique_ptr<Gyoji::context::Error> error = std::make_unique<Gyoji::context::Error>("Function argument mismatch");
		error->add_message(
		    function_definition.get_source_ref(),
		    std::string("Function has ") + std::to_string(arguments.size()) + std::string(" arguments defined")
		    );
		error->add_message(
		    symbol_type->get_defined_source_ref(),
		    std::string("First declared here with ") + std::to_string(function_arguments.size())
		    );
		compiler_context
		    .get_errors()
		    .add_error(std::move(error));
		
		return false;
	    }
	    bool arg_error = false;

	    if (symbol_type->is_unsafe() != is_unsafe) {
		std::unique_ptr<Gyoji::context::Error> error = std::make_unique<Gyoji::context::Error>("Function safety modifier does not match declaration.");
		error->add_message(
		    function_definition.get_return_type().get_source_ref(),
		    std::string("Function defined as ") + (is_unsafe ? std::string("unsafe") : std::string("not unsafe")) + std::string(".")
		    );
		error->add_message(
		    symbol_type->get_defined_source_ref(),
		    std::string("Does not match previous declaration as ") + (symbol_type->is_unsafe() ? std::string("unsafe") : std::string("not unsafe"))
		    );
		compiler_context
		    .get_errors()
		    .add_error(std::move(error));
		arg_error = true;
	    }
	    
	    if (symbol_type->get_return_type()->get_name() != return_type->get_name()) {
		std::unique_ptr<Gyoji::context::Error> error = std::make_unique<Gyoji::context::Error>("Return-value does not match declaration");
		error->add_message(
		    function_definition.get_return_type().get_source_ref(),
		    std::string("Return-value defined as ") + return_type->get_name() + std::string(".")
		    );
		error->add_message(
		    symbol_type->get_defined_source_ref(),
		    std::string("Does not match declaration ") + symbol_type->get_return_type()->get_name()
		    );
		compiler_context
		    .get_errors()
		    .add_error(std::move(error));
		arg_error = true;
	    }

	    for (size_t i = 0; i < arguments.size(); i++) {
		const FunctionArgument & fa = arguments.at(i);
		const Argument & ma = function_arguments.at(i);
		if (fa.get_type()->get_name() != ma.get_type()->get_name()) {
		    std::unique_ptr<Gyoji::context::Error> error = std::make_unique<Gyoji::context::Error>("Method argument mismatch");
		    error->add_message(
			fa.get_type_source_ref(),
			std::string("Argument defined as ") + fa.get_type()->get_name() + std::string(" does not match declaration.")
			);
		    error->add_message(
			ma.get_source_ref(),
			std::string("First declared here as ") + ma.get_type()->get_name()
			);
		    compiler_context
			.get_errors()
			.add_error(std::move(error));
		    arg_error = true;
		}
	    }
	    if (arg_error) {
		return false;
	    }
	}
    }
    
    function = std::make_unique<Function>(
	fully_qualified_function_name,
	return_type,
	arguments,
	is_unsafe,
	function_definition.get_source_ref());

    
    // Create a new basic block for the start
    
    current_block = function->add_block();
    
    if (!extract_from_statement_list(
	    false, // do-unwind automatically
	    function_definition.get_scope_body().get_statements())) {
	return false;
    }
    
    //scope_tracker.dump();

    // This is a list of goto operations (ScopeOperation*)
    // each with a vector of variable declarations (std::vector<ScopeOperation*>)
    // that represent variables that should be 'undeclared' just prior to
    // the Jump instruction of each basic block.  The first element
    // of the pair will always be a Goto and the second will always be
    // variable declarations.  The point of this is to collect all of them
    // here because they aren't known earlier while the funciton is still under
    // construction so that we can insert the required instructions
    // now that we know what should be going out of scope
    // for the goto statements.
    std::vector<std::pair<const ScopeOperation*, std::vector<const ScopeOperation*>>> goto_fixups;
    if (!scope_tracker.check(goto_fixups)) {
	return false;
    }

    for (const auto & fixup : goto_fixups) {
	const ScopeOperation *goto_operation = fixup.first;
	size_t basic_block_id = goto_operation->get_goto_point().get_basic_block_id();
	size_t location = goto_operation->get_goto_point().get_location();
	for (const auto & unwind : fixup.second) {
	    fprintf(stderr, "Unwinding variable %s\n", unwind->get_variable_name().c_str());

	    // TODO:
	    // when we implement destructors, they will be called here
	    // just before we undeclare the variables.
	    
	    // Insert the 'undeclare' operation to mark that
	    // this variable is no longer in scope.
	    auto unwind_operation = std::make_unique<OperationLocalUndeclare>(
		goto_operation->get_source_ref(),
		unwind->get_variable_name());
	    function->get_basic_block(basic_block_id)
		.insert_operation(location, std::move(unwind_operation));
	    
	    // This increment is what makes sure that
	    // the unwind operations are emitted in the
	    // correct order so that variable destructors
	    // are called in the reverse order as the
	    // declarations.
	    location++;
	}
    }

    // We can now calculate the reachability graph
    // of the basic blocks and cull any empty blocks that
    // are unreachable.  This doesn't seem very
    // elegant, but we need to know the reachability
    // of non-terminating blocks so we know if we need
    // to add return statements to them
    // or if they can just be culled.
    function->calculate_block_reachability();

    // Case: Reachable block with no terminator.
    //       This will be a missing return, so we need to
    //       add it or raise an error.
    // Case  Reachable block with terminator.
    //       This is the normal case.
    // Case  Unreachable block with no terminator.
    //       It may still contain unreachable statements
    //       that we need to raise an error for.
    // Case  Unreachable block with terminator.
    //       It contains unreachable code, so this
    //       is an error.
    
    // Check for missing return statements
    // and insert them if the function is 'void'.
    // If the function is not 'void', then
    // we need to raise an error for it.
    bool is_ok = true;
    const auto & blocks = function->get_blocks();
    for (const auto & block_it : blocks) {
	const BasicBlock & block = *block_it.second;
	if (!block.contains_terminator()
	    // A block is reachable if it is reachable from another block
	    // OR it is the 'entry' block with ID 0.
	    && (block.get_reachable_from().size() != 0 || block_it.first == 0)
	    ) {
	    if (return_type->is_void()) {
		std::vector<std::string> unwind_scope = scope_tracker.get_variables_to_unwind_for_scope();
		leave_scope(function_definition.get_scope_body().get_source_ref(), unwind_scope);
	
		auto operation = std::make_unique<OperationReturnVoid>(
		    return_type_specifier.get_source_ref()
		    );
		function->get_basic_block(block_it.first).add_operation(std::move(operation));
	    }
	    else {
		fprintf(stderr, "Block %ld\n", block_it.first);
		std::unique_ptr<Gyoji::context::Error> error = std::make_unique<Gyoji::context::Error>("Control reaches end of non-void function");
		error->add_message(
		    function_definition.get_scope_body().get_end_source_ref(),
		    std::string("Function ")
		    + fully_qualified_function_name
		    + std::string(" returns ")
		    + return_type->get_name()
		    + std::string(" but is missing a return statement at the end of the function.")
		    );
		error->add_message(
		    return_type_specifier.get_source_ref(),
		    std::string("Return type defined here")
		    );
		compiler_context
		    .get_errors()
		    .add_error(std::move(error));
		is_ok = false;
	    }
	}
    }
    // If we had an error, that's ok because
    // we're still safe to process our next function.
//    if (!is_ok) {
//	return true;
//    }
    
    mir.get_functions().add_function(std::move(function));

    return true;
}

bool
FunctionDefinitionResolver::extract_from_expression_primary_identifier(
    size_t & returned_tmpvar,
    const Gyoji::frontend::tree::ExpressionPrimaryIdentifier & expression
    )
{
    // At this point, we should try to identify what this
    // identifier actually refers to.
    // * First look inside the function's declared variables in scope.
    // * Next, look in the global namespace for functions.
    // * Finally, look in the global namespace for external
    //   variables like 'global' or 'static' variables.
    // If the identifier is a function.
    // There are a few cases:
    // * The it is a function and is used in an assignment
    //   to a function pointer.  In this case, we should
    //   return the type as a function pointer type
    // * If it is used in a function call directly, we should not
    //   emit the code, but should just use an 'immediate'
    //   execution of whatever it is.
    // * Otherwise, emit it as a 'load' and just follow our nose
    //   at runtime.
    // * Maybe we really should 'flatten' our access here.
    
    if (expression.get_identifier().get_identifier_type() == Terminal::IDENTIFIER_LOCAL_SCOPE) {
	// This block is obsolete, it should
	// no longer be possible to get here, so we should
	// confirm that fact and remove this block altogether.
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_source_ref(),
		"Local variable could not be resolved: should not be reachable.",
		std::string("Local variable ") + expression.get_identifier().get_value() + std::string(" was not found in this scope.")
		);
	return false;
    }
    else if (expression.get_identifier().get_identifier_type() == Terminal::IDENTIFIER_GLOBAL_SCOPE) {
	// First, check to see if there is a variable of that name
	// declared in our current scope.
	std::string local_variable_name = expression.get_identifier().get_name();
	const LocalVariable *localvar = scope_tracker.get_variable(local_variable_name);
	if (localvar != nullptr) {
	    returned_tmpvar = function->tmpvar_define(localvar->get_type());
	    auto operation = std::make_unique<OperationLocalVariable>(
		expression.get_identifier().get_source_ref(),
		returned_tmpvar,
		local_variable_name,
		localvar->get_type()
		);
	    function->get_basic_block(current_block).add_operation(std::move(operation));

	    return true;
	}

	if (is_method()) {
	    const TypeMember *member = class_type->member_get(local_variable_name);
	    // This is a class member, so we can resolve it
	    // by dereferencing 'this'.
	    if (member != nullptr) {
		size_t this_tmpvar = function->tmpvar_define(class_pointer_type);
		auto this_operation = std::make_unique<OperationLocalVariable>(
		    expression.get_identifier().get_source_ref(),
		    this_tmpvar,
		    "<this>",
		    class_pointer_type
		    );
		function->get_basic_block(current_block).add_operation(std::move(this_operation));

		size_t class_reference_tmpvar = function->tmpvar_define(class_type);
		auto dereference_operation = std::make_unique<OperationUnary>(
		    Operation::OP_DEREFERENCE,
		    expression.get_source_ref(),
		    class_reference_tmpvar,
		    this_tmpvar
		    );
		
		function
		    ->get_basic_block(current_block)
		    .add_operation(std::move(dereference_operation));
		
		returned_tmpvar = function->tmpvar_define(member->get_type());
		auto operation = std::make_unique<OperationDot>(
		    expression.get_source_ref(),
		    returned_tmpvar,
		    class_reference_tmpvar,
		    local_variable_name
		    );
		function
		    ->get_basic_block(current_block)
		    .add_operation(std::move(operation));
		return true;
	    }
	}
	
	// Next, check for member variable and insert the 'dereference' operation
	// and then a 'dot' operation to get the actual value of the variable.
	// temp var for the operation should be an argument that we've defined in the
	// scope, but only addressable by 'member' name (i.e. there is no 'this' argument
	// according to the method).
	
	
	// Look in the list of functions,
	// this might be a function pointer assignment or
	// a global variable.
	const Gyoji::mir::Symbol *symbol = mir.get_symbols().get_symbol(
	    expression.get_identifier().get_fully_qualified_name()
	    );
	if (symbol == nullptr) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    expression.get_source_ref(),
		    "Unresolved symbol",
		    std::string("Local variable ") + expression.get_identifier().get_fully_qualified_name() + std::string(" was not found in this scope.")
		    );
	    return false;
	}
	returned_tmpvar = function->tmpvar_define(symbol->get_type());

	auto operation = std::make_unique<OperationSymbol>(
	    expression.get_identifier().get_source_ref(),
	    returned_tmpvar,
	    expression.get_identifier().get_fully_qualified_name()
	    );
	function->get_basic_block(current_block).add_operation(std::move(operation));
	return true;
    }
    return false;
}

bool
FunctionDefinitionResolver::extract_from_expression_primary_nested(
    size_t & returned_tmpvar,
    const Gyoji::frontend::tree::ExpressionPrimaryNested & expression)
{
    // Nested expressions don't emit blocks on their own, just run whatever is nested.
    return extract_from_expression(
	returned_tmpvar,
	expression.get_expression()
	);
}

bool
FunctionDefinitionResolver::extract_from_expression_primary_literal_char(
    size_t & returned_tmpvar,
    const Gyoji::frontend::tree::ExpressionPrimaryLiteralChar & expression)
{
    std::string string_unescaped;
    size_t location;
    bool escape_success = Gyoji::misc::string_c_unescape(string_unescaped, location, expression.get_value(), true);
    char c;
    if (!escape_success) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_source_ref(),
		"Invalid Character Literal",
		std::string("Unknown escape sequence found at character offset ") + std::to_string(location) + std::string(" in character literal")
		);
	c = '!';
    }
    else {
	if (string_unescaped.size() != 1) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    expression.get_source_ref(),
		    "Invalid Character Literal",
		    std::string("Character literal must consist of a single byte.")
		    );
	    c = '%';
	}
	else {
	    // Finally, we're sure it's valid and
	    // has one element, we can pass it down
	    // to the operation.
	    c = string_unescaped[0];
	}
    }

    
    returned_tmpvar = function->tmpvar_define(mir.get_types().get_type("u8"));
    auto operation = std::make_unique<OperationLiteralChar>(
	expression.get_source_ref(),
	returned_tmpvar,
	c
	);

    function
	->get_basic_block(current_block)
	.add_operation(std::move(operation));
    return true;
}

bool
FunctionDefinitionResolver::extract_from_expression_primary_literal_string(
    size_t & returned_tmpvar,
    const Gyoji::frontend::tree::ExpressionPrimaryLiteralString & expression)
{
    // The hardest part here is that we need to extract the escape sequences from
    // the source representation and place the raw data into the operation
    // so that it is working with the actual literal value that will be
    // placed into the machine code during the code-generation stage.

    std::string string_unescaped;
    size_t location;
    bool escape_success = Gyoji::misc::string_c_unescape(string_unescaped, location, expression.get_value(), false);
    if (!escape_success) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_source_ref(),
		"Invalid String Literal",
		std::string("Unknown escape sequence found at character offset ") + std::to_string(location) + std::string(" in string")
		);
	// We can actually continue processing
	// the file so we can extract more errors later.
	// We've already found an error, so there's no danger
	// or producing an invalid file because the code-generator
	// won't run if there's an error.
    }

    
    
    returned_tmpvar = function->tmpvar_define(mir.get_types().get_type("u8*"));
    auto operation = std::make_unique<OperationLiteralString>(
	expression.get_source_ref(),
	returned_tmpvar,
	string_unescaped
	);
    function
	->get_basic_block(current_block)
	.add_operation(std::move(operation));
    return true;
}

bool
FunctionDefinitionResolver::create_constant_integer_one(
    const Gyoji::mir::Type *type,
    size_t & returned_tmpvar,
    const Gyoji::context::SourceReference & _src_ref
)
{
    Gyoji::frontend::integers::ParseLiteralIntResult parse_result;
    parse_result.parsed_type = type;

    switch (type->get_type()) {
    case Type::TYPE_PRIMITIVE_u8:
	parse_result.u8_value = (unsigned char)1;
	break;
    case Type::TYPE_PRIMITIVE_u16:
	parse_result.u16_value = (unsigned short)1;
	break;
    case Type::TYPE_PRIMITIVE_u32:
	parse_result.u32_value = (unsigned int)1;
	break;
    case Type::TYPE_PRIMITIVE_u64:
	parse_result.u64_value = (unsigned long)1;
	break;
    case Type::TYPE_PRIMITIVE_i8:
	parse_result.i8_value = (char)1;
	break;
    case Type::TYPE_PRIMITIVE_i16:
	parse_result.i16_value = (short)1;
	break;
    case Type::TYPE_PRIMITIVE_i32:
	parse_result.i32_value = (int)1;
	break;
    case Type::TYPE_PRIMITIVE_i64:
	parse_result.i64_value = (long)1;
	break;
    default:
	compiler_context
	    .get_errors()
	    .add_simple_error(
		_src_ref,
		"Compiler Bug! Invalid integer literal",
		std::string("Unsupported primitive literal type creating literal one value") + type->get_name()
		);
	return false;
    }
    return create_constant_integer(parse_result, returned_tmpvar, _src_ref);
}

bool
FunctionDefinitionResolver::create_constant_integer(
    const Gyoji::frontend::integers::ParseLiteralIntResult & parse_result,
    size_t & returned_tmpvar,
    const Gyoji::context::SourceReference & _src_ref
    )
{
    const Type *type_part = parse_result.parsed_type;
    returned_tmpvar = function->tmpvar_define(type_part);
    
    Gyoji::owned<Gyoji::mir::Operation> operation;
    switch (type_part->get_type()) {
    case Type::TYPE_PRIMITIVE_u8:
    {
	operation = std::make_unique<OperationLiteralInt>(
	    _src_ref,
	    returned_tmpvar,
	    type_part->get_type(),
	    parse_result.u8_value
	    );
    }
    case Type::TYPE_PRIMITIVE_u16:
    {
	operation = std::make_unique<OperationLiteralInt>(
	    _src_ref,
	    returned_tmpvar,
	    type_part->get_type(),
	    parse_result.u16_value
	    );
    }
	break;
    case Type::TYPE_PRIMITIVE_u32:
    {
	operation = std::make_unique<OperationLiteralInt>(
	    _src_ref,
	    returned_tmpvar,
	    type_part->get_type(),
	    parse_result.u32_value
	    );
    }
	break;
    case Type::TYPE_PRIMITIVE_u64:
    {
	operation = std::make_unique<OperationLiteralInt>(
	    _src_ref,
	    returned_tmpvar,
	    type_part->get_type(),
	    parse_result.u64_value
	    );
    }
	break;

    // Signed
    case Type::TYPE_PRIMITIVE_i8:
    {
	operation = std::make_unique<OperationLiteralInt>(
	    _src_ref,
	    returned_tmpvar,
	    type_part->get_type(),
	    parse_result.i8_value
	    );
    }
    case Type::TYPE_PRIMITIVE_i16:
    {
	operation = std::make_unique<OperationLiteralInt>(
	    _src_ref,
	    returned_tmpvar,
	    type_part->get_type(),
	    parse_result.i16_value
	    );
    }
	break;
    case Type::TYPE_PRIMITIVE_i32:
    {
	operation = std::make_unique<OperationLiteralInt>(
	    _src_ref,
	    returned_tmpvar,
	    type_part->get_type(),
	    parse_result.i32_value
	    );
    }
	break;
    case Type::TYPE_PRIMITIVE_i64:
    {
	operation = std::make_unique<OperationLiteralInt>(
	    _src_ref,
	    returned_tmpvar,
	    type_part->get_type(),
	    parse_result.i64_value
	    );
    }
	break;

    default:
	compiler_context
	    .get_errors()
	    .add_simple_error(
		_src_ref,
		"Compiler Bug! Invalid integer literal",
		std::string("Unsupported primitive literal type ") + type_part->get_name()
		);
	return false;
    }
    function
	->get_basic_block(current_block)
	.add_operation(std::move(operation));

    return true;
}

// There is a LOT of logic here for handling various
// kinds of integer literals that might be specified here.
// Of course, the simplest is something like
//     u32 a = 10;
// We also support more interesting
// things like hexidecimal, octal, and binary
// such at:
//     u64 a = 0xfeeda747u64;
//     u16 b = 0o03423;
// We also allow separators for readability, consider:
//     u64 h = 0xfeed_a_747_u64;
//     u16 o = 0o034_23;
//     u16 b = 0b0100_0200_u16;
// Combine that with range-checking all of the
// possible representations, and you have a lot of things
// that can go wrong such as a number being badly formatted
// or with characters outside the radix.
// Condier what happens when the user specifies these:
//     u8 n = 342343u8; // Number too big for u8
//     u8 n = -12u8;    // It's an unsigned, but you put a negative there.
//     u8 n = 0b23334u8;  // It's a binary number, but it's not really binary (and too big also).
// SO many error messages so that the user knows PRECISELY what they did wrong and where.
bool
FunctionDefinitionResolver::extract_from_expression_primary_literal_int(
    size_t & returned_tmpvar,
    const Gyoji::frontend::tree::ExpressionPrimaryLiteralInt & expression)
{
    Gyoji::frontend::integers::ParseLiteralIntResult parse_result;
    const Terminal & literal_int_token = expression.get_literal_int_token();
    bool parsed = parse_literal_int(compiler_context, mir.get_types(), literal_int_token, parse_result);
    if (!parsed || parse_result.parsed_type == nullptr) {
	return false;
    }

    return create_constant_integer(
	    parse_result,
	    returned_tmpvar,
	    literal_int_token.get_source_ref());
}

bool
FunctionDefinitionResolver::extract_from_expression_primary_literal_float(
    size_t & returned_tmpvar,
    const Gyoji::frontend::tree::ExpressionPrimaryLiteralFloat & expression)
{
    std::string literal_type_name = expression.get_type();
    returned_tmpvar = function->tmpvar_define(mir.get_types().get_type(literal_type_name));
    Gyoji::owned<OperationLiteralFloat> operation;
    char *endptr;
    const char *source_cstring = expression.get_value().c_str();
    size_t length = expression.get_value().size();
    errno = 0;
    if (literal_type_name == "f32") {

	float converted_value = strtof(source_cstring, &endptr);
	if (endptr != (source_cstring + length)) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    expression.get_source_ref(),
		    "Invalid floating-point literal",
		    std::string("Could not correctly parse the literal value.")
		    );
	    return false;
	}
	if (errno == ERANGE) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    expression.get_source_ref(),
		    "Invalid floating-point literal",
		    std::string("Floating-point literal does not fit in the range of an f32.")
		    );
	    return false;
	}
	operation = std::make_unique<OperationLiteralFloat>(
	    expression.get_source_ref(),
	    returned_tmpvar,
	    (float)converted_value
	    );
    }
    else {
	double converted_value = strtod(source_cstring, &endptr);
	if (endptr != (source_cstring + length)) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    expression.get_source_ref(),
		    "Invalid floating-point literal",
		    std::string("Could not correctly parse the literal value.")
		    );
	    return false;
	}
	if (errno == ERANGE) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    expression.get_source_ref(),
		    "Invalid floating-point literal",
		    std::string("Floating-point literal does not fit in the range of an f64.")
		    );
	    return false;
	}
	operation = std::make_unique<OperationLiteralFloat>(
	    expression.get_source_ref(),
	    returned_tmpvar,
	    converted_value
	    );
    }
    function
	->get_basic_block(current_block)
	.add_operation(std::move(operation));
    return true;
}

bool
FunctionDefinitionResolver::extract_from_expression_postfix_array_index(
    size_t & returned_tmpvar,
    const ExpressionPostfixArrayIndex & expression)
{
    size_t array_tmpvar;
    size_t index_tmpvar;
    if (!extract_from_expression(array_tmpvar, expression.get_array())) {
	return false;
    }
    if (!extract_from_expression(index_tmpvar, expression.get_index())) {
	return false;
    }

    const Type *array_type = function->tmpvar_get(array_tmpvar);
    if (!array_type->is_array()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_array().get_source_ref(),
		"Array type must be an array type",
		std::string("Type of array is not an array type.")
		);
	return false;
    }

    const Type *index_type = function->tmpvar_get(index_tmpvar);
    if (index_type->get_type() != Type::TYPE_PRIMITIVE_u32) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_index().get_source_ref(),
		"Array index must be an unsigned 32-bit (u32) type",
		std::string("Type of index is not a u32 index")
		);
	return false;
    }
    
    returned_tmpvar = function->tmpvar_define(array_type->get_pointer_target());
    auto operation = std::make_unique<OperationArrayIndex>(
	expression.get_source_ref(),
	returned_tmpvar,
	array_tmpvar,
	index_tmpvar
	);
    function
	->get_basic_block(current_block)
	.add_operation(std::move(operation));

    return true;
}


bool
FunctionDefinitionResolver::check_function_call_signature(
    bool is_method,
    const std::vector<size_t> & passed_arguments,
    const std::vector<const Gyoji::context::SourceReference *> & passed_src_refs,
    const Type *function_pointer_type,
    const Gyoji::context::SourceReference & src_ref    
    )
{
    // Here, we need to check that the arguments we're passing to the
    // function match the function/method signature.
    const std::vector<Argument> & function_pointer_args = function_pointer_type->get_argument_types();

    bool is_ok = true;
    
    if (passed_arguments.size() != function_pointer_args.size()) {
	std::unique_ptr<Gyoji::context::Error> error = std::make_unique<Gyoji::context::Error>(
	    (is_method ?
	     std::string("Wrong number of arguments passed to method call.") :
	     std::string("Wrong number of arguments passed to function call.")
	    )    
	    );
	error->add_message(src_ref,
			   std::string("Passing ")
			   + std::to_string(passed_arguments.size() - (is_method ? 1 : 0))
			   + ( is_method ? std::string(" to method") : std::string(" to function."))
	    );
	error->add_message(function_pointer_type->get_declared_source_ref(),
			   (is_method ? std::string("Method was declared to have ") : std::string("Function was declared to have "))
			   + std::to_string(function_pointer_args.size() - (is_method ? 1 : 0))
			   + std::string(" arguments.")
	    );
	compiler_context
	    .get_errors()
	    .add_error(std::move(error));
	is_ok = false;
    }

    // If we're not in an unsafe context (i.e. we're in safe mode)
    // then we cannot call an unsafe function.
    if (!scope_tracker.is_unsafe()) {
	if (function_pointer_type->is_unsafe()) {
	    std::unique_ptr<Gyoji::context::Error> error = std::make_unique<Gyoji::context::Error>(
		(is_method ?
		 std::string("Calling an unsafe method from a safe context.") : 
		 std::string("Calling an unsafe function from a safe context.")
		    )    
		);
	    error->add_message(src_ref,
			       (is_method ? std::string("Method ") : std::string("Function ") )
			       + std::string("is declared as unsafe, but this is not inside a scope marked unsafe.")
		);
	    compiler_context
		.get_errors()
		.add_error(std::move(error));
	    is_ok = false;
	}
    }
    
    size_t minsize = std::min(passed_arguments.size(), function_pointer_args.size());
    for (size_t i = 0; i < minsize; i++) {
	const Type *passed_type = function->tmpvar_get(passed_arguments.at(i));
	const SourceReference & passed_src_ref = *passed_src_refs.at(i);
	const Argument & arg = function_pointer_args.at(i);
	const Type *required_type = arg.get_type();
	if (required_type->get_name() != passed_type->get_name()) {
	    std::unique_ptr<Gyoji::context::Error> error = std::make_unique<Gyoji::context::Error>("Incorrect argument type passed to call");
	    error->add_message(passed_src_ref,
			       std::string("Passing type ")
			       + passed_type->get_name()
			       + std::string(" as argument ")
			       + std::to_string(i+1)
			       
		);
	    error->add_message(arg.get_source_ref(),
			       std::string("Argument type was declared as ")
			       + required_type->get_name()
		);
	    compiler_context
		.get_errors()
		.add_error(std::move(error));
	    is_ok = false;
	}
    }
    
    return is_ok;
}

bool
FunctionDefinitionResolver::extract_from_expression_postfix_function_call(
    size_t & returned_tmpvar,
    const ExpressionPostfixFunctionCall & expression)
{
    // Extract the expression itself from the arguments.
    size_t function_type_tmpvar;
    if (!extract_from_expression(function_type_tmpvar, expression.get_function())) {
	return false;
    }
    // For method calls, function_type_tmpvar must somehow encode
    // both the type of function pointer to call
    // as well as another tmpvar for the class value itself.
    // The value will end up encoding pair of "class_tmpvar" and "function_type_tmpvar".
    
    std::vector<size_t> passed_arguments;
    std::vector<const Gyoji::context::SourceReference *> passed_src_refs;
    for (const auto & arg_expr : expression.get_arguments().get_arguments()) {
	size_t arg_returned_value;
	if (!extract_from_expression(arg_returned_value, *arg_expr)) {
	    return false;
	}
	passed_arguments.push_back(arg_returned_value);
	passed_src_refs.push_back(&arg_expr->get_source_ref());
    }

    const Type *call_type = function->tmpvar_get(function_type_tmpvar);
    if (call_type->get_type() == Type::TYPE_FUNCTION_POINTER) {
	const Type *function_pointer_type = call_type;
	// We declare that we return the vale that the function
	// will return.
	returned_tmpvar = function->tmpvar_define(function_pointer_type->get_return_type());

	// Check that the function signature we're calling matches
	// the declaration of that function.
	if (!check_function_call_signature(false, passed_arguments, passed_src_refs, function_pointer_type, expression.get_source_ref())) {
	    return false;
	}
	
	auto operation = std::make_unique<OperationFunctionCall>(
	    Operation::OP_FUNCTION_CALL,
	    expression.get_source_ref(),
	    returned_tmpvar,
	    function_type_tmpvar,
	    passed_arguments
	    );
	
	function
	    ->get_basic_block(current_block)
	    .add_operation(std::move(operation));
	
	return true;
    }
    else if (call_type->get_type() == Type::TYPE_METHOD_CALL) {
	const Type *method_call_type = call_type;
	// First, extract the object from the method call into a tmpvar.
	// Next, extract the function into another tmpvar.
	// Finally, put together the arguments and make the call.
	
	size_t function_pointer_type_tmpvar = function->tmpvar_define(method_call_type->get_function_pointer_type());

	// This is what resolves the method to call.
	auto operation_get_function = std::make_unique<OperationUnary>(
	    Operation::OP_METHOD_GET_FUNCTION,
	    expression.get_source_ref(),
	    function_pointer_type_tmpvar,
	    function_type_tmpvar
	    );
	function
	    ->get_basic_block(current_block)
	    .add_operation(std::move(operation_get_function));

	// This is what resolves the object to make
	// the functionn call on.  We push it as the
	// implicit first argument to the call.
	const Type * pointer_to_object_type = mir.get_types().get_pointer_to(method_call_type->get_class_type(), expression.get_source_ref());
	size_t method_object_tmpvar = function->tmpvar_define(pointer_to_object_type);
	
	auto operation_get_object = std::make_unique<OperationUnary>(
	    Operation::OP_METHOD_GET_OBJECT,
	    expression.get_source_ref(),
	    method_object_tmpvar,
	    function_type_tmpvar
	    );
	passed_arguments.insert(passed_arguments.begin(), method_object_tmpvar);
	passed_src_refs.insert(passed_src_refs.begin(), &expression.get_source_ref());
	function
	    ->get_basic_block(current_block)
	    .add_operation(std::move(operation_get_object));
	
	const Type *function_pointer_type = method_call_type->get_function_pointer_type();
	
	// Check that the method signature we're calling matches
	// the declaration of that method
	if (!check_function_call_signature(true, passed_arguments, passed_src_refs, function_pointer_type, expression.get_source_ref())) {
	    return false;
	}
	
	// This is what the function pointer type should return.
	returned_tmpvar = function->tmpvar_define(function_pointer_type->get_return_type());

	// Make the actual function call.
	auto operation = std::make_unique<OperationFunctionCall>(
	    Operation::OP_FUNCTION_CALL,
	    expression.get_source_ref(),
	    returned_tmpvar,
	    function_pointer_type_tmpvar,
	    passed_arguments
	    );
	function
	    ->get_basic_block(current_block)
	    .add_operation(std::move(operation));
	
	return true;
    }
    else {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_function().get_source_ref(),
		"Called object is not a function.",
		std::string("Type of object being called is not a function, but is a ") + call_type->get_name() + std::string(" instead.")
		);
	return false;
    }
}

bool
FunctionDefinitionResolver::extract_from_expression_postfix_dot(
    size_t & returned_tmpvar,
    const ExpressionPostfixDot & expression)
{
    size_t class_tmpvar;
    if (!extract_from_expression(class_tmpvar, expression.get_expression())) {
	return false;
    }

    const Type *class_type = function->tmpvar_get(class_tmpvar);
    if (class_type->get_type() != Type::TYPE_COMPOSITE) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_expression().get_source_ref(),
		"Member access must be applied to a class.",
		std::string("Type of object being accessed is not a class, but is a ") + class_type->get_name() + std::string(" instead.")
		);
	return false;
    }
    const std::string & member_name = expression.get_identifier().get_name();

    const TypeMember *member = class_type->member_get(member_name);
    if (member != nullptr) {
	returned_tmpvar = function->tmpvar_define(member->get_type());
	auto operation = std::make_unique<OperationDot>(
	    expression.get_source_ref(),
	    returned_tmpvar,
	    class_tmpvar,
	    member_name
	    );
	function
	    ->get_basic_block(current_block)
	    .add_operation(std::move(operation));
	return true;
    }
    
    const TypeMethod *method = class_type->method_get(member_name);
    if (method != nullptr) {
	std::string fully_qualified_function_name = class_type->get_name() + NS2Context::NAMESPACE_DELIMITER + member_name;
	const Gyoji::mir::Symbol *symbol = mir.get_symbols().get_symbol(fully_qualified_function_name);
	if (symbol == nullptr) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    expression.get_expression().get_source_ref(),
		    "Class method not found.",
		    std::string("Method ") + fully_qualified_function_name + std::string(" was not found on class ") + class_type->get_name()
		    );
	    return false;
	}

	const Type * method_call_type = mir.get_types().get_method_call(class_type, symbol->get_type(), expression.get_source_ref());
	returned_tmpvar = function->tmpvar_define(method_call_type);
	
	auto operation = std::make_unique<OperationGetMethod>(
	    expression.get_source_ref(),
	    returned_tmpvar,
	    class_tmpvar,
 	    fully_qualified_function_name
	    );
	function
	    ->get_basic_block(current_block)
	    .add_operation(std::move(operation));
	return true;
    }
    compiler_context
	.get_errors()
	.add_simple_error(
	    expression.get_expression().get_source_ref(),
	    "Member or method not found.",
	    std::string("Class does not have member or method '") + member_name + std::string("'.")
	    );
    
    return false;
}

bool
FunctionDefinitionResolver::extract_from_expression_postfix_arrow(
    size_t & returned_tmpvar,
    const ExpressionPostfixArrow & expression)
{
    size_t classptr_tmpvar;
    if (!extract_from_expression(classptr_tmpvar, expression.get_expression())) {
	return false;
    }

    const Type *classptr_type = function->tmpvar_get(classptr_tmpvar);
    if (classptr_type->get_type() != Type::TYPE_POINTER) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_expression().get_source_ref(),
		"Arrow (->) operator must be used on a pointer to a class.",
		std::string("Type of object being accessed is not a pointer to a class, but is a ") + classptr_type->get_name() + std::string(" instead.")
		);
	return false;
    }
    const Type *class_type = classptr_type->get_pointer_target();
    
    if (class_type->get_type() != Type::TYPE_COMPOSITE) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_expression().get_source_ref(),
		"Arrow (->) access must be applied to a pointer to a class.",
		std::string("Type of object being accessed is not a pointer to a class , but is a pointer to ") + class_type->get_name() + std::string(" instead.")
		);
	return false;
    }

    if (!scope_tracker.is_unsafe()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_expression().get_source_ref(),
		"De-referencing pointers (->) must be done inside an 'unsafe' block.",
		std::string("De-referencing a pointer outside an 'unsafe' block breaks the safety guarantees of the language.")
		);
	return false;
    }
    // First takes 'operand' as a pointer
    // and de-references it into an lvalue.
    size_t class_reference_tmpvar = function->tmpvar_define(class_type);
    auto dereference_operation = std::make_unique<OperationUnary>(
	    Operation::OP_DEREFERENCE,
	    expression.get_source_ref(),
	    class_reference_tmpvar,
	    classptr_tmpvar
	    );
    
    function
	->get_basic_block(current_block)
	.add_operation(std::move(dereference_operation));
    
    const std::string & member_name = expression.get_identifier().get_name();
    const TypeMember *member = class_type->member_get(member_name);
    if (member == nullptr) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_expression().get_source_ref(),
		"Attempt to access an undeclared member",
		std::string("Member ") + member_name + std::string(" was not declared in ") + class_type->get_name()
		);
	return false;
    }
    
    returned_tmpvar = function->tmpvar_define(member->get_type());
    auto dot_operation = std::make_unique<OperationDot>(
	expression.get_source_ref(),
	returned_tmpvar,
	class_reference_tmpvar,
	member_name
	);
    function
	->get_basic_block(current_block)
	.add_operation(std::move(dot_operation));
    return true;

}

bool
FunctionDefinitionResolver::extract_from_expression_postfix_incdec(
    size_t & returned_tmpvar,
    const ExpressionPostfixIncDec & expression)
{
    size_t operand_tmpvar;
    if (!extract_from_expression(operand_tmpvar, expression.get_expression())) {
	return false;
    }

    return create_incdec_operation(
	expression.get_source_ref(),
	returned_tmpvar,
	operand_tmpvar,
	(expression.get_type() == ExpressionPostfixIncDec::INCREMENT), // is_increment
	true // is_postfix
	);
}

bool
FunctionDefinitionResolver::create_incdec_operation(
    const Gyoji::context::SourceReference & src_ref,
    size_t & returned_tmpvar,
    const size_t & operand_tmpvar,
    bool is_increment,
    bool is_postfix
    )
{
    // This should be implemented as:
    //         _1 = load(variable)
    //         _2 = constant(1)
    //         _3 = add/sub _1 _2 <==== Add or subtract depending on is_increment
    //              store(_3, variable)
    //         _4 = <==== This will be either _1 or _3 depending on is_postfix.
    //
    
    const Type *operand_type = function->tmpvar_get(operand_tmpvar);
    size_t constant_one_tmpvar;
    if (!create_constant_integer_one(
	    operand_type,
	    constant_one_tmpvar,
	    src_ref
	    )) {
	return false;
    }
    
    
    size_t addresult_tmpvar = function->tmpvar_duplicate(operand_tmpvar);
    if (is_increment) {
	auto operation = std::make_unique<OperationBinary>(
	    Operation::OP_ADD,
	    src_ref,
	    addresult_tmpvar,
	    operand_tmpvar,
	    constant_one_tmpvar
	    );
	function
	    ->get_basic_block(current_block)
	    .add_operation(std::move(operation));
    }
    else {
	auto operation = std::make_unique<OperationBinary>(
	    Operation::OP_SUBTRACT,
	    src_ref,
	    addresult_tmpvar,
	    operand_tmpvar,
	    constant_one_tmpvar
	    );
	function
	    ->get_basic_block(current_block)
	    .add_operation(std::move(operation));
    }

    // We perform a 'store' to store
    // the value back into the variable.
    size_t ignore_tmpvar = function->tmpvar_duplicate(operand_tmpvar);
    auto operation_store = std::make_unique<OperationBinary>(
	Operation::OP_ASSIGN,
	src_ref,
	ignore_tmpvar,
	operand_tmpvar,
	addresult_tmpvar
	);
    function
	->get_basic_block(current_block)
	.add_operation(std::move(operation_store));
    
    // This is a post-decrement, so we return
    // the value as it was before we incremented
    // it.
    returned_tmpvar = is_postfix ? operand_tmpvar : addresult_tmpvar;
    return true;
}

bool
FunctionDefinitionResolver::extract_from_expression_unary_prefix(
    size_t & returned_tmpvar,
    const ExpressionUnaryPrefix & expression)
{

    // Extract the prior expression
    // and if not otherwise specified,
    // the operation will return the
    // same type as the operand.
    size_t operand_tmpvar;
    if (!extract_from_expression(
	    operand_tmpvar,
	    expression.get_expression()
	    )) {
	return false;
    }
    
    const Type *operand_type = function->tmpvar_get(operand_tmpvar);
    if (operand_type == nullptr) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_source_ref(),
		"Compiler bug!  Please report this message(4)",
		"Operand must be a valid type."
		);
    }

    ExpressionUnaryPrefix::OperationType op_type = expression.get_type();
    switch (op_type) {    
    case ExpressionUnaryPrefix::INCREMENT:
        {
	    return create_incdec_operation(
		expression.get_source_ref(),
		returned_tmpvar,
		operand_tmpvar,
		true, // is_increment
		false // is_postfix
		);
        }
        break;
    case ExpressionUnaryPrefix::DECREMENT:
        {
	    return create_incdec_operation(
		expression.get_source_ref(),
		returned_tmpvar,
		operand_tmpvar,
		false, // is_increment
		false // is_postfix
		);
        }
        break;
    case ExpressionUnaryPrefix::ADDRESSOF:
        {
	const Type * pointer_to_operand_type = mir.get_types().get_pointer_to(operand_type, expression.get_source_ref());
	returned_tmpvar = function->tmpvar_define(pointer_to_operand_type);
	auto operation = std::make_unique<OperationUnary>(
	    Operation::OP_ADDRESSOF,
	    expression.get_source_ref(),
	    returned_tmpvar,
	    operand_tmpvar
	    );
	function
	    ->get_basic_block(current_block)
	    .add_operation(std::move(operation));
	}
        break;
    case ExpressionUnaryPrefix::DEREFERENCE:
        {
	    bool is_ok = true;
	    if (!operand_type->is_pointer() && !operand_type->is_reference()) {
		compiler_context
		    .get_errors()
		    .add_simple_error(
			expression.get_expression().get_source_ref(),
			"Cannot dereference non-pointer",
			std::string("Attempting to de-reference non-pointer type ") + operand_type->get_name()
			);
		is_ok = false;
	    }
	    if (!scope_tracker.is_unsafe() && !operand_type->is_reference()) {
		compiler_context
		    .get_errors()
		    .add_simple_error(
			expression.get_expression().get_source_ref(),
			"De-referencing pointers (*) must be done inside an 'unsafe' block.",
			std::string("De-referencing a pointer outside an 'unsafe' block breaks the safety guarantees of the language.")
			);
		is_ok = false;
	    }
	    if (!is_ok) {
		return false;
	    }
	    returned_tmpvar = function->tmpvar_define(operand_type->get_pointer_target());
	    auto operation = std::make_unique<OperationUnary>(
		Operation::OP_DEREFERENCE,
		expression.get_source_ref(),
		returned_tmpvar,
		operand_tmpvar
		);
	    function
		->get_basic_block(current_block)
		.add_operation(std::move(operation));
        }
        break;
    case ExpressionUnaryPrefix::PLUS:
        {
	// Unary plus does nothing, really, so why bother?  We just don't
	// bother to do anything and just wire the return into the operand
	// directly instead of creating a new tmpvar and assigning it.
	returned_tmpvar = operand_tmpvar;
        }
    break;
    case ExpressionUnaryPrefix::MINUS:
        {
	returned_tmpvar = function->tmpvar_duplicate(operand_tmpvar);
	auto operation = std::make_unique<OperationUnary>(
	    Operation::OP_NEGATE,
	    expression.get_source_ref(),	    
	    returned_tmpvar,
	    operand_tmpvar
	    );
	function
	    ->get_basic_block(current_block)
	    .add_operation(std::move(operation));
        }
        break;
    case ExpressionUnaryPrefix::BITWISE_NOT:
        {
	returned_tmpvar = function->tmpvar_duplicate(operand_tmpvar);
	auto operation = std::make_unique<OperationUnary>(
	    Operation::OP_BITWISE_NOT,
	    expression.get_source_ref(),	    
	    returned_tmpvar,
	    operand_tmpvar
	    );
	function
	    ->get_basic_block(current_block)
	    .add_operation(std::move(operation));
        }
        break;
    case ExpressionUnaryPrefix::LOGICAL_NOT:
        {
	if (!function->tmpvar_get(operand_tmpvar)->is_bool()) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    expression.get_expression().get_source_ref(),
		    "Logical not (!) must operate on 'bool' expressions.",
		    std::string("Type of condition expression should be 'bool' and was ") + function->tmpvar_get(operand_tmpvar)->get_name()
		    );
	}
	returned_tmpvar = function->tmpvar_duplicate(operand_tmpvar);
	auto operation = std::make_unique<OperationUnary>(
	    Operation::OP_LOGICAL_NOT,
	    expression.get_source_ref(),	    
	    returned_tmpvar,
	    operand_tmpvar
	    );
	function
	    ->get_basic_block(current_block)
	    .add_operation(std::move(operation));
	}
        break;
    default:
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_expression().get_source_ref(),
		"Compiler Bug!",
		"Encountered unknown unary prefix expression"
		);
	return false;
    }
    return true;
}
bool
FunctionDefinitionResolver::extract_from_expression_unary_sizeof_type(
    size_t & returned_tmpvar,
    const ExpressionUnarySizeofType & expression)
{
    const Type * operand_type = type_resolver.extract_from_type_specifier(expression.get_type_specifier());
    const Type * u64_type = mir.get_types().get_type("u64");
    returned_tmpvar = function->tmpvar_define(u64_type);
    auto operation = std::make_unique<OperationSizeofType>(
	expression.get_source_ref(),	    
	returned_tmpvar,
	operand_type
	);
    function
	->get_basic_block(current_block)
	.add_operation(std::move(operation));
    return true;
}

bool
FunctionDefinitionResolver::numeric_widen(
    const Gyoji::context::SourceReference & _src_ref,
    size_t & _widen_var,
    const Type *widen_to
    )
{
    // The new b is the result of widening b
    // to the type of a.
    Operation::OperationType widen_type;
    if (widen_to->is_integer() && widen_to->is_signed()) {
	widen_type = Operation::OP_WIDEN_SIGNED;
    }
    else if (widen_to->is_integer() && widen_to->is_unsigned()) {
	widen_type = Operation::OP_WIDEN_UNSIGNED;
    }
    else if (widen_to->is_float()) {
	widen_type = Operation::OP_WIDEN_FLOAT;
    }
    else {
	fprintf(stderr, "Compiler Bug! Widening number of unknown type\n");
	return false;
    }
    
    size_t widened_var = function->tmpvar_define(widen_to);
    auto operation = std::make_unique<OperationCast>(
	widen_type,
	_src_ref,
	widened_var,
	_widen_var,
	widen_to
	);
    function
	->get_basic_block(current_block)
	.add_operation(std::move(operation));
    _widen_var = widened_var;
    return true;
}
bool
FunctionDefinitionResolver::numeric_widen_binary_operation(
    const Gyoji::context::SourceReference & _src_ref,
    size_t & a_tmpvar,
    size_t & b_tmpvar,
    const Gyoji::mir::Type *atype,
    const Gyoji::mir::Type *btype,
    const Gyoji::mir::Type **widened
    )
{
    if (atype->is_integer()) {
	// Deal with signed-ness.  We can't mix signed and unsigned.
	if (atype->is_signed() && btype->is_signed()) {
	    if (atype->get_primitive_size() > btype->get_primitive_size()) {
		if (!numeric_widen(_src_ref, b_tmpvar, atype)) {
		    return false;
		}
		*widened = atype;
	    }
	    else if (atype->get_primitive_size() < btype->get_primitive_size()) {
		if (!numeric_widen(_src_ref, a_tmpvar, btype)) {
		    return false;
		}
		*widened = btype;
	    }
	    else {
		// Both types are the same size already, so we do nothing
		// and it doesn't matter whether we picked 'a' or 'b'.
		*widened = atype;
	    }
	}
	else if (atype->is_unsigned() && btype->is_unsigned()) {
	    if (atype->get_primitive_size() > btype->get_primitive_size()) {
		if (!numeric_widen(_src_ref, b_tmpvar, atype)) {
		    return false;
		}
		*widened = atype;
	    }
	    else if (atype->get_primitive_size() < btype->get_primitive_size()) {
		if (!numeric_widen(_src_ref, a_tmpvar, btype)) {
		    return false;
		}
		*widened = btype;
	    }
	    else {
		// Both types are the same size already, so we do nothing
		// and it doesn't matter whether we picked 'a' or 'b'.
		*widened = atype;
	    }
	}
	else {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    _src_ref,
		    "Type mismatch in binary operation",
		    std::string("The type of operands both signed or unsigned.  Automatic cast from signed to unsigned is not supported. a= ") +
		    atype->get_name() + std::string(" b=") + btype->get_name()
		    );
	    return false;
	}
    }
    // We don't have to check because we already
    // know that we have a float in the else.
    else {
	if (atype->get_primitive_size() > btype->get_primitive_size()) {
	    if (!numeric_widen(_src_ref, b_tmpvar, atype)) {
		return false;
	    }
	    *widened = atype;
	}
	else if (atype->get_primitive_size() < btype->get_primitive_size()) {
	    if (!numeric_widen(_src_ref, a_tmpvar, btype)) {
		return false;
	    }
	    *widened = btype;
	}
	else {
	    *widened = atype;
	}
    }
    return true;
}

bool
FunctionDefinitionResolver::handle_binary_operation_arithmetic(
    const Gyoji::context::SourceReference & _src_ref,
    Operation::OperationType type,
    size_t & returned_tmpvar,
    size_t a_tmpvar,
    size_t b_tmpvar
    )
{
    const Type *atype = function->tmpvar_get(a_tmpvar);
    const Type *btype = function->tmpvar_get(b_tmpvar);
    // Check that both operands are numeric.
    if (!atype->is_numeric() ||	!btype->is_numeric()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		_src_ref,
		"Type mismatch in binary operation",
		std::string("The type of operands should be numeric, but were: a= ") + atype->get_name() + std::string(" b=") + btype->get_name()
		);
	return false;
    }
    // Special-case for modulo because it doesn't support floating-point types.
    if (type == Operation::OP_MODULO) {
	if (atype->is_float() || btype->is_float()) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    _src_ref,
		    "Type mismatch in binary operation",
		    std::string("The type of operands should be integer, but were a= ") + atype->get_name() + std::string(" b=") + btype->get_name()
		    );
	    return false;
	}
    }
    
    // The arguments must be either both integer
    // or both float.
    if 	(!(
	     (atype->is_integer() && btype->is_integer()) ||
	     (atype->is_float() && btype->is_float())
        )) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		_src_ref,
		"Type mismatch in binary operation",
		std::string("The type of operands both integer or floating-point types.  Automatic cast from int to float is not supported. a= ") +
		atype->get_name() + std::string(" b=") + btype->get_name()
		);
	return false;
    }

    // Now, both of them are either int or float, so we need to handle each separately
    // so that we can choose the appropriate cast type.
    const Type *widened = nullptr;
    if (!numeric_widen_binary_operation(_src_ref, a_tmpvar, b_tmpvar, atype, btype, &widened)) {
	return false;
    }

    // The return type is whatever
    // we widened the add to be.
    returned_tmpvar = function->tmpvar_define(widened);
    
    // We emit the appropriate operation as either an integer or
    // floating-point add depending on which one it was.
    // The back-end may assume that the operand types are both
    // integer or floating point of the same type and
    // the return type will also be of the same type.
    auto operation = std::make_unique<OperationBinary>(
	type,
	_src_ref,
	returned_tmpvar,
	a_tmpvar,
	b_tmpvar
	);
    function
	->get_basic_block(current_block)
	.add_operation(std::move(operation));
    
    return true;
}

bool
FunctionDefinitionResolver::handle_binary_operation_logical(
    const Gyoji::context::SourceReference & _src_ref,
    Operation::OperationType type,
    size_t & returned_tmpvar,
    size_t a_tmpvar,
    size_t b_tmpvar
    )
{
    const Type *atype = function->tmpvar_get(a_tmpvar);
    const Type *btype = function->tmpvar_get(b_tmpvar);
    // Check that both operands are numeric.
    if (!atype->is_bool() || !btype->is_bool()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		_src_ref,
		"Type mismatch in logical operation",
		std::string("The type of operands should be bool , but were: a= ") + atype->get_name() + std::string(" b=") + btype->get_name()
		);
	return false;
    }
    auto operation = std::make_unique<OperationBinary>(
	type,
	_src_ref,
	returned_tmpvar,
	a_tmpvar,
	b_tmpvar
	);
    function
	->get_basic_block(current_block)
	.add_operation(std::move(operation));
    return true;
}

bool
FunctionDefinitionResolver::handle_binary_operation_bitwise(
    const Gyoji::context::SourceReference & _src_ref,
    Operation::OperationType type,
    size_t & returned_tmpvar,
    size_t a_tmpvar,
    size_t b_tmpvar
    )
{
    const Type *atype = function->tmpvar_get(a_tmpvar);
    const Type *btype = function->tmpvar_get(b_tmpvar);
    // Check that both operands are numeric.
    if (!atype->is_unsigned() || !btype->is_unsigned()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		_src_ref,
		"Type mismatch in binary operation",
		std::string("The type of operands should be unsigned integers, but were: a= ") + atype->get_name() + std::string(" b=") + btype->get_name()
		);
	return false;
    }
    // Now widen them to the appropriate sizes.
    const Type *widened = nullptr;
    if (!numeric_widen_binary_operation(_src_ref, a_tmpvar, b_tmpvar, atype, btype, &widened)) {
	return false;
    }

    // The return type is whatever
    // we widened the add to be.
    returned_tmpvar = function->tmpvar_define(widened);
    
    // We emit the appropriate operation as either an integer or
    // floating-point add depending on which one it was.
    // The back-end may assume that the operand types are both
    // integer or floating point of the same type and
    // the return type will also be of the same type.
    auto operation = std::make_unique<OperationBinary>(
	type,
	_src_ref,
	returned_tmpvar,
	a_tmpvar,
	b_tmpvar
	);
    function
	->get_basic_block(current_block)
	.add_operation(std::move(operation));
    
    return true;
}


bool
FunctionDefinitionResolver::handle_binary_operation_shift(
    const Gyoji::context::SourceReference & _src_ref,
    Operation::OperationType type,
    size_t & returned_tmpvar,
    size_t a_tmpvar,
    size_t b_tmpvar
    )
{
    const Type *atype = function->tmpvar_get(a_tmpvar);
    const Type *btype = function->tmpvar_get(b_tmpvar);
    // Check that both operands are numeric.
    if (!atype->is_unsigned() || !btype->is_unsigned()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		_src_ref,
		"Type mismatch in binary operation",
		std::string("The type of operands should be unsigned integers, but were: a= ") + atype->get_name() + std::string(" b=") + btype->get_name()
		);
	return false;
    }

    // Notably, we never widen a shift operation.
    // Instead, we operate on whatever it is because
    // it will end up being masked down to an 8-bit
    // value to avoid overflowing the LHS when the
    // shift happens if it's any bigger.

    // We always return the same size that the LHS is.
    returned_tmpvar = function->tmpvar_define(atype);
    
    // We emit the appropriate operation as either an integer or
    // floating-point add depending on which one it was.
    // The back-end may assume that the operand types are both
    // integer or floating point of the same type and
    // the return type will also be of the same type.
    auto operation = std::make_unique<OperationBinary>(
	type,
	_src_ref,
	returned_tmpvar,
	a_tmpvar,
	b_tmpvar
	);
    function
	->get_basic_block(current_block)
	.add_operation(std::move(operation));
    
    return true;
}

bool
FunctionDefinitionResolver::handle_binary_operation_compare(
    const Gyoji::context::SourceReference & _src_ref,
    Operation::OperationType type,
    size_t & returned_tmpvar,
    size_t a_tmpvar,
    size_t b_tmpvar
    )
{
    const Type *atype = function->tmpvar_get(a_tmpvar);
    const Type *btype = function->tmpvar_get(b_tmpvar);
    // Check that both operands are the same type.
    if (atype->get_name() != btype->get_name()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		_src_ref,
		"Type mismatch in compare operation",
		std::string("The operands of a comparision should be the same type, but were: a= ") + atype->get_name() + std::string(" b=") + btype->get_name()
		);
	return false;
    }
    if (atype->is_void()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		_src_ref,
		"Type mismatch in compare operation",
		std::string("The operands of a comparison must not be void, but were: a= ") + atype->get_name() + std::string(" b=") + btype->get_name()
		);
	return false;
    }
    if (atype->is_composite()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		_src_ref,
		"Type mismatch in compare operation",
		std::string("The operands of a comparison must not be composite structures or classes, but were: a= ") + atype->get_name() + std::string(" b=") + btype->get_name()
		);
	return false;
    }
    if (
	(atype->is_pointer() || atype->is_reference())
	&&
	!(type == Operation::OP_COMPARE_EQUAL ||
	  type == Operation::OP_COMPARE_NOT_EQUAL)
	) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		_src_ref,
		"Type mismatch in compare operation",
		std::string("The operands of a comparison of pointers and references may not be used except for equality comparisions, but were: a= ") + atype->get_name() + std::string(" b=") + btype->get_name()
		);
	return false;	
    }
    

    // Notably, we never widen a shift operation.
    // Instead, we operate on whatever it is because
    // it will end up being masked down to an 8-bit
    // value to avoid overflowing the LHS when the
    // shift happens if it's any bigger.

    // We always return the same size that the LHS is.
    returned_tmpvar = function->tmpvar_define(mir.get_types().get_type("bool"));
    
    // We emit the appropriate operation as either an integer or
    // floating-point add depending on which one it was.
    // The back-end may assume that the operand types are both
    // integer or floating point of the same type and
    // the return type will also be of the same type.
    auto operation = std::make_unique<OperationBinary>(
	type,
	_src_ref,
	returned_tmpvar,
	a_tmpvar,
	b_tmpvar
	);
    function
	->get_basic_block(current_block)
	.add_operation(std::move(operation));
    
    return true;
}

bool
FunctionDefinitionResolver::handle_binary_operation_assignment(
    const Gyoji::context::SourceReference & _src_ref,
    Operation::OperationType type,
    size_t & returned_tmpvar,
    size_t a_tmpvar,
    size_t b_tmpvar
    )
{
    const Type *atype = function->tmpvar_get(a_tmpvar);
    const Type *btype = function->tmpvar_get(b_tmpvar);
    // Check that both operands are the same type.
    if (atype->get_name() != btype->get_name()) {
	// If we're assigning a reference to a pointer, we
	// should allow it in some circumstances.
	if (atype->is_reference() && btype->is_pointer()) {
	    if (!scope_tracker.is_unsafe()) {
		compiler_context
		    .get_errors()
		    .add_simple_error(
			_src_ref,
			"Assigning a reference to a raw pointer must be done inside an 'unsafe' block",
			std::string("Assigning a pointer to a reference must be done inside an unsafe block")
			);
		return false;
	    }
	}
	else if (atype->is_pointer() && btype->is_reference()) {
	    // Nothing to do.  This is a valid assignment
	    // even inside an unsafe block.
	}
	else {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    _src_ref,
		    "Type mismatch in assignment operation",
		    std::string("The operands of an assignment should be the same type, but were: a= ") + atype->get_name() + std::string(" b=") + btype->get_name()
		    );
	    return false;
	}
    }
    if (atype->is_void()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		_src_ref,
		"Type mismatch in assignment operation",
		std::string("The operands of an assignment must not be void, but were: a= ") + atype->get_name() + std::string(" b=") + btype->get_name()
		);
	return false;
    }
    if (atype->is_composite()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		_src_ref,
		"Type mismatch in assignment operation",
		std::string("The operands of an assignment must not be composite structures or classes, but were: a= ") + atype->get_name() + std::string(" b=") + btype->get_name()
		);
	return false;
    }

    // Notably, we never widen a shift operation.
    // Instead, we operate on whatever it is because
    // it will end up being masked down to an 8-bit
    // value to avoid overflowing the LHS when the
    // shift happens if it's any bigger.

    // We always return the same size that the LHS is.
    returned_tmpvar = function->tmpvar_define(atype);
    
    // We emit the appropriate operation as either an integer or
    // floating-point add depending on which one it was.
    // The back-end may assume that the operand types are both
    // integer or floating point of the same type and
    // the return type will also be of the same type.
    auto operation = std::make_unique<OperationBinary>(
	type,
	_src_ref,
	returned_tmpvar,
	// a_tmpvar is an lvalue and b_tmpvar is an rvalue
	// should we wait to resolve the rvalue
	// until we actually consume it?
	a_tmpvar,
	b_tmpvar
	);
    function
	->get_basic_block(current_block)
	.add_operation(std::move(operation));
    
    return true;
}

bool
FunctionDefinitionResolver::extract_from_expression_binary(
    size_t & returned_tmpvar,
    const ExpressionBinary & expression)
{
    size_t a_tmpvar;
    size_t b_tmpvar;
    
    if (!extract_from_expression(a_tmpvar, expression.get_a())) {
	return false;
    }
    if (!extract_from_expression(b_tmpvar, expression.get_b())) {
	return false;
    }

    ExpressionBinary::OperationType op_type;
    op_type = expression.get_operator();

    if (op_type == ExpressionBinary::ADD) {
	if (!handle_binary_operation_arithmetic(
	    expression.get_source_ref(),
	    Operation::OP_ADD,
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar)) {
	    return false;
	}
    }	
    else if (op_type == ExpressionBinary::SUBTRACT) {
	if (!handle_binary_operation_arithmetic(
	    expression.get_source_ref(),
	    Operation::OP_SUBTRACT,
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::MULTIPLY) {
	if (!handle_binary_operation_arithmetic(
	    expression.get_source_ref(),
	    Operation::OP_MULTIPLY,
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::DIVIDE) {
	if (!handle_binary_operation_arithmetic(
	    expression.get_source_ref(),
	    Operation::OP_DIVIDE,
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::MODULO) {
	if (!handle_binary_operation_arithmetic(
	    expression.get_source_ref(),
	    Operation::OP_MODULO,
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::LOGICAL_AND) {
	if (!handle_binary_operation_logical(
	    expression.get_source_ref(),
	    Operation::OP_LOGICAL_AND,
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::LOGICAL_OR) {
	if (!handle_binary_operation_logical(
	    expression.get_source_ref(),
	    Operation::OP_LOGICAL_OR,
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::BITWISE_AND) {
	if (!handle_binary_operation_bitwise(
	    expression.get_source_ref(),
	    Operation::OP_BITWISE_AND,
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::BITWISE_OR) {
	if (!handle_binary_operation_bitwise(
	    expression.get_source_ref(),
	    Operation::OP_BITWISE_OR,
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::BITWISE_XOR) {
	if (!handle_binary_operation_bitwise(
	    expression.get_source_ref(),
	    Operation::OP_BITWISE_XOR,
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::SHIFT_LEFT) {
	if (!handle_binary_operation_shift(
	    expression.get_source_ref(),
	    Operation::OP_SHIFT_LEFT,
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::SHIFT_RIGHT) {
	if (!handle_binary_operation_shift(
	    expression.get_source_ref(),
	    Operation::OP_SHIFT_RIGHT,
	    returned_tmpvar,
	    a_tmpvar,
	    b_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::COMPARE_LESS) {
	if (!handle_binary_operation_compare(
		expression.get_source_ref(),
		Operation::OP_COMPARE_LESS,
		returned_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::COMPARE_GREATER) {
	if (!handle_binary_operation_compare(
		expression.get_source_ref(),
		Operation::OP_COMPARE_GREATER,
		returned_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::COMPARE_LESS_EQUAL) {
	if (!handle_binary_operation_compare(
		expression.get_source_ref(),
		Operation::OP_COMPARE_LESS_EQUAL,
		returned_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::COMPARE_GREATER_EQUAL) {
	if (!handle_binary_operation_compare(
		expression.get_source_ref(),
		Operation::OP_COMPARE_GREATER_EQUAL,
		returned_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::COMPARE_EQUAL) {
	if (!handle_binary_operation_compare(
		expression.get_source_ref(),
		Operation::OP_COMPARE_EQUAL,
		returned_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::COMPARE_NOT_EQUAL) {
	if (!handle_binary_operation_compare(
		expression.get_source_ref(),
		Operation::OP_COMPARE_NOT_EQUAL,
		returned_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::ASSIGNMENT) {
	if (!handle_binary_operation_assignment(
		expression.get_source_ref(),
		Operation::OP_ASSIGN,
		returned_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
    }	
    else if (op_type == ExpressionBinary::MUL_ASSIGNMENT) {
	// This is just syntax sugar for * followed by =
	size_t arithmetic_tmpvar;
	if (!handle_binary_operation_arithmetic(
		expression.get_source_ref(),
		Operation::OP_MULTIPLY,
		arithmetic_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
	if (!handle_binary_operation_assignment(
		expression.get_source_ref(),
		Operation::OP_ASSIGN,
		returned_tmpvar,
		a_tmpvar,
		arithmetic_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::DIV_ASSIGNMENT) {
	// This is just syntax sugar for / followed by =
	size_t arithmetic_tmpvar;
	if (!handle_binary_operation_arithmetic(
		expression.get_source_ref(),
		Operation::OP_DIVIDE,
		arithmetic_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
	if (!handle_binary_operation_assignment(
		expression.get_source_ref(),
		Operation::OP_ASSIGN,
		returned_tmpvar,
		a_tmpvar,
		arithmetic_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::MOD_ASSIGNMENT) {
	// This is just syntax sugar for % followed by =
	size_t arithmetic_tmpvar;
	if (!handle_binary_operation_arithmetic(
		expression.get_source_ref(),
		Operation::OP_MODULO,
		arithmetic_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
	if (!handle_binary_operation_assignment(
		expression.get_source_ref(),
		Operation::OP_ASSIGN,
		returned_tmpvar,
		a_tmpvar,
		arithmetic_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::ADD_ASSIGNMENT) {
	// This is just syntax sugar for + followed by =
	size_t arithmetic_tmpvar;
	if (!handle_binary_operation_arithmetic(
		expression.get_source_ref(),
		Operation::OP_ADD,
		arithmetic_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
	if (!handle_binary_operation_assignment(
		expression.get_source_ref(),
		Operation::OP_ASSIGN,
		returned_tmpvar,
		a_tmpvar,
		arithmetic_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::SUB_ASSIGNMENT) {
	// This is just syntax sugar for - followed by =
	size_t arithmetic_tmpvar;
	if (!handle_binary_operation_arithmetic(
		expression.get_source_ref(),
		Operation::OP_SUBTRACT,
		arithmetic_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
	if (!handle_binary_operation_assignment(
		expression.get_source_ref(),
		Operation::OP_ASSIGN,
		returned_tmpvar,
		a_tmpvar,
		arithmetic_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::LEFT_ASSIGNMENT) {
	// This is just syntax sugar for << followed by =
	size_t arithmetic_tmpvar;
	if (!handle_binary_operation_shift(
		expression.get_source_ref(),
		Operation::OP_SHIFT_LEFT,
		arithmetic_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
	if (!handle_binary_operation_assignment(
		expression.get_source_ref(),
		Operation::OP_ASSIGN,
		returned_tmpvar,
		a_tmpvar,
		arithmetic_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::RIGHT_ASSIGNMENT) {
	// This is just syntax sugar for >> followed by =
	size_t arithmetic_tmpvar;
	if (!handle_binary_operation_shift(
		expression.get_source_ref(),
		Operation::OP_SHIFT_RIGHT,
		arithmetic_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
	if (!handle_binary_operation_assignment(
		expression.get_source_ref(),
		Operation::OP_ASSIGN,
		returned_tmpvar,
		a_tmpvar,
		arithmetic_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::AND_ASSIGNMENT) {
	// This is just syntax sugar for & followed by =
	size_t arithmetic_tmpvar;
	if (!handle_binary_operation_bitwise(
		expression.get_source_ref(),
		Operation::OP_BITWISE_AND,
		arithmetic_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
	if (!handle_binary_operation_assignment(
		expression.get_source_ref(),
		Operation::OP_ASSIGN,
		returned_tmpvar,
		a_tmpvar,
		arithmetic_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::OR_ASSIGNMENT) {
	// This is just syntax sugar for | followed by =
	size_t arithmetic_tmpvar;
	if (!handle_binary_operation_bitwise(
		expression.get_source_ref(),
		Operation::OP_BITWISE_OR,
		arithmetic_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
	if (!handle_binary_operation_assignment(
		expression.get_source_ref(),
		Operation::OP_ASSIGN,
		returned_tmpvar,
		a_tmpvar,
		arithmetic_tmpvar
		)) {
	    return false;
	}
    }
    else if (op_type == ExpressionBinary::XOR_ASSIGNMENT) {
	// This is just syntax sugar for ^ followed by =
	size_t arithmetic_tmpvar;
	if (!handle_binary_operation_bitwise(
		expression.get_source_ref(),
		Operation::OP_BITWISE_XOR,
		arithmetic_tmpvar,
		a_tmpvar,
		b_tmpvar
		)) {
	    return false;
	}
	if (!handle_binary_operation_assignment(
		expression.get_source_ref(),
		Operation::OP_ASSIGN,
		returned_tmpvar,
		a_tmpvar,
		arithmetic_tmpvar
		)) {
	    return false;
	}
    }
    else {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		expression.get_source_ref(),
		"Compiler bug! unknown binary operator",
		std::string("Invalid binary operator type ") + std::to_string(op_type)
		);
	return false;
    }
    return true;
}
bool
FunctionDefinitionResolver::extract_from_expression_trinary(
    size_t & returned_tmpvar,
    const ExpressionTrinary & expression)
{
    fprintf(stderr, "TODO: Trinary expressions are not yet supported.\n");
    return false;
}
bool
FunctionDefinitionResolver::extract_from_expression_cast(
    size_t & returned_tmpvar,
    const ExpressionCast & expression)
{
    fprintf(stderr, "TODO: Cast expressions are not yet supported.\n");
    return false;
}


bool
FunctionDefinitionResolver::extract_from_expression(
    size_t & returned_tmpvar,
    const Expression & expression_container)
{
  const auto & expression_type = expression_container.get_expression();

  if (std::holds_alternative<Gyoji::owned<ExpressionPrimaryIdentifier>>(expression_type)) {
    const auto & expression = std::get<Gyoji::owned<ExpressionPrimaryIdentifier>>(expression_type);
    return extract_from_expression_primary_identifier(returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<Gyoji::owned<ExpressionPrimaryNested>>(expression_type)) {
    const auto & expression = std::get<Gyoji::owned<ExpressionPrimaryNested>>(expression_type);
    return extract_from_expression_primary_nested(returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<Gyoji::owned<ExpressionPrimaryLiteralChar>>(expression_type)) {
    const auto & expression = std::get<Gyoji::owned<ExpressionPrimaryLiteralChar>>(expression_type);
    return extract_from_expression_primary_literal_char(returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<Gyoji::owned<ExpressionPrimaryLiteralString>>(expression_type)) {
    const auto & expression = std::get<Gyoji::owned<ExpressionPrimaryLiteralString>>(expression_type);
    return extract_from_expression_primary_literal_string(returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<Gyoji::owned<ExpressionPrimaryLiteralInt>>(expression_type)) {
    const auto & expression = std::get<Gyoji::owned<ExpressionPrimaryLiteralInt>>(expression_type);
    return extract_from_expression_primary_literal_int(returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<Gyoji::owned<ExpressionPrimaryLiteralFloat>>(expression_type)) {
    const auto & expression = std::get<Gyoji::owned<ExpressionPrimaryLiteralFloat>>(expression_type);
    return extract_from_expression_primary_literal_float(returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<Gyoji::owned<ExpressionPostfixArrayIndex>>(expression_type)) {
    const auto & expression = std::get<Gyoji::owned<ExpressionPostfixArrayIndex>>(expression_type);
    return extract_from_expression_postfix_array_index(returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<Gyoji::owned<ExpressionPostfixFunctionCall>>(expression_type)) {
    const auto & expression = std::get<Gyoji::owned<ExpressionPostfixFunctionCall>>(expression_type);
    return extract_from_expression_postfix_function_call(returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<Gyoji::owned<ExpressionPostfixDot>>(expression_type)) {
    const auto & expression = std::get<Gyoji::owned<ExpressionPostfixDot>>(expression_type);
    return extract_from_expression_postfix_dot(returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<Gyoji::owned<ExpressionPostfixArrow>>(expression_type)) {
    const auto & expression = std::get<Gyoji::owned<ExpressionPostfixArrow>>(expression_type);
    return extract_from_expression_postfix_arrow(returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<Gyoji::owned<ExpressionPostfixIncDec>>(expression_type)) {
    const auto & expression = std::get<Gyoji::owned<ExpressionPostfixIncDec>>(expression_type);
    return extract_from_expression_postfix_incdec(returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<Gyoji::owned<ExpressionUnaryPrefix>>(expression_type)) {
    const auto & expression = std::get<Gyoji::owned<ExpressionUnaryPrefix>>(expression_type);
    return extract_from_expression_unary_prefix(returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<Gyoji::owned<ExpressionUnarySizeofType>>(expression_type)) {
    const auto & expression = std::get<Gyoji::owned<ExpressionUnarySizeofType>>(expression_type);
    return extract_from_expression_unary_sizeof_type(returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<Gyoji::owned<ExpressionBinary>>(expression_type)) {
    const auto & expression = std::get<Gyoji::owned<ExpressionBinary>>(expression_type);
    return extract_from_expression_binary(returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<Gyoji::owned<ExpressionTrinary>>(expression_type)) {
    const auto & expression = std::get<Gyoji::owned<ExpressionTrinary>>(expression_type);
    return extract_from_expression_trinary(returned_tmpvar, *expression);
  }
  else if (std::holds_alternative<Gyoji::owned<ExpressionCast>>(expression_type)) {
    const auto & expression = std::get<Gyoji::owned<ExpressionCast>>(expression_type);
    return extract_from_expression_cast(returned_tmpvar, *expression);
  }
  else {
    fprintf(stderr, "Compiler bug, invalid expression type\n");
    return false;
  }
}

bool
FunctionDefinitionResolver::local_declare_or_error(
    const Gyoji::mir::Type *mir_type,
    const std::string & name,
    const SourceReference & source_ref
    )
{
    scope_tracker.add_variable(name, mir_type, source_ref);
    
    auto operation = std::make_unique<OperationLocalDeclare>(
	source_ref,
	name,
	mir_type
	);
    function->get_basic_block(current_block).add_operation(std::move(operation));

    return true;
}

bool
FunctionDefinitionResolver::extract_from_statement_variable_declaration(
    const StatementVariableDeclaration & statement
    )
{
    // TODO:
    // For arrays, it is at this point that we define a type
    // for the 'array' so we can make it the correct size based on the literal given
    // for the 'opt_array' stuff.
    
    // Once the variable exists, we can start performing the initialization
    // and assigning the value to something.
    const Gyoji::mir::Type * mir_type = type_resolver.extract_from_type_specifier(statement.get_type_specifier());
    
    if (!local_declare_or_error(
	    mir_type,
	    statement.get_identifier().get_name(),
	    statement.get_identifier().get_source_ref()
	    )) {
	return false;
    }

    if (statement.is_constructor()) {
	// Here, we should call the constructor
	// since it is the one that forces initialization
	// of all class members (if applicable).
	// First, check that this is a class type.  If it isn't, then
	// we it must be a single initializer for a primitive.
	if (!mir_type->is_composite()) {
	    compiler_context
		.get_errors()
		.add_simple_error(
		    statement.get_argument_expression_list().get_source_ref(),
		    "Constructors are not supported for non-class types",
		    std::string("Constructors must be called on class types and not primitive types like ") + mir_type->get_name()
		    );
	    return false;
	}

	// This lowers to a single MIR operation
	// so that we can easily tell from the MIR
	// whether this variable is initialized before use.
	// Get the variable.
	size_t variable_tmpvar = function->tmpvar_define(mir_type);
	auto op_variable = std::make_unique<OperationLocalVariable>(
	    statement.get_source_ref(),
	    variable_tmpvar,
	    statement.get_identifier().get_name(),
	    mir_type
	    );
	function->get_basic_block(current_block).add_operation(std::move(op_variable));

	// Extract the 'this' pointer
	// from the variable so we can add it to the constructor
	// args.
	const Type *variable_pointer_type = mir.get_types().get_pointer_to(mir_type, statement.get_source_ref());
	size_t variable_pointer_tmpvar = function->tmpvar_define(variable_pointer_type);
	auto operation_this_pointer = std::make_unique<OperationUnary>(
	    Operation::OP_ADDRESSOF,
	    statement.get_source_ref(),
	    variable_pointer_tmpvar,
	    variable_tmpvar
	    );
	function
	    ->get_basic_block(current_block)
	    .add_operation(std::move(operation_this_pointer));

	std::vector<size_t> passed_arguments;
	std::vector<const Gyoji::context::SourceReference *> passed_src_refs;

	// First argument is the <this> pointer:
	passed_arguments.push_back(variable_pointer_tmpvar);
	passed_src_refs.push_back(&statement.get_source_ref());
	
	for (const auto & arg_expr : statement.get_argument_expression_list().get_arguments()) {
	    size_t arg_returned_value;
	    if (!extract_from_expression(arg_returned_value, *arg_expr)) {
		return false;
	    }
	    passed_arguments.push_back(arg_returned_value);
	    passed_src_refs.push_back(&arg_expr->get_source_ref());
	}

	std::string fully_qualified_function_name("jlang::Foo::Foo");
	const Gyoji::mir::Symbol *constructor_symbol = mir.get_symbols().get_symbol(fully_qualified_function_name);
	if (constructor_symbol == nullptr) {
	    std::unique_ptr<Gyoji::context::Error> error = std::make_unique<Gyoji::context::Error>("No constructor found.");
	    error->add_message(
		function_definition.get_source_ref(),
		std::string("Constructor ") + fully_qualified_function_name + std::string(" was not defined for class ") + mir_type->get_name()
		);
	    compiler_context
		.get_errors()
		.add_error(std::move(error));
	    return false;
	}
	const Type *constructor_fptr_type = constructor_symbol->get_type();
	if (constructor_fptr_type->get_type() != Type::TYPE_FUNCTION_POINTER) {
	    std::unique_ptr<Gyoji::context::Error> error = std::make_unique<Gyoji::context::Error>("Symbol is not a constructor");
	    error->add_message(
		function_definition.get_source_ref(),
		std::string("Symbol ") + fully_qualified_function_name + std::string(" is not declared as a constructor.")
		);
	    compiler_context
		.get_errors()
		.add_error(std::move(error));
	    return false;
	}
	// Check that the method signature we're calling matches
	// the declaration of that method
	if (!check_function_call_signature(true, passed_arguments, passed_src_refs, constructor_fptr_type, statement.get_argument_expression_list().get_source_ref())) {
	    return false;
	}
	
	size_t constructor_fptr_tmpvar = function->tmpvar_define(constructor_fptr_type);
	auto operation_get_constructor_function = std::make_unique<OperationSymbol>(
	    statement.get_argument_expression_list().get_source_ref(),
	    constructor_fptr_tmpvar,
	    fully_qualified_function_name
	    );
	function->get_basic_block(current_block).add_operation(std::move(operation_get_constructor_function));
	
	// This is what the function pointer type should return.
	size_t constructor_result_tmpvar = function->tmpvar_define(constructor_fptr_type->get_return_type());
	
	auto op_constructor = std::make_unique<OperationFunctionCall>(
            Operation::OP_CONSTRUCTOR,
	    statement.get_argument_expression_list().get_source_ref(),
	    constructor_result_tmpvar,
	    constructor_fptr_tmpvar,
	    passed_arguments
	    );
	function->get_basic_block(current_block).add_operation(std::move(op_constructor));
    }
    else {
	// If this is a class type, throw an error because this
	// would be not the correct way to initialize it.
	
	const auto & initializer_expression = statement.get_initializer_expression();
	if (initializer_expression.has_expression()) {
	    // From here, we need to:
	    // 1) call LocalVariable
	    // 2) Evaluate the expression
	    // 3) Perform an assignment.
	    size_t variable_tmpvar = function->tmpvar_define(mir_type);
	    auto operation = std::make_unique<OperationLocalVariable>(
		statement.get_source_ref(),
		variable_tmpvar,
		statement.get_identifier().get_name(),
		mir_type
		);
	    function->get_basic_block(current_block).add_operation(std::move(operation));
	    
	    size_t initial_value_tmpvar;
	    if (!extract_from_expression(initial_value_tmpvar, initializer_expression.get_expression())) {
		return false;
	    }
	    
	    size_t returned_tmpvar; // We don't save the returned val because nobody wants it.
	    if (!handle_binary_operation_assignment(
		    initializer_expression.get_source_ref(),
		    Operation::OP_ASSIGN,
		    returned_tmpvar,
		    variable_tmpvar,
		    initial_value_tmpvar
		    )) {
		return false;
	    }
	}
    }

    
    return true;
}

bool
FunctionDefinitionResolver::extract_from_statement_ifelse(
    const StatementIfElse & statement
    )
{
    size_t condition_tmpvar;
    if (!extract_from_expression(condition_tmpvar, statement.get_expression())) {
	return false;
    }
    // First, evaluate the expression to get our condition.
    if (!function->tmpvar_get(condition_tmpvar)->is_bool()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(
		statement.get_expression().get_source_ref(),
		"Invalid condition in if statement.",
		std::string("Type of condition expression should be 'bool' and was ") + function->tmpvar_get(condition_tmpvar)->get_name()
		);
	return false;
    }
    
    size_t blockid_if = function->add_block();
    size_t blockid_done = function->add_block();
    size_t blockid_else = -1;
    if (statement.has_else() || statement.has_else_if()) {
	// If we have an 'else', then
	// dump to it based on the condition.
	blockid_else = function->add_block();
	auto operation = std::make_unique<OperationJumpConditional>(
	    statement.get_source_ref(),
	    condition_tmpvar,
	    blockid_if,
	    blockid_else
	    );
	function->get_basic_block(current_block).add_operation(std::move(operation));
    }
    else {
	// Otherwise, jump to done
	// based on condition.
	auto operation = std::make_unique<OperationJumpConditional>(
	    statement.get_source_ref(),
	    condition_tmpvar,
	    blockid_if,
	    blockid_done
	    );
	function->get_basic_block(current_block).add_operation(std::move(operation));
    }
    
    current_block = blockid_if;

    // Perform the stuff inside the 'if' block.
    // If blocks cannot inherently be defined as unsafe,
    // so if you need that, put the if statement inside
    // an unsafe block.
    scope_tracker.scope_push(false, statement.get_if_scope_body().get_source_ref());
    if (!extract_from_statement_list(
	    true,
	    statement.get_if_scope_body().get_statements()
	    )) {
	return false;
    }
    scope_tracker.scope_pop();
    
    // Unconditionally jump to done
    // unless the block is alreayd terminated by
    // another jump or a return.
    if (!function->get_basic_block(current_block).contains_terminator()) {
	auto operation = std::make_unique<OperationJump>(
	    statement.get_source_ref(),
	    blockid_done
	    );
	function->get_basic_block(current_block).add_operation(std::move(operation));
    }
    
    if (statement.has_else()) {
	// Perform the stuff in the 'else' block.
	// If/Else blocks cannot inherently be defined as unsafe,
	// so if you need that, put the if statement inside
	// an unsafe block.
	scope_tracker.scope_push(false, statement.get_else_scope_body().get_source_ref());
	size_t blockid_tmp = current_block;
	current_block = blockid_else;
	if (!extract_from_statement_list(
		true,
		statement.get_else_scope_body().get_statements()
		)) {
	    return false;
	}
	current_block = blockid_tmp;
	scope_tracker.scope_pop();
	// Jump to the 'done' block when the 'else' block is finished
	// unless it has terminated already.
	if (!function->get_basic_block(blockid_else).contains_terminator()) {
	    auto operation = std::make_unique<OperationJump>(
		statement.get_source_ref(),
		blockid_done
		);
	    function->get_basic_block(blockid_else).add_operation(std::move(operation));
	}
    }
    else if (statement.has_else_if()) {
	if (!extract_from_statement_ifelse(
	    statement.get_else_if()
		)) {
	    return false;
	}
    }
    current_block = blockid_done;
    return true;
}

bool
FunctionDefinitionResolver::extract_from_statement_while(
    const Gyoji::frontend::tree::StatementWhile & statement
    )
{
    size_t condition_tmpvar;

    size_t blockid_evaluate_expression = function->add_block();
    size_t blockid_if = function->add_block();
    size_t blockid_done = function->add_block();

    auto operation_jump_initial = std::make_unique<OperationJump>(
	statement.get_source_ref(),
	blockid_evaluate_expression
	);
    function->get_basic_block(current_block).add_operation(std::move(operation_jump_initial));

    current_block = blockid_evaluate_expression;
    
    if (!extract_from_expression(condition_tmpvar, statement.get_expression())) {
	return false;
    }

    auto operation_jump_conditional = std::make_unique<OperationJumpConditional>(
	statement.get_source_ref(),
	condition_tmpvar,
	blockid_if,
	blockid_done
	);
    function->get_basic_block(current_block).add_operation(std::move(operation_jump_conditional));

    // Push a loop scope.
    current_block = blockid_if;
    scope_tracker.scope_push_loop(
	statement.get_scope_body().get_source_ref(),
	blockid_done,
	blockid_evaluate_expression
	);
    if (!extract_from_statement_list(
	    true,
	    statement.get_scope_body().get_statements()
	    )) {
	return false;
    }
    // Pop back from the scope.
    scope_tracker.scope_pop();
    
    auto operation_jump_to_evaluate = std::make_unique<OperationJump>(
	statement.get_source_ref(),
	blockid_evaluate_expression
	);
    function->get_basic_block(current_block).add_operation(std::move(operation_jump_to_evaluate));
    
    current_block = blockid_done;
    
    return true;
}

bool
FunctionDefinitionResolver::extract_from_statement_for(
    const Gyoji::frontend::tree::StatementFor & statement
    )
{
    size_t condition_tmpvar;

    size_t blockid_evaluate_expression_termination = function->add_block();
    size_t blockid_if = function->add_block();
    size_t blockid_done = function->add_block();

    if (statement.is_declaration()) {
	const Gyoji::mir::Type * mir_type = type_resolver.extract_from_type_specifier(statement.get_type_specifier());
	
	if (!local_declare_or_error(
		mir_type,
		statement.get_identifier().get_name(),
		statement.get_identifier().get_source_ref()
		)) {
	    return false;
	}
    }
    
    // Evaluate the initialization expression
    if (!extract_from_expression(condition_tmpvar, statement.get_expression_initial())) {
	return false;
    }
    
    auto operation_jump_initial = std::make_unique<OperationJump>(
	statement.get_source_ref(),
	blockid_evaluate_expression_termination
	);
    function->get_basic_block(current_block).add_operation(std::move(operation_jump_initial));

    // Evaluate the termination condition.
    size_t blockid_tmp = current_block;
    current_block = blockid_evaluate_expression_termination;
    if (!extract_from_expression(condition_tmpvar, statement.get_expression_termination())) {
	return false;
    }
    current_block = blockid_tmp;

    auto operation_jump_conditional = std::make_unique<OperationJumpConditional>(
	statement.get_source_ref(),
	condition_tmpvar,
	blockid_if,
	blockid_done
	);
    function->get_basic_block(blockid_evaluate_expression_termination).add_operation(std::move(operation_jump_conditional));

    scope_tracker.scope_push_loop(
	statement.get_scope_body().get_source_ref(),
	blockid_done,
	blockid_evaluate_expression_termination
	);
    
    size_t blockid_tmp_if = current_block;
    current_block = blockid_if;
    if (!extract_from_statement_list(
	    true,
	    statement.get_scope_body().get_statements()
	    )) {
	return false;
    }
    
    scope_tracker.scope_pop();

    // Evaluate the 'increment' expression
    if (!extract_from_expression(condition_tmpvar, statement.get_expression_increment())) {
	return false;
    }
    current_block = blockid_tmp_if;
    
    auto operation_jump_to_evaluate = std::make_unique<OperationJump>(
	statement.get_source_ref(),
	blockid_evaluate_expression_termination
	);
    function->get_basic_block(blockid_if).add_operation(std::move(operation_jump_to_evaluate));
    
    current_block = blockid_done;

    // Un-declare the variable we declared.
//    leave_scope(function, current_block, statement.get_source_ref(), unwind);
    
    return true;
}

bool	
FunctionDefinitionResolver::extract_from_statement_switch(
    const Gyoji::frontend::tree::StatementSwitch & statement
    )
{

    // Extract the value of the expression
    // we will be testing.
    size_t switch_value_tmpvar;
    if (!extract_from_expression(switch_value_tmpvar, statement.get_expression())) {
	return false;
    }
    const Type *switch_value_type = function->tmpvar_get(switch_value_tmpvar);

    size_t blockid_done = function->add_block();
	
    bool is_ok = true;
    const StatementSwitchContent & switch_content = statement.get_switch_content();

    size_t blockid_else;

    const auto & blocks = switch_content.get_blocks();
    size_t nblocks = blocks.size();
    size_t i = 0;

    bool has_default = false;
    for (const auto & block_ptr : blocks) {
	if (block_ptr->is_default()) {
	    if (i != nblocks-1) {
		compiler_context
		    .get_errors()
		    .add_simple_error(
			block_ptr->get_source_ref(),
			"Default clause must be the last clause in a switch statement.",
			std::string("Default clause must be the last clause in a switch statement.")
			);
		return false;
	    }
	    has_default = true;
	    blockid_else = blockid_done;
	}
	else {
	    size_t test_value_tmpvar;
	    if (!extract_from_expression(test_value_tmpvar, block_ptr->get_expression())) {
		return false;
	    }
	    const Type *test_value_type = function->tmpvar_get(test_value_tmpvar);
	    if (test_value_type->get_name() != switch_value_type->get_name()) {
		std::unique_ptr<Gyoji::context::Error> error = std::make_unique<Gyoji::context::Error>("Case must match switch type");
		error->add_message(
		    block_ptr->get_source_ref(),
		    std::string("Case type ") + test_value_type->get_name() + std::string(" must match switch type ") + switch_value_type->get_name()
		    );
		error->add_message(
		    statement.get_expression().get_source_ref(),
		    "Switch declared here."
		    );
		compiler_context
		    .get_errors()
		    .add_error(std::move(error));
		is_ok = false;
	    }
	    size_t condition_tmpvar = function->tmpvar_define(mir.get_types().get_type("bool"));
	    auto operation_compare_equal = std::make_unique<OperationBinary>(
		Operation::OP_COMPARE_EQUAL,
		block_ptr->get_source_ref(),
		condition_tmpvar,
		test_value_tmpvar,
		switch_value_tmpvar
		);
	    function
		->get_basic_block(current_block)
		.add_operation(std::move(operation_compare_equal));
	    
	    size_t blockid_if = function->add_block();
	    blockid_else = function->add_block();
	    auto operation_jump_if = std::make_unique<OperationJumpConditional>(
		block_ptr->get_source_ref(),
		condition_tmpvar,
		blockid_if,
		blockid_else
		);
	    function
		->get_basic_block(current_block)
		.add_operation(std::move(operation_jump_if));
	    
	    current_block = blockid_if;
	}

	// Switch blocks are not inherently unsafe.
	// If you need that, you need to declare the unsafe block
	// either inside the block or outside the switch.
	scope_tracker.scope_push(false, block_ptr->get_scope_body().get_source_ref());
	if (!extract_from_statement_list(true, block_ptr->get_scope_body().get_statements())) {
	    return false;
	}
	scope_tracker.scope_pop();

	// If we have returned from the block,
	// then it's safe to leave off the jump back
	// to outside the switch.
	if (!function->get_basic_block(current_block).contains_terminator()) {
	    auto operation_jump_to_done = std::make_unique<OperationJump>(
		block_ptr->get_source_ref(),
		blockid_done
		);
	    function->get_basic_block(current_block).add_operation(std::move(operation_jump_to_done));
	}
	current_block = blockid_else;

	i++;
    }

    // If there is no default clause, we add an 'empty' default
    // clause that does nothing.
    current_block = blockid_else;
    if (!has_default) {
	// This just handles the end case where there are no more
	// conditions, so we unconditionaly jump back to done.
	auto operation_jump_to_done = std::make_unique<OperationJump>(
	    statement.get_source_ref(),
	    blockid_done
	    );
	function->get_basic_block(current_block).add_operation(std::move(operation_jump_to_done));
	current_block = blockid_done;
    }
    
    return is_ok;
}


bool
FunctionDefinitionResolver::extract_from_statement_break(
    const Gyoji::frontend::tree::StatementBreak & statement
    )
{
    if (!scope_tracker.is_in_loop()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(statement.get_source_ref(),
			      "'break' statement not in loop or switch statement",
			      "'break' keyword must appear inside a loop (for/while)"
		);
	return true;
    }

    std::vector<std::string> unwind_break = scope_tracker.get_variables_to_unwind_for_break();
    leave_scope(statement.get_source_ref(), unwind_break);
    
    auto operation_jump_to_break = std::make_unique<OperationJump>(
	statement.get_source_ref(),
	scope_tracker.get_loop_break_blockid()
	);
    function->get_basic_block(current_block).add_operation(std::move(operation_jump_to_break));
    current_block = function->add_block();
    
    return true;
}

bool
FunctionDefinitionResolver::extract_from_statement_continue(
    const Gyoji::frontend::tree::StatementContinue & statement
    )
{
    if (!scope_tracker.is_in_loop()) {
	compiler_context
	    .get_errors()
	    .add_simple_error(statement.get_source_ref(),
			      "'continue' statement not in loop or switch statement",
			      "'continue' keyword must appear inside a loop (for/while)"
		);
	return true;
    }
    auto operation_jump_to_continue = std::make_unique<OperationJump>(
	statement.get_source_ref(),
	scope_tracker.get_loop_continue_blockid()
	);
    function->get_basic_block(current_block).add_operation(std::move(operation_jump_to_continue));
    current_block = function->add_block();
    
    return true;
}


bool
FunctionDefinitionResolver::extract_from_statement_label(
    const Gyoji::frontend::tree::StatementLabel & statement
    )
{
    // We're starting a new label, so this is, by definition,
    // a new basic block.  This means we need to create a new
    // block and issue a 'jump' to it.

    const std::string & label_name = statement.get_name();
    size_t label_block;
    
    const FunctionLabel *label = scope_tracker.get_label(label_name);
    if (label == nullptr) {
	label_block = function->add_block();
	scope_tracker.label_define(label_name, label_block, statement.get_name_source_ref());
    }
    else {
	if (!label->is_resolved()) {
	    scope_tracker.label_define(label_name, statement.get_name_source_ref());
	    label_block = label->get_block();
	}
	else {
	    std::unique_ptr<Gyoji::context::Error> error = std::make_unique<Gyoji::context::Error>("Labels in functions must be unique");
	    error->add_message(statement.get_name_source_ref(),
			       std::string("Duplicate label ") + label_name);
	    error->add_message(label->get_source_ref(),
			       "First declared here.");
	    compiler_context
		.get_errors()
		.add_error(std::move(error));
	    return true;
	}
    }
    auto operation = std::make_unique<OperationJump>(
	statement.get_source_ref(),
	label_block
	);
    function->get_basic_block(current_block).add_operation(std::move(operation));
    // Then whatever we add next will be in this new block.
    current_block = label_block;
    
    return true;
}

bool
FunctionDefinitionResolver::extract_from_statement_goto(
    const Gyoji::frontend::tree::StatementGoto & statement
    )
{
    const std::string & label_name = statement.get_label();

    const FunctionLabel *label = scope_tracker.get_label(label_name);
    size_t label_block;
    if (label == nullptr) {
	label_block = function->add_block();
	scope_tracker.label_declare(label_name, label_block);
    }
    else {
	label_block = label->get_block();
    }

    // We need to track what point
    // in the MIR the goto appears so that
    // we can insert the unwindings before that point.
    Gyoji::owned<FunctionPoint> function_point = std::make_unique<FunctionPoint>(current_block, function->get_basic_block(current_block).size());
    scope_tracker.add_goto(label_name, std::move(function_point), statement.get_label_source_ref());
    
    auto operation = std::make_unique<OperationJump>(
	statement.get_source_ref(),
	label_block
	);
    function->get_basic_block(current_block).add_operation(std::move(operation));
    // This jump ends the basic block, so we start a new one.
    size_t next_block = function->add_block();
    current_block = next_block;

    return true;
}
	
bool
FunctionDefinitionResolver::extract_from_statement_return(
    const StatementReturn & statement
    )
{
    std::vector<std::string> unwind_root = scope_tracker.get_variables_to_unwind_for_root();

    if (statement.is_void()) {
	leave_scope(statement.get_source_ref(), unwind_root);
	auto operation = std::make_unique<OperationReturnVoid>(
	    statement.get_source_ref()
	    );
	function->get_basic_block(current_block).add_operation(std::move(operation));
    }
    else {
	size_t expression_tmpvar;
	if (!extract_from_expression(expression_tmpvar, statement.get_expression())) {
	    return false;
	}
	leave_scope(statement.get_source_ref(), unwind_root);
	auto operation = std::make_unique<OperationReturn>(
	    statement.get_source_ref(),
	    expression_tmpvar
	    );
	function->get_basic_block(current_block).add_operation(std::move(operation));
    }
    return true;
}


void
FunctionDefinitionResolver::leave_scope(
    const SourceReference & src_ref,
    std::vector<std::string> & unwind)
{
    
    for (const auto & undecl : unwind) {
	auto operation = std::make_unique<OperationLocalUndeclare>(
	    src_ref,
	    undecl);
	function->get_basic_block(current_block).add_operation(std::move(operation));
    }
    unwind.clear();
}
	    
bool
FunctionDefinitionResolver::extract_from_statement_list(
    bool automatic_unwind,
    const StatementList & statement_list)
{

    bool did_return = false;
    for (const auto & statement_el : statement_list.get_statements()) {
	const auto & statement_type = statement_el->get_statement();
	if (std::holds_alternative<Gyoji::owned<StatementVariableDeclaration>>(statement_type)) {
	    const auto & statement = std::get<Gyoji::owned<StatementVariableDeclaration>>(statement_type);
	    if (!extract_from_statement_variable_declaration(*statement)) {
		return false;
	    }
	}
	else if (std::holds_alternative<Gyoji::owned<StatementBlock>>(statement_type)) {
	    const auto & statement = std::get<Gyoji::owned<StatementBlock>>(statement_type);
	    scope_tracker.scope_push(
		statement->get_unsafe_modifier().is_unsafe(),
		statement->get_scope_body().get_source_ref()
		);
	    if (!extract_from_statement_list(true, statement->get_scope_body().get_statements())) {
		return false;
	    }
	    scope_tracker.scope_pop();
	}
	else if (std::holds_alternative<Gyoji::owned<StatementExpression>>(statement_type)) {
	    const auto & statement = std::get<Gyoji::owned<StatementExpression>>(statement_type);
	    size_t returned_tmpvar;
	    if (!extract_from_expression(
		    returned_tmpvar,
		    statement->get_expression())) {
		return false;
	    }
	}
	else if (std::holds_alternative<Gyoji::owned<StatementIfElse>>(statement_type)) {
	    const auto & statement = std::get<Gyoji::owned<StatementIfElse>>(statement_type);
	    if (!extract_from_statement_ifelse(
		    *statement)) {
		return false;
	    }
	}
	else if (std::holds_alternative<Gyoji::owned<StatementWhile>>(statement_type)) {
	    const auto & statement = std::get<Gyoji::owned<StatementWhile>>(statement_type);
	    if (!extract_from_statement_while(
		    *statement)) {
		return false;
	    }
	}
	else if (std::holds_alternative<Gyoji::owned<StatementFor>>(statement_type)) {
	    const auto & statement = std::get<Gyoji::owned<StatementFor>>(statement_type);
	    if (!extract_from_statement_for(
		    *statement)) {
		return false;
	    }
	}
	else if (std::holds_alternative<Gyoji::owned<StatementSwitch>>(statement_type)) {
	    const auto & statement = std::get<Gyoji::owned<StatementSwitch>>(statement_type);
	    if (!extract_from_statement_switch(
		    *statement)) {
		return false;
	    }
	}
	else if (std::holds_alternative<Gyoji::owned<StatementLabel>>(statement_type)) {
	    const auto & statement = std::get<Gyoji::owned<StatementLabel>>(statement_type);
	    if (!extract_from_statement_label(*statement)) {
		return false;
	    }
	}
	else if (std::holds_alternative<Gyoji::owned<StatementGoto>>(statement_type)) {
	    const auto & statement = std::get<Gyoji::owned<StatementGoto>>(statement_type);
	    if (!extract_from_statement_goto(*statement)) {
		return false;
	    }
	}
	else if (std::holds_alternative<Gyoji::owned<StatementBreak>>(statement_type)) {
	    const auto & statement = std::get<Gyoji::owned<StatementBreak>>(statement_type);
	    if (!extract_from_statement_break(
		    *statement)) {
		return false;
	    }
	}
	else if (std::holds_alternative<Gyoji::owned<StatementContinue>>(statement_type)) {
	    const auto & statement = std::get<Gyoji::owned<StatementContinue>>(statement_type);
	    if (!extract_from_statement_continue(
		    *statement)) {
		return false;
	    }
	}
	else if (std::holds_alternative<Gyoji::owned<StatementReturn>>(statement_type)) {
	    const auto & statement = std::get<Gyoji::owned<StatementReturn>>(statement_type);
	    // The return may need to unwind local declarations
	    // and ensure destructors are called.
	    if (!extract_from_statement_return(*statement)) {
		return false;
	    }
	    did_return = true;
	}
	else {
	    fprintf(stderr, "Compiler bug, invalid statement type\n");
	    return false;
	}
    }
    // If we did not do a return, we should
    // unwind the current scope.
    // If we did a return already, we will already
    // have called these, so we should skip it.
    // Note that in the outer-most scope, we don't
    // do this because we'll add the return and
    // the scope unwinding based on the end of
    // the function (if it is reachable)
    if (!did_return && automatic_unwind) {
	std::vector<std::string> unwind_scope = scope_tracker.get_variables_to_unwind_for_scope();
	leave_scope(statement_list.get_source_ref(), unwind_scope);
    }

    return true;
}

