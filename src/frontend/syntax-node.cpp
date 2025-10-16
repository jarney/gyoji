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
#include <gyoji-frontend.hpp>

using namespace Gyoji::context;
using namespace Gyoji::frontend;
using namespace Gyoji::frontend::ast;

///////////////////////////////////////////////////
SyntaxNode::SyntaxNode(
    TokenID _type,
    SyntaxNode::specific_type_t _data,
    const SourceReference & _source_ref
    )
    : source_ref(_source_ref)
    , type(_type)
    , data(_data)
{}
SyntaxNode::~SyntaxNode()
{}
void
SyntaxNode::add_child(const SyntaxNode & node)
{
    children.push_back(node);
}
void
SyntaxNode::prepend_child(const SyntaxNode & node)
{
    children.insert(children.begin(), node);
}
const std::vector<std::reference_wrapper<const SyntaxNode>> &
SyntaxNode::get_children() const
{
    return children;
}
const TokenID & SyntaxNode::get_type() const
{
    return type;
}
const SyntaxNode &
SyntaxNode::get_syntax_node() const
{ return *this; }

const SourceReference &
SyntaxNode::get_source_ref() const
{ return source_ref; }
