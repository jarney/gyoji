#pragma once

class JSyntaxFunctionDefinitionArgList;
class JSyntaxStatementList;

class JSyntaxFunctionDefinition {
public:
  typedef std::shared_ptr<JSyntaxFunctionDefinition> ptr;
  JSyntaxFunctionDefinition(
      std::string _function_name,
      std::shared_ptr<JSyntaxExpression> _return_type,
      std::shared_ptr<JSyntaxFunctionDefinitionArgList> _argument_types,
      std::shared_ptr<JSyntaxStatementList> _body
  );
  ~JSyntaxFunctionDefinition();
  std::string function_name;
  std::shared_ptr<JSyntaxExpression> return_type;
  std::shared_ptr<JSyntaxFunctionDefinitionArgList> argument_types;
  std::shared_ptr<JSyntaxStatementList> body;
};

class JSyntaxFunctionDefinitionArg;

class JSyntaxFunctionDefinitionArgList {
public:
  typedef std::shared_ptr<JSyntaxFunctionDefinitionArgList> ptr;
  JSyntaxFunctionDefinitionArgList();
  ~JSyntaxFunctionDefinitionArgList();
  std::list<std::shared_ptr<JSyntaxFunctionDefinitionArg>> list;
};

class JSyntaxFunctionDefinitionArg {
public:
  typedef std::shared_ptr<JSyntaxFunctionDefinitionArg> ptr;
  JSyntaxFunctionDefinitionArg(std::shared_ptr<JSyntaxExpression> _type_expr, std::string _arg_name);
  ~JSyntaxFunctionDefinitionArg();
  std::shared_ptr<JSyntaxExpression> type_expr;
  std::string arg_name;
};

