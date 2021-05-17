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
#include <map>

#include "Parser.hpp"

class Slowjs;

using namespace std;

class Lexical_Environment;
class PropertyDescriptor;
class DataDescriptor;
class AccessorDescriptor;
class JSObject;
class JSFunction;

enum EXCEPTION_ENUM
{
    EXCEPTION_TYPE,
    EXCEPTION_REFERENCE,
};

enum JS_TAG_ENUM
{
    JS_TAG_FUNCTION = -2,
    JS_TAG_OBJECT = -1,

    JS_TAG_NUMBER,
    JS_TAG_BOOLEAN,
    JS_TAG_STRING,
    JS_TAG_NULL,
    JS_TAG_UNDEFINED,
    JS_TAG_UNINITIALIZED,
    JS_TAG_EXCEPTION,
    JS_TAG_NAN,
};

typedef union JSValueUnion
{
    double double64;
    bool boolean;
    void *ptr;
} JSValueUnion;

class JSValue
{
public:
    JSValue() : _tag(JS_TAG_NUMBER), _string("_default_JSValue"){};

    JS_TAG_ENUM getTag() const { return _tag; }
    double getNumber() const { return _u.double64; }
    bool getBoolean() const { return _u.boolean; }
    string getString() const { return _string; }
    void *getPtr() const { return _u.ptr; }
    JSFunction *getFunction() const { return (JSFunction *)getPtr(); }
    JSObject *getObject() const { return (JSObject *)getPtr(); }

    bool isNumber() const { return _tag == JS_TAG_NUMBER; }
    bool isBoolean() const { return _tag == JS_TAG_BOOLEAN; }
    bool isString() const { return _tag == JS_TAG_STRING; }
    bool isUndefined() const { return _tag == JS_TAG_UNDEFINED; }
    bool isNull() const { return _tag == JS_TAG_NULL; }
    bool isBaseObject() const { return _tag == JS_TAG_OBJECT; }
    bool isObject() const { return _tag == JS_TAG_OBJECT || _tag == JS_TAG_FUNCTION; }
    bool isFunction() const { return _tag == JS_TAG_FUNCTION; }
    bool isException() const { return _tag == JS_TAG_EXCEPTION; }
    bool isNaN() const { return _tag == JS_TAG_NAN; }

    JSValue ToJSValue() { return *((JSValue *)this); }

protected:
    JSValueUnion _u;
    JS_TAG_ENUM _tag;
    string _string;
};

/* special values */
#define JS_EXCEPTION JSException().ToJSValue()
#define JS_UNINITIALIZED JSUinitialized().ToJSValue()
#define JS_NAN JSNaN().ToJSValue()
#define JS_NULL JSNull().ToJSValue()
#define JS_UNDEFINED JSUndefined().ToJSValue()
#define JS_TRUE JSBoolean(true).ToJSValue()
#define JS_FALSE JSBoolean(false).ToJSValue()

class JSException : public JSValue
{
public:
    JSException()
    {
        _tag = JS_TAG_EXCEPTION;
        _string = "Unknown Exception";
    }
    JSException(string value)
    {
        _tag = JS_TAG_EXCEPTION;
        _string = value;
    }
};
class JSUinitialized : public JSValue
{
public:
    JSUinitialized()
    {
        _tag = JS_TAG_UNINITIALIZED;
        _string = "Uninitialized";
    }
};
class JSNaN : public JSValue
{
public:
    JSNaN()
    {
        _tag = JS_TAG_NAN;
        _string = "NaN";
    }
};
class JSPrimitive : public JSValue
{
};

class JSNull : public JSPrimitive
{
public:
    JSNull()
    {
        _tag = JS_TAG_NULL;
        _string = "null";
    }
};

class JSUndefined : public JSPrimitive
{
public:
    JSUndefined()
    {
        _tag = JS_TAG_UNDEFINED;
        _string = "undefined";
    }
};

class JSNumber : public JSPrimitive
{
public:
    JSNumber(int value)
    {
        _tag = JS_TAG_NUMBER;
        _u.double64 = value;
    }
    JSNumber(double value)
    {
        _tag = JS_TAG_NUMBER;
        _u.double64 = value;
    }

    double Value() { return this->getNumber(); }
};

class JSBoolean : public JSPrimitive
{
public:
    JSBoolean(bool value)
    {
        _tag = JS_TAG_BOOLEAN;
        _u.boolean = value;
        _string = value ? "true" : "false";
    }

    bool Value() { return this->getBoolean(); }
};

class JSString : public JSPrimitive
{
public:
    JSString(string value)
    {
        _tag = JS_TAG_STRING;
        _string = value;
    }

    string Value() { return this->getString(); }
};

// https://262.ecma-international.org/5.1/#sec-8.6
class JSObject : public JSValue
{
public:
    // builtins
    static JSObject *ObjectPrototype;
    static JSFunction *Object;
    static JSObject *FunctionPrototype;
    static JSFunction *Function;
    static void CreateBuiltinObject();

    JSObject()
    {
        _u.ptr = (void *)this;
        _tag = JS_TAG_OBJECT;
        Prototype = ObjectPrototype;
    };
    DataDescriptor *GetOwnProperty(string P);
    DataDescriptor *GetProperty(string P);
    JSValue Get(string P);
    bool CanPut(string P);
    void Put(string P, JSValue V);
    bool HasProperty(string P);
    void Delete();
    JSValue DefaultValue();
    void DefineOwnProperty(string P, DataDescriptor *D);

    JSObject *Prototype = nullptr;
    string Class = "Object";
    bool Extensible = true;

    map<string, DataDescriptor *> Properties;

    JSValue ToJSValue() { return *((JSValue *)this); }
};

class JSFunction : public JSObject
{
public:
    JSFunction(string name, void *ptr = nullptr)
        : Name(name), _c_function_ptr(ptr)
    {
        initializeFunction();
    };
    JSFunction(
        AST_Node *formal_param,
        AST_Node *func_code,
        Lexical_Environment *scope,
        string name)
        : FormalParameters(formal_param),
          Code(func_code),
          Scope(scope),
          Name(name)
    {
        initializeFunction();
    };
    // https://262.ecma-international.org/5.1/#sec-13.2
    void initializeFunction();

    AST_Node *FormalParameters;
    AST_Node *Code;
    Lexical_Environment *Scope;
    string Name;
    string Class = "Function";

    // https://262.ecma-international.org/5.1/#sec-13.2.1
    JSValue Call(Slowjs *slow, JSValue thisValue, vector<JSValue> args);

    // https://262.ecma-international.org/5.1/#sec-13.2.2
    JSValue Construct(Slowjs *slow, vector<JSValue> args);

    bool isIntrinsic() { return !!_c_function_ptr; };

    typedef JSValue (*C_Function)(JSFunction *fo, Slowjs *slow, JSValue thisValue, vector<JSValue>);
    C_Function getCFunction() { return (C_Function)_c_function_ptr; };

private:
    void *_c_function_ptr = nullptr;
};

#endif /* JSValue_Type_hpp */
