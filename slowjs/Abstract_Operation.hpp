//
//  Abstract_Operation.hpp
//  slowjs
//
//  Created by zx on 2021/4/27.
//

#ifndef Abstract_Operation_hpp
#define Abstract_Operation_hpp

#include <iostream>
#include <stdio.h>
#include <stack>
#include "Spec_Type.hpp"
#include "Execution_Context.hpp"

class Slowjs;

// https://262.ecma-international.org/5.1/#sec-8.7
BaseUnion GetBase(const Reference &);
string GetReferencedName(const Reference &);
bool HasPrimitiveBase(const Reference &);
bool IsPropertyReference(const Reference &);
bool IsUnresolvableReference(const Reference &);

// https://262.ecma-international.org/5.1/#sec-8.7.1
JSValue GetValue(Reference);
JSValue GetValue(JSValue);

void PutValue(const JSValue &, const JSValue &);
void PutValue(const Reference &, const JSValue &);

// https://262.ecma-international.org/5.1/#sec-10.3.1
// https://262.ecma-international.org/5.1/#sec-10.2.2.1
Reference IdentifierResolution(Lexical_Environment *lex, const string &name);

// https://262.ecma-international.org/5.1/#sec-9
JSValue ToPrimitive(const JSValue &);
JSValue ToBoolean(const JSValue &);
JSValue ToNumber(const JSValue &);
JSValue ToString(const JSValue &);
JSValue ToObject(const JSValue &);

class Function_Data
{
public:
    Function_Data(
        Slowjs *_slow,
        JSFunction *_fo,
        JSValue _thisValue,
        vector<JSValue> _args)
        : fo(_fo),
          slow(_slow),
          thisValue(_thisValue),
          args(_args) {}

    Slowjs *const slow;
    JSFunction *const fo;
    const JSValue thisValue;
    const vector<JSValue> args;
};

// helper function
int ThrowRuntimeException(const EXCEPTION_ENUM &, const string &);
void printJSValue(JSValue &);
void EnqueueTask(const Function_Data &);
void EnqueueJob(const Function_Data &);

// Builtin Objects
JSValue Builtin_Console_log(const Function_Data &);
JSValue Builtin_Object(const Function_Data &);
JSValue Builtin_GetPrototypeOf(const Function_Data &);
JSValue Builtin_Function_Prototype_Call(const Function_Data &);
JSValue Builtin_Function_Prototype_Bind(const Function_Data &);
JSValue Builtin_SetTimeout(const Function_Data &);
JSValue Builtin_Process_Nexttick(const Function_Data &);

#endif /* Abstract_Operation_hpp */
