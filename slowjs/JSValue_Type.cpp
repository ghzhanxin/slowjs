//
//  JSValue_Type.cpp
//  slowjs
//
//  Created by zx on 2021/4/26.
//

#include "JSValue_Type.hpp"
#include "Slowjs.hpp"

DataDescriptor JSObject::GetOwnProperty(string P)
{
    map<string, JSValue>::iterator it = this->Properties.find(P);
    DataDescriptor D;
    D.Value = it == this->Properties.end() ? JS_UNDEFINED : it->second;
    return D;
}
DataDescriptor JSObject::GetProperty(string P)
{
    DataDescriptor prop = this->GetOwnProperty(P);
    if (prop.Value.isUndefined())
        return prop;
    else
    {
        JSObject *proto = this->Prototype;
        if (proto == nullptr)
        {
            DataDescriptor D;
            D.Value = JS_UNDEFINED;
            return D;
        }
        else
            return proto->GetProperty(P);
    }
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
    JSValue result = this->Call(slow, JSValue(JS_TAG_OBJECT, obj), args);
    return result.isObject() ? result : JSValue(JS_TAG_OBJECT, obj);
}
