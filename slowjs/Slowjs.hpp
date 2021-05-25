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
#include "Event_Loop.hpp"

class Slowjs
{
public:
    Slowjs()
    {
        loop = new Event_Loop(this);
        global_obj = new JSObject();
        ctx_stack = new stack<Execution_Context *>;
    }
    JSValue run(const string &);
    queue<Token *> tokenize(const string &);
    AST_Node *parse(const queue<Token *> &);

    void addIntrinsic();
    // https://262.ecma-international.org/5.1/#sec-10.4.1.1
    void initGlobalExecutionContext(AST_Node *);
    // https://262.ecma-international.org/5.1/#sec-10.4.3
    void initFunctionExecutionContext(JSFunction *, const JSValue &, const vector<JSValue> &);
    // https://262.ecma-international.org/5.1/#sec-10.5
    void declarationBindingInstantiation(AST_Node *, const vector<JSValue> &);

    Execution_Context *getCurrentContext();
    vector<JSValue> getArgumentList(AST_Node *);

    Event_Loop *loop;
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
    JSValue evaluateReturnStatement(AST_Node *);
    JSValue evaluateBreakStatement(AST_Node *);
    JSValue evaluateContinueStatement(AST_Node *);
    JSValue evaluateUnaryExpression(AST_Node *);
    JSValue evaluateThisExpression(AST_Node *);
    JSValue evaluateMemberExpression(AST_Node *);
    JSValue evaluateFunctionExpression(AST_Node *);
    JSValue evaluateThrowStatement(AST_Node *);
    JSValue evaluateDoWhileStatement(AST_Node *);
    JSValue evaluateWhileStatement(AST_Node *);
    JSValue evaluateConditionalExpression(AST_Node *);
    JSValue evaluateArrayExpression(AST_Node *);
    JSValue evaluateObjectExpression(AST_Node *);

    Reference getMemberExpressionReference(AST_Node *);
    Reference getReference(AST_Node *);

    void checkException(const JSValue &);
    vector<JSValue> getParamList(AST_Node *);
    JSFunction *CreateFunctionObject(AST_Node *);
};
#endif /* Slowjs_hpp */
