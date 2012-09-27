#include <iostream>
#include <ostream>
#include <string>

#include <stack_unwinding.hpp>

using namespace std;

struct ThrowableDestructor
{
    ~ThrowableDestructor()
    {
        throw 0;
    }
};

template<typename FirstScoped, typename SecondScoped=ThrowableDestructor>
struct ExptSwallower
{
    ~ExptSwallower()
    {
        try
        {
            FirstScoped a;
            SecondScoped b;
            (void)a;(void)b; // unused warnings prevention
        }catch(int){}
    }
};

// Refer Andrei Alexandrescu talk at:
// http://channel9.msdn.com/Events/Lang-NEXT/Lang-NEXT-2012/Three-Unlikely-Successful-Features-of-D
class DLikeScopeGuard
{
    stack_unwinding::unwinding_indicator indicator;
public:
    void scope_success()
    {
        cout << "scope_success" << endl;
    }
    void scope_failure()
    {
        cout << "scope_failure" << endl;
    }
    void scope_exit()
    {
        cout << "scope_exit" << endl;
    }
    ~DLikeScopeGuard()
    {
        if(!indicator.unwinding())
            scope_success();
        else
            scope_failure();
        scope_exit();
    }
};

int main(int,char *[])
{
    {
        cout  << "no exception:" << endl;
        DLikeScopeGuard success;
    }
    try
    {
        cout << endl << "basic throw:" << endl;
        DLikeScopeGuard fail;
        throw 0;
    }catch(int){}
    try
    {
        cout << endl << "fail in previous destructor:" << endl;
        DLikeScopeGuard fail;
        ThrowableDestructor d;
    }catch(int){}
    try
    {
        cout << endl << "fail in next destructor:" << endl;
        ThrowableDestructor d;
        DLikeScopeGuard success;
    }catch(int){}
    try
    {
        cout << endl << "ExptSwallower<DLikeScopeGuard,ThrowableDestructor> during throw:" << endl;
        ExptSwallower<DLikeScopeGuard,ThrowableDestructor> fail;
        throw 0;
    }catch(int){}
    try
    {
        cout << endl << "ExptSwallower<ThrowableDestructor,DLikeScopeGuard> during throw:" << endl;
        ExptSwallower<ThrowableDestructor,DLikeScopeGuard> fail;
        throw 0;
    }catch(int){}
    try
    {
        cout << endl << "ExptSwallower<DLikeScopeGuard,int> during throw:" << endl;
        ExptSwallower<DLikeScopeGuard,int> success;
        throw 0;
    }catch(int){}
    return 0;
}

