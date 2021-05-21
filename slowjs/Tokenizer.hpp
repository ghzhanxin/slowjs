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
        lineBreak,

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
        bitwiseAND,
        bitwiseOR,
        bitwiseXOR,

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
        _throw,
        _do,
        _while,
    };
}

class Token
{
public:
    Token(tt::Token_Type t, string v = "", int l = 0, int c = 0, int p = 0)
        : type(t), value(v), line(l), column(c) {}

    tt::Token_Type type;
    string value;
    int line;
    int column;
    bool isNextLineBreak = false;
};

class Tokenizer
{
public:
    static tt::Token_Type getIdentifierType(const string &);
    static void printTokenQueue(queue<Token *>);

    void throwCharError(char);
    void throwTokenError(const string &);
    string getNumString(const string &);
    bool isIdentifierStart(char c);
    void pushSingleToken(tt::Token_Type, char);
    Token *NewToken(tt::Token_Type, const string &);

    queue<Token *> tokenize(const string &s);

private:
    queue<Token *> _q;
    uint _pos;
    int _line = 1;
    int _column = 1;
};

#endif /* Tokenizer_hpp */
