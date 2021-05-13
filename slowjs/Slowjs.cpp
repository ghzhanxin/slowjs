//
//  Slowjs.cpp
//  slowjs
//
//  Created by zx on 2021/4/20.
//

#include "Slowjs.hpp"
#include <iostream>
#include "Abstract_Operation.hpp"

JSValue Slowjs::run(string input)
{
    queue<Token *> q = tokenize(input);
    AST_Node *ast = parse(q);
    initCallStack();
    return evaluate(ast);
}
queue<Token *> Slowjs::tokenize(string input)
{
    Tokenizer tokenizer = Tokenizer();
    return tokenizer.tokenize(input);
}
AST_Node *Slowjs::parse(queue<Token *> token_queue)
{
    Parser parser = Parser();
    return parser.parse(token_queue);
}

void Slowjs::initCallStack()
{
    ctx_stack = new stack<Execution_Context *>;
};

Execution_Context *Slowjs::getCurrentContext()
{
    return ctx_stack->top();
}
void Slowjs::initFunctionExecutionContext(JSFunction *fo, JSValue thisValue, vector<JSValue> args)
{
    AST_Node *codeNode = fo->Code;
    Lexical_Environment *outer = fo->Scope;
    Lexical_Environment *local_env = new Lexical_Environment();
    local_env->outer = outer;
    local_env->record = new Declarative_ER();

    Execution_Context *func_ctx = new Execution_Context();
    func_ctx->lex_env = local_env;
    func_ctx->var_env = local_env;
    func_ctx->this_binding = thisValue;

    ctx_stack->push(func_ctx);
    declarationBindingInstantiation(codeNode, args);
}
void Slowjs::addIntrinsic()
{
    Environment_Record *record = global_ctx->var_env->record;
    string fn_name = "print";
    JSFunction *fo = new JSFunction(fn_name);
    JSValue value = JSValue(JS_TAG_FUNCTION, fo);
    record->CreateMutableBinding(fn_name);
    record->SetMutableBinding(fn_name, value);
}
void Slowjs::initGlobalExecutionContext(AST_Node *node)
{
    Lexical_Environment *global_env = new Lexical_Environment();
    global_env->outer = nullptr;
    global_env->record = new Object_ER();

    global_ctx = new Execution_Context();
    global_ctx->lex_env = global_env;
    global_ctx->var_env = global_env;
    global_ctx->this_binding = JS_UNDEFINED;

    ctx_stack->push(global_ctx);
    addIntrinsic();
    vector<JSValue> placeholder;
    declarationBindingInstantiation(node, placeholder);
}

void Slowjs::declarationBindingInstantiation(AST_Node *node, vector<JSValue> args)
{
    Execution_Context *running_ctx = getCurrentContext();
    Environment_Record *env = running_ctx->var_env->record;
    vector<AST_Node *> top_levels;
    if (node->type == nt::Program)
    {
        top_levels = node->childs;
    }
    else if (node->type == nt::FunctionDeclaration)
    {
        top_levels = node->childs[2]->childs;
        AST_Node *names = node->childs[1];
        size_t argCount = args.size();
        for (size_t i = 0; i < names->childs.size(); i++)
        {
            AST_Node *argNameNode = names->childs[i];
            string argName = argNameNode->value;
            JSValue v = JS_UNDEFINED;
            if (i < argCount)
            {
                v = args[i];
            }
            else
            {
                v = JS_UNDEFINED;
            }
            bool argAlreadyDeclared = env->HasBinding(argName);
            if (!argAlreadyDeclared)
            {
                env->CreateMutableBinding(argName);
            }
            env->SetMutableBinding(argName, v);
        }
    }

    for (size_t i = 0; i < top_levels.size(); i++)
    {
        AST_Node *top_level = top_levels[i];
        nt::Node_Type stmt_type = top_level->type;

        if (stmt_type == nt::FunctionDeclaration)
        {
            AST_Node *identifier = top_level->childs[0];
            AST_Node *formal_param = top_level->childs[1];
            string fn = identifier->value;
            JSFunction *fo = new JSFunction(formal_param, top_level, running_ctx->var_env, fn);
            JSValue f = JSValue(JS_TAG_FUNCTION, fo);
            env->CreateMutableBinding(fn);
            env->SetMutableBinding(fn, f);
        }
        else if (stmt_type == nt::VariableDeclaration || stmt_type == nt::ForStatement || stmt_type == nt::IfStatement)
        {
            vector<AST_Node *> arr;
            if (stmt_type == nt::VariableDeclaration)
            {
                arr.push_back(top_level);
            }
            else if (stmt_type == nt::ForStatement)
            {
                arr.push_back(top_level->childs[0]);
            }
            else if (stmt_type == nt::IfStatement)
            {
                AST_Node *blockSeqs = top_level->childs[1];
                for (size_t j = 0; j < blockSeqs->childs.size(); j++)
                {
                    AST_Node *seq_stmt = blockSeqs->childs[j];
                    if (seq_stmt->type == nt::VariableDeclaration)
                    {
                        arr.push_back(seq_stmt);
                    }
                }
                if (top_level->childs.size() == 3)
                {
                    AST_Node *blockAlts = top_level->childs[2];
                    for (size_t j = 0; j < blockAlts->childs.size(); j++)
                    {
                        AST_Node *alt_stmt = blockAlts->childs[j];
                        if (alt_stmt->type == nt::VariableDeclaration)
                        {
                            arr.push_back(alt_stmt);
                        }
                    }
                }
            }

            for (size_t k = 0; k < arr.size(); k++)
            {
                AST_Node *AsignNode = arr[k]->childs[0];
                string dn = AsignNode->childs[0]->value;
                bool varAlreadyDeclared = env->HasBinding(dn);
                if (!varAlreadyDeclared)
                {
                    env->CreateMutableBinding(dn);
                    env->SetMutableBinding(dn, JS_UNDEFINED);
                }
            }
        }
    }
}

void Slowjs::checkException(JSValue value)
{
    if (value.isException())
    {
        string msg = value.getException();
        throwRuntimeException(EXCEPTION_TYPE, msg);
    }
}

// evaluate
JSValue Slowjs::evaluate(AST_Node *node)
{
    switch (node->type)
    {
    case nt::Program:
        return evaluateProgram(node);
    case nt::EmptyStatement:
        return evaluateEmptyStatement(node);
    case nt::ExpressionStatement:
        return evaluateExpressionStatement(node);
    case nt::VariableDeclaration:
        return evaluateVariableDeclaration(node);
    case nt::BinaryExpression:
        return evaluateBinaryExpression(node);
    case nt::Literal:
        return evaluateLiteral(node);
    case nt::AssignmentExpression:
        return evaluateAssignmentExpression(node);
    case nt::Identifier:
        return evaluateIdentifier(node);
    case nt::IfStatement:
        return evaluateIfStatement(node);
    case nt::BlockStatement:
        return evaluateBlockStatement(node);
    case nt::ForStatement:
        return evaluateForStatement(node);
    case nt::UpdateExpression:
        return evaluateUpdateExpression(node);
    case nt::CallExpression:
        return evaluateCallExpression(node);
    case nt::NewExpression:
        return evaluateNewExpression(node);
    case nt::ReturnStatement:
        return evaluateReturnStatement(node);
    case nt::BreakStatement:
        return evaluateBreakStatement(node);
    case nt::ContinueStatement:
        return evaluateContinueStatement(node);
    case nt::UnaryExpression:
        return evaluateUnaryExpression(node);
    default:
        throw string("Unknown AST Node, tip: console.log is not available");
    }
}

JSValue Slowjs::evaluateEmptyStatement(AST_Node *)
{
    return JS_UNDEFINED;
}

JSValue Slowjs::evaluateProgram(AST_Node *node)
{
    initGlobalExecutionContext(node);
    vector<AST_Node *> childs = node->childs;
    JSValue result;
    for (size_t i = 0; i < childs.size(); i++)
    {
        AST_Node *stmt = childs[i];
        if (stmt->type == nt::FunctionDeclaration)
        {
            continue;
        }
        else
        {
            result = evaluate(stmt);
            checkException(result);
        }
    }
    return result;
}

JSValue Slowjs::evaluateVariableDeclaration(AST_Node *node)
{
    evaluate(node->childs[0]);
    return JS_UNDEFINED;
}

JSValue Slowjs::evaluateExpressionStatement(AST_Node *node)
{
    AST_Node *expression = node->childs[0];
    return evaluate(expression);
}

JSValue Slowjs::evaluateUnaryExpression(AST_Node *node)
{
    string op = node->value;
    AST_Node *expre = node->childs[0];
    JSValue result = this->evaluate(expre);
    if (result.isBoolean())
        return JSValue(JS_TAG_BOOLEAN, !result.getBoolean());
    else if (result.isNumber())
        return JSValue(JS_TAG_BOOLEAN, !result.getNumber());
    else if (result.isString())
        return JSValue(JS_TAG_BOOLEAN, result.getString().size() == 0);
    else
        return JSValue(JS_TAG_EXCEPTION, string("Support only boolean number and string."));
}

JSValue Slowjs::evaluateBinaryExpression(AST_Node *node)
{
    string op = node->value;
    vector<AST_Node *> fields = node->childs;
    JSValue left = evaluate(fields[0]);
    JSValue right = evaluate(fields[1]);

    checkException(left);
    checkException(right);

    if (left.isNumber() && right.isNumber())
    {
        double d_left = left.getNumber();
        double d_right = right.getNumber();

        double number_res = 0;
        bool bool_res = false;

        if (op == "+")
            number_res = d_left + d_right;
        else if (op == "-")
            number_res = d_left - d_right;
        else if (op == "*")
            number_res = d_left * d_right;
        else if (op == "/")
            number_res = d_left / d_right;
        else if (op == ">")
            bool_res = d_left > d_right;
        else if (op == ">=")
            bool_res = d_left >= d_right;
        else if (op == "<")
            bool_res = d_left < d_right;
        else if (op == "<=")
            bool_res = d_left <= d_right;
        else if (op == "==")
            bool_res = d_left == d_right;
        else if (op == "===")
            bool_res = d_left == d_right;
        else if (op == "&&")
            bool_res = d_left && d_right;
        else if (op == "||")
            bool_res = d_left || d_right;
        else
            return JSValue(JS_TAG_EXCEPTION, string("Unsupported Operator '") + op + "'");

        if (op == "+" || op == "-" || op == "*" || op == "/")
            return JSValue(JS_TAG_NUMBER, number_res);
        else
            return JSValue(JS_TAG_BOOLEAN, bool_res);
    }
    else if (left.isString() && right.isString() && op == "+")
    {
        return JSValue(JS_TAG_STRING, left.getString() + right.getString());
    }
    else
        return JSValue(JS_TAG_EXCEPTION, string("Operand must be number or can be string if operator is '+' !"));
}

JSValue getJSValueFromLiteralNode(AST_Node *node)
{
    string value = node->value;
    switch (node->rawType)
    {
    case rt::Boolean:
        return JSValue(JS_TAG_BOOLEAN, value == "true" ? true : false);
    case rt::Number:
        return JSValue(JS_TAG_NUMBER, stod(value));
    case rt::String:
        return JSValue(JS_TAG_STRING, value);
    case rt::Undefined:
        return JS_UNDEFINED;
    case rt::Null:
        return JS_NULL;
    default:
        return JSValue(JS_TAG_EXCEPTION, string("Literal support only Boolean, Number and String"));
    }
}
JSValue Slowjs::evaluateLiteral(AST_Node *node)
{
    return getJSValueFromLiteralNode(node);
}

JSValue Slowjs::evaluateAssignmentExpression(AST_Node *node)
{
    vector<AST_Node *> fields = node->childs;
    AST_Node *left_node = fields[0];

    Lexical_Environment *lex = getCurrentContext()->lex_env;
    Reference lhs = IdentifierResolution(lex, left_node->value);

    AST_Node *right_node = fields[1];
    JSValue value = evaluate(right_node);
    checkException(value);

    PutValue(lhs, value);
    return value;
}

JSValue Slowjs::evaluateIdentifier(AST_Node *node)
{
    Lexical_Environment *lex = getCurrentContext()->lex_env;
    Reference ref = IdentifierResolution(lex, node->value);
    return GetValue(ref);
}

JSValue Slowjs::evaluateIfStatement(AST_Node *node)
{
    vector<AST_Node *> fields = node->childs;
    JSValue test = evaluate(fields[0]);
    checkException(test);
    if ((test.isBoolean() && test.getBoolean()) || (test.isNumber() && test.getNumber() != 0))
    {
        return evaluate(fields[1]);
    }
    else if (fields.size() == 3)
    {
        return evaluate(fields[2]);
    }
    else
        return JS_UNDEFINED;
}

JSValue Slowjs::evaluateBlockStatement(AST_Node *node)
{
    vector<AST_Node *> fields = node->childs;
    JSValue result;
    for (size_t i = 0; i < fields.size(); i++)
    {
        if (fields[i]->type == nt::FunctionDeclaration)
        {
            continue;
        }
        else
        {
            result = evaluate(fields[i]);
            checkException(result);
        }
    }
    return result;
}
JSValue Slowjs::evaluateForStatement(AST_Node *node)
{
    vector<AST_Node *> fields = node->childs;
    AST_Node *assign = fields[0]->childs[0];
    AST_Node *test = fields[1];
    AST_Node *update = fields[2];
    AST_Node *block = fields[3];

    JSValue result;

    JSValue assignValue = evaluate(assign);
    checkException(assignValue);

    JSValue testValue = evaluate(test);
    checkException(testValue);
    bool condition = testValue.getBoolean();

    JSValue updateValue;

    while (condition)
    {
        try
        {
            result = evaluate(block);
            checkException(result);
        }
        catch (string &value)
        {
            if (value == "break")
                break;
            else if (value == "continue")
            {
                updateValue = evaluate(update);
                checkException(updateValue);

                testValue = evaluate(test);
                checkException(testValue);
                condition = testValue.getBoolean();

                continue;
            }
        }

        updateValue = evaluate(update);
        checkException(updateValue);

        testValue = evaluate(test);
        checkException(testValue);
        condition = testValue.getBoolean();
    }
    return result;
}

JSValue Slowjs::evaluateUpdateExpression(AST_Node *node)
{
    Lexical_Environment *lex = getCurrentContext()->lex_env;
    Reference lhs = IdentifierResolution(lex, node->childs[0]->value);
    string op = node->value;

    JSValue oldValue = GetValue(lhs);
    checkException(oldValue);
    if (oldValue.isNumber())
    {
        double v = op == "++" ? oldValue.getNumber() + 1 : oldValue.getNumber() - 1;
        JSValue newValue = JSValue(JS_TAG_NUMBER, v);
        PutValue(lhs, newValue);
        return newValue;
    }
    else
        return JSValue(JS_TAG_EXCEPTION, string("UpdateExpression only support Number"));
}

vector<JSValue> Slowjs::getArgumentList(AST_Node *node)
{
    vector<JSValue> vec;
    for (size_t i = 0; i < node->childs.size(); i++)
    {
        JSValue v = evaluate(node->childs[i]);
        checkException(v);
        vec.push_back(v);
    }
    return vec;
}
JSValue Slowjs::evaluateIntrinsicFunction(string fnName, vector<JSValue> argVector)
{
    if (fnName == "print")
    {
        for (size_t i = 0; i < argVector.size(); i++)
        {
            intrinsicPrint(argVector[i]);
        }
        cout << endl;
        return JS_UNDEFINED;
    }
    else
        return JSValue(JS_TAG_EXCEPTION, string("'" + fnName + "' is not a function"));
}
JSValue Slowjs::evaluateCallExpression(AST_Node *node)
{
    vector<AST_Node *> fields = node->childs;
    AST_Node *left = fields[0];
    AST_Node *arguments = fields[1];
    vector<JSValue> argVector;
    if (arguments->childs.size() > 0)
        argVector = getArgumentList(arguments);

    Reference ref = evaluateCallExpressionLeft(left);
    JSValue result = GetValue(ref);

    if (result.isFunction())
    {
        JSFunction *fo = result.getFunction();
        if (fo->isIntrinsic())
        {
            return evaluateIntrinsicFunction(fo->Name, argVector);
        }
        else
        {
            JSValue thisValue = JS_UNDEFINED;
            if (IsPropertyReference(ref))
            {
                thisValue = *GetBase(ref).js_value;
            }
            return fo->Call(this, thisValue, argVector);
        }
    }
    else
        return JSValue(JS_TAG_EXCEPTION, string("'" + GetReferencedName(ref) + "' is not a function"));
}
JSValue Slowjs::evaluateNewExpression(AST_Node *node)
{
    return JSValue();
}
JSValue Slowjs::evaluateReturnStatement(AST_Node *node)
{
    throw evaluate(node->childs[0]);
}
JSValue Slowjs::evaluateBreakStatement(AST_Node *)
{
    throw string("break");
}
JSValue Slowjs::evaluateContinueStatement(AST_Node *)
{
    throw string("continue");
}
vector<string> getIdentifierFromMemberExpression(AST_Node *node, vector<string> idArr)
{
    vector<AST_Node *> fields = node->childs;
    if (fields[0]->type == nt::Identifier)
    {
        idArr.push_back(fields[0]->value);
    }

    if (fields[1]->type == nt::Identifier)
    {
        idArr.push_back(fields[1]->value);
        return idArr;
    }
    else if (fields[1]->type == nt::MemberExpression)
    {
        return getIdentifierFromMemberExpression(fields[1], idArr);
    }
    else
        throw string("getIdentifierFromMemberExpression");
}
Reference Slowjs::evaluateCallExpressionLeft(AST_Node *node)
{
    vector<string> idArr;
    if (node->type == nt::Identifier)
    {
        return IdentifierResolution(getCurrentContext()->lex_env, node->value);
    }
    else if (node->type == nt::MemberExpression)
    {
        idArr = getIdentifierFromMemberExpression(node, idArr);
    }

    JSValue temp;
    Lexical_Environment *lex = getCurrentContext()->lex_env;
    size_t count = idArr.size();
    for (size_t i = 0; i < count; i++)
    {
        string name = idArr[i];
        if (i == 0)
        {
            Reference ref = IdentifierResolution(lex, name);
            temp = GetValue(ref);
            if (!temp.isObject())
                throw string("is not a Object");
        }
        else if (i == count - 1)
        {
            JSValue *v = new JSValue();
            *v = temp;
            return Reference(v, name);
        }
        else
        {
            JSObject *obj = temp.getObject();
            JSValue res = obj->GetProperty(name).Value;
            if (res.isObject())
            {
                temp = res;
            }
            else
                throw string("is not a Object");
        }
    }
    throw string("SyntaxError");
}
