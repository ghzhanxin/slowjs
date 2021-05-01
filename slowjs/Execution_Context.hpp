//
//  Execution_Context.hpp
//  slowjs
//
//  Created by zx on 2021/4/19.
//

#ifndef Execution_Context_hpp
#define Execution_Context_hpp

#include <iostream>
#include "Lexical_Environment.hpp"

class Execution_Context
{
public:
    Lexical_Environment *lex_env;
    Lexical_Environment *var_env;
    JSValue this_binding;
};

#endif /* Execution_Context_hpp */
