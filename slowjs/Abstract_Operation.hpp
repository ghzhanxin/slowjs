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

// access the components of references

BaseUnion GetBase(Reference V);
string GetReferencedName(Reference V);
bool HasPrimitiveBase(Reference V);
bool IsPropertyReference(Reference V);
bool IsUnresolvableReference(Reference V);

// operate on references:
JSValue GetValue(JSValue V);
JSValue GetValue(Reference V);

void PutValue(JSValue V, JSValue W);
void PutValue(Reference V, JSValue W);

Reference IdentifierResolution(Lexical_Environment *lex, string name);

void intrinsicPrint(JSValue);
int throwRuntimeException(int64_t t, string s);

JSValue ToPrimitive(JSValue);
JSValue ToBoolean(JSValue);
JSValue ToNumber(JSValue);
JSValue ToString(JSValue);
JSValue ToObject(JSValue);

#endif /* Abstract_Operation_hpp */
