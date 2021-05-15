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
//          ForStatement
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
// ForStatement : for (VariableDeclaration ExpressionStatement Expression) Statement
// VariableDeclaration : var AssignmentExpression ;
// FunctionDeclaration : function Identifier (FormalParameters) BlockStatement
// FunctionExpression : function [Identifier] (FormalParameters) BlockStatement
// BlockStatement : { StatementList }
// ExpressionStatement : Expression ;
// FormalParameters :
//          [empty]
//          IdentifierList
// IdentifierList :
//          Identifier
//          IdentifierList, Identifier

// Expression : AssignmentExpression
// AssignmentExpression:
//          LogicalExpression
//          MemberExpression = Expression
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
//          NewExpression
//          CallExpression
// CallExpression :
//          MemberExpression Arguments
// Arguments :
//          ()
//          (ExpressionList)
// ExpressionList :
//          Expression
//          ExpressionList, Expression
// NewExpression :
//          MemberExpression
//          new NewExpression
// MemberExpression:
//          PrimaryExpression
//          FunctionExpression
//          this . MemberExpression
//          Identifier . MemberExpression
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

int Parser::throwParseSyntaxError(string s = "Unknown Parse Error")
{
    string msg = SyntaxErrorPrefix;
    throw msg + s;
}
void Parser::nextToken()
{
    if (!tokenQueue.empty())
    {
        lookahead = tokenQueue.front();
        tokenQueue.pop();
    }
    else
    {
        lookahead = nullptr;
    }
}

// match a string
bool Parser::match(string s)
{
    if (lookahead && lookahead->value == s)
    {
        nextToken();
        return true;
    }
    else
    {
        return false;
    }
}

// Program : StatementList
AST_Node *Parser::Program()
{
    root = new AST_Node(nt::Program);
    vector<AST_Node *> childs = StatementList();
    root->childs = childs;
    return root;
}

// StatementList :
//          Statement
//          StatementList Statement
vector<AST_Node *> Parser::StatementList()
{
    vector<AST_Node *> childs;
    while (lookahead && lookahead->value != "}")
    {
        AST_Node *stmt = Statement();
        if (stmt)
            childs.push_back(stmt);
        else
            throw throwParseSyntaxError("Statement exception");
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
//          ForStatement
//          VariableDeclaration
//          FunctionDeclaration
//          BlockStatement
//          ExpressionStatement
AST_Node *Parser::Statement()
{
    if (!lookahead)
        throw throwParseSyntaxError();

    if (lookahead->value == "break")
        return BreakStatement();
    if (lookahead->value == "continue")
        return ContinueStatement();
    if (lookahead->value == "return")
        return ReturnStatement();
    if (lookahead->value == "throw")
        return ThrowStatement();
    if (lookahead->value == ";")
        return EmptyStatement();
    if (lookahead->value == "if")
        return IfStatement();
    if (lookahead->value == "for")
        return ForStatement();
    if (lookahead->value == "var")
        return VariableDeclaration();
    if (lookahead->value == "function")
        return FunctionDeclaration();
    if (lookahead->value == "{")
        return BlockStatement();

    return ExpressionStatement();
}
// BreakStatement: break ;
AST_Node *Parser::BreakStatement()
{
    if (match("break") && match(";"))
        return new AST_Node(nt::BreakStatement);
    else
        throw throwParseSyntaxError("Expect 'break' or ';' in BreakStatement");
}
// ContinueStatement: continue ;
AST_Node *Parser::ContinueStatement()
{
    if (match("continue") && match(";"))
        return new AST_Node(nt::ContinueStatement);
    else
        throw throwParseSyntaxError("Expect 'continue' or ';' in ContinueStatement");
}
// ReturnStatement:
//          return ;
//          return expression ;
AST_Node *Parser::ReturnStatement()
{
    if (match("return"))
    {
        AST_Node *ret = new AST_Node(nt::ReturnStatement);
        if (match(";"))
            return ret;
        else
        {
            AST_Node *expre = Expression();
            if (expre && match(";"))
            {
                ret->childs.push_back(expre);
                return ret;
            }
            else
                throw throwParseSyntaxError("Expect expression or ';' in ReturnStatement");
        }
    }
    else
        throw throwParseSyntaxError("Expect 'return' in ReturnStatement");
}

// ThrowStatement: throw Expression ;
AST_Node *Parser::ThrowStatement()
{
    if (match("throw"))
    {
        AST_Node *expr = Expression();
        if (expr && match(";"))
        {
            AST_Node *thr = new AST_Node(nt::ThrowStatement);
            thr->childs.push_back(expr);
            return thr;
        }
        throw throwParseSyntaxError("Expect expression or ';' in ReturnStatement");
    }
    else
        throw throwParseSyntaxError("Expect 'throw' in ReturnStatement");
}
// EmptyStatement : ;
AST_Node *Parser::EmptyStatement()
{
    if (!match(";"))
        throw throwParseSyntaxError("Expect ';' in EmptyStatement");

    return new AST_Node(nt::EmptyStatement);
}

// IfStatement :
//          if (Expression) Statement
//          if (Expression) Statement else Statement
AST_Node *Parser::IfStatement()
{
    AST_Node *node = new AST_Node(nt::IfStatement);

    if (match("if") && match("("))
    {
        AST_Node *expre = Expression();
        if (expre && match(")"))
        {
            node->childs.push_back(expre);
            AST_Node *stmt = Statement();

            if (stmt)
                node->childs.push_back(stmt);
            else
                throw throwParseSyntaxError("IfStatement");

            if (match("else"))
            {
                AST_Node *stmt = Statement();
                if (stmt)
                {
                    node->childs.push_back(stmt);
                    return node;
                }
                else
                    throw throwParseSyntaxError("IfStatement");
            }
            else
                return node;
        }
        else
            throw throwParseSyntaxError("Expect expression or ')' in IfStatement");
    }
    else
        throw throwParseSyntaxError("Expect 'if' or '(' in IfStatement");
}

// ForStatement : for (VariableDeclaration ExpressionStatement Expression) Statement
AST_Node *Parser::ForStatement()
{
    AST_Node *node = new AST_Node(nt::ForStatement);

    if (match("for") && match("("))
    {
        AST_Node *var = VariableDeclaration();
        AST_Node *expre_stmt = ExpressionStatement();
        AST_Node *expre = Expression();
        if (var && expre_stmt && expre && match(")"))
        {
            node->childs.push_back(var);
            node->childs.push_back(expre_stmt);
            node->childs.push_back(expre);
            AST_Node *stmt = Statement();
            if (stmt)
            {
                node->childs.push_back(stmt);
                return node;
            }
            else
                throw throwParseSyntaxError("ForStatement");
        }
        else
            throw throwParseSyntaxError("Expect ')' in ForStatement");
    }
    else
        throw throwParseSyntaxError("Expect 'for' or '(' in ForStatement");
}

// VariableDeclaration : var AssignmentExpression ;
AST_Node *Parser::VariableDeclaration()
{
    AST_Node *node = new AST_Node(nt::VariableDeclaration);

    if (match("var"))
    {
        AST_Node *assign = AssignmentExpression();

        if (assign && match(";"))
        {
            node->childs.push_back(assign);
            return node;
        }
        else
            throw throwParseSyntaxError("Expect assignment or ';' in VariableDeclaration");
    }
    else
        throw throwParseSyntaxError("Expect 'var' in VariableDeclaration");
}

// FunctionDeclaration : function Identifier (FormalParameters) BlockStatement
AST_Node *Parser::FunctionDeclaration()
{
    AST_Node *node = new AST_Node(nt::FunctionDeclaration);

    if (match("function"))
    {
        AST_Node *id = Identifier();
        if (id && match("("))
        {
            AST_Node *param = FormalParameters();
            if (param && match(")"))
            {
                AST_Node *block = BlockStatement();
                if (block)
                {
                    node->childs.push_back(id);
                    node->childs.push_back(param);
                    node->childs.push_back(block);
                    return node;
                }
                else
                    throw throwParseSyntaxError("FunctionDeclaration");
            }
            else
                throw throwParseSyntaxError("Expect parameter or ')' in FunctionDeclaration");
        }
        else
            throw throwParseSyntaxError("Expect function identifier or '(' in FunctionDeclaration");
    }
    else
        throw throwParseSyntaxError("Expect 'function' in FunctionDeclaration");
}
// FunctionExpression : function [Identifier] (FormalParameters) BlockStatement
AST_Node *Parser::FunctionExpression()
{
    AST_Node *node = new AST_Node(nt::FunctionExpression);

    if (match("function"))
    {
        AST_Node *id = nullptr;
        if (lookahead && lookahead->value != "(")
        {
            id = Identifier();
            if (!id)
                throw throwParseSyntaxError("Expect 'Identifier' in FunctionExpression");
        }
        if (match("("))
        {
            AST_Node *param = FormalParameters();
            if (param && match(")"))
            {
                AST_Node *block = BlockStatement();
                if (block)
                {
                    node->childs.push_back(id);
                    node->childs.push_back(param);
                    node->childs.push_back(block);
                    return node;
                }
                else
                    throw throwParseSyntaxError("FunctionExpression");
            }
            else
                throw throwParseSyntaxError("Expect parameter or ')' in FunctionExpression");
        }
        else
            throw throwParseSyntaxError("Expect function identifier or '(' in FunctionExpression");
    }
    else
        throw throwParseSyntaxError("Expect 'function' in FunctionExpression");
}

// BlockStatement : { StatementList }
AST_Node *Parser::BlockStatement()
{
    AST_Node *node = new AST_Node(nt::BlockStatement);

    if (match("{"))
    {
        vector<AST_Node *> childs = StatementList();

        if (match("}"))
        {
            node->childs = childs;
            return node;
        }
        else
            throw throwParseSyntaxError("Expect '}' in BlockStatement");
    }
    else
        throw throwParseSyntaxError("Expect '{' in BlockStatement");
}

// ExpressionStatement : Expression ;
AST_Node *Parser::ExpressionStatement()
{
    AST_Node *node = new AST_Node(nt::ExpressionStatement);

    AST_Node *expre = Expression();
    if (!match(";") || !expre)
        throw throwParseSyntaxError("Expect ';' or expression in ExpressionStatement");

    node->childs.push_back(expre);
    return node;
}

// FormalParameters :
//          [empty]
//          IdentifierList
AST_Node *Parser::FormalParameters()
{
    AST_Node *node = new AST_Node(nt::FormalParameters);
    if (lookahead && lookahead->value == ")")
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
    while (lookahead && lookahead->value != ")")
    {
        AST_Node *id = Identifier();
        if (id)
        {
            childs.push_back(id);

            if (lookahead && lookahead->value == ")")
                return childs;

            if (!match(","))
                throw throwParseSyntaxError("Expect ',' in IdentifierList");
        }
        else
            throw throwParseSyntaxError();
    }
    throw throwParseSyntaxError("in IdentifierList");
}

// Expression : AssignmentExpression
AST_Node *Parser::Expression()
{
    AST_Node *assign = AssignmentExpression();
    if (!assign)
        return nullptr;
    return assign;
}

void Parser::storeAssignmentExpression()
{
    AssignmentExpressionTokenQueue = tokenQueue;
    AssignmentExpressionLookahead = lookahead;
}
void Parser::restoreAssignmentExpression()
{
    tokenQueue = AssignmentExpressionTokenQueue;
    lookahead = AssignmentExpressionLookahead;
}

// AssignmentExpression:
//          LogicalExpression
//          MemberExpression = Expression
AST_Node *Parser::AssignmentExpression()
{
    storeAssignmentExpression();
    AST_Node *member = MemberExpression();
    if (member && match("="))
    {
        AST_Node *expre = Expression();
        if (expre)
        {
            AST_Node *node = new AST_Node(nt::AssignmentExpression);
            node->childs.push_back(member);
            node->childs.push_back(expre);
            return node;
        }
        else
            throw throwParseSyntaxError("Expect expression in AssignmentExpression");
    }
    else
    {
        restoreAssignmentExpression();
        return LogicalExpression();
    }
}

AST_Node *buildBinary(AST_Node *left, AST_Node *right, string op)
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
        return nullptr;

    string op = lookahead->value;
    if (match("&&") || match("||"))
    {
        AST_Node *logical = LogicalExpression();
        if (logical)
            return buildBinary(equality, logical, op);
        else
            throw throwParseSyntaxError("LogicalExpression");
    }
    else
    {
        return equality;
    }
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
        return nullptr;

    string op = lookahead->value;
    if (match("==") || match("!=") || match("===") || match("!=="))
    {
        AST_Node *equality = EqualityExpression();
        if (equality)
            return buildBinary(relational, equality, op);
        else
            throw throwParseSyntaxError("EqualityExpression");
    }
    else
    {
        return relational;
    }
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

    string op = lookahead->value;
    if (match(">") || match("<") || match(">=") || match("<="))
    {
        AST_Node *relation = RelationalExpression();
        if (relation)
            return buildBinary(add, relation, op);
        else
            throw throwParseSyntaxError("RelationalExpression");
    }
    else
    {
        return add;
    }
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

    string op = lookahead->value;
    if (match("+") || match("-"))
    {
        AST_Node *add = AdditiveExpression();
        if (add)
            return buildBinary(multi, add, op);
        else
            throw throwParseSyntaxError("AdditiveExpression");
    }
    else
    {
        return multi;
    }
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

    string op = lookahead->value;
    if (match("*") || match("/") || match("%"))
    {
        AST_Node *multi = MultiplicativeExpression();
        if (multi)
            return buildBinary(postfix, multi, op);
        else
            throw throwParseSyntaxError("MultiplicativeExpression");
    }
    else
    {
        return postfix;
    }
}

// UnaryExpression:
//          PostfixExpression
//          ! UnaryExpression
AST_Node *Parser::UnaryExpression()
{
    if (match("!"))
    {
        AST_Node *unary = UnaryExpression();
        if (unary)
        {
            AST_Node *node = new AST_Node(nt::UnaryExpression);
            node->value = "!";
            node->childs.push_back(unary);
            return node;
        }
        else
            throw throwParseSyntaxError("UnaryExpression");
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
        return nullptr;

    string op = lookahead->value;
    if (match("++") || match("--"))
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
//          NewExpression
//          CallExpression
AST_Node *Parser::LeftHandSideExpression()
{
    AST_Node *newExpre = NewExpression();
    return newExpre ? newExpre : CallExpression();
}

// CallExpression :
//          MemberExpression
//          MemberExpression Arguments
AST_Node *Parser::CallExpression()
{
    AST_Node *member = MemberExpression();
    if (member)
    {
        if (lookahead && lookahead->value == "(")
        {
            AST_Node *args = Arguments();
            if (args)
            {
                AST_Node *call = new AST_Node(nt::CallExpression);
                call->childs.push_back(member);
                call->childs.push_back(args);
                return call;
            }
            else
                throw throwParseSyntaxError("Expect Arguments in CallExpression");
        }
        else
            return member;
    }
    else
        return nullptr;
}
// Arguments :
//          ()
//          (ExpressionList)
AST_Node *Parser::Arguments()
{
    if (match("("))
    {
        AST_Node *args = new AST_Node(nt::Arguments);
        if (match(")"))
            return args;
        else
        {
            try
            {
                vector<AST_Node *> childs = ExpressionList();
                if (match(")"))
                {
                    args->childs = childs;
                    return args;
                }
                else
                    throw throwParseSyntaxError("Expect ')' in Arguments");
            }
            catch (...)
            {
                throw throwParseSyntaxError("in Arguments");
            }
        }
    }
    else
        throw throwParseSyntaxError("Expect '(' in Arguments");
}

// ExpressionList :
//          Expression
//          ExpressionList, Expression
vector<AST_Node *> Parser::ExpressionList()
{
    vector<AST_Node *> childs;
    while (lookahead && lookahead->value != ")")
    {
        AST_Node *expre = Expression();
        if (expre)
        {
            childs.push_back(expre);

            if (lookahead->value == ")")
                return childs;

            if (!match(","))
                throw throwParseSyntaxError("Expect ',' in ExpressionList");
        }
        else
            throw throwParseSyntaxError("in ExpressionList");
    }
    throw throwParseSyntaxError("in ExpressionList");
}

// NewExpression :
//          new MemberExpression Arguments
AST_Node *Parser::NewExpression()
{
    if (match("new"))
    {
        AST_Node *member = MemberExpression();
        if (member)
        {
            AST_Node *args = Arguments();
            if (args)
            {
                AST_Node *newExpre = new AST_Node(nt::NewExpression);
                newExpre->childs.push_back(member);
                newExpre->childs.push_back(args);
                return newExpre;
            }
            else
                throw throwParseSyntaxError();
        }
        else
            throw throwParseSyntaxError();
    }
    else
        return nullptr;
}

void Parser::storeMemberExpression()
{
    MemberExpressionTokenQueue = tokenQueue;
    MemberExpressionLookahead = lookahead;
}
void Parser::restoreMemberExpression()
{
    tokenQueue = MemberExpressionTokenQueue;
    lookahead = MemberExpressionLookahead;
}

// MemberExpression:
//          PrimaryExpression
//          FunctionExpression
//          this . MemberExpression
//          Identifier . MemberExpression
AST_Node *Parser::MemberExpression()
{
    storeMemberExpression();
    if (lookahead && lookahead->value == "function")
        return FunctionExpression();
    else if (match("this") && match("."))
    {
        AST_Node *member = MemberExpression();
        AST_Node *memberNode = new AST_Node(nt::MemberExpression);
        memberNode->childs.push_back(new AST_Node(nt::ThisExpression, "this"));
        memberNode->childs.push_back(member);
        return memberNode;
    }
    else
    {
        restoreMemberExpression();
        AST_Node *id = Identifier();
        if (id && match("."))
        {
            AST_Node *member = MemberExpression();
            AST_Node *memberNode = new AST_Node(nt::MemberExpression);
            memberNode->childs.push_back(id);
            memberNode->childs.push_back(member);
            return memberNode;
        }
        else
        {
            restoreMemberExpression();
            return PrimaryExpression();
        }
    }
}

// PrimaryExpression :
//          this
//          Identifier
//          Literal
//          (Expression)
AST_Node *Parser::PrimaryExpression()
{
    if (!lookahead)
        throw throwParseSyntaxError();

    if (lookahead->type == tt::_this)
    {
        nextToken();
        return new AST_Node(nt::ThisExpression, lookahead->value);
    }
    else

        if (lookahead->type == tt::name)
    {
        return Identifier();
    }
    else if (match("("))
    {
        AST_Node *expre = Expression();
        if (!match(")") || !expre)
            throw throwParseSyntaxError("Expect ')'");

        return expre;
    }
    else
    {
        return Literal();
    }
}

// Literal :
//          NullLiteral
//          BooleanLiteral
//          NumericLiteral
//          StringLiteral
AST_Node *Parser::Literal()
{
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

    if (lookahead && lookahead->type == tt::name)
    {
        node->value = lookahead->value;
        nextToken();
        return node;
    }
    else
        return nullptr;
}

void Parser::traversal(AST_Node *node, string prefix)
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

AST_Node *Parser::parse(queue<Token *> que)
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
