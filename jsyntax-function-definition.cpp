#include "jsyntax.hpp"

JSyntaxFunctionDefinition::JSyntaxFunctionDefinition(
      std::string _function_name,
      std::shared_ptr<JSyntaxExpression> _return_type,
      std::shared_ptr<JSyntaxFunctionDefinitionArgList> _argument_types,
      std::shared_ptr<JSyntaxStatementList> _body
)
  : function_name(_function_name)
  , return_type(_return_type)
  , argument_types(_argument_types)
  , body(_body)
{}

JSyntaxFunctionDefinition::~JSyntaxFunctionDefinition()
{}

JSyntaxFunctionDefinitionArg::JSyntaxFunctionDefinitionArg(std::shared_ptr<JSyntaxExpression> _type_expr, std::string _arg_name)
  : type_expr(_type_expr)
  , arg_name(_arg_name)
{}

JSyntaxFunctionDefinitionArg::~JSyntaxFunctionDefinitionArg()
{}

JSyntaxFunctionDefinitionArgList::JSyntaxFunctionDefinitionArgList()
{}

JSyntaxFunctionDefinitionArgList::~JSyntaxFunctionDefinitionArgList()
{}
