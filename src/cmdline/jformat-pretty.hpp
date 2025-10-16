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
#pragma once

#include <gyoji-backend/jbackend.hpp>

namespace Gyoji::backend {
    using namespace Gyoji::frontend::ast;
    /**
     * This is the identity transformation that should always
     * format the code exactly as it appeared in the input,
     * assuming that the input is a valid syntax tree.
     */
    class JBackendFormatPretty : public JBackend {
    public:
	JBackendFormatPretty();
	~JBackendFormatPretty();
	virtual int process(const SyntaxNode * file);
	
	std::string collect_comments(const SyntaxNode * node);
	std::string break_multiline_comment(std::string str);
	
	void print_whitespace(ASTDataNonSyntax::ptr node);
	void print_comments(std::vector<ASTDataNonSyntax::ptr> &non_syntax_list);
	void print_comment_single_line(ASTDataNonSyntax::ptr node);
	void print_comment_multi_line(ASTDataNonSyntax::ptr node);
	void print_file_metadata(ASTDataNonSyntax::ptr node);
	void print_non_syntax(ASTDataNonSyntax::ptr node);
	
	void print_node_generic(const SyntaxNode * node);
	void print_node_plain(const SyntaxNode * node);
	void print_node(const SyntaxNode * node);
	void print_scope_body(const SyntaxNode * node);
	void newline();
	
	void print_node_function_def(const SyntaxNode * node);
	
	int indent_level;
	int depth;
    };

};
