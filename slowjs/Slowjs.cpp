//
//  Slowjs.cpp
//  slowjs
//
//  Created by zx on 2021/4/20.
//

#include "Slowjs.hpp"
#include <iostream>
#include <fstream>

string Slowjs::getContentFromFile(const string &fileName)
{
    ifstream in(fileName);
    if (!in)
        throw string("error: open file fail! file does not exist");

    istreambuf_iterator<char> begin(in);
    istreambuf_iterator<char> end;
    string code(begin, end);
    in.close();
    return code;
}
JSValue Slowjs::evalFile(const string &fileName)
{
    return eval(getContentFromFile(fileName));
}
JSValue Slowjs::eval(const string &input)
{
    return evaluate(parse(tokenize(input)));
}
queue<Token *> Slowjs::tokenize(const string &input)
{
    return Tokenizer().tokenize(input);
}
AST_Node *Slowjs::parse(const queue<Token *> &token_queue)
{
    return Parser().parse(token_queue);
}

Execution_Context *Slowjs::getCurrentContext()
{
    return ctx_stack->top();
}
void Slowjs::addIntrinsic()
{
    global_obj->Put("global", global_obj->ToJSValue());
    global_obj->Put("Object", JSObject::Object->ToJSValue());
    global_obj->Put("Function", JSObject::Function->ToJSValue());
    global_obj->Put("Array", JSObject::Array->ToJSValue());

    JSFunction *Worker_fo = new JSFunction("Worker", (void *)Builtin_Worker);
    global_obj->Put("Worker", Worker_fo->ToJSValue());

    JSObject *process = new JSObject();
    global_obj->Put("process", process->ToJSValue());
    JSFunction *nextTick_fo = new JSFunction("nextTick", (void *)Builtin_Process_Nexttick);
    process->Put("nextTick", nextTick_fo->ToJSValue());

    JSFunction *setTimeout_fo = new JSFunction("setTimeout", (void *)Builtin_SetTimeout);
    global_obj->Put("setTimeout", setTimeout_fo->ToJSValue());

    JSObject *console = new JSObject();
    JSFunction *clog_JSFunction = new JSFunction("log", (void *)Builtin_Console_log);
    console->Put("log", clog_JSFunction->ToJSValue());
    global_obj->Put("console", console->ToJSValue());

    JSFunction *cprint_JSFunction = new JSFunction("print", (void *)Builtin_Console_log);
    global_obj->Put("print", cprint_JSFunction->ToJSValue());
}
void Slowjs::initFunctionExecutionContext(JSFunction *fo, const JSValue &thisValue, const vector<JSValue> &args)
{
    Lexical_Environment *outer = fo->Scope;
    Lexical_Environment *local_env = new Lexical_Environment(new Declarative_ER(), outer);

    Execution_Context *func_ctx = new Execution_Context(local_env, thisValue);

    ctx_stack->push(func_ctx);
    declarationBindingInstantiation(fo->Code, args);
}
void Slowjs::initGlobalExecutionContext(AST_Node *node)
{
    Lexical_Environment *global_env = new Lexical_Environment(new Object_ER(global_obj), nullptr);
    global_ctx = new Execution_Context(global_env, JS_UNDEFINED);
    ctx_stack->push(global_ctx);

    vector<JSValue> placeholder;
    declarationBindingInstantiation(node, placeholder);
}

JSFunction *Slowjs::CreateFunctionObject(AST_Node *node)
{
    AST_Node *identifier = node->childs[0];
    AST_Node *formal_param = node->childs[1];
    string fn_name = identifier ? identifier->value : "";
    return new JSFunction(fn_name, formal_param, node, getCurrentContext()->var_env);
}
void Slowjs::declarationBindingInstantiation(AST_Node *node, const vector<JSValue> &args)
{
    Execution_Context *running_ctx = getCurrentContext();
    Environment_Record *env = running_ctx->var_env->record;
    vector<AST_Node *> top_levels;
    if (node->type == nt::Program)
        top_levels = node->childs;
    else if (node->type == nt::FunctionDeclaration || node->type == nt::FunctionExpression)
    {
        top_levels = node->childs[2]->childs;
        AST_Node *names = node->childs[1];
        size_t argCount = args.size();
        for (size_t i = 0; i < names->childs.size(); i++)
        {
            AST_Node *argNameNode = names->childs[i];
            string argName = argNameNode->value;
            JSValue v = i < argCount ? args[i] : JS_UNDEFINED;
            bool argAlreadyDeclared = env->HasBinding(argName);
            if (!argAlreadyDeclared)
                env->CreateMutableBinding(argName);

            env->SetMutableBinding(argName, v);
        }
    }

    for (size_t i = 0; i < top_levels.size(); i++)
    {
        AST_Node *top_level = top_levels[i];
        nt::Node_Type stmt_type = top_level->type;

        if (stmt_type == nt::FunctionDeclaration)
        {
            string fn = top_level->childs[0]->value;
            JSFunction *fo = CreateFunctionObject(top_level);
            env->CreateMutableBinding(fn);
            env->SetMutableBinding(fn, fo->ToJSValue());
        }
        else if (stmt_type == nt::VariableDeclaration || stmt_type == nt::ForStatement || stmt_type == nt::IfStatement)
        {
            vector<AST_Node *> declarations;
            if (stmt_type == nt::VariableDeclaration)
                declarations.push_back(top_level);
            else if (stmt_type == nt::ForStatement)
            {
                if (top_level->childs[0]->type == nt::VariableDeclaration)
                    declarations.push_back(top_level->childs[0]);
            }
            else if (stmt_type == nt::IfStatement)
            {
                AST_Node *blockSeqs = top_level->childs[1];
                for (size_t j = 0; j < blockSeqs->childs.size(); j++)
                {
                    AST_Node *seq_stmt = blockSeqs->childs[j];
                    if (seq_stmt->type == nt::VariableDeclaration)
                        declarations.push_back(seq_stmt);
                }
                if (top_level->childs.size() == 3)
                {
                    AST_Node *blockAlts = top_level->childs[2];
                    for (size_t j = 0; j < blockAlts->childs.size(); j++)
                    {
                        AST_Node *alt_stmt = blockAlts->childs[j];
                        if (alt_stmt->type == nt::VariableDeclaration)
                            declarations.push_back(alt_stmt);
                    }
                }
            }

            for (size_t k = 0; k < declarations.size(); k++)
            {
                AST_Node *AsignNode = declarations[k]->childs[0];
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

void Slowjs::checkException(const JSValue &value)
{
    if (value.isException())
        ThrowRuntimeException(EXCEPTION_TYPE, value.getString());
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
    case nt::ThisExpression:
        return evaluateThisExpression(node);
    case nt::MemberExpression:
        return evaluateMemberExpression(node);
    case nt::FunctionExpression:
        return evaluateFunctionExpression(node);
    case nt::ThrowStatement:
        return evaluateThrowStatement(node);
    case nt::DoWhileStatement:
        return evaluateDoWhileStatement(node);
    case nt::WhileStatement:
        return evaluateWhileStatement(node);
    case nt::ConditionalExpression:
        return evaluateConditionalExpression(node);
    case nt::ArrayExpression:
        return evaluateArrayExpression(node);
    case nt::ObjectExpression:
        return evaluateObjectExpression(node);
    default:
        throw string("Unknown AST Node");
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
    JSValue result = JS_UNDEFINED;
    for (size_t i = 0; i < childs.size(); i++)
    {
        AST_Node *stmt = childs[i];
        if (stmt->type == nt::FunctionDeclaration)
            continue;
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
    AST_Node *expr = node->childs[0];
    JSValue result = this->evaluate(expr);
    JSValue oldValue = ToBoolean(result);

    if (op == "!")
        return oldValue.getBoolean() ? JS_FALSE : JS_TRUE;
    else
        return JSException("Unsupported Type in UnaryExpression").ToJSValue();
}

JSValue strictEqualityComparison(JSValue left, JSValue right)
{
    // https://262.ecma-international.org/5.1/#sec-11.9.6
    if (left.getTag() != right.getTag())
        return JS_FALSE;
    else if (left.isUndefined())
        return JS_TRUE;
    else if (left.isNull())
        return JS_TRUE;
    else if (left.isNumber())
        // TODO:
        return JSBoolean(left.getNumber() == right.getNumber()).ToJSValue();
    else if (left.isString())
        return JSBoolean(left.getString() == right.getString()).ToJSValue();
    else if (left.isBoolean())
        return JSBoolean(left.getBoolean() == right.getBoolean()).ToJSValue();
    else if (left.isObject())
        return JSBoolean(left.getPtr() == right.getPtr()).ToJSValue();
    else
        return JSException("Unsupported Operator or Operand").ToJSValue();
}
JSValue Slowjs::evaluateBinaryExpression(AST_Node *node)
{
    string op = node->value;
    vector<AST_Node *> fields = node->childs;
    JSValue left = evaluate(fields[0]);

    if (op == "&&")
        return !ToBoolean(left).getBoolean() ? left : evaluate(fields[1]);
    else if (op == "||")
        return ToBoolean(left).getBoolean() ? left : evaluate(fields[1]);

    JSValue right = evaluate(fields[1]);

    checkException(left);
    checkException(right);

    if (op == "==" || op == "!=")
        // TODO: https://262.ecma-international.org/5.1/#sec-11.9.1
        cerr << "'==' '!=' is not currently supported, now has the same effect as '===' '!=='. please use '===' '!==' instead" << endl;

    if (op == "===" || op == "==")
        return strictEqualityComparison(left, right);
    else if (op == "!==" || op == "!=")
    {
        JSValue r = strictEqualityComparison(left, right);
        return JSBoolean(!r.getBoolean()).ToJSValue();
    }
    else if (op == "+")
    {
        // TODO: https://262.ecma-international.org/5.1/#sec-11.6.1
        JSValue lp = ToPrimitive(left);
        JSValue rp = ToPrimitive(right);
        if (lp.isNumber() && rp.isNumber())
            return JSNumber(lp.getNumber() + rp.getNumber()).ToJSValue();
        else if (lp.isString() && rp.isString())
            return JSString(lp.getString() + rp.getString()).ToJSValue();
        else
            return JSException("Unsupported Type in '+'").ToJSValue();
    }
    else if (op == "-")
    {
        JSValue ln = ToNumber(left);
        JSValue rn = ToNumber(right);
        return JSNumber(ln.getNumber() - rn.getNumber()).ToJSValue();
    }
    else if (op == "*")
    {
        JSValue ln = ToNumber(left);
        JSValue rn = ToNumber(right);
        return JSNumber(ln.getNumber() * rn.getNumber()).ToJSValue();
    }
    else if (op == "/")
    {
        JSValue ln = ToNumber(left);
        JSValue rn = ToNumber(right);
        return JSNumber(ln.getNumber() / rn.getNumber()).ToJSValue();
    }
    else if (op == ">")
    {
        JSValue ln = ToNumber(left);
        JSValue rn = ToNumber(right);
        return JSBoolean(ln.getNumber() > rn.getNumber()).ToJSValue();
    }
    else if (op == ">=")
    {
        JSValue ln = ToNumber(left);
        JSValue rn = ToNumber(right);
        return JSBoolean(ln.getNumber() >= rn.getNumber()).ToJSValue();
    }
    else if (op == "<")
    {
        JSValue ln = ToNumber(left);
        JSValue rn = ToNumber(right);
        return JSBoolean(ln.getNumber() < rn.getNumber()).ToJSValue();
    }
    else if (op == "<=")
    {
        JSValue ln = ToNumber(left);
        JSValue rn = ToNumber(right);
        return JSBoolean(ln.getNumber() <= rn.getNumber()).ToJSValue();
    }
    else
        return JSException("Unsupported Operator or Operand").ToJSValue();
}

JSValue getJSValueFromLiteralNode(AST_Node *node)
{
    string value = node->value;
    switch (node->rawType)
    {
    case rt::Boolean:
        return JSBoolean(value == "true" ? true : false).ToJSValue();
    case rt::Number:
        return JSNumber(stod(value)).ToJSValue();
    case rt::String:
        return JSString(value).ToJSValue();
    case rt::Undefined:
        return JS_UNDEFINED;
    case rt::Null:
        return JS_NULL;
    default:
        return JSException("Literal support only Boolean, Number and String").ToJSValue();
    }
}
JSValue Slowjs::evaluateLiteral(AST_Node *node)
{
    return getJSValueFromLiteralNode(node);
}

Reference Slowjs::getReference(AST_Node *node)
{
    switch (node->type)
    {
    case nt::Identifier:
        return IdentifierResolution(getCurrentContext()->lex_env, node->value);
    case nt::MemberExpression:
        return getMemberExpressionReference(node);
    default:
        throw ThrowRuntimeException(EXCEPTION_TYPE, "Unknown Reference");
    }
}
JSValue Slowjs::evaluateAssignmentExpression(AST_Node *node)
{
    vector<AST_Node *> fields = node->childs;

    AST_Node *left_node = fields[0];
    AST_Node *right_node = fields[1];

    Reference lhs = getReference(left_node);
    JSValue rValue = JS_UNDEFINED;
    if (right_node->type == nt::Identifier || right_node->type == nt::MemberExpression)
        rValue = GetValue(getReference(right_node));
    else
        rValue = this->evaluate(right_node);

    checkException(rValue);

    PutValue(lhs, rValue);
    return rValue;
}

JSValue Slowjs::evaluateIdentifier(AST_Node *node)
{
    Lexical_Environment *lex = getCurrentContext()->lex_env;
    Reference ref = IdentifierResolution(lex, node->value);
    return GetValue(ref);
}

JSValue Slowjs::evaluateConditionalExpression(AST_Node *node)
{
    return evaluateIfStatement(node);
}
JSValue Slowjs::evaluateIfStatement(AST_Node *node)
{
    vector<AST_Node *> fields = node->childs;
    JSValue testValue = evaluate(fields[0]);
    checkException(testValue);
    if (ToBoolean(testValue).getBoolean())
        return evaluate(fields[1]);
    else if (fields.size() == 3)
        return evaluate(fields[2]);
    else
        return JS_UNDEFINED;
}

JSValue Slowjs::evaluateBlockStatement(AST_Node *node)
{
    vector<AST_Node *> fields = node->childs;
    JSValue result = JS_UNDEFINED;
    for (size_t i = 0; i < fields.size(); i++)
    {
        if (fields[i]->type == nt::FunctionDeclaration)
            continue;
        else
        {
            result = evaluate(fields[i]);
            checkException(result);
        }
    }
    return result;
}
JSValue Slowjs::evaluateDoWhileStatement(AST_Node *node)
{
    vector<AST_Node *> fields = node->childs;
    AST_Node *block = fields[0];
    AST_Node *expr = fields[1];

    JSValue result = JS_UNDEFINED;

    JSValue testValue = evaluate(expr);
    checkException(testValue);
    bool condition = ToBoolean(testValue).getBoolean();

    do
    {
        try
        {
            result = evaluate(block);
            checkException(result);
        }
        catch (string &msg)
        {
            if (msg == "break")
                break;
            else if (msg == "continue")
            {
                testValue = evaluate(expr);
                checkException(testValue);
                condition = ToBoolean(testValue).getBoolean();

                continue;
            }
            else
                return JSException("evaluateDoWhileStatement").ToJSValue();
        }

        testValue = evaluate(expr);
        checkException(testValue);
        condition = ToBoolean(testValue).getBoolean();

    } while (condition);

    return result;
}
JSValue Slowjs::evaluateWhileStatement(AST_Node *node)
{
    vector<AST_Node *> fields = node->childs;
    AST_Node *expr = fields[0];
    AST_Node *block = fields[1];

    JSValue result = JS_UNDEFINED;

    JSValue testValue = evaluate(expr);
    checkException(testValue);
    bool condition = ToBoolean(testValue).getBoolean();

    while (condition)
    {
        try
        {
            result = evaluate(block);
            checkException(result);
        }
        catch (string &msg)
        {
            if (msg == "break")
                break;
            else if (msg == "continue")
            {
                testValue = evaluate(expr);
                checkException(testValue);
                condition = ToBoolean(testValue).getBoolean();

                continue;
            }
            else
                return JSException("evaluateWhileStatement").ToJSValue();
        }

        testValue = evaluate(expr);
        checkException(testValue);
        condition = ToBoolean(testValue).getBoolean();
    }

    return result;
}
JSValue Slowjs::evaluateForStatement(AST_Node *node)
{
    vector<AST_Node *> fields = node->childs;
    AST_Node *first = fields[0];
    AST_Node *test = fields[1];
    AST_Node *update = fields[2];
    AST_Node *block = fields[3];

    JSValue result = JS_UNDEFINED;

    JSValue firstValue = evaluate(first);
    checkException(firstValue);

    JSValue testValue = evaluate(test);
    checkException(testValue);
    bool condition = ToBoolean(testValue).getBoolean();

    JSValue updateValue = JS_UNDEFINED;

    while (condition)
    {
        try
        {
            result = evaluate(block);
            checkException(result);
        }
        catch (string &msg)
        {
            if (msg == "break")
                break;
            else if (msg == "continue")
            {
                updateValue = evaluate(update);
                checkException(updateValue);

                testValue = evaluate(test);
                checkException(testValue);
                condition = ToBoolean(testValue).getBoolean();

                continue;
            }
            else
                return JSException("evaluateForStatement").ToJSValue();
        }

        updateValue = evaluate(update);
        checkException(updateValue);

        testValue = evaluate(test);
        checkException(testValue);
        condition = ToBoolean(testValue).getBoolean();
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
    if (!oldValue.isNumber())
        return JSException("UpdateExpression only support Number").ToJSValue();

    double v = op == "++" ? oldValue.getNumber() + 1 : oldValue.getNumber() - 1;
    JSValue newValue = JSNumber(v).ToJSValue();
    PutValue(lhs, newValue);
    return newValue;
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
JSValue Slowjs::evaluateCallExpression(AST_Node *node)
{
    vector<AST_Node *> fields = node->childs;
    AST_Node *left = fields[0];
    AST_Node *arguments = fields[1];
    vector<JSValue> argVector;
    if (arguments->childs.size() > 0)
        argVector = getArgumentList(arguments);

    Reference ref = getReference(left);
    JSValue result = GetValue(ref);

    if (!result.isFunction())
        return JSException("'" + GetReferencedName(ref) + "' is not a function").ToJSValue();

    JSFunction *fo = result.getFunction();
    JSValue thisValue = JS_UNDEFINED;
    if (IsPropertyReference(ref))
        thisValue = *GetBase(ref).js_value;

    return fo->Call(this, thisValue, argVector);
}
JSValue Slowjs::evaluateNewExpression(AST_Node *node)
{
    vector<AST_Node *> fields = node->childs;
    AST_Node *left = fields[0];
    AST_Node *arguments = fields[1];
    vector<JSValue> argVector;
    JSValue v = evaluate(node->childs[0]);

    if (arguments->childs.size() > 0)
        argVector = getArgumentList(arguments);

    Reference ref = getReference(left);
    JSValue result = GetValue(ref);

    if (!result.isFunction())
        return JSException("not a construtor").ToJSValue();

    JSFunction *ctor = result.getFunction();
    return ctor->Construct(this, argVector);
}
JSValue Slowjs::evaluateReturnStatement(AST_Node *node)
{
    throw node->childs.size() == 0 ? JS_UNDEFINED : evaluate(node->childs[0]);
}
JSValue Slowjs::evaluateBreakStatement(AST_Node *)
{
    throw string("break");
}
JSValue Slowjs::evaluateContinueStatement(AST_Node *)
{
    throw string("continue");
}
JSValue Slowjs::evaluateThisExpression(AST_Node *node)
{
    return getCurrentContext()->this_binding;
}
Reference Slowjs::getMemberExpressionReference(AST_Node *node)
{
    JSValue base = evaluate(node->childs[0]);
    AST_Node *right = node->childs[1];
    string propertyString = node->computed ? ToString(evaluate(right)).getString() : right->value;

    return Reference(new JSValue(base), propertyString);
}
JSValue Slowjs::evaluateMemberExpression(AST_Node *node)
{
    return GetValue(getMemberExpressionReference(node));
}
JSValue Slowjs::evaluateFunctionExpression(AST_Node *node)
{
    return CreateFunctionObject(node)->ToJSValue();
}
JSValue Slowjs::evaluateThrowStatement(AST_Node *node)
{
    JSValue expr = evaluate(node->childs[0]);
    if (!expr.isString())
        return JSException("ThrowStatement Supported only string currently").ToJSValue();

    throw expr.getString();
}
JSValue Slowjs::evaluateArrayExpression(AST_Node *node)
{
    vector<JSValue> args;
    JSValue arr_value = JSObject::Array->Construct(this, args);
    vector<AST_Node *> list = node->childs;
    int len = (int)(list.size());
    JSArray *arr_o = arr_value.getArray();
    arr_o->Put("length", JSNumber(len).ToJSValue());
    for (size_t i = 0; i < len; i++)
    {
        string key = ToString(JSNumber((int)i).ToJSValue()).getString();
        AST_Node *value_node = list[i];
        JSValue value = value_node ? evaluate(value_node) : JS_UNDEFINED;
        arr_o->Put(key, value);
    }

    return arr_value;
}
JSValue Slowjs::evaluateObjectExpression(AST_Node *node)

{
    vector<JSValue> args;
    JSValue obj_value = JSObject::Object->Construct(this, args);
    if (node->childs.size() == 0)
        return obj_value;

    vector<AST_Node *> kvs = node->childs;
    JSObject *obj_o = obj_value.getObject();
    for (size_t i = 0; i < kvs.size(); i++)
    {
        string key;
        AST_Node *key_node = kvs[i]->childs[0];
        if (key_node->type == nt::Identifier)
            key = key_node->value;
        else
            key = ToString(evaluate(key_node)).getString();

        AST_Node *value_node = kvs[i]->childs[1];

        obj_o->Put(key, evaluate(value_node));
    }

    return obj_value;
}
