//
//  Parser.cpp
//  slowjs
//
//  Created by zx on 2021/4/19.
//

// Grammer

// Program : StatementList
// StatementList :
//          Statement
//          StatementList Statement
// Statement :
//          BreakStatement
//          ContinueStatement
//          ReturnStatement
//          ThrowStatement
//          EmptyStatement
//          IfStatement
//          IterationStatement
//          VariableDeclaration
//          FunctionDeclaration
//          BlockStatement
//          ExpressionStatement
// BreakStatement: break ;
// ContinueStatement: continue ;
// ReturnStatement:
//          return ;
//          return expression ;
// ThrowStatement: throw Expression ;
// EmptyStatement : ;
// IfStatement :
//          if (Expression) Statement
//          if (Expression) Statement else Statement
// IterationStatement :
//          do Statement while ( Expression );
//          while ( Expression ) Statement
//          for (var AssignmentExpression ; Expression ; Expression) Statement
//          for ( [Expression] ; Expression ; Expression) Statement
// VariableDeclaration : var AssignmentExpression ;
// FunctionDeclaration : function Identifier (FormalParameterList) BlockStatement
// FunctionExpression : function [Identifier] (FormalParameterList) BlockStatement
// BlockStatement : { StatementList }
// ExpressionStatement : Expression ;
// FormalParameterList :
//          [empty]
//          IdentifierList
// IdentifierList :
//          Identifier
//          IdentifierList, Identifier

// Expression : AssignmentExpression
// AssignmentExpression:
//          ConditionalExpression
//          LeftHandSideExpression = Expression
// ConditionalExpression:
//          LogicalExpression
//          LogicalExpression ? LogicalExpression : LogicalExpression
// LogicalExpression:
//          EqualityExpression
//          EqualityExpression && LogicalExpression
//          EqualityExpression || LogicalExpression
// EqualityExpression :
//          RelationalExpression
//          RelationalExpression == EqualityExpression
//          RelationalExpression != EqualityExpression
//          RelationalExpression === EqualityExpression
//          RelationalExpression !== EqualityExpression
// RelationalExpression :
//          AdditiveExpression
//          AdditiveExpression > RelationalExpression
//          AdditiveExpression < RelationalExpression
//          AdditiveExpression >= RelationalExpression
//          AdditiveExpression <= RelationalExpression
// AdditiveExpression :
//          MultiplicativeExpression
//          MultiplicativeExpression + AdditiveExpression
//          MultiplicativeExpression - AdditiveExpression
// MultiplicativeExpression :
//          UnaryExpression
//          UnaryExpression * MultiplicativeExpression
//          UnaryExpression / MultiplicativeExpression
//          UnaryExpression % MultiplicativeExpression
// UnaryExpression:
//          PostfixExpression
//          ! UnaryExpression
// PostfixExpression :
//          LeftHandSideExpression
//          LeftHandSideExpression++
//          LeftHandSideExpression--
// LeftHandSideExpression :
//          CallExpression
//          NewExpression
// CallExpression :
//          MemberExpression
//          MemberExpression Arguments
// Arguments :
//          ()
//          (ExpressionList)
// ExpressionList :
//          Expression
//          ExpressionList, Expression
// NewExpression :
//          new MemberExpression Arguments
// MemberExpression:
//          PrimaryExpression
//          FunctionExpression
//          MemberExpression . Identifier
//          MemberExpression [ Expression ]
// PrimaryExpression :
//          this
//          Identifier
//          Literal
//          (Expression)

// Identifier: a-z+
// Literal :
//          NullLiteral
//          BooleanLiteral
//          NumericLiteral
//          StringLiteral
// NullLiteral : null
// BooleanLiteral : true | false
// StringLiteral : 'a-z'+
// NumericLiteral : 0-9+

#include "Parser.hpp"

using namespace std;

int Parser::check(bool result)
{
    if (!result)
        throw ExceptionTokenPrefix + string(" '" + lookahead->value + "' " + " in line: " + to_string(lookahead->line) + " column: " + to_string(lookahead->column));

    return 0;
}
void Parser::nextToken()
{
    if (!tokenQueue.empty())
    {
        current = lookahead;
        lookahead = tokenQueue.front();
        tokenQueue.pop();
    }
    else
        lookahead = nullptr;
}
template <typename T>
bool Parser::eat(const T &t)
{
    if (expect(t))
    {
        nextToken();
        return true;
    }
    else
        return false;
}
bool Parser::expect(const string &s)
{
    return lookahead && lookahead->value == s ? true : false;
}
bool Parser::expect(tt::Token_Type t)
{
    return lookahead && lookahead->type == t ? true : false;
}

bool Parser::expectNot(const string &s)
{
    return lookahead && lookahead->value != s ? true : false;
}
bool Parser::expectNot(tt::Token_Type t)
{
    return lookahead && lookahead->type != t ? true : false;
}
bool Parser::semicolon()
{
    bool canInsertSemicolon = !lookahead || lookahead->type == tt::parenR || current->isNextLineBreak;
    if (!eat(";") && !canInsertSemicolon)
        return false;

    return true;
}
// Program : StatementList
AST_Node *Parser::Program()
{
    vector<AST_Node *> childs = StatementList();
    root = new AST_Node(nt::Program);
    root->childs = childs;
    return root;
}

// StatementList :
//          Statement
//          StatementList Statement
vector<AST_Node *> Parser::StatementList()
{
    vector<AST_Node *> childs;
    while (expectNot("}"))
    {
        AST_Node *stmt = Statement();
        childs.push_back(stmt);
    }
    return childs;
}

// Statement :
//          BreakStatement
//          ContinueStatement
//          ReturnStatement
//          ThrowStatement
//          EmptyStatement
//          IfStatement
//          IterationStatement
//          VariableDeclaration
//          FunctionDeclaration
//          BlockStatement
//          ExpressionStatement
AST_Node *Parser::Statement()
{
    if (expect("break"))
        return BreakStatement();
    else if (expect("continue"))
        return ContinueStatement();
    else if (expect("return"))
        return ReturnStatement();
    else if (expect("throw"))
        return ThrowStatement();
    else if (expect(";"))
        return EmptyStatement();
    else if (expect("if"))
        return IfStatement();
    else if (expect("for") || expect("while") || expect("do"))
        return IterationStatement();
    else if (expect("var"))
        return VariableDeclaration();
    else if (expect("function"))
        return FunctionDeclaration();
    else if (expect("{"))
        return BlockStatement();

    return ExpressionStatement();
}

// BreakStatement: break ;
AST_Node *Parser::BreakStatement()
{
    check(eat("break"));
    check(semicolon());

    return new AST_Node(nt::BreakStatement);
}

// ContinueStatement: continue ;
AST_Node *Parser::ContinueStatement()
{
    check(eat("continue"));
    check(semicolon());

    return new AST_Node(nt::ContinueStatement);
}

// ReturnStatement:
//          return ;
//          return expression ;
AST_Node *Parser::ReturnStatement()
{
    check(eat("return"));

    AST_Node *ret = new AST_Node(nt::ReturnStatement);
    if (semicolon())
        return ret;
    else
    {
        AST_Node *expr = Expression();
        check(semicolon());

        ret->childs.push_back(expr);
        return ret;
    }
}

// ThrowStatement: throw Expression ;
AST_Node *Parser::ThrowStatement()
{
    check(eat("throw"));

    AST_Node *expr = Expression();
    check(semicolon());

    AST_Node *thr = new AST_Node(nt::ThrowStatement);
    thr->childs.push_back(expr);
    return thr;
}

// EmptyStatement : ;
AST_Node *Parser::EmptyStatement()
{
    check(semicolon());
    return new AST_Node(nt::EmptyStatement);
}

// IfStatement :
//          if (Expression) Statement
//          if (Expression) Statement else Statement
AST_Node *Parser::IfStatement()
{
    check(eat("if"));
    check(eat("("));

    AST_Node *expr = Expression();
    check(eat(")"));
    AST_Node *stmt = Statement();

    AST_Node *node = new AST_Node(nt::IfStatement);
    node->childs.push_back(expr);
    node->childs.push_back(stmt);

    if (eat("else"))
    {
        AST_Node *stmt = Statement();
        node->childs.push_back(stmt);
        return node;
    }
    else
        return node;
}

// IterationStatement :
//          do Statement while ( Expression );
//          while ( Expression ) Statement
//          for (var AssignmentExpression ; Expression ; Expression) Statement
//          for ( [Expression] ; Expression ; Expression) Statement
AST_Node *Parser::IterationStatement()
{
    if (eat("do"))
    {
        AST_Node *stmt = Statement();
        check(eat("while"));
        check(eat("("));

        AST_Node *expr = Expression();
        check(eat(")"));
        check(semicolon());

        AST_Node *node = new AST_Node(nt::DoWhileStatement);
        node->childs.push_back(stmt);
        node->childs.push_back(expr);
        return node;
    }
    else if (eat("while"))
    {
        check(eat("("));

        AST_Node *expr = Expression();
        check(eat(")"));

        AST_Node *stmt = Statement();

        AST_Node *node = new AST_Node(nt::WhileStatement);
        node->childs.push_back(expr);
        node->childs.push_back(stmt);
        return node;
    }
    else
    {
        check(eat("for"));
        check(eat("("));

        AST_Node *first = nullptr;
        if (eat("var"))
        {
            first = new AST_Node(nt::VariableDeclaration);
            first->childs.push_back(AssignmentExpression());
        }
        else if (expect(";"))
            first = new AST_Node(nt::EmptyStatement);
        else
            first = Expression();

        check(semicolon());

        AST_Node *second = Expression();
        check(semicolon());

        AST_Node *third = Expression();
        check(eat(")"));

        AST_Node *node = new AST_Node(nt::ForStatement);
        node->childs.push_back(first);
        node->childs.push_back(second);
        node->childs.push_back(third);
        AST_Node *stmt = Statement();

        node->childs.push_back(stmt);
        return node;
    }
}

// VariableDeclaration : var AssignmentExpression ;
AST_Node *Parser::VariableDeclaration()
{
    check(eat("var"));
    AST_Node *assign = AssignmentExpression();
    check(assign);
    check(semicolon());

    AST_Node *node = new AST_Node(nt::VariableDeclaration);
    node->childs.push_back(assign);
    return node;
}

// FunctionDeclaration : function Identifier (FormalParameterList) BlockStatement
AST_Node *Parser::FunctionDeclaration()
{
    check(eat("function"));

    AST_Node *id = Identifier();
    check(id);
    check(eat("("));

    AST_Node *param = FormalParameterList();
    check(eat(")"));

    AST_Node *block = BlockStatement();

    AST_Node *node = new AST_Node(nt::FunctionDeclaration);
    node->childs.push_back(id);
    node->childs.push_back(param);
    node->childs.push_back(block);
    return node;
}

// FunctionExpression : function [Identifier] (FormalParameterList) BlockStatement
AST_Node *Parser::FunctionExpression()
{
    check(eat("function"));

    AST_Node *id = nullptr;
    if (expectNot("("))
    {
        id = Identifier();
        check(id);
    }

    check(eat("("));

    AST_Node *param = FormalParameterList();
    check(eat(")"));

    AST_Node *block = BlockStatement();

    AST_Node *node = new AST_Node(nt::FunctionExpression);
    node->childs.push_back(id);
    node->childs.push_back(param);
    node->childs.push_back(block);
    return node;
}

// BlockStatement : { StatementList }
AST_Node *Parser::BlockStatement()
{
    check(eat("{"));
    vector<AST_Node *> childs = StatementList();

    check(eat("}"));

    AST_Node *node = new AST_Node(nt::BlockStatement);
    node->childs = childs;
    return node;
}

// ExpressionStatement : Expression ;
AST_Node *Parser::ExpressionStatement()
{
    AST_Node *expr = Expression();
    check(semicolon());

    AST_Node *node = new AST_Node(nt::ExpressionStatement);
    node->childs.push_back(expr);
    return node;
}

// FormalParameterList :
//          [empty]
//          IdentifierList
AST_Node *Parser::FormalParameterList()
{
    AST_Node *node = new AST_Node(nt::FormalParameterList);
    if (expect(")"))
        return node;

    vector<AST_Node *> childs = IdentifierList();
    node->childs = childs;
    return node;
}

// IdentifierList :
//          Identifier
//          IdentifierList, Identifier
vector<AST_Node *> Parser::IdentifierList()
{
    vector<AST_Node *> childs;
    while (expectNot(")"))
    {
        AST_Node *id = Identifier();
        check(id);
        childs.push_back(id);

        if (expect(")"))
            return childs;

        check(eat(","));
    }
    throw "IdentifierList Error";
}

// Expression : AssignmentExpression
AST_Node *Parser::Expression()
{
    AST_Node *assign = AssignmentExpression();
    check(assign);

    return assign;
}

// TODO:
int Parser::checkLeftHandSideValue(AST_Node *node)
{
    return 0;
};

// AssignmentExpression:
//          ConditionalExpression
//          LeftHandSideExpression = Expression
AST_Node *Parser::AssignmentExpression()
{
    AST_Node *left = ConditionalExpression();
    check(left);

    if (!eat("="))
        return left;

    checkLeftHandSideValue(left);

    AST_Node *right = Expression();
    AST_Node *node = new AST_Node(nt::AssignmentExpression);
    node->childs.push_back(left);
    node->childs.push_back(right);
    return node;
}

// ConditionalExpression:
//          LogicalExpression
//          LogicalExpression ? LogicalExpression : LogicalExpression
AST_Node *Parser::ConditionalExpression()
{
    AST_Node *test = LogicalExpression();
    if (!test)
        return nullptr;

    if (!eat("?"))
        return test;

    AST_Node *consequence = LogicalExpression();
    check(consequence);

    check(eat(":"));

    AST_Node *alternate = LogicalExpression();
    check(alternate);

    AST_Node *node = new AST_Node(nt::ConditionalExpression);
    node->childs.push_back(test);
    node->childs.push_back(consequence);
    node->childs.push_back(alternate);
    return node;
}

AST_Node *Parser::buildBinary(AST_Node *left, AST_Node *right, const string &op)
{
    AST_Node *node = new AST_Node(nt::BinaryExpression);
    node->value = op;
    node->childs.push_back(left);
    node->childs.push_back(right);
    return node;
}

// LogicalExpression:
//          EqualityExpression
//          EqualityExpression && LogicalExpression
//          EqualityExpression || LogicalExpression
AST_Node *Parser::LogicalExpression()
{
    AST_Node *equality = EqualityExpression();
    if (!equality)
        return nullptr;

    if (!lookahead)
        return equality;

    string op = lookahead->value;
    if (eat("&&") || eat("||"))
    {
        AST_Node *logical = LogicalExpression();
        check(logical);

        return buildBinary(equality, logical, op);
    }
    else
        return equality;
}

// EqualityExpression :
//          RelationalExpression
//          RelationalExpression == EqualityExpression
//          RelationalExpression != EqualityExpression
//          RelationalExpression === EqualityExpression
//          RelationalExpression !== EqualityExpression
AST_Node *Parser::EqualityExpression()
{
    AST_Node *relational = RelationalExpression();
    if (!relational)
        return nullptr;

    if (!lookahead)
        return relational;

    string op = lookahead->value;
    if (eat("==") || eat("!=") || eat("===") || eat("!=="))
    {
        AST_Node *equality = EqualityExpression();
        check(equality);

        return buildBinary(relational, equality, op);
    }
    else
        return relational;
}

// RelationalExpression :
//          AdditiveExpression
//          AdditiveExpression > RelationalExpression
//          AdditiveExpression < RelationalExpression
//          AdditiveExpression >= RelationalExpression
//          AdditiveExpression <= RelationalExpression
AST_Node *Parser::RelationalExpression()
{
    AST_Node *add = AdditiveExpression();
    if (!add)
        return nullptr;

    if (!lookahead)
        return add;

    string op = lookahead->value;
    if (eat(">") || eat("<") || eat(">=") || eat("<="))
    {
        AST_Node *relation = RelationalExpression();
        check(relation);

        return buildBinary(add, relation, op);
    }
    else
        return add;
}

// AdditiveExpression :
//          MultiplicativeExpression
//          MultiplicativeExpression + AdditiveExpression
//          MultiplicativeExpression - AdditiveExpression
AST_Node *Parser::AdditiveExpression()
{
    AST_Node *multi = MultiplicativeExpression();
    if (!multi)
        return nullptr;

    if (!lookahead)
        return multi;

    string op = lookahead->value;
    if (eat("+") || eat("-"))
    {
        AST_Node *add = AdditiveExpression();
        check(add);

        return buildBinary(multi, add, op);
    }
    else
        return multi;
}

// MultiplicativeExpression :
//          UnaryExpression
//          UnaryExpression * MultiplicativeExpression
//          UnaryExpression / MultiplicativeExpression
//          UnaryExpression % MultiplicativeExpression
AST_Node *Parser::MultiplicativeExpression()
{
    AST_Node *postfix = UnaryExpression();
    if (!postfix)
        return nullptr;

    if (!lookahead)
        return postfix;

    string op = lookahead->value;
    if (eat("*") || eat("/") || eat("%"))
    {
        AST_Node *multi = MultiplicativeExpression();
        check(multi);

        return buildBinary(postfix, multi, op);
    }
    else
        return postfix;
}

// UnaryExpression:
//          PostfixExpression
//          ! UnaryExpression
AST_Node *Parser::UnaryExpression()
{
    if (eat("!"))
    {
        AST_Node *unary = UnaryExpression();
        check(unary);

        AST_Node *node = new AST_Node(nt::UnaryExpression);
        node->value = "!";
        node->childs.push_back(unary);
        return node;
    }
    else
        return PostfixExpression();
}

// PostfixExpression :
//          LeftHandSideExpression
//          LeftHandSideExpression++
//          LeftHandSideExpression--
AST_Node *Parser::PostfixExpression()
{
    AST_Node *lhs = LeftHandSideExpression();
    if (!lhs)
        return nullptr;

    if (!lookahead)
        return lhs;

    string op = lookahead->value;
    if (eat("++") || eat("--"))
    {
        AST_Node *node = new AST_Node(nt::UpdateExpression);
        node->value = op;
        node->childs.push_back(lhs);
        return node;
    }
    else
        return lhs;
};

// LeftHandSideExpression :
//          CallExpression
//          NewExpression
AST_Node *Parser::LeftHandSideExpression()
{
    AST_Node *newExpr = NewExpression();
    return newExpr ? newExpr : CallExpression();
}

// CallExpression :
//          MemberExpression
//          MemberExpression Arguments
AST_Node *Parser::CallExpression()
{
    AST_Node *member = MemberExpression();
    if (!member)
        return nullptr;

    if (expect("("))
    {
        AST_Node *args = Arguments();

        AST_Node *call = new AST_Node(nt::CallExpression);
        call->childs.push_back(member);
        call->childs.push_back(args);
        return call;
    }
    else
        return member;
}

// Arguments :
//          ()
//          (ExpressionList)
AST_Node *Parser::Arguments()
{
    check(eat("("));
    AST_Node *args = new AST_Node(nt::Arguments);
    if (eat(")"))
        return args;
    else
    {
        vector<AST_Node *> childs = ExpressionList();
        check(eat(")"));

        args->childs = childs;
        return args;
    }
}

// ExpressionList :
//          Expression
//          ExpressionList, Expression
vector<AST_Node *> Parser::ExpressionList()
{
    vector<AST_Node *> childs;
    while (expectNot(")"))
    {
        AST_Node *expr = Expression();
        childs.push_back(expr);

        if (expect(")"))
            return childs;

        check(eat(","));
    }
    throw "ExpressionList Error";
}

// NewExpression :
//          new MemberExpression Arguments
AST_Node *Parser::NewExpression()
{
    if (!eat("new"))
        return nullptr;

    AST_Node *member = MemberExpression();
    check(member);

    AST_Node *args = Arguments();

    AST_Node *newExpr = new AST_Node(nt::NewExpression);
    newExpr->childs.push_back(member);
    newExpr->childs.push_back(args);
    return newExpr;
}

AST_Node *Parser::MemberExpressionRight(AST_Node *node)
{
    if (!expect(".") && !expect("["))
        return node;

    AST_Node *memberNode = new AST_Node(nt::MemberExpression);
    memberNode->childs.push_back(node);
    AST_Node *right;

    if (eat("."))
    {
        right = Identifier();
        check(right);
    }
    else
    {
        check(eat("["));
        right = Expression();
        check(eat("]"));
        memberNode->computed = true;
    }

    memberNode->childs.push_back(right);
    return MemberExpression(memberNode);
}
// MemberExpression:
//          PrimaryExpression
//          FunctionExpression
//          MemberExpression . Identifier
//          MemberExpression [ Expression ]
AST_Node *Parser::MemberExpression(AST_Node *node)
{
    if (node && node->type == nt::MemberExpression)
        return MemberExpressionRight(node);

    if (expect("function"))
        return FunctionExpression();

    AST_Node *primary = PrimaryExpression();
    check(primary);

    return MemberExpressionRight(primary);
}

// PrimaryExpression :
//          this
//          Identifier
//          Literal
//          (Expression)
AST_Node *Parser::PrimaryExpression()
{
    if (eat(tt::_this))
        return new AST_Node(nt::ThisExpression, "this");
    else if (expect(tt::name))
        return Identifier();
    else if (expectNot("("))
        return Literal();
    else if (eat("("))
    {
        AST_Node *expr = Expression();
        check(eat(")"));

        return expr;
    }
    else
        throw "PrimaryExpression";
}

// Literal :
//          NullLiteral
//          BooleanLiteral
//          NumericLiteral
//          StringLiteral
AST_Node *Parser::Literal()
{
    if (!lookahead)
        throw "Literal lookahead is null";

    AST_Node *node = new AST_Node(nt::Literal);
    node->value = lookahead->value;

    tt::Token_Type type = lookahead->type;
    switch (type)
    {
    case tt::num:
        node->rawType = rt::Number;
        nextToken();
        break;
    case tt::string:
        node->rawType = rt::String;
        nextToken();
        break;
    case tt::_true:
    case tt::_false:
        node->rawType = rt::Boolean;
        nextToken();
        break;
    case tt::_null:
        node->rawType = rt::Null;
        nextToken();
        break;
    case tt::_undefined:
        node->rawType = rt::Undefined;
        nextToken();
        break;
    default:
        return nullptr;
    }

    return node;
}

// Identifier: a-z+
AST_Node *Parser::Identifier()
{
    AST_Node *node = new AST_Node(nt::Identifier);

    if (expect(tt::name))
    {
        node->value = lookahead->value;
        nextToken();
        return node;
    }
    else
        return nullptr;
}

void Parser::traversal(AST_Node *node, string &prefix)
{
    cout << prefix << node->type << " -> " << node->value << endl;
    prefix += " ";
    vector<AST_Node *> childs = node->childs;
    for (size_t i = 0; i < childs.size(); i++)
    {
        traversal(childs[i], prefix);
    }
}
void Parser::printAST(AST_Node *node)
{
    string prefix = " ";
    cout << "---------- AST Result ----------" << endl;
    traversal(node, prefix);
}

AST_Node *Parser::parse(const queue<Token *> &que)
{
    tokenQueue = que;
    nextToken();

    if (Program())
    {
        cout << endl
             << "---------- Congratulations! Parse Success!!! ----------" << endl
             << endl;
    }
    else
    {
        cerr << endl
             << "---------- Parse Error ----------" << endl
             << endl;
    };

    return root;
}
