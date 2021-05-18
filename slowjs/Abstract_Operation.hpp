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
BaseUnion GetBase(Reference V);
string GetReferencedName(Reference V);
bool HasPrimitiveBase(Reference V);
bool IsPropertyReference(Reference V);
bool IsUnresolvableReference(Reference V);

// https://262.ecma-international.org/5.1/#sec-8.7.1
JSValue GetValue(JSValue V);
JSValue GetValue(Reference V);

void PutValue(JSValue V, JSValue W);
void PutValue(Reference V, JSValue W);

// https://262.ecma-international.org/5.1/#sec-10.3.1
// https://262.ecma-international.org/5.1/#sec-10.2.2.1
Reference IdentifierResolution(Lexical_Environment *lex, string name);

// https://262.ecma-international.org/5.1/#sec-9
JSValue ToPrimitive(JSValue);
JSValue ToBoolean(JSValue);
JSValue ToNumber(JSValue);
JSValue ToString(JSValue);
JSValue ToObject(JSValue);

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

    Slowjs *slow;
    JSFunction *fo;
    JSValue thisValue;
    vector<JSValue> args;
};

// helper function
void printJSValue(JSValue);
int ThrowRuntimeException(EXCEPTION_ENUM t, string s);
void EnqueueTask(const Function_Data &);
void EnqueueJob(const Function_Data &);

// Builtin Objects
JSValue Builtin_Console_log(const Function_Data &);
JSValue Builtin_Object(const Function_Data &);
JSValue Builtin_GetPrototypeOf(const Function_Data &);
JSValue Builtin_Function_Prototype_Call(const Function_Data &);
JSValue Builtin_SetTimeout(const Function_Data &);
JSValue Builtin_Process_Nexttick(const Function_Data &);

#endif /* Abstract_Operation_hpp */
