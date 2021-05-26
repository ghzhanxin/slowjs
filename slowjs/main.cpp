//
//  main.cpp
//  slowjs
//
//  Created by zx on 2021/4/19.
//

#include <iostream>
#include "Slowjs.hpp"

using namespace std;

string main_dir;

int main(int argc, const char *argv[])
{
    try
    {
        if (argc == 1)
            throw string("error: no input file");

        string js_path = argv[1];
        main_dir = js_path.substr(0, js_path.find_last_of('/') + 1);

        Slowjs slow;
        slow.evalFile(js_path);
        slow.loop->startLoop();

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
