//
//  Abstract_Operation.cpp
//  slowjs
//
//  Created by zx on 2021/4/27.
//

#include "Abstract_Operation.hpp"
#include "Slowjs.hpp"

#define TypeErrorPrefix "TypeError: "
#define ReferenceErrorPrefix "ReferenceError: "

int ThrowRuntimeException(EXCEPTION_ENUM t = EXCEPTION_TYPE, string s = "Unknown Runtime Error")
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

BaseUnion GetBase(Reference V) { return V.base; };
string GetReferencedName(Reference V) { return V.name; };
bool HasPrimitiveBase(Reference V)
{
    if (V.getBaseType() != BASE_TYPE_JSVALUE)
        return false;

    JSValue *value = GetBase(V).js_value;
    return value->isBoolean() || value->isString() || value->isNumber();
};
bool IsPropertyReference(Reference V)
{
    if (V.getBaseType() != BASE_TYPE_JSVALUE)
        return false;

    return GetBase(V).js_value->isObject() || HasPrimitiveBase(V);
};
bool IsUnresolvableReference(Reference V)
{
    if (V.getBaseType() != BASE_TYPE_JSVALUE)
        return false;

    return GetBase(V).js_value->isUndefined();
};

JSValue GetValue(JSValue V) { return V; };
JSValue GetValue(Reference V)
{
    BaseUnion base = GetBase(V);
    if (IsUnresolvableReference(V))
        throw ThrowRuntimeException(EXCEPTION_REFERENCE, V.name);

    if (IsPropertyReference(V))
    {
        if (HasPrimitiveBase(V))
            throw ThrowRuntimeException(EXCEPTION_REFERENCE, V.name);
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
    throw ThrowRuntimeException(EXCEPTION_REFERENCE, "");
}
void PutValue(Reference V, JSValue value)
{
    BaseUnion base = GetBase(V);
    if (IsUnresolvableReference(V))
        throw ThrowRuntimeException(EXCEPTION_REFERENCE, V.name);
    else if (IsPropertyReference(V))
    {
        if (HasPrimitiveBase(V))
            throw ThrowRuntimeException(EXCEPTION_REFERENCE, V.name);
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
        return Reference(new JSUndefined, name);

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
    JSFunction *fo;

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
        fo = value.getFunction();
        if (fo->isIntrinsic())
            cout << "function " << fo->Name << "() { [native code] }";
        else
            cout << "function " << fo->Name << "() { USER CODE TODO }";
        break;
    case JS_TAG_OBJECT:
        printJSObject(value.getObject());
        break;
    default:
        throw ThrowRuntimeException(EXCEPTION_TYPE, "printJSValue");
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
        throw ThrowRuntimeException(EXCEPTION_TYPE, "ToPrimitive");
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
        return JSBoolean(value.getNumber() != 0).ToJSValue();
    case JS_TAG_STRING:
        return JSBoolean(value.getString().size() != 0).ToJSValue();
    case JS_TAG_OBJECT:
    case JS_TAG_FUNCTION:
        return JS_TRUE;
    default:
        throw ThrowRuntimeException(EXCEPTION_TYPE, "ToBoolean");
    }
}
JSValue ToNumber(JSValue value)
{
    switch (value.getTag())
    {
    case JS_TAG_UNDEFINED:
        return JS_NAN;
    case JS_TAG_NULL:
        return JSNumber(0.0).ToJSValue();
    case JS_TAG_BOOLEAN:
        return JSNumber(value.getBoolean() ? 1.0 : 0.0).ToJSValue();
    case JS_TAG_NUMBER:
        return value;
    case JS_TAG_STRING:
        // TODO:
        return JSNumber(value.getString().size() == 0 ? 0.0 : 1.0).ToJSValue();
    case JS_TAG_OBJECT:
    case JS_TAG_FUNCTION:
        return ToNumber(ToPrimitive(value));
    default:
        throw ThrowRuntimeException(EXCEPTION_TYPE, "ToNumber");
    }
}
JSValue ToString(JSValue value)
{
    switch (value.getTag())
    {
    case JS_TAG_UNDEFINED:
    case JS_TAG_NULL:
    case JS_TAG_BOOLEAN:
        return JSString(value.getString()).ToJSValue();
    case JS_TAG_NUMBER:
        // TODO:
        return value;
    case JS_TAG_STRING:
        return value;
    case JS_TAG_OBJECT:
    case JS_TAG_FUNCTION:
        return ToNumber(ToPrimitive(value));
    default:
        throw ThrowRuntimeException(EXCEPTION_TYPE, "ToString");
    }
}
JSValue ToObject(JSValue value)
{
    // TODO:
    throw ThrowRuntimeException(EXCEPTION_TYPE, "ToObject");
}

JSValue Builtin_Console_log(const Function_Data &param)
{
    vector<JSValue> args = param.args;

    for (size_t i = 0; i < args.size(); i++)
        printJSValue(args[i]);

    cout << endl;
    return JS_UNDEFINED;
}

JSValue Builtin_Object(const Function_Data &param)
{
    return JS_UNDEFINED;
}
JSValue Builtin_GetPrototypeOf(const Function_Data &param)
{
    vector<JSValue> args = param.args;

    if (args[0].isObject())
    {
        if (args[0].getObject()->Prototype)
            return args[0].getObject()->Prototype->ToJSValue();
        else
            return JS_NULL;
    }
    else
        return JSException("getPrototypeOf args is not a object").ToJSValue();
}
JSValue Builtin_Function_Prototype_Call(const Function_Data &param)
{
    vector<JSValue> args = param.args;
    JSValue thisValue = JS_UNDEFINED;
    if (args.size() > 0)
    {
        thisValue = args[0];
        args.erase(args.begin());
    }
    JSFunction *caller_fo = param.thisValue.getFunction();
    return caller_fo->Call(param.slow, thisValue, args);
}
void EnqueueTask(const Function_Data &param)
{
    Task task(new Function_Data(param));
    param.slow->loop->task_queue.push(task);
}
void EnqueueJob(const Function_Data &param)
{
    Job job(new Function_Data(param));
    param.slow->loop->job_queue.push(job);
}
JSValue Builtin_SetTimeout(const Function_Data &param)
{
    vector<JSValue> args = param.args;

    if (args.size() == 0)
        return JSException("1 argument required, but only 0 present.");
    else
    {
        JSFunction *delay_fo = args[0].getFunction();
        args.erase(args.begin());

        if (args.size() != 0)
            // TODO: timeout supported 0 currently, just erase it
            args.erase(args.begin());

        EnqueueTask(Function_Data(param.slow, delay_fo, param.thisValue, args));
        return JS_UNDEFINED;
    }
}
JSValue Builtin_Process_Nexttick(const Function_Data &param)
{
    vector<JSValue> args = param.args;

    if (args.size() != 0 && args[0].isFunction())
    {
        JSFunction *delay_fo = args[0].getFunction();
        args.erase(args.begin());

        EnqueueJob(Function_Data(param.slow, delay_fo, param.thisValue, args));
        return JS_UNDEFINED;
    }
    else
        return JSException("Callback must be a function.");
}
