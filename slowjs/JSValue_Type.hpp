//
//  JSValue_Type.hpp
//  slowjs
//
//  Created by zx on 2021/4/26.
//

#ifndef JSValue_Type_hpp
#define JSValue_Type_hpp

#include <stdio.h>
#include <string>
#include <iostream>
#include "Parser.hpp"
#include <map>

using namespace std;

class Lexical_Environment;

enum
{
    EXCEPTION_TYPE,
    EXCEPTION_REFERENCE,
};

enum
{
    JS_TAG_FUNCTION = -2,
    JS_TAG_OBJECT = -1,

    JS_TAG_INT = 0,
    JS_TAG_BOOL = 1,
    JS_TAG_NULL = 2,
    JS_TAG_UNDEFINED = 3,
    JS_TAG_UNINITIALIZED = 4,
    JS_TAG_EXCEPTION = 6,
    JS_TAG_FLOAT64 = 7,
    JS_TAG_STRING = 8,
};

typedef union JSValueUnion
{
    double float64;
    bool boolean;
    void *ptr;
} JSValueUnion;

class JSFunction;
class JSObject;
class JSValue
{
public:
    JSValue()
    {
        u.float64 = 0;
        tag = JS_TAG_FLOAT64;
        _string = "";
    };
    JSValue(int64_t tag, double val)
    {
        this->tag = tag;
        u.float64 = val;
    };
    JSValue(int64_t tag, string val)
    {
        this->tag = tag;
        _string = val;
    };
    JSValue(int64_t tag, bool val)
    {
        this->tag = tag;
        u.boolean = val;
        _string = val ? "true" : "false";
    };
    JSValue(int64_t tag, void *p)
    {
        this->tag = tag;
        u.ptr = p;
    };

    JSValueUnion u;
    int64_t tag;
    string _string;

    int64_t getTag() { return tag; }
    double getFloat() { return u.float64; }
    bool getBool() { return u.boolean; }
    string getBoolString() { return _string; }
    string getString() { return _string; }
    void *getPtr() { return u.ptr; }
    JSFunction *getFunction() { return (JSFunction *)getPtr(); }
    JSObject *getObject() { return (JSObject *)getPtr(); }
    string getException() { return _string; }
    string getUndefined() { return _string; }
    string getUninitialized() { return _string; }

    bool isNumber() { return tag == JS_TAG_INT || tag == JS_TAG_FLOAT64; }
    bool isFloat() { return tag == JS_TAG_FLOAT64; }
    bool isBool() { return tag == JS_TAG_BOOL; }
    bool isString() { return tag == JS_TAG_STRING; }
    bool isUndefined() { return tag == JS_TAG_UNDEFINED; }
    bool isNull() { return tag == JS_TAG_NULL; }
    bool isObject() { return tag == JS_TAG_OBJECT; }
    bool isFunction() { return tag == JS_TAG_FUNCTION; }
    bool isException() { return tag == JS_TAG_EXCEPTION; }
};

/* special values */
#define JS_NULL JSValue(JS_TAG_NULL, 0.0)
#define JS_FALSE JSValue(JS_TAG_BOOL, false)
#define JS_TRUE JSValue(JS_TAG_BOOL, true)
#define JS_UNDEFINED JSValue(JS_TAG_UNDEFINED, string("undefined"))
#define JS_EXCEPTION JSValue(JS_TAG_EXCEPTION, string("Unknow Exception"))
#define JS_UNINITIALIZED JSValue(JS_TAG_UNINITIALIZED, string("Uninitialized"))

class PropertyDescriptor;
class JSObject : public JSValue
{
public:
    JSObject() { tag = JS_TAG_OBJECT; };
    PropertyDescriptor GetOwnProperty(string P);
    PropertyDescriptor GetProperty(string P);

    map<string, JSValue> Properties;
};

class JSFunction : public JSObject
{
public:
    JSFunction(string name)
    {
        Name = name;
        tag = JS_TAG_FUNCTION;
        _intrinsic = true;
    };
    JSFunction(AST_Node *formal_param, AST_Node *func_code, Lexical_Environment *scope, string name)
    {
        FormalParameters = formal_param;
        Code = func_code;
        Scope = scope;
        Name = name;
        tag = JS_TAG_FUNCTION;
        _intrinsic = false;
    };
    AST_Node *FormalParameters;
    AST_Node *Code;
    Lexical_Environment *Scope;
    string Name;
    JSObject Prototype;
    bool _intrinsic;

    JSValue _Call(JSValue thisValue, vector<JSValue> params);
    JSValue _Construct(JSValue thisValue, vector<JSValue> params);

    bool isIntrinsic() { return _intrinsic; };
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

#endif /* JSValue_Type_hpp */
