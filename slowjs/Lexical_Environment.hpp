//
//  Lexical_Environment.hpp
//  slowjs
//
//  Created by zx on 2021/4/19.
//

#ifndef Lexical_Environment_hpp
#define Lexical_Environment_hpp

#include <stdio.h>
#include "Environment_Record.hpp"

class Lexical_Environment
{
public:
    Environment_Record *record;
    Lexical_Environment *outer;
};

#endif /* Lexical_Environment_hpp */