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
#include "Environment_Record.hpp"

typedef union BaseUnion
{
    JSValue *js_value;
    Environment_Record *env_recored;
} BaseUnion;

class Reference
{
public:
    Reference(){};
    Reference(Environment_Record *record, string name)
    {
        base.env_recored = record;
        this->name = name;
    }
    Reference(JSValue *value, string name)
    {
        base.js_value = value;
        this->name = name;
    }

    BaseUnion base;
    string name;
};

class Descriptor
{
public:
    JSValue Enumerable = JS_FALSE;
    JSValue Configurable = JS_FALSE;
};

class PropertyDescriptor : public Descriptor
{
public:
    PropertyDescriptor(){};
    JSValue Value = JS_UNDEFINED;
    JSValue Writable = JS_FALSE;
};

#endif /* Spec_Type_hpp */
