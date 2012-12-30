// Refer http://boost.2283326.n4.nabble.com/scope-exit-D-style-scope-failure-and-scope-success-in-C-tp4636441p4636477.html

#include <iostream>
#include <ostream>
#include <string>
#include <memory>

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

auto_ptr<ExpCountPrinter> global;

struct DtorStuff
{
    ~DtorStuff()
    {
        global.reset(new ExpCountPrinter);
    }
};

int main()
{
    try
    {
        DtorStuff d; suppress_unused_warning(d);
        throw 1;
    }
    catch(int) {}

    global.reset();

    return 0;
}

ExpectedStdoutTest test_cout
(
    "uncaught_exception_count=1\n"
    "uncaught_exception_count=0\n"
);
