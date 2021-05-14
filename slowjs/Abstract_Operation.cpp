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

int throwRuntimeException(int64_t t = EXCEPTION_TYPE, string s = "Unknown Runtime Error")
{
    string msg;
    if (t == EXCEPTION_REFERENCE)
        msg = ReferenceErrorPrefix + s + " is not defined";
    else if (t == EXCEPTION_TYPE)
        msg = TypeErrorPrefix + s;
    else
        msg = s;
    throw msg;
}
BaseUnion GetBase(Reference V)
{
    return V.base;
};
string GetReferencedName(Reference V) { return V.name; };
bool HasPrimitiveBase(Reference V)
{
    if (V.getBaseType() == BASE_TYPE_JSVALUE)
    {
        JSValue *value = GetBase(V).js_value;
        if (value->isBoolean() || value->isString() || value->isNumber())
            return true;
        else
            return false;
    }
    else
        return false;
};
bool IsPropertyReference(Reference V)
{
    if (V.getBaseType() == BASE_TYPE_JSVALUE)
        return GetBase(V).js_value->isObject() || HasPrimitiveBase(V);
    else
        return false;
};
bool IsUnresolvableReference(Reference V)
{
    if (V.getBaseType() == BASE_TYPE_JSVALUE)
        return GetBase(V).js_value->isUndefined();
    else
        return false;
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
        if (HasPrimitiveBase(V))
            throw throwRuntimeException(EXCEPTION_REFERENCE, V.name);
        else
        {
            JSObject *obj = base.js_value->getObject();
            return obj->Get(GetReferencedName(V));
        }
    }
    else
        return base.env_recored->GetBindingValue(GetReferencedName(V));
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
    else if (IsPropertyReference(V))
    {
        if (HasPrimitiveBase(V))
            throw throwRuntimeException(EXCEPTION_REFERENCE, V.name);
        else
        {
            JSObject *obj = base.js_value->getObject();
            obj->Put(GetReferencedName(V), value);
        }
    }
    else
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

bool is_top_level_obj = true;
void printJSObject(JSObject *obj)
{
    map<string, DataDescriptor *>::iterator it;
    map<string, DataDescriptor *> m = obj->Properties;
    cout << endl
         << "{" << endl;
    for (it = m.begin(); it != m.end(); it++)
    {
        cout << "  " << it->first << ": ";
        if (it->second->Value.isBaseObject())
            cout << "[ Object ]";
        else
            intrinsicPrint(it->second->Value);
        cout << "," << endl;
    }
    cout << "}";
}

void intrinsicPrint(JSValue value)
{
    string s;
    JSFunction *fn;

    switch (value.getTag())
    {
    case JS_TAG_NUMBER:
        cout << value.getNumber();
        break;
    case JS_TAG_BOOLEAN:
        cout << value.getBooleanString();
        break;
    case JS_TAG_STRING:
        s = value.getString();
        if (s == "\\n")
            cout << endl;
        else
            cout << s;
        break;
    case JS_TAG_UNDEFINED:
        cout << value.getUndefined();
        break;
    case JS_TAG_EXCEPTION:
        cout << value.getException();
        break;
    case JS_TAG_FUNCTION:
        fn = value.getFunction();
        if (fn->isIntrinsic())
            cout << "function " << fn->Name << "() { [native code] }";
        else
            cout << "function " << fn->Name << "() { USER CODE TODO }";
        break;
    case JS_TAG_OBJECT:
        printJSObject(value.getObject());
        break;
    case JS_TAG_NULL:
        cout << value.getString();
        break;
    case JS_TAG_UNINITIALIZED:
        cout << value.getString();
        break;
    }
    cout << " ";
}
