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
#ifndef _GYOJI_INTERNAL
#error "This header is intended to be used internally as a part of the Gyoji front-end.  Please include frontend.hpp instead."
#endif
#pragma once

/*!
 *  \addtogroup Frontend
 *  @{
 */

/**
 * @brief Abstract syntax tree
 *
 * @details
 * Weakly-typed representation of the parse tree
 * resulting from reading and parsing an input file.
 * While the interface to the data is weakly typed,
 * the underlying data can be accessed through
 * a variant that exposes all of the different
 * types of production that the grammar can produce.
 */
namespace Gyoji::frontend::ast {
    
    /**
     * @brief Weakly-typed syntax node
     *
     * @details
     * The syntax tree can be viewed as a tree of nodes
     * that follow the structure of the parsed grammar.
     * 
     * This view of the syntax tree can be processed
     * in a weakly-typed sense where each node of
     * type SyntaxNode may have zero or more children
     * that represent grammar productions below them.
     *
     * Each of these nodes can also, optionally, be
     * examined to find the specific strongly-typed
     * tree node corresponding to the data parsed.
     *
     * For example, the top-level SyntaxNode will hold
     * a TranslationUnit so that it can be extracted:
     *
     * <pre>
     *     SyntaxNode *s;
     *     if (s->holds_alternative<TranslationUnit>()) {
     *         const TranslationUnit & tu = s->get<TranslationUnit>();
     *     }
     * </pre>
     *
     * The leaf nodes of the SyntaxTree will always be of
     * type "Terminal" indicating that it is a "terminal"
     * symbol of the grammer which corresponds to a parsed
     * token from the lexer (gyoji.l).
     */
    class SyntaxNode {
    public:
	typedef std::variant<GYOJI_SYNTAX_NODE_VARIANT_LIST> specific_type_t;
	
	/**
	 * Create a new syntax node of the given type holding
	 * the data associated with that production in the BNF
	 * grammar.
	 * @param _type Type of node this represents.
	 * @param _data The specific data associated with this node.
	 */
	SyntaxNode(Gyoji::context::TokenID _type, specific_type_t _data, const Gyoji::context::SourceReference & _source_ref);
	~SyntaxNode();
	
	
	/**
	 * This method returns a reference to an immutable array
	 * of children of this node.
	 */
	const std::vector<std::reference_wrapper<const SyntaxNode>> & get_children() const;
	
	/**
	 * This method returns an immutable reference to
	 * the type of the node.
	 */
	const Gyoji::context::TokenID & get_type() const;
	
	/**
	 * This interrogates the node to determine if it
	 * contains data of the specified type T.
	 * @tparam T Check type T to see if it is held in this node.
	 * @return True if this class contains that specific type of data.
	 */
	template <class T> bool has_data() const {
	    return std::holds_alternative<T*>(data);
	}
	
	/**
	 * This returns an immutable reference to the
	 * data of type T contained in this node.
	 * Note that this is only safe to use if the
	 * holds_alternative<T>() returns true for
	 * this specific type.
	 * @tparam T The type of data to return.
	 */
	template <class T> const T &get_data() const {
	    const T *d = std::get<T*>(data);
	    return *d;
	}
	
	/**
	 * This method is provided so that callers
	 * of derived classes can be sure to get access
	 * to the SyntaxNode base-class instance.
	 */
	const SyntaxNode & get_syntax_node() const;
	
	/**
	 * Returns the source reference for where
	 * this node appears in the source tree.
	 */
	const Gyoji::context::SourceReference & get_source_ref() const;
	
    private:
	// This list does NOT own its children, so
	// the class deriving from this one must
	// agree to own the pointers separately.
	std::vector<std::reference_wrapper<const SyntaxNode>> children; 
	const Gyoji::context::SourceReference & source_ref;
	
    protected:
	// Children are owned by their parents, so this is
	// private and can only be called by the
	// deriving class.
	void add_child(const SyntaxNode & node);
	void prepend_child(const SyntaxNode & node);
	
	Gyoji::context::TokenID type;
	specific_type_t data;
	
    };
    
};
/*! @} End of Doxygen Groups*/
