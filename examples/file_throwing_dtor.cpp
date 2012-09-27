#include <iostream>
#include <ostream>
#include <string>

#include <stack_unwinding.hpp>

using namespace std;

class File
{
    stack_unwinding::unwinding_indicator indicator;
public:
    void close()
    {
        throw 1;
    }
    ~File()
    {
        if(indicator.unwinding())
        {
            cout << "~File(): can't throw" << endl;
            try
            {
                close();
            }
            catch(...)
            {
            }
        }
        else
        {
            cout << "~File(): can throw" << endl;
            close();
        }
    }
};

int main(int,char *[])
{
    try
    { // new-style
        cout << "---------Case #1:" << endl;
        File a,b;
        // ...
    }
    catch(int)
    {
        cout << "Handling single failure" << endl;
    }

    try
    { // old-syle
        cout << "---------Case #2:" << endl;
        File a,b;
        // ...
        a.close();
        b.close();
    }
    catch(int){}

    try
    { // But if we want to handle each failure explicitly,
      //  then we will have same solution for old and new style:
        cout << "---------Case #3:" << endl;
        File a,b;
        // ...
        try { a.close(); } catch(int) { cout << "Handling faulure in a" << endl;}
        try { a.close(); } catch(int) { cout << "Handling faulure in b" << endl;}
    }
    catch(int)
    {
        cout << "Handling single failure" << endl;
    }

    return 0;
}

