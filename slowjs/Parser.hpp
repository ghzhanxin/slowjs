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
        ContinueStatement,
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
    AST_Node(nt::Node_Type t) : type(t){};

    nt::Node_Type type;
    string value;
    rt::Raw_Type rawType = rt::Default;
    vector<AST_Node *> childs;
};

class Parser
{
public:
    AST_Node *root = nullptr;
    Token *lookahead = nullptr;
    queue<Token *> tokenQueue;

    void nextToken();
    bool match(string s);

    AST_Node *parse(queue<Token *>);

    AST_Node *Program();
    vector<AST_Node *> StatementList();
    AST_Node *Statement();
    AST_Node *EmptyStatement();
    AST_Node *BreakStatement();
    AST_Node *ContinueStatement();
    AST_Node *ReturnStatement();
    AST_Node *IfStatement();
    AST_Node *ForStatement();
    AST_Node *BlockStatement();
    AST_Node *ExpressionStatement();
    AST_Node *VariableDeclaration();
    AST_Node *FunctionDeclaration();
    AST_Node *FormalParameters();
    vector<AST_Node *> IdentifierList();

    AST_Node *Expression();
    AST_Node *AssignmentExpression();
    AST_Node *LogicalExpression();
    AST_Node *EqualityExpression();
    AST_Node *RelationalExpression();
    AST_Node *AdditiveExpression();
    AST_Node *MultiplicativeExpression();
    AST_Node *PostfixExpression();
    AST_Node *LeftHandSideExpression();
    AST_Node *CallExpression();
    AST_Node *Arguments();
    AST_Node *NewExpression();
    AST_Node *MemberExpression();
    AST_Node *PrimaryExpression();
    vector<AST_Node *> ExpressionList();
    AST_Node *Literal();
    AST_Node *Identifier();

    queue<Token *> AssignmentExpressionTokenQueue;
    Token *AssignmentExpressionLookahead;
    void storeAssignmentExpression();
    void restoreAssignmentExpression();

    queue<Token *> MemberExpressionTokenQueue;
    Token *MemberExpressionLookahead;
    void storeMemberExpression();
    void restoreMemberExpression();

    int throwParseSyntaxError(string);

    static void traversal(AST_Node *node, string prefix);
    static void printAST(AST_Node *node);
};

#endif /* Parser_hpp */
