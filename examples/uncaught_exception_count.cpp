#include <iostream>
#include <ostream>
#include <string>

#include <stack_unwinding.hpp>

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
    return 0;
}

