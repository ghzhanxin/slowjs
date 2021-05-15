//
//  main.cpp
//  slowjs
//
//  Created by zx on 2021/4/19.
//

#include <iostream>
#include <fstream>
#include "Slowjs.hpp"

using namespace std;

string getInputCode(string fileName)
{
    ifstream in(fileName);
    if (in)
    {
        istreambuf_iterator<char> begin(in);
        istreambuf_iterator<char> end;
        string code(begin, end);
        in.close();
        return code;
    }
    else
        throw string("error: open file fail! file does not exist");
}
int main(int argc, const char *argv[])
{
    try
    {
        if (argc == 1)
            throw string("error: no input file");

        string code = getInputCode(argv[1]);

        Slowjs slow = Slowjs();
        JSValue result = slow.run(code);

        cout << endl;
        cout << "---------- Congratulations! Evaluate Success!!! ----------" << endl;
        cout << endl;
    }
    catch (string &msg)
    {
        cerr << endl;
        cerr << msg << endl;
        cerr << endl;
        return 1;
    }

    return 0;
}
