#include <iostream>
#include <ostream>
#include <string>
#include <functional>

#include <scope_action.hpp>

using namespace stack_unwinding;
using namespace std;

class Print
{
    std::string str_;
public:
    Print(const std::string &str): str_(str){}
    void operator()() const
    {
        cout << str_;
    }
};

struct Foo
{
    Foo()
    {
        Print("0")();
    }
    ~Foo()
    {
        Print("1")();
    }
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
        this(){ write("0"); }
        ~this() { write("1"); }
    }
    try {
        scope(exit) write("2");
        scope(success) write("3");
        scope Foo f = new Foo();
        scope(failure) write("4");
        throw new Exception("msg");
        scope(exit) write("5");
        scope(success) write("6");
        scope(failure) write("7");
    }
    catch (Exception e) { }
    writeln();

    writes: 0412
*/
    try
    {   // need more syntactic sugar: UNIQUE_NAME macro, maybe boost::function, etc, etc..
        scope_action a=make<scope_exit>(Print("2"));
        scope_action b=make<scope_success>(Print("3"));
        Foo c;
        scope_action d=make<scope_failure>(Print("4"));
        func_throw(1); // unreachiable warrnings prevention
        scope_action e=make<scope_exit>(Print("5"));
        scope_action f=make<scope_success>(Print("6"));
        scope_action g=make<scope_failure>(Print("7"));

        (void)a;(void)b;(void)c;(void)d;(void)e;(void)f;(void)g; // unused warnings prevention
    }
    catch(int){}
    std::cout << endl;
    return 0;
}

