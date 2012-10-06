#include <iostream>
#include <ostream>
#include <string>

#include <stack_unwinding.hpp>

#include "examples_common.hpp"

using namespace std;

struct ExpCountPrinter
{
    ~ExpCountPrinter()
    {
        cout << "uncaught_exception_count=" << stack_unwinding::uncaught_exception_count() << endl;
    }
};

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

int main(int,char *[])
{
    ExpCountPrinter a;
    ExptSwallower<ExpCountPrinter> b;
    ExptSwallower<ExptSwallower<ExpCountPrinter> > c;
    ExptSwallower<ExptSwallower<ExptSwallower<ExpCountPrinter> > > d;
    ExptSwallower<ExptSwallower<ExptSwallower<ExptSwallower<ExpCountPrinter> > > > e;
    (void)a;(void)b;(void)c;(void)d;(void)e; // unused warnings prevention
    return 0;
}

ExpectedStdoutTest test_cout
(
    "uncaught_exception_count=4\n"
    "uncaught_exception_count=3\n"
    "uncaught_exception_count=2\n"
    "uncaught_exception_count=1\n"
    "uncaught_exception_count=0\n"
);
