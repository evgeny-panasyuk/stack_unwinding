#include <iostream>
#include <ostream>
#include <string>

#include <stack_unwinding.hpp>

#include "examples_common.hpp"

using namespace std;
using namespace stack_unwinding;

struct ExpCountPrinter
{
    ExpCountPrinter()
    {
        cout << "uncaught_exception_count=" << uncaught_exception_count() << endl;
    }
    ~ExpCountPrinter()
    {
        cout << "uncaught_exception_count=" << uncaught_exception_count() << endl;
    }
};

ExpCountPrinter *global_ptr=0;

struct DtorStuff
{
    ~DtorStuff()
    {
        global_ptr=new ExpCountPrinter;
    }
};

int main(int,char *[])
{
    try
    {
        DtorStuff d; suppress_unused_warning(d);
        throw 1;
    }
    catch(int) {}

    delete global_ptr;

    return 0;
}

ExpectedStdoutTest test_cout
(
    "uncaught_exception_count=1\n"
    "uncaught_exception_count=0\n"
);
