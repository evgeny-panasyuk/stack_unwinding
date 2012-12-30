#include <iostream>
#include <ostream>
#include <string>

#include <stack_unwinding.hpp>

#include "examples_common.hpp"

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

            suppress_unused_warning(a,b);
        }catch(int){}
    }
};

// Refer Andrei Alexandrescu talk at:
// http://channel9.msdn.com/Events/Lang-NEXT/Lang-NEXT-2012/Three-Unlikely-Successful-Features-of-D
class DLikeScopeGuard
{
    stack_unwinding::unwinding_indicator indicator;

    DLikeScopeGuard(const DLikeScopeGuard&);
    DLikeScopeGuard &operator=(const DLikeScopeGuard&);
public:
    DLikeScopeGuard(){}
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

int main()
{
    {
        cout  << "no exception:" << endl;
        DLikeScopeGuard success;
    }
    try
    {
        cout << endl << "basic throw:" << endl;
        DLikeScopeGuard fail; suppress_unused_warning(fail);
        throw 0;
    }catch(int){}
    try
    {
        cout << endl << "fail in previous destructor:" << endl;
        DLikeScopeGuard fail;
        ThrowableDestructor d;

        suppress_unused_warning(d,fail);
    }catch(int){}
    try
    {
        cout << endl << "fail in next destructor:" << endl;
        ThrowableDestructor d;
        DLikeScopeGuard success;

        suppress_unused_warning(d,success);
    }catch(int){}
    try
    {
        cout << endl << "ExptSwallower<DLikeScopeGuard,ThrowableDestructor> during throw:" << endl;
        ExptSwallower<DLikeScopeGuard,ThrowableDestructor> fail; suppress_unused_warning(fail);
        throw 0;
    }catch(int){}
    try
    {
        cout << endl << "ExptSwallower<ThrowableDestructor,DLikeScopeGuard> during throw:" << endl;
        ExptSwallower<ThrowableDestructor,DLikeScopeGuard> fail; suppress_unused_warning(fail);
        throw 0;
    }catch(int){}
    try
    {
        cout << endl << "ExptSwallower<DLikeScopeGuard,int> during throw:" << endl;
        ExptSwallower<DLikeScopeGuard,int> success; suppress_unused_warning(success);
        throw 0;
    }catch(int){}
    return 0;
}

ExpectedStdoutTest test_cout
(
    "no exception:\n"
    "scope_success\n"
    "scope_exit\n"
    "\n"
    "basic throw:\n"
    "scope_failure\n"
    "scope_exit\n"
    "\n"
    "fail in previous destructor:\n"
    "scope_failure\n"
    "scope_exit\n"
    "\n"
    "fail in next destructor:\n"
    "scope_success\n"
    "scope_exit\n"
    "\n"
    "ExptSwallower<DLikeScopeGuard,ThrowableDestructor> during throw:\n"
    "scope_failure\n"
    "scope_exit\n"
    "\n"
    "ExptSwallower<ThrowableDestructor,DLikeScopeGuard> during throw:\n"
    "scope_success\n"
    "scope_exit\n"
    "\n"
    "ExptSwallower<DLikeScopeGuard,int> during throw:\n"
    "scope_success\n"
    "scope_exit\n"
);
