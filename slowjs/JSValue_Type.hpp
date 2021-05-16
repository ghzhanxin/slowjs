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

enum JS_TAG
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
    JSValue(JS_TAG tag, string val) : _tag(tag), _string(val){};
    JSValue(JS_TAG tag, double val) : _tag(tag)
    {
        _u.double64 = val;
    };
    JSValue(JS_TAG tag, int val) : _tag(tag)
    {
        _u.double64 = val;
    };
    JSValue(JS_TAG tag, bool val) : _tag(tag), _string(val ? "true" : "false")
    {
        _u.boolean = val;
    };
    JSValue(JS_TAG tag, void *p) : _tag(tag)
    {
        _u.ptr = p;
    };

    JS_TAG getTag() { return _tag; }
    double getNumber() { return _u.double64; }
    bool getBoolean() { return _u.boolean; }
    string getString() { return _string; }
    void *getPtr() { return _u.ptr; }
    JSFunction *getFunction() { return (JSFunction *)getPtr(); }
    JSObject *getObject() { return (JSObject *)getPtr(); }

    bool isNumber() { return _tag == JS_TAG_NUMBER; }
    bool isBoolean() { return _tag == JS_TAG_BOOLEAN; }
    bool isString() { return _tag == JS_TAG_STRING; }
    bool isUndefined() { return _tag == JS_TAG_UNDEFINED; }
    bool isNull() { return _tag == JS_TAG_NULL; }
    bool isBaseObject() { return _tag == JS_TAG_OBJECT; }
    bool isObject() { return _tag == JS_TAG_OBJECT || _tag == JS_TAG_FUNCTION; }
    bool isFunction() { return _tag == JS_TAG_FUNCTION; }
    bool isException() { return _tag == JS_TAG_EXCEPTION; }
    bool isNaN() { return _tag == JS_TAG_NAN; }

protected:
    JSValueUnion _u;
    JS_TAG _tag;
    string _string;
};

/* special values */
#define JS_TRUE JSValue(JS_TAG_BOOLEAN, true)
#define JS_FALSE JSValue(JS_TAG_BOOLEAN, false)
#define JS_NULL JSValue(JS_TAG_NULL, string("null"))
#define JS_UNDEFINED JSValue(JS_TAG_UNDEFINED, string("undefined"))
#define JS_EXCEPTION JSValue(JS_TAG_EXCEPTION, string("Unknow Exception"))
#define JS_UNINITIALIZED JSValue(JS_TAG_UNINITIALIZED, string("Uninitialized"))
#define JS_NAN JSValue(JS_TAG_NAN, string("NaN"))

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
