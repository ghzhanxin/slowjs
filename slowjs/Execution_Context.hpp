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

class Execution_Context
{
public:
    Lexical_Environment *lex_env;
    Lexical_Environment *var_env;
    JSValue this_binding;
};

class Lexical_Environment
{
public:
    Environment_Record *record;
    Lexical_Environment *outer;
};

class Environment_Record : public map<string, JSValue>
{
public:
    Environment_Record(){};
    bool HasBinding(string name);
    void CreateMutableBinding(string name);
    void SetMutableBinding(string name, JSValue value);
    JSValue GetBindingValue(string name);
};

class Object_ER : public Environment_Record
{
};

class Declarative_ER : public Environment_Record
{
};

#endif /* Execution_Context_hpp */
