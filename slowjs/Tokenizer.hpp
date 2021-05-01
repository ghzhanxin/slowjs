//
//  Tokenizer.hpp
//  slowjs
//
//  Created by zx on 2021/4/19.
//

#ifndef Tokenizer_hpp
#define Tokenizer_hpp

#include <stdio.h>
#include <string>
#include <queue>

#define SyntaxErrorPrefix "SyntaxError: "
#define UnexpectedTokenPrefix "Unexpected token "
#define ExceptionTokenPrefix SyntaxErrorPrefix UnexpectedTokenPrefix

using namespace std;

namespace tt
{
    enum Token_Type
    {
        num,
        regexp,
        string,
        name,
        eof,

        // punctuation
        bracketL,
        bracketR,
        braceL,
        braceR,
        parenL,
        parenR,
        comma,
        semi,
        colon,
        dot,
        question,

        // operator
        eq,
        equality,
        binOp,
        plus,
        minus,
        star,
        slash,
        inc,
        dec,

        // keyword
        _true,
        _false,
        _null,
        _undefined,
        _break,
        _return,
        _new,
        _this,
    };
}

class Token
{
public:
    Token(tt::Token_Type type, string value = "");

    tt::Token_Type type;
    string value;
};

class Tokenizer
{
public:
    queue<Token *> tokenize(string s);
};

#endif /* Tokenizer_hpp */
