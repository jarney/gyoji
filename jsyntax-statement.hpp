#pragma once

class JSyntaxStatement;

class JSyntaxStatementList {
public:
  typedef std::shared_ptr<JSyntaxStatementList> ptr;
  JSyntaxStatementList();
  ~JSyntaxStatementList();
  std::list<std::shared_ptr<JSyntaxStatement>> list;
};

enum JSyntaxStatementType {
  STMT_EXPRESSION,
  STMT_VARIABLE_DECLARATION,
  STMT_ASSIGNMENT,
  STMT_IFELSE,
  STMT_WHILE,
  STMT_RETURN
};

class JSyntaxStatementExpression;
class JSyntaxStatementVariableDeclaration;
class JSyntaxStatementAssignment;
class JSyntaxStatementIfElse;
class JSyntaxStatementWhile;
class JSyntaxStatementReturn;

class JSyntaxStatement {
public:
  typedef std::shared_ptr<JSyntaxStatement> ptr;
  JSyntaxStatement(std::shared_ptr<JSyntaxStatementExpression> stmt);
  JSyntaxStatement(std::shared_ptr<JSyntaxStatementVariableDeclaration> stmt);
  JSyntaxStatement(std::shared_ptr<JSyntaxStatementAssignment> stmt);
  JSyntaxStatement(std::shared_ptr<JSyntaxStatementIfElse> stmt);
  JSyntaxStatement(std::shared_ptr<JSyntaxStatementWhile> stmt);
  JSyntaxStatement(std::shared_ptr<JSyntaxStatementReturn> stmt);
  ~JSyntaxStatement();
  JSyntaxStatementType type;
  std::variant<
        std::shared_ptr<JSyntaxStatementExpression>,
        std::shared_ptr<JSyntaxStatementVariableDeclaration>,
        std::shared_ptr<JSyntaxStatementAssignment>,
        std::shared_ptr<JSyntaxStatementIfElse>,
        std::shared_ptr<JSyntaxStatementWhile>,
        std::shared_ptr<JSyntaxStatementReturn>
        > value;
};

class JSyntaxStatementExpression {
public:
  typedef std::shared_ptr<JSyntaxStatementExpression> ptr;
  JSyntaxStatementExpression(std::shared_ptr<JSyntaxExpression> _expr);
  ~JSyntaxStatementExpression();
  std::shared_ptr<JSyntaxExpression> expr;
};

class JSyntaxStatementVariableDeclaration {
public:
  typedef std::shared_ptr<JSyntaxStatementVariableDeclaration> ptr;
  JSyntaxStatementVariableDeclaration(
      std::shared_ptr<JSyntaxExpression> _type_expr,
      std::string _variable_name
  );
  ~JSyntaxStatementVariableDeclaration();
  std::shared_ptr<JSyntaxExpression> type_expr;
  std::string variable_name;
};

class JSyntaxStatementAssignment {
public:
  typedef std::shared_ptr<JSyntaxStatementAssignment> ptr;
  JSyntaxStatementAssignment(
      std::string _identifier,
      std::shared_ptr<JSyntaxExpression> _value_expr
  );
  ~JSyntaxStatementAssignment();
  std::string identifier;
  std::shared_ptr<JSyntaxExpression> value_expr;
};

class JSyntaxStatementIfElse {
public:
  typedef std::shared_ptr<JSyntaxStatementIfElse> ptr;
  JSyntaxStatementIfElse(
                         std::shared_ptr<JSyntaxExpression> _expr,
                         std::shared_ptr<JSyntaxStatementList> _ifbody,
                         std::shared_ptr<JSyntaxStatementList> _elsebody
                         );
  ~JSyntaxStatementIfElse();
  std::shared_ptr<JSyntaxExpression> expr;
  std::shared_ptr<JSyntaxStatementList> ifbody;
  std::shared_ptr<JSyntaxStatementList> elsebody;
};

class JSyntaxStatementWhile {
public:
  typedef std::shared_ptr<JSyntaxStatementWhile> ptr;
  JSyntaxStatementWhile(
                        std::shared_ptr<JSyntaxExpression> _expr,
                        std::shared_ptr<JSyntaxStatementList> _body
                        );
  ~JSyntaxStatementWhile();
  std::shared_ptr<JSyntaxExpression> expr;
  std::shared_ptr<JSyntaxStatementList> body;
};

class JSyntaxStatementReturn {
public:
  typedef std::shared_ptr<JSyntaxStatementReturn> ptr;
  JSyntaxStatementReturn(std::shared_ptr<JSyntaxExpression> _expr);
  ~JSyntaxStatementReturn();
  std::shared_ptr<JSyntaxExpression> expr;
};
