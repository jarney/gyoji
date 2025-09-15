//#ifndef _JLANG_INTERNAL
//#error "This header is intended to be used internally as a part of the JLang front-end.  Please include jsyntax.hpp instead."
//#endif
#pragma once

namespace JLang::frontend {

  class SyntaxNode {
    public:
      typedef std::variant<JLANG_SYNTAX_NODE_VARIANT_LIST> specific_type_t;
      
      SyntaxNode(std::string _type, specific_type_t _data);
      ~SyntaxNode();
      
      // Access to the child list should be
      // immutable because this is really
      // just a view over the list.
      const std::vector<std::reference_wrapper<const SyntaxNode>> & get_children() const;
      const std::string & get_type() const;
      //const SyntaxNode::specific_type_t &get_data() const;
      template <class T> bool has_data() const {
        return std::holds_alternative<T*>(data);
      }
      template <class T> const T &get_data() const {
        const T *d = std::get<T*>(data);
        return *d;
      }
      const SyntaxNode & get_syntax_node() const;
      
    private:
      // This list does NOT own its children, so
      // the class deriving from this one must
      // agree to own the pointers separately.
      std::vector<std::reference_wrapper<const SyntaxNode>> children; 
      
    protected:
      // Children are owned by their parents, so this is
      // private and can only be called by the
      // deriving class.
      void add_child(const SyntaxNode & node);
      void prepend_child(const SyntaxNode & node);
      
      std::string type;
      specific_type_t data;
    };

};
