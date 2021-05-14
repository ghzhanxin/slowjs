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

enum
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
    string getBooleanString() { return _string; }
    string getString() { return _string; }
    void *getPtr() { return _u.ptr; }
    JSFunction *getFunction() { return (JSFunction *)getPtr(); }
    JSObject *getObject() { return (JSObject *)getPtr(); }
    string getException() { return _string; }
    string getUndefined() { return _string; }
    string getUninitialized() { return _string; }

    bool isNumber() { return _tag == JS_TAG_NUMBER; }
    bool isBoolean() { return _tag == JS_TAG_BOOLEAN; }
    bool isString() { return _tag == JS_TAG_STRING; }
    bool isUndefined() { return _tag == JS_TAG_UNDEFINED; }
    bool isNull() { return _tag == JS_TAG_NULL; }
    bool isBaseObject() { return _tag == JS_TAG_OBJECT; }
    bool isObject() { return _tag == JS_TAG_OBJECT || _tag == JS_TAG_FUNCTION; }
    bool isFunction() { return _tag == JS_TAG_FUNCTION; }
    bool isException() { return _tag == JS_TAG_EXCEPTION; }

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
    void DefaultValue();
    void DefineOwnProperty(string P, DataDescriptor *D);

    JSObject *Prototype = nullptr;
    string Class = "Object";
    bool Extensible = true;

    map<string, DataDescriptor *> Properties;
};

class JSFunction : public JSObject
{
public:
    JSFunction(string name) : Name(name), _intrinsic(true)
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
          Name(name),
          _intrinsic(false)
    {
        initializeFunction();
    };
    void initializeFunction();

    AST_Node *FormalParameters;
    AST_Node *Code;
    Lexical_Environment *Scope;
    string Name;
    string Class = "Function";

    JSValue Call(Slowjs *slow, JSValue thisValue, vector<JSValue> args);
    JSValue Construct(Slowjs *slow, vector<JSValue> args);

    bool isIntrinsic() { return _intrinsic; };

private:
    bool _intrinsic;
};

#endif /* JSValue_Type_hpp */
