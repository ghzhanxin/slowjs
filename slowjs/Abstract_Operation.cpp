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
            printJSValue(it->second->Value);
        cout << "," << endl;
    }
    cout << "}";
}

void printJSValue(JSValue value)
{
    string s;
    JSFunction *fn;

    switch (value.getTag())
    {
    case JS_TAG_UNDEFINED:
    case JS_TAG_EXCEPTION:
    case JS_TAG_NAN:
    case JS_TAG_BOOLEAN:
    case JS_TAG_NULL:
    case JS_TAG_UNINITIALIZED:
        cout << value.getString();
        break;
    case JS_TAG_NUMBER:
        cout << value.getNumber();
        break;
    case JS_TAG_STRING:
        s = value.getString();
        if (s == "\\n")
            cout << endl;
        else
            cout << s;
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
    default:
        throw throwRuntimeException(EXCEPTION_TYPE, "printJSValue");
    }
    cout << " ";
}

JSValue ToPrimitive(JSValue value)
{
    switch (value.getTag())
    {
    case JS_TAG_UNDEFINED:
    case JS_TAG_NULL:
    case JS_TAG_BOOLEAN:
    case JS_TAG_NUMBER:
    case JS_TAG_STRING:
        return value;
    case JS_TAG_OBJECT:
        return value.getObject()->DefaultValue();
    case JS_TAG_FUNCTION:
        return value.getFunction()->DefaultValue();
    default:
        throw throwRuntimeException(EXCEPTION_TYPE, "ToPrimitive");
    }
}
JSValue ToBoolean(JSValue value)
{
    switch (value.getTag())
    {
    case JS_TAG_UNDEFINED:
    case JS_TAG_NULL:
        return JS_FALSE;
    case JS_TAG_BOOLEAN:
        return value;
    case JS_TAG_NUMBER:
        return JSValue(JS_TAG_BOOLEAN, value.getNumber() != 0);
    case JS_TAG_STRING:
        return JSValue(JS_TAG_BOOLEAN, value.getString().size() != 0);
    case JS_TAG_OBJECT:
        return JS_TRUE;
    default:
        throw throwRuntimeException(EXCEPTION_TYPE, "ToBoolean");
    }
}
JSValue ToNumber(JSValue value)
{
    switch (value.getTag())
    {
    case JS_TAG_UNDEFINED:
        return JS_NAN;
    case JS_TAG_NULL:
        return JSValue(JS_TAG_NUMBER, 0.0);
    case JS_TAG_BOOLEAN:
        return JSValue(JS_TAG_NUMBER, value.getBoolean() ? 1.0 : 0.0);
    case JS_TAG_NUMBER:
        return value;
    case JS_TAG_STRING:
        // TODO:
        return JSValue(JS_TAG_NUMBER, value.getString().size() == 0 ? 0.0 : 1.0);
    case JS_TAG_OBJECT:
        return ToNumber(ToPrimitive(value));
    default:
        throw throwRuntimeException(EXCEPTION_TYPE, "ToNumber");
    }
}
JSValue ToString(JSValue value)
{
    switch (value.getTag())
    {
    case JS_TAG_UNDEFINED:
    case JS_TAG_NULL:
    case JS_TAG_BOOLEAN:
        return JSValue(JS_TAG_STRING, value.getString());
    case JS_TAG_NUMBER:
        // TODO:
        return value;
    case JS_TAG_STRING:
        return value;
    case JS_TAG_OBJECT:
        return ToNumber(ToPrimitive(value));
    default:
        throw throwRuntimeException(EXCEPTION_TYPE, "ToString");
    }
}
JSValue ToObject(JSValue value)
{
    // TODO:
    throw throwRuntimeException(EXCEPTION_TYPE, "ToObject");
}

JSValue CPrint(vector<JSValue> args)
{
    for (size_t i = 0; i < args.size(); i++)
    {
        printJSValue(args[i]);
    }
    cout << endl;
    return JS_UNDEFINED;
}

JSValue CObject(vector<JSValue> args)
{
    return JS_UNDEFINED;
}
JSValue CGetPrototypeOf(vector<JSValue> args)
{
    if (args[0].isObject())
    {
        if (args[0].getObject()->Prototype)
            return JSValue(JS_TAG_OBJECT, args[0].getObject()->Prototype);
        else
            return JS_NULL;
    }
    else
        return JSValue(JS_TAG_EXCEPTION, string("getPrototypeOf args is not a object"));
}
JSValue CCall(vector<JSValue> args)
{
    JSValue thisValue = JS_UNDEFINED;
    if (args.size() > 0)
    {
        thisValue = args[0];
        args.erase(args.begin());
    }
    // return fo->Call(this, thisValue, args);
    return JS_UNDEFINED;
}
