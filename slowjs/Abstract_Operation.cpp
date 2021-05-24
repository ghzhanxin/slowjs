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

BaseUnion GetBase(const Reference &V) { return V.base; };
string GetReferencedName(const Reference &V) { return V.name; };
bool HasPrimitiveBase(const Reference &V)
{
    if (V.getBaseType() != BASE_TYPE_JSVALUE)
        return false;

    JSValue *base = GetBase(V).js_value;
    return base->isBoolean() || base->isString() || base->isNumber();
};
bool IsPropertyReference(const Reference &V)
{
    if (V.getBaseType() != BASE_TYPE_JSVALUE)
        return false;

    return GetBase(V).js_value->isObject() || HasPrimitiveBase(V);
};
bool IsUnresolvableReference(const Reference &V)
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

    if (!IsPropertyReference(V))
        return base.env_recored->GetBindingValue(GetReferencedName(V));

    if (HasPrimitiveBase(V))
        throw ThrowRuntimeException(EXCEPTION_REFERENCE, V.name);
    else
        return base.js_value->getObject()->Get(GetReferencedName(V));
}

void PutValue(const JSValue &V, const JSValue &W)
{
    throw ThrowRuntimeException(EXCEPTION_REFERENCE, "");
}
void PutValue(const Reference &V, const JSValue &W)
{
    BaseUnion base = GetBase(V);
    if (IsUnresolvableReference(V))
        throw ThrowRuntimeException(EXCEPTION_REFERENCE, V.name);

    if (!IsPropertyReference(V))
        return base.env_recored->SetMutableBinding(GetReferencedName(V), W);

    if (HasPrimitiveBase(V))
        throw ThrowRuntimeException(EXCEPTION_REFERENCE, V.name);
    else
        return base.js_value->getObject()->Put(GetReferencedName(V), W);
}

Reference IdentifierResolution(Lexical_Environment *lex, const string &name)
{
    if (!lex)
        return Reference((JSValue *)JSValue::GLOBAL_JS_UNDEFINED, name);

    Environment_Record *env_record = lex->record;

    bool exists = env_record->HasBinding(name);
    if (exists)
        return Reference(env_record, name);

    return IdentifierResolution(lex->outer, name);
}

JSValue ToPrimitive(const JSValue &value)
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
JSValue ToBoolean(const JSValue &value)
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
JSValue ToNumber(const JSValue &value)
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
JSValue ToString(const JSValue &value)
{
    switch (value.getTag())
    {
    case JS_TAG_UNDEFINED:
    case JS_TAG_NULL:
    case JS_TAG_BOOLEAN:
        return JSString(value.getString()).ToJSValue();
    case JS_TAG_NUMBER:
        // TODO:
        return JSString(to_string((int)(value.getNumber())));
    case JS_TAG_STRING:
        return value;
    case JS_TAG_OBJECT:
    case JS_TAG_FUNCTION:
        return ToNumber(ToPrimitive(value));
    default:
        throw ThrowRuntimeException(EXCEPTION_TYPE, "ToString");
    }
}
JSValue ToObject(const JSValue &value)
{
    // TODO:
    throw ThrowRuntimeException(EXCEPTION_TYPE, "ToObject");
}

int ThrowRuntimeException(const EXCEPTION_ENUM &t = EXCEPTION_TYPE, const string &s = "Unknown Runtime Error")
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
void printString(const JSValue &value, string quote = "")
{
    if (quote.size() != 0)
        cout << quote;

    string s = value.getString();
    if (s == "\\n")
        cout << endl;
    else
        cout << s;

    if (quote.size() != 0)
        cout << quote;
}
void printJSObject(JSObject *obj)
{
    map<string, DataDescriptor *>::iterator it;
    map<string, DataDescriptor *> m = obj->Properties;
    cout << "{" << endl;
    for (it = m.begin(); it != m.end();)
    {
        cout << "  " << it->first << ": ";
        if (it->second->Value.isBaseObject())
            cout << "[ Object ]";
        else if (it->second->Value.isArray())
            cout << "[ Array ]";
        else
            printJSValue(it->second->Value);

        it++;
        if (it != m.end())
            cout << "," << endl;
    }
    cout << endl
         << "}";
}
void printJSArray(JSArray *arr)
{
    map<string, DataDescriptor *>::iterator it;
    map<string, DataDescriptor *> m = arr->Properties;
    cout << "[";
    for (it = m.begin(); it != m.end();)
    {
        if (it->first == "length")
        {
            it++;
            continue;
        }

        cout << " ";
        JSValue value = it->second->Value;
        if (value.isBaseObject())
            cout << "[ Object ]";
        else if (value.isArray())
            cout << "[ Array ]";
        else if (value.isString())
            printString(value, "'");
        else
            printJSValue(value);

        it++;
        if (it != m.end())
            cout << ",";
    }
    cout << " ]";
}
void printJSValue(JSValue &value)
{
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
        printString(value);
        break;
    case JS_TAG_FUNCTION:
    {
        JSFunction *fo = value.getFunction();
        if (fo->isIntrinsic())
            cout << "function " << fo->Name << "() { [native code] }";
        else
            cout << "function " << fo->Name << "() { USER CODE TODO }";
        break;
    }
    case JS_TAG_OBJECT:
        printJSObject(value.getObject());
        break;
    case JS_TAG_ARRAY:
        printJSArray(value.getArray());
        break;
    default:
        throw ThrowRuntimeException(EXCEPTION_TYPE, "printJSValue");
    }
}

void EnqueueTask(const Function_Data &fn_data)
{
    Task *task = new Task(new Function_Data(fn_data));
    fn_data.slow->loop->task_queue.push(task);
}
void EnqueueJob(const Function_Data &fn_data)
{
    Job *job = new Job(new Function_Data(fn_data));
    fn_data.slow->loop->job_queue.push(job);
}

JSValue Builtin_Console_log(const Function_Data &fn_data)
{
    vector<JSValue> args = fn_data.args;

    for (size_t i = 0; i < args.size(); i++)
        printJSValue(args[i]);

    cout << endl;
    return JS_UNDEFINED;
}

JSValue Builtin_Object(const Function_Data &fn_data)
{
    return JS_UNDEFINED;
}
JSValue Builtin_GetPrototypeOf(const Function_Data &fn_data)
{
    vector<JSValue> args = fn_data.args;

    if (!args[0].isObject())
        return JSException("getPrototypeOf args is not a object").ToJSValue();

    JSObject *proto = args[0].getObject()->Prototype;
    return proto ? proto->ToJSValue() : JS_NULL;
}
JSValue Builtin_Function_Prototype_Call(const Function_Data &fn_data)
{
    vector<JSValue> args = fn_data.args;
    JSValue thisValue = JS_UNDEFINED;
    if (args.size() > 0)
    {
        thisValue = args[0];
        args.erase(args.begin());
    }
    JSFunction *caller_fo = fn_data.thisValue.getFunction();
    return caller_fo->Call(fn_data.slow, thisValue, args);
    return JS_UNDEFINED;
}
JSValue Builtin_Function_Prototype_Bind(const Function_Data &fn_data)
{
    vector<JSValue> args = fn_data.args;
    JSValue thisArg;
    vector<JSValue> boundArgs;
    if (args.size() == 0)
        thisArg = JS_UNDEFINED;
    else
    {
        thisArg = args[0];
        args.erase(args.begin());
        boundArgs = args;
    }
    JSFunction *target = fn_data.thisValue.getFunction();
    JSFunction *F = new JSFunction(target->Name);
    F->TargetFunction = target;
    F->BoundThis = thisArg;
    F->BoundArgs = boundArgs;
    return F->ToJSValue();
}
JSValue Builtin_SetTimeout(const Function_Data &fn_data)
{
    vector<JSValue> args = fn_data.args;

    if (args.size() == 0)
        return JSException("1 argument required, but only 0 present.");

    JSFunction *delay_fo = args[0].getFunction();
    args.erase(args.begin());

    if (args.size() != 0)
        // TODO: timeout supported 0 currently, just erase it
        args.erase(args.begin());

    EnqueueTask(Function_Data(fn_data.slow, delay_fo, fn_data.thisValue, args));
    return JS_UNDEFINED;
}
JSValue Builtin_Process_Nexttick(const Function_Data &fn_data)
{
    vector<JSValue> args = fn_data.args;

    if (args.size() == 0 || !args[0].isFunction())
        return JSException("Callback must be a function.");

    JSFunction *delay_fo = args[0].getFunction();
    args.erase(args.begin());

    EnqueueJob(Function_Data(fn_data.slow, delay_fo, fn_data.thisValue, args));
    return JS_UNDEFINED;
}
JSValue Builtin_Function(const Function_Data &fn_data)
{
    return JSException("TODO:");
}
JSValue Builtin_Array(const Function_Data &fn_data)
{
    vector<JSValue> args = fn_data.args;
    JSArray *arr = new JSArray();
    int args_count = (int)args.size();

    if (args_count == 1)
    {
        JSValue first = args[0];
        int len = 0;
        if (first.isNumber())
            len = (int)first.getNumber();
        else
        {
            len = 1;
            arr->Put("0", first);
        }
        arr->Put("length", JSNumber(len).ToJSValue());
        return arr->ToJSValue();
    }

    for (size_t i = 0; i < args_count; i++)
    {
        arr->Put(to_string(i), args[i]);
    }
    arr->Put("length", JSNumber(args_count).ToJSValue());
    return arr->ToJSValue();
}
JSValue Builtin_Array_IsArray(const Function_Data &fn_data)
{
    return JSBoolean(fn_data.args[0].isArray()).ToJSValue();
}
JSValue Builtin_Array_Prototype_Push(const Function_Data &fn_data)
{
    JSObject *O = fn_data.thisValue.getObject();
    vector<JSValue> args = fn_data.args;
    int n = (int)(O->Get("length").getNumber());
    while (!args.empty())
    {
        JSValue E = args[0];
        args.erase(args.begin());
        O->Put(to_string(n), E);
        n++;
    }
    JSValue v = JSNumber(n).ToJSValue();
    O->Put("length", v);
    return v;
}
JSValue Builtin_Array_Prototype_Pop(const Function_Data &fn_data)
{
    JSObject *O = fn_data.thisValue.getObject();
    vector<JSValue> args = fn_data.args;
    int len = (int)(O->Get("length").getNumber());
    if (len == 0)
    {
        O->Put("length", JSNumber(0).ToJSValue());
        return JS_UNDEFINED;
    }
    else
    {
        JSValue indx = JSNumber(len - 1).ToJSValue();
        string indx_key = ToString(indx).getString();
        JSValue element = O->Get(indx_key);
        O->Delete(indx_key);
        O->Put("length", indx);
        return element;
    }
}
