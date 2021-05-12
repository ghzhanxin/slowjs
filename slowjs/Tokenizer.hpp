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
#include <iostream>
#include <cctype>

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
        prefix,
        eq,
        assign,
        equality,
        plus,
        minus,
        star,
        slash,
        inc,
        dec,
        modulo,
        relational,
        logicalOR,
        logicalAND,

        // keyword
        _true,
        _false,
        _null,
        _undefined,
        _break,
        _return,
        _new,
        _this,
        _continue,
    };
}

class Token
{
public:
    Token(tt::Token_Type t, string v = "") : type(t), value(v) {}

    tt::Token_Type type;
    string value;
};

class Tokenizer
{
public:
    static void throwTokenizeError(char);
    static tt::Token_Type getIdentifierType(string);

    string getNumString(string input);
    void printTokenQueue();
    bool isIdentifierStart(char c);
    void pushSingleToken(tt::Token_Type t, char c);

    queue<Token *> tokenize(string s);

private:
    queue<Token *> _q;
    uint _current;
};

#endif /* Tokenizer_hpp */
