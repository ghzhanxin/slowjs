//
//  Parser.hpp
//  slowjs
//
//  Created by zx on 2021/4/19.
//

#ifndef Parser_hpp
#define Parser_hpp

#include <stdio.h>
#include <iostream>
#include <string>
#include "Tokenizer.hpp"

using namespace std;

namespace nt
{
    enum Node_Type
    {
        Program,
        StatementList,
        Statement,
        EmptyStatement,
        VariableDeclaration,
        ExpressionStatement,
        FunctionDeclaration,
        FormalParameters,
        ForStatement,
        IfStatement,
        BlockStatement,
        BreakStatement,
        ReturnStatement,
        AssignmentExpression,
        Literal,
        Identifier,
        BinaryExpression,
        UpdateExpression,
        NewExpression,
        CallExpression,
        MemberExpression,
        Arguments,
        ThisExpression,
    };
}

namespace rt
{
    enum Raw_Type
    {
        Default,

        Number,
        String,
        Boolean,
        Null,
        Undefined,
    };
}

class AST_Node
{
public:
    AST_Node(){};
    AST_Node(nt::Node_Type nodeType) { type = nodeType; }

    nt::Node_Type type;
    string value;
    rt::Raw_Type rawType = rt::Default;
    vector<AST_Node *> childs;
};

class Parser
{
public:
    static queue<Token *> tokenQueue;
    static AST_Node *root;
    static Token *lookahead;

    static void nextToken();
    static bool match(string s);

    static AST_Node *parse(queue<Token *>);

    static AST_Node *Program();
    static vector<AST_Node *> StatementList();
    static AST_Node *Statement();
    static AST_Node *EmptyStatement();
    static AST_Node *BreakStatement();
    static AST_Node *ReturnStatement();
    static AST_Node *IfStatement();
    static AST_Node *ForStatement();
    static AST_Node *BlockStatement();
    static AST_Node *ExpressionStatement();
    static AST_Node *VariableDeclaration();
    static AST_Node *FunctionDeclaration();
    static AST_Node *FormalParameters();
    static vector<AST_Node *> IdentifierList();

    static AST_Node *Expression();
    static AST_Node *AssignmentExpression();
    static AST_Node *EqualityExpression();
    static AST_Node *RelationalExpression();
    static AST_Node *AdditiveExpression();
    static AST_Node *MultiplicativeExpression();
    static AST_Node *PostfixExpression();
    static AST_Node *LeftHandSideExpression();
    static AST_Node *CallExpression();
    static AST_Node *Arguments();
    static AST_Node *NewExpression();
    static AST_Node *MemberExpression();
    static AST_Node *PrimaryExpression();
    static vector<AST_Node *> ExpressionList();
    static AST_Node *Literal();
    static AST_Node *Identifier();
};

#endif /* Parser_hpp */
