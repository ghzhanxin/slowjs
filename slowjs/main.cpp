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

string getContentFromFile(string fileName)
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

class Base
{
public:
    Base() { cout << "Base" << endl; }
    Base(int _a) : a(_a)
    {
        p = this;
        cout << "Baseint" << a << endl;
    }
    int a;
    string b;
    bool c;
    void go() { cout << "go"; }
    int k;
    double m;

    void *p;
};
class D : public Base
{
public:
    D() { cout << "D" << endl; }
    D(int x) : Base(x)
    {

        cout << "Dint" << a << endl;
    }
    // double z;
    void run() { cout << "runt"; }
    void go() { cout << "dgo" << endl; }
};

int main(int argc, const char *argv[])
{
    Base b(10);
    D d(888);
    b.go();
    d.go();
    cout << sizeof(b) << (Base *)b.p << endl
         << &b << endl;
    cout << sizeof(d) << (D *)d.p << endl
         << &d << endl;
    cout << "end" << endl;
    try
    {
        if (argc == 1)
            throw string("error: no input file");

        Slowjs slow = Slowjs();
        slow.run(getContentFromFile(argv[1]));
        slow.loop->startLoop();

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
