//
//  JSValue_Type.cpp
//  slowjs
//
//  Created by zx on 2021/4/26.
//

#include "JSValue_Type.hpp"
#include "Slowjs.hpp"

PropertyDescriptor JSObject::GetOwnProperty(string P)
{
    return PropertyDescriptor();
}
PropertyDescriptor JSObject::GetProperty(string P)
{
    return PropertyDescriptor();
}

JSValue JSFunction::Call(Slowjs *slow, JSValue thisValue, vector<JSValue> args)
{
    JSFunction *fo = this;
    slow->initFunctionExecutionContext(fo, thisValue, args);
    AST_Node *func_body = fo->Code->childs[2];
    JSValue normal_Result;
    try
    {
        normal_Result = slow->evaluate(func_body);
    }
    catch (JSValue &value)
    {
        slow->checkException(value);
        slow->ctx_stack->pop();
        return value;
    }
    slow->checkException(normal_Result);
    slow->ctx_stack->pop();
    return normal_Result;
}
JSValue JSFunction::Construct(Slowjs *slow, vector<JSValue> args)
{
    JSObject *obj = new JSObject();
    JSFunction *fo = this;
    JSValue thisValue = JS_UNDEFINED;
    JSValue result = fo->Call(slow, thisValue, args);
    return result.isObject() ? result : JSValue(JS_TAG_OBJECT, obj);
}
