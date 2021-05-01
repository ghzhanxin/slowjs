//
//  JSValue_Type.cpp
//  slowjs
//
//  Created by zx on 2021/4/26.
//

#include "JSValue_Type.hpp"

PropertyDescriptor JSObject::GetOwnProperty(string P)
{
    return PropertyDescriptor();
}
PropertyDescriptor JSObject::GetProperty(string P)
{
    return PropertyDescriptor();
}

JSValue JSFunction::_Call(JSValue thisValue, vector<JSValue> params)
{
    return JSValue();
}
JSValue JSFunction::_Construct(JSValue thisValue, vector<JSValue> params)
{
    return JSValue();
}


