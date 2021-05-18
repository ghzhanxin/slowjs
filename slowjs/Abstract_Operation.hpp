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

// helper function
void printJSValue(JSValue);
int throwRuntimeException(EXCEPTION_ENUM t, string s);

// C_Function
JSValue C_Print(JSFunction *, Slowjs *, JSValue, vector<JSValue>);
JSValue C_Builtin_Object(JSFunction *, Slowjs *, JSValue, vector<JSValue>);
JSValue C_GetPrototypeOf(JSFunction *, Slowjs *, JSValue, vector<JSValue>);
JSValue C_FunctionPrototypeCall(JSFunction *, Slowjs *, JSValue, vector<JSValue>);
JSValue C_EnqueueTask(JSFunction *, Slowjs *, JSValue, vector<JSValue>);
JSValue C_EnqueueJob(JSFunction *, Slowjs *, JSValue, vector<JSValue>);

#endif /* Abstract_Operation_hpp */
