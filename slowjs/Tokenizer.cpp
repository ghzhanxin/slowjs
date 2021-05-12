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

string Tokenizer::getNumString(string input, uint *current)
{
    string digitStr = "";
    digitStr.push_back(input[*current]);
    char nextChar = input[++*current];
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
        nextChar = input[++*current];
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
        cout << tok->type << "  -> " << tok->value << endl;
    }
}

bool Tokenizer::isIdentifierStart(char c)
{
    return isalpha(c) || c == '_' || c == '$';
}

queue<Token *> Tokenizer::tokenize(string input)
{
    queue<Token *> q;

    size_t inputSize = input.size();
    uint current = 0;
    while (current < inputSize)
    {
        char c = input[current];

        if (isspace(c))
        {
            current++;
            continue;
        }

        if (c == '(')
        {
            q.push(new Token(tt::parenL, "("));
            current++;
            continue;
        }

        if (c == ')')
        {
            q.push(new Token(tt::parenR, ")"));
            current++;
            continue;
        }

        if (isdigit(c))
        {
            string digitStr = getNumString(input, &current);
            q.push(new Token(tt::num, digitStr));
            continue;
        }

        if (isIdentifierStart(c))
        {
            string alphaStr = "";
            while (isIdentifierStart(c) || isdigit(c))
            {
                alphaStr += c;
                c = input[++current];
            }

            tt::Token_Type type = tt::name;
            Token *tok = new Token(type, alphaStr);
            if (alphaStr == "true")
                type = tt::_true;
            if (alphaStr == "false")
                type = tt::_false;
            if (alphaStr == "null")
                type = tt::_null;
            if (alphaStr == "undefined")
                type = tt::_undefined;
            if (alphaStr == "break")
                type = tt::_break;
            if (alphaStr == "return")
                type = tt::_return;
            if (alphaStr == "new")
                type = tt::_new;
            if (alphaStr == "this")
                type = tt::_this;
            if (alphaStr == "continue")
                type = tt::_continue;

            tok->type = type;
            q.push(tok);
            continue;
        }

        if (c == '\'' || c == '"')
        {
            char quote = c;
            string s = "";
            char nextChar = input[++current];
            while (nextChar != quote)
            {
                s += nextChar;
                nextChar = input[++current];
            }
            q.push(new Token(tt::string, s));
            current++;
            continue;
        }

        if (c == '*' || c == '>' || c == '<')
        {
            string tempS;
            tempS.push_back(c);
            q.push(new Token(tt::binOp, tempS));
            current++;
            continue;
        }

        if (c == '/')
        {
            char nextChar = input[++current];
            if (nextChar == '/')
            {
                string commentStr = "//";
                nextChar = input[++current];
                while (nextChar != '\n')
                {
                    commentStr += nextChar;
                    nextChar = input[++current];
                }
                continue;
            }
            else
                throwTokenizeError(nextChar);
        }

        if (c == '+')
        {
            string s = "+";
            char nextChar = input[++current];
            if (nextChar == '+')
            {
                while (nextChar == '+')
                {
                    s += nextChar;
                    nextChar = input[++current];
                }
                size_t count = s.size();
                if (count == 2)
                {
                    q.push(new Token(tt::inc, s));
                    continue;
                }
                else
                    throwTokenizeError(c);
            }
            else if (isdigit(nextChar))
            {
                string digiStr = getNumString(input, &current);
                string signedStr = s + digiStr;
                q.push(new Token(tt::num, signedStr));
                continue;
            }
            else
            {
                q.push(new Token(tt::plus, s));
                continue;
            }
        }

        if (c == '-')
        {
            string s = "-";
            char nextChar = input[++current];
            if (nextChar == '-')
            {
                while (nextChar == '-')
                {
                    s += nextChar;
                    nextChar = input[++current];
                }
                size_t count = s.size();
                if (count == 2)
                {
                    q.push(new Token(tt::dec, s));
                    continue;
                }
                else
                    throwTokenizeError(nextChar);
            }
            else if (isdigit(nextChar))
            {
                string digiStr = getNumString(input, &current);
                string signedStr = s + digiStr;
                q.push(new Token(tt::num, signedStr));
                continue;
            }
            else
            {
                q.push(new Token(tt::minus, s));
                continue;
            }
        }

        if (c == '=')
        {
            string s = "=";
            char nextChar = input[++current];
            while (nextChar == '=')
            {
                s += nextChar;
                nextChar = input[++current];
            }

            size_t count = s.size();
            if (count == 1)
            {
                q.push(new Token(tt::eq, s));
                continue;
            }
            if (count == 2 || count == 3)
            {
                q.push(new Token(tt::equality, s));
                continue;
            }
            else
                throwTokenizeError(nextChar);
        }

        if (c == ';')
        {
            string tempS;
            tempS.push_back(c);
            q.push(new Token(tt::semi, tempS));
            current++;
            continue;
        }

        if (c == ',')
        {
            string tempS;
            tempS.push_back(c);
            q.push(new Token(tt::comma, tempS));
            current++;
            continue;
        }

        if (c == '{')
        {
            q.push(new Token(tt::braceL, "{"));
            current++;
            continue;
        }

        if (c == '}')
        {
            q.push(new Token(tt::braceR, "}"));
            current++;
            continue;
        }

        if (c == '.')
        {
            q.push(new Token(tt::dot, "."));
            current++;
            continue;
        }

        throwTokenizeError(c);
    };

    //    queue<Token *> p = q;
    //    printTokenQueue(p);

    return q;
};
