//
//  Spec_Type.hpp
//  slowjs
//
//  Created by zx on 2021/4/26.
//

#ifndef Spec_Type_hpp
#define Spec_Type_hpp

#include <stdio.h>
#include <string>
#include "JSValue_Type.hpp"
#include "Execution_Context.hpp"

typedef union BaseUnion
{
    JSValue *js_value;
    Environment_Record *env_recored;
} BaseUnion;

class Reference
{
public:
    Reference(){};
    Reference(Environment_Record *record, string _name) : name(_name)
    {
        base.env_recored = record;
    }
    Reference(JSValue *value, string _name) : name(_name)
    {
        base.js_value = value;
    }

    BaseUnion base;
    string name;
};

class PropertyDescriptor
{
public:
    JSValue Enumerable = JS_FALSE;
    JSValue Configurable = JS_FALSE;
};

class DataDescriptor : public PropertyDescriptor
{
public:
    DataDescriptor(){};
    JSValue Value = JS_UNDEFINED;
    JSValue Writable = JS_FALSE;
};
class AccessorDescriptor : public PropertyDescriptor
{
public:
    JSFunction Get;
    JSFunction Set;
};

#endif /* Spec_Type_hpp */
