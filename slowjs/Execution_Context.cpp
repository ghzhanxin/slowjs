//
//  Execution_Context.cpp
//  slowjs
//
//  Created by zx on 2021/4/19.
//

#include "Execution_Context.hpp"

bool Environment_Record::HasBinding(string name)
{
    return this->find(name) != this->end();
}

void Environment_Record::CreateMutableBinding(string name)
{
    this->insert(pair<string, JSValue>(name, JS_UNDEFINED));
};

void Environment_Record::SetMutableBinding(string name, JSValue value)
{
    Environment_Record::iterator it = this->find(name);
    if (it == this->end())
        this->insert(pair<string, JSValue>(name, value));
    else
        it->second = value;
};

JSValue Environment_Record::GetBindingValue(string name)
{
    Environment_Record::iterator it = this->find(name);
    return it == this->end() ? JS_UNDEFINED : it->second;
}

// bool Object_ER::HasBinding(string name)
// {
//     return this->binding_obj.getObject()->HasProperty(name);
// }

// void Object_ER::CreateMutableBinding(string name)
// {
//     this->binding_obj.getObject()->Put(name, JS_UNDEFINED);
// };

// void Object_ER::SetMutableBinding(string name, JSValue value)
// {
//     this->binding_obj.getObject()->Put(name, value);
// };

// JSValue Object_ER::GetBindingValue(string name)
// {
//     return this->binding_obj.getObject()->Get(name);
// }
