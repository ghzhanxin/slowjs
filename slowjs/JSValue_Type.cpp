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

JSValue JSFunction::Call(Slowjs *rt, JSValue thisValue, vector<JSValue> args)
{
    JSFunction *fo = this;
    rt->initFunctionExecutionContext(fo, thisValue, args);
    AST_Node *func_body = fo->Code->childs[2];
    JSValue normal_Result;
    try
    {
        normal_Result = rt->evaluate(func_body);
    }
    catch (JSValue &value)
    {
        rt->checkException(value);
        rt->ctx_stack->pop();
        return value;
    }
    rt->checkException(normal_Result);
    rt->ctx_stack->pop();
    return normal_Result;
}
JSValue JSFunction::Construct(JSValue thisValue, vector<JSValue> args)
{
    return JSValue();
}
