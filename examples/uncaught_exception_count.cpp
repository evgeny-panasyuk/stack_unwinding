#include <iostream>
#include <ostream>
#include <string>

#include <boost/exception/uncaught_exception_count.hpp>

#include "examples_common.hpp"

using namespace std;

struct ExpCountPrinter
{
    ~ExpCountPrinter()
    {
        cout << "uncaught_exception_count=" << boost::uncaught_exception_count() << endl;
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
            suppress_unused_warning(a,b);
        }catch(int){}
    }
};

int main()
{
    ExpCountPrinter a;
    ExptSwallower<ExpCountPrinter> b;
    ExptSwallower<ExptSwallower<ExpCountPrinter> > c;
    ExptSwallower<ExptSwallower<ExptSwallower<ExpCountPrinter> > > d;
    ExptSwallower<ExptSwallower<ExptSwallower<ExptSwallower<ExpCountPrinter> > > > e;

    suppress_unused_warning(a,b,c,d,e);
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
