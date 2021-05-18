//
//  Execution_Context.hpp
//  slowjs
//
//  Created by zx on 2021/4/19.
//

#ifndef Execution_Context_hpp
#define Execution_Context_hpp

#include <iostream>
#include <map>

#include "JSValue_Type.hpp"

class Environment_Record;
class Lexical_Environment;

// https://262.ecma-international.org/5.1/#sec-10.3
class Execution_Context
{
public:
    Execution_Context(Lexical_Environment *env, JSValue thisValue)
        : lex_env(env), var_env(env), this_binding(thisValue){};

    Lexical_Environment *lex_env;
    Lexical_Environment *var_env;
    JSValue this_binding;
};

// https://262.ecma-international.org/5.1/#sec-10.2
class Lexical_Environment
{
public:
    Lexical_Environment(Environment_Record *r, Lexical_Environment *o)
        : record(r), outer(o){};

    Environment_Record *record;
    Lexical_Environment *outer;
};

// https://262.ecma-international.org/5.1/#sec-10.2.1
class Environment_Record : public map<string, JSValue>
{
public:
    virtual bool HasBinding(const string &);
    virtual void CreateMutableBinding(const string &);
    virtual void SetMutableBinding(const string &, const JSValue &);
    virtual JSValue GetBindingValue(const string &);
};

class Object_ER : public Environment_Record
{
public:
    Object_ER(JSObject *obj) : binding_obj(obj){};

    bool HasBinding(const string &);
    void CreateMutableBinding(const string &);
    void SetMutableBinding(const string &, const JSValue &);
    JSValue GetBindingValue(const string &);

private:
    JSObject *const binding_obj;
};

class Declarative_ER : public Environment_Record
{
};

#endif /* Execution_Context_hpp */
