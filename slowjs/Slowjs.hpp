//
//  Slowjs.hpp
//  slowjs
//
//  Created by zx on 2021/4/20.
//

#ifndef Slowjs_hpp
#define Slowjs_hpp

#include <stdio.h>
#include <stack>
#include "Tokenizer.hpp"
#include "Parser.hpp"
#include "JSValue_Type.hpp"
#include "Execution_Context.hpp"
#include "Spec_Type.hpp"
#include "Abstract_Operation.hpp"

class Slowjs
{
public:
    JSValue run(string input);
    queue<Token *> tokenize(string input);
    AST_Node *parse(queue<Token *> q);

    void initCallStack();
    void addIntrinsic();
    void initGlobalExecutionContext(AST_Node *);
    void initFunctionExecutionContext(JSFunction *, JSValue, vector<JSValue>);
    void declarationBindingInstantiation(AST_Node *, vector<JSValue>);
    JSValue evaluateIntrinsicFunction(JSFunction *, vector<JSValue>);
    Execution_Context *getCurrentContext();
    vector<JSValue> getArgumentList(AST_Node *);

    JSObject *global_obj;
    Execution_Context *global_ctx;
    stack<Execution_Context *> *ctx_stack;

    JSValue evaluate(AST_Node *);
    JSValue evaluateEmptyStatement(AST_Node *);
    JSValue evaluateLiteral(AST_Node *);
    JSValue evaluateProgram(AST_Node *);
    JSValue evaluateExpressionStatement(AST_Node *);
    JSValue evaluateBinaryExpression(AST_Node *);
    JSValue evaluateAssignmentExpression(AST_Node *);
    JSValue evaluateIdentifier(AST_Node *);
    JSValue evaluateIfStatement(AST_Node *);
    JSValue evaluateBlockStatement(AST_Node *);
    JSValue evaluateForStatement(AST_Node *);
    JSValue evaluateUpdateExpression(AST_Node *);
    JSValue evaluateVariableDeclaration(AST_Node *);
    JSValue evaluateCallExpression(AST_Node *);
    JSValue evaluateNewExpression(AST_Node *);
    vector<string> getIdentifiersFromMemberExpression(AST_Node *, vector<string>);
    JSValue evaluateReturnStatement(AST_Node *);
    JSValue evaluateBreakStatement(AST_Node *);
    JSValue evaluateContinueStatement(AST_Node *);
    JSValue evaluateUnaryExpression(AST_Node *);
    JSValue evaluateThisExpression(AST_Node *);
    JSValue evaluateMemberExpression(AST_Node *);
    JSValue evaluateFunctionExpression(AST_Node *);
    void checkException(JSValue);

    Reference getMemberExpressionReference(AST_Node *);
    Reference getReference(AST_Node *);

    vector<JSValue> getParamList(AST_Node *);

    JSFunction *CreateFunctionObject(AST_Node *);
};
#endif /* Slowjs_hpp */
