//
//  Tokenizer.cpp
//  slowjs
//
//  Created by zx on 2021/4/19.
//

#include "Tokenizer.hpp"

using namespace std;

void Tokenizer::throwTokenizeError(char c)
{
    string msg = ExceptionTokenPrefix;
    msg.push_back('\'');
    msg.push_back(c);
    msg.push_back('\'');
    throw msg;
}

tt::Token_Type Tokenizer::getIdentifierType(string s)
{
    if (s == "true")
        return tt::_true;
    else if (s == "false")
        return tt::_false;
    else if (s == "null")
        return tt::_null;
    else if (s == "undefined")
        return tt::_undefined;
    else if (s == "break")
        return tt::_break;
    else if (s == "return")
        return tt::_return;
    else if (s == "new")
        return tt::_new;
    else if (s == "this")
        return tt::_this;
    else if (s == "continue")
        return tt::_continue;
    else
        return tt::name;
}
void Tokenizer::pushSingleToken(tt::Token_Type type, char c)
{
    string s;
    _q.push(new Token(type, s + c));
    _current++;
}

string Tokenizer::getNumString(string input)
{
    string digitStr;
    digitStr += input[_current];
    char nextChar = input[++_current];
    bool hasDot = false;
    while (isdigit(nextChar) || nextChar == '.')
    {
        if (nextChar == '.')
        {
            if (!hasDot)
                hasDot = true;
            else
            {
                throwTokenizeError(nextChar);
            }
        }

        digitStr += nextChar;
        nextChar = input[++_current];
    }
    return digitStr;
}

void Tokenizer::printTokenQueue()
{
    cout << endl
         << "---------- token result ----------" << endl
         << endl;
    while (!_q.empty())
    {
        Token *tok = _q.front();
        _q.pop();
        cout << tok->type << "  -> " << tok->value << endl;
    }
}

bool Tokenizer::isIdentifierStart(char c)
{
    return isalpha(c) || c == '_' || c == '$';
}

queue<Token *> Tokenizer::tokenize(string input)
{
    _current = 0;
    size_t inputSize = input.size();
    while (_current < inputSize)
    {
        char c = input[_current];

        if (isspace(c))
        {
            _current++;
            continue;
        }

        if (c == '(')
        {
            pushSingleToken(tt::parenL, c);
            continue;
        }

        if (c == ')')
        {
            pushSingleToken(tt::parenR, c);
            continue;
        }

        if (c == ';')
        {
            pushSingleToken(tt::semi, c);
            continue;
        }

        if (c == ',')
        {
            pushSingleToken(tt::comma, c);
            continue;
        }

        if (c == '{')
        {
            pushSingleToken(tt::braceL, c);
            continue;
        }

        if (c == '}')
        {
            pushSingleToken(tt::braceR, c);
            continue;
        }

        if (c == '.')
        {
            pushSingleToken(tt::dot, c);
            continue;
        }

        if (isdigit(c))
        {
            string digitStr = getNumString(input);
            _q.push(new Token(tt::num, digitStr));
            continue;
        }

        if (isIdentifierStart(c))
        {
            string alphaStr;
            while (isIdentifierStart(c) || isdigit(c))
            {
                alphaStr += c;
                c = input[++_current];
            }

            tt::Token_Type type = getIdentifierType(alphaStr);
            _q.push(new Token(type, alphaStr));
            continue;
        }

        if (c == '\'' || c == '"')
        {
            char quote = c;
            string s;
            char nextChar = input[++_current];
            while (nextChar != quote)
            {
                if (nextChar == '\n')
                    throwTokenizeError(nextChar);

                s += nextChar;
                nextChar = input[++_current];
            }
            _q.push(new Token(tt::string, s));
            _current++;
            continue;
        }

        if (c == '>')
        {
            string s = ">";
            char nextChar = input[++_current];
            while (nextChar == '=')
            {
                s += nextChar;
                nextChar = input[++_current];
            }

            if (s == ">" || s == ">=")
                _q.push(new Token(tt::relational, s));
            else
                throwTokenizeError(nextChar);
            continue;
        }

        if (c == '<')
        {
            string s = "<";
            char nextChar = input[++_current];
            while (nextChar == '=')
            {
                s += nextChar;
                nextChar = input[++_current];
            }

            if (s == "<" || s == "<=")
                _q.push(new Token(tt::relational, s));
            else
                throwTokenizeError(nextChar);
            continue;
        }

        if (c == '%')
        {
            string s = "%";
            char nextChar = input[++_current];
            while (nextChar == '=')
            {
                s += nextChar;
                nextChar = input[++_current];
            }

            if (s == "%")
                _q.push(new Token(tt::equality, s));
            else if (s == "%=")
                _q.push(new Token(tt::assign, s));
            else
                throwTokenizeError(nextChar);
            continue;
        }

        if (c == '*')
        {
            string s = "*";
            char nextChar = input[++_current];
            while (nextChar == '=')
            {
                s += nextChar;
                nextChar = input[++_current];
            }

            if (s == "*")
                _q.push(new Token(tt::star, s));
            else if (s == "*=")
                _q.push(new Token(tt::assign, s));
            else
                throwTokenizeError(nextChar);
            continue;
        }

        if (c == '/')
        {
            char nextChar = input[++_current];
            if (nextChar == '/')
            {
                string commentStr = "//";
                nextChar = input[++_current];
                while (nextChar != '\n')
                {
                    commentStr += nextChar;
                    nextChar = input[++_current];
                }
                continue;
            }
            else
            {
                string s = "/";
                s += nextChar;
                while (nextChar == '=')
                {
                    s += nextChar;
                    nextChar = input[++_current];
                }

                if (s == "/")
                    _q.push(new Token(tt::slash, s));
                else if (s == "/=")
                    _q.push(new Token(tt::assign, s));
                else
                    throwTokenizeError(nextChar);
                continue;
            }
        }

        if (c == '+')
        {
            string s = "+";
            char nextChar = input[++_current];
            if (isdigit(nextChar))
            {
                string digiStr = getNumString(input);
                string signedStr = s + digiStr;
                _q.push(new Token(tt::num, signedStr));
                continue;
            }
            else
            {
                while (nextChar == '=' || nextChar == '+')
                {
                    s += nextChar;
                    nextChar = input[++_current];
                }

                if (s == "+")
                    _q.push(new Token(tt::plus, s));
                else if (s == "+=")
                    _q.push(new Token(tt::assign, s));
                else if (s == "++")
                    _q.push(new Token(tt::inc, s));
                else
                    throwTokenizeError(nextChar);
                continue;
            }
        }

        if (c == '-')
        {
            string s = "-";
            char nextChar = input[++_current];
            if (isdigit(nextChar))
            {
                string digiStr = getNumString(input);
                string signedStr = s + digiStr;
                _q.push(new Token(tt::num, signedStr));
                continue;
            }
            else
            {
                while (nextChar == '=' || nextChar == '-')
                {
                    s += nextChar;
                    nextChar = input[++_current];
                }

                if (s == "-")
                    _q.push(new Token(tt::minus, s));
                else if (s == "-=")
                    _q.push(new Token(tt::assign, s));
                else if (s == "--")
                    _q.push(new Token(tt::dec, s));
                else
                    throwTokenizeError(nextChar);
                continue;
            }
        }

        if (c == '!')
        {
            string s = "!";
            char nextChar = input[++_current];
            while (nextChar == '=')
            {
                s += nextChar;
                nextChar = input[++_current];
            }

            if (s == "!")
                _q.push(new Token(tt::prefix, s));
            else if (s == "!=")
                _q.push(new Token(tt::equality, s));
            else if (s == "!==")
                _q.push(new Token(tt::equality, s));
            else
                throwTokenizeError(nextChar);
            continue;
        }

        if (c == '=')
        {
            string s = "=";
            char nextChar = input[++_current];
            while (nextChar == '=')
            {
                s += nextChar;
                nextChar = input[++_current];
            }

            if (s == "=")
                _q.push(new Token(tt::eq, s));
            else if (s == "==" || s == "===")
                _q.push(new Token(tt::equality, s));
            else
                throwTokenizeError(nextChar);
            continue;
        }

        throwTokenizeError(c);
    };

    //    queue<Token *> p = q;
    //    printTokenQueue(p);

    return _q;
};
