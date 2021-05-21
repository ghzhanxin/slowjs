//
//  Tokenizer.cpp
//  slowjs
//
//  Created by zx on 2021/4/19.
//

#include "Tokenizer.hpp"

using namespace std;

void Tokenizer::throwCharError(char c)
{
    string msg = ExceptionTokenPrefix;
    msg.push_back('\'');
    msg.push_back(c);
    msg.push_back('\'');
    msg += " in line: " + to_string(_line) + " column: " + to_string(_column);
    throw msg;
}
void Tokenizer::throwTokenError(const string &token)
{
    string msg = ExceptionTokenPrefix;
    msg += "'" + token + "'" + " in line: " + to_string(_line) + " column: " + to_string(_column);
    throw msg;
}

tt::Token_Type Tokenizer::getIdentifierType(const string &s)
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
    else if (s == "throw")
        return tt::_throw;
    else if (s == "do")
        return tt::_do;
    else if (s == "while")
        return tt::_while;
    else
        return tt::name;
}
void Tokenizer::pushSingleToken(tt::Token_Type type, char c)
{
    string s;
    _q.push(NewToken(type, s + c));
    ++_pos;
}
Token *Tokenizer::NewToken(tt::Token_Type t, const string &v = "")
{
    return new Token(t, v, _line, _column, _pos);
}

string Tokenizer::getNumString(const string &input)
{
    string digitStr;
    digitStr += input[_pos];
    char nextChar = input[++_pos];
    bool hasDot = false;
    while (isdigit(nextChar) || nextChar == '.')
    {
        if (nextChar == '.')
        {
            if (hasDot)
                throwCharError(nextChar);
            else
                hasDot = true;
        }

        digitStr += nextChar;
        nextChar = input[++_pos];
    }
    return digitStr;
}

void Tokenizer::printTokenQueue(queue<Token *> q)
{
    cout << endl
         << "---------- token result ----------" << endl
         << endl;
    while (!q.empty())
    {
        Token *tok = q.front();
        q.pop();
        cout << tok->type << "  -> " << tok->value << "    line: " << tok->line << "    column: " << tok->column << endl;
        cout << "linebreak   " <<tok->isNextLineBreak << endl;
    }
}

bool Tokenizer::isIdentifierStart(char c)
{
    return isalpha(c) || c == '_' || c == '$';
}

queue<Token *> Tokenizer::tokenize(const string &input)
{
    _pos = 0;
    _line = 1;
    _column = 0;
    int last_pos = _pos;

    size_t inputSize = input.size();
    while (_pos < inputSize)
    {
        int token_length = _pos - last_pos;
        _column += token_length;

        last_pos = _pos;

        char c = input[_pos];

        if (isspace(c))
        {
            if (c == '\n')
            {
                _line++;
                _column = 0;

                if (!_q.empty())
                    _q.back()->isNextLineBreak = true;
            }
            _pos++;
            continue;
        }

        if (c == '^')
        {
            pushSingleToken(tt::bitwiseXOR, c);
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

        if (c == '?')
        {
            pushSingleToken(tt::question, c);
            continue;
        }

        if (c == ':')
        {
            pushSingleToken(tt::colon, c);
            continue;
        }

        if (isdigit(c))
        {
            string digitStr = getNumString(input);
            _q.push(NewToken(tt::num, digitStr));
            continue;
        }

        if (isIdentifierStart(c))
        {
            string alphaStr;
            while (isIdentifierStart(c) || isdigit(c))
            {
                alphaStr += c;
                c = input[++_pos];
            }

            tt::Token_Type type = getIdentifierType(alphaStr);
            _q.push(NewToken(type, alphaStr));
            continue;
        }

        if (c == '\'' || c == '"')
        {
            char quote = c;
            string s;
            char nextChar = input[++_pos];
            while (nextChar != quote)
            {
                if (nextChar == '\n')
                    throwCharError(nextChar);

                s += nextChar;
                nextChar = input[++_pos];
            }
            _q.push(NewToken(tt::string, s));
            _pos++;
            continue;
        }

        if (c == '>')
        {
            string s = ">";
            char nextChar = input[++_pos];
            while (nextChar == '=')
            {
                s += nextChar;
                nextChar = input[++_pos];
            }

            if (s == ">" || s == ">=")
                _q.push(NewToken(tt::relational, s));
            else
                throwTokenError(s);
            continue;
        }

        if (c == '<')
        {
            string s = "<";
            char nextChar = input[++_pos];
            while (nextChar == '=')
            {
                s += nextChar;
                nextChar = input[++_pos];
            }

            if (s == "<" || s == "<=")
                _q.push(NewToken(tt::relational, s));
            else
                throwTokenError(s);
            continue;
        }

        if (c == '%')
        {
            string s = "%";
            char nextChar = input[++_pos];
            while (nextChar == '=')
            {
                s += nextChar;
                nextChar = input[++_pos];
            }

            if (s == "%")
                _q.push(NewToken(tt::equality, s));
            else if (s == "%=")
                _q.push(NewToken(tt::assign, s));
            else
                throwTokenError(s);
            continue;
        }

        if (c == '*')
        {
            string s = "*";
            char nextChar = input[++_pos];
            while (nextChar == '=')
            {
                s += nextChar;
                nextChar = input[++_pos];
            }

            if (s == "*")
                _q.push(NewToken(tt::star, s));
            else if (s == "*=")
                _q.push(NewToken(tt::assign, s));
            else
                throwTokenError(s);
            continue;
        }

        if (c == '/')
        {
            char nextChar = input[++_pos];
            if (nextChar == '/')
            {
                string commentStr = "//";
                nextChar = input[++_pos];
                while (nextChar != '\n')
                {
                    commentStr += nextChar;
                    nextChar = input[++_pos];
                }
                continue;
            }
            else
            {
                string s = "/";
                while (nextChar == '=')
                {
                    s += nextChar;
                    nextChar = input[++_pos];
                }

                if (s == "/")
                    _q.push(NewToken(tt::slash, s));
                else if (s == "/=")
                    _q.push(NewToken(tt::assign, s));
                else
                    throwTokenError(s);
                continue;
            }
        }

        if (c == '+')
        {
            string s = "+";
            char nextChar = input[++_pos];
            if (isdigit(nextChar))
            {
                string digiStr = getNumString(input);
                string signedStr = s + digiStr;
                _q.push(NewToken(tt::num, signedStr));
                continue;
            }
            else
            {
                while (nextChar == '=' || nextChar == '+')
                {
                    s += nextChar;
                    nextChar = input[++_pos];
                }

                if (s == "+")
                    _q.push(NewToken(tt::plus, s));
                else if (s == "+=")
                    _q.push(NewToken(tt::assign, s));
                else if (s == "++")
                    _q.push(NewToken(tt::inc, s));
                else
                    throwTokenError(s);
                continue;
            }
        }

        if (c == '-')
        {
            string s = "-";
            char nextChar = input[++_pos];
            if (isdigit(nextChar))
            {
                string digiStr = getNumString(input);
                string signedStr = s + digiStr;
                _q.push(NewToken(tt::num, signedStr));
                continue;
            }
            else
            {
                while (nextChar == '=' || nextChar == '-')
                {
                    s += nextChar;
                    nextChar = input[++_pos];
                }

                if (s == "-")
                    _q.push(NewToken(tt::minus, s));
                else if (s == "-=")
                    _q.push(NewToken(tt::assign, s));
                else if (s == "--")
                    _q.push(NewToken(tt::dec, s));
                else
                    throwTokenError(s);
                continue;
            }
        }

        if (c == '!')
        {
            string s = "!";
            char nextChar = input[++_pos];
            while (nextChar == '=')
            {
                s += nextChar;
                nextChar = input[++_pos];
            }

            if (s == "!")
                _q.push(NewToken(tt::prefix, s));
            else if (s == "!=")
                _q.push(NewToken(tt::equality, s));
            else if (s == "!==")
                _q.push(NewToken(tt::equality, s));
            else
                throwTokenError(s);
            continue;
        }

        if (c == '=')
        {
            string s = "=";
            char nextChar = input[++_pos];
            while (nextChar == '=')
            {
                s += nextChar;
                nextChar = input[++_pos];
            }

            if (s == "=")
                _q.push(NewToken(tt::eq, s));
            else if (s == "==" || s == "===")
                _q.push(NewToken(tt::equality, s));
            else
                throwTokenError(s);
            continue;
        }

        if (c == '&')
        {
            string s = "&";
            char nextChar = input[++_pos];
            while (nextChar == '&')
            {
                s += nextChar;
                nextChar = input[++_pos];
            }

            if (s == "&")
                _q.push(NewToken(tt::bitwiseAND, s));
            else if (s == "&&")
                _q.push(NewToken(tt::logicalAND, s));
            else
                throwTokenError(s);
            continue;
        }

        if (c == '|')
        {
            string s = "|";
            char nextChar = input[++_pos];
            while (nextChar == '|')
            {
                s += nextChar;
                nextChar = input[++_pos];
            }

            if (s == "|")
                _q.push(NewToken(tt::bitwiseOR, s));
            else if (s == "||")
                _q.push(NewToken(tt::logicalOR, s));
            else
                throwTokenError(s);
            continue;
        }

        throwCharError(c);
    };

    return _q;
};
