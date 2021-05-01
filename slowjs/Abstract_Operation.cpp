//
//  Abstract_Operation.cpp
//  slowjs
//
//  Created by zx on 2021/4/27.
//

#include "Abstract_Operation.hpp"
#include <string>

#define TypeErrorPrefix "TypeError: "
#define ReferenceErrorPrefix "ReferenceError: "

int throwRuntimeException(int64_t t, string s)
{
    string msg;
    if (t == EXCEPTION_REFERENCE)
    {
        msg = ReferenceErrorPrefix + s + " is not defined";
    }
    else if (t == EXCEPTION_TYPE)
    {
        msg = TypeErrorPrefix + s;
    }
    else
    {
        msg = "Unknown Error";
    }
    throw msg;
}
BaseUnion GetBase(Reference V)
{
    return V.base;
};
string GetReferencedName(Reference V) { return V.name; };
bool HasPrimitiveBase(Reference V)
{
    JSValue *value = V.base.js_value;
    if (value->isBool() || value->isNumber() || value->isNumber())
        return true;
    else
        return false;
};
bool IsPropertyReference(Reference V)
{
    return HasPrimitiveBase(V);
};
bool IsUnresolvableReference(Reference V)
{
    return V.base.js_value->isUndefined();
};

JSValue GetValue(JSValue V)
{
    return V;
};
JSValue GetValue(Reference V)
{
    BaseUnion base = GetBase(V);
    if (IsUnresolvableReference(V))
        throw throwRuntimeException(EXCEPTION_REFERENCE, V.name);

    if (IsPropertyReference(V))
    {
        return JSValue(JS_TAG_STRING, string("TODO"));
    }
    else
    {
        return base.env_recored->GetBindingValue(GetReferencedName(V));
    }
}

void PutValue(JSValue V, JSValue value)
{
    throw throwRuntimeException(EXCEPTION_REFERENCE, "");
}
void PutValue(Reference V, JSValue value)
{
    BaseUnion base = GetBase(V);
    if (IsUnresolvableReference(V))
        throw throwRuntimeException(EXCEPTION_REFERENCE, V.name);

    base.env_recored->SetMutableBinding(GetReferencedName(V), value);
}

Reference IdentifierResolution(Lexical_Environment *lex, string name)
{
    if (!lex)
    {
        JSValue *Undefined = new JSValue(JS_TAG_UNDEFINED, string("undefined"));
        return Reference(Undefined, name);
    }

    Environment_Record *env_record = lex->record;
    bool exists = env_record->HasBinding(name);
    if (exists)
        return Reference(env_record, name);

    Lexical_Environment *outer = lex->outer;
    return IdentifierResolution(outer, name);
}

void intrinsicPrint(JSValue value)
{
    string s;
    switch (value.tag)
    {
    case JS_TAG_FLOAT64:
        cout << value.getFloat();
        break;
    case JS_TAG_BOOL:
        cout << value.getBoolString();
        break;
    case JS_TAG_STRING:
        s = value.getString();
        if (s == "\\n")
        {
            cout << endl;
        }
        else
        {
            cout << s;
        }
        break;
    case JS_TAG_UNDEFINED:
        cout << value.getUndefined();
        break;
    case JS_TAG_EXCEPTION:
        cout << value.getException();
        break;
    case JS_TAG_FUNCTION:
        JSFunction *fn = value.getFunction();
        if (fn->isIntrinsic())
            cout << "function " << fn->Name << "() { [native code] }";
        else
            cout << "function " << fn->Name << "() { USER CODE TODO }";
        break;
    }
    cout << " ";
}