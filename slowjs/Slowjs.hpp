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
#include "Object_ER.hpp"
#include "Declarative_ER.hpp"

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
    JSValue evaluateIntrinsicFunction(string, vector<JSValue>);
    Execution_Context *getCurrentContext();
    JSValue FunctionCall(JSFunction *, JSValue, vector<JSValue>);
    vector<JSValue> getArgumentList(AST_Node *);

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
    Reference evaluateCallExpressionLeft(AST_Node *);
    JSValue evaluateReturnStatement(AST_Node *);
    JSValue evaluateBreakStatement(AST_Node *);
    JSValue evaluateContinueStatement(AST_Node *);
    void checkException(JSValue);

    vector<JSValue> getParamList(AST_Node *);
};
#endif /* Slowjs_hpp */
