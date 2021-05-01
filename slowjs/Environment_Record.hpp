//
//  Environment_Record.hpp
//  slowjs
//
//  Created by zx on 2021/4/19.
//

#ifndef Environment_Record_hpp
#define Environment_Record_hpp

#include <stdio.h>
#include <iostream>
#include <map>
#include "JSValue_Type.hpp"

using namespace std;

class Environment_Record : public map<string, JSValue>
{

public:
    Environment_Record(){};
    bool HasBinding(string name);
    void CreateMutableBinding(string name);
    void SetMutableBinding(string name, JSValue value);
    JSValue GetBindingValue(string name);
};

#endif /* Environment_Record_hpp */
