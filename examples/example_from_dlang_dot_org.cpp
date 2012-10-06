#include <iostream>
#include <ostream>
#include <string>
#include <functional>

#include <scope_action.hpp>

#include "examples_common.hpp"

using namespace stack_unwinding;
using namespace std;

class Write
{
    std::string str_;
public:
    explicit Write(const std::string &str): str_(str){}
    void operator()() const
    {
        cout << str_;
    }
};

struct Foo
{
    Foo() { Write("0")(); }
    ~Foo() { Write("1")(); }
};
void func_throw(int i)
{
    throw i;
}
int main(int,char *[])
{
/*
    Refer http://dlang.org/statement.html#ScopeGuardStatement
    Example in D language:
    class Foo {
        this(){ Write("0"); }
        ~this() { Write("1"); }
    }
    try {
        scope(exit) Write("2");
        scope(success) Write("3");
        scope Foo f = new Foo();
        scope(failure) Write("4");
        throw new Exception("msg");
        scope(exit) Write("5");
        scope(success) Write("6");
        scope(failure) Write("7");
    }
    catch (Exception e) { }
    Writeln();

    Writes: 0412
*/
    try
    {   // need more syntactic sugar: UNIQUE_NAME macro, maybe boost::function, etc, etc..
        scope_action a=make<scope_exit>(Write("2"));
        scope_action b=make<scope_success>(Write("3"));
        Foo c;
        scope_action d=make<scope_failure>(Write("4"));
        func_throw(1); // unreachiable warnings prevention
        scope_action e=make<scope_exit>(Write("5"));
        scope_action f=make<scope_success>(Write("6"));
        scope_action g=make<scope_failure>(Write("7"));

        suppress_unused_warning(a,b,c,d,e,f,g);
    }
    catch(int){}
    std::cout << endl;
    return 0;
}

ExpectedStdoutTest test_cout("0412\n");
