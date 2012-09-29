#include <iostream>
#include <ostream>
#include <string>

#include <stack_unwinding.hpp>

using namespace std;

class File
{
    stack_unwinding::unwinding_indicator indicator;
    bool tried_close;
public:
    File() : tried_close(false) {}
    void close()
    {
        cout << "trying close" << endl;
        tried_close=true;
        throw 1;
    }
    ~File()
    {
        const bool unwinding=indicator.unwinding();
        cout << "~File(): " << (unwinding ? "can't throw" : "can throw") << endl;
        if(!tried_close)
        {
            if(unwinding)
            {
                try { close(); } catch(...) {}
            }
            else
            {
                close();
            }
        }
    }
};

int main(int,char *[])
{
    try
    {
        cout << "---------Case #1: new-style, throwing destructors" << endl;
        File a,b;
        // ...
    }
    catch(int)
    {
        cout << "Handling single failure" << endl;
    }

    try
    {
        cout << "---------Case #2: old-style, manual close" << endl;
        File a,b;
        // ...
        a.close();
        b.close();
    }
    catch(int)
    {
        cout << "Handling single failure" << endl;
    }

    try
    { // But if we want to handle each failure explicitly,
      //  then we will have same solution for old and new style:
        cout << "---------Case #3: handling each failure explicitly" << endl;
        File a,b;
        // ...
        try { a.close(); } catch(int) { cout << "Handling faulure in a" << endl;}
        try { b.close(); } catch(int) { cout << "Handling faulure in b" << endl;}
    }
    catch(int)
    {
        cout << "Handling single failure" << endl;
    }

    return 0;
}

