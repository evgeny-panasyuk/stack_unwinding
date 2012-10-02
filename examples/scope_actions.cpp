#include <iostream>
#include <ostream>
#include <string>
#include <functional>

#include <scope_action.hpp>

#include "examples_common.hpp"

using namespace stack_unwinding;
using namespace std;

class Print
{
    std::string str_;
public:
    Print(const std::string &str): str_(str){}
    void operator()() const
    {
        cout << str_ << endl;
    }
};

int main(int,char *[])
{
    try
    {
        cout << "Case #1: stack unwinding" << endl;
        scope_action exit=make<scope_exit>(Print(" exit"));
        scope_action failure=make<scope_failure>(Print(" failure")); 
        scope_action success=make<scope_success>(Print(" success"));
        throw 1;

        (void)exit;(void)failure;(void)success; // unused warnings prevention
    } catch(int){}
    {
        cout << "Case #2: normal exit" << endl;
        scope_action exit=make<scope_exit>(Print(" exit"));
        scope_action failure=make<scope_failure>(Print(" failure")); 
        scope_action success=make<scope_success>(Print(" success"));

        (void)exit;(void)failure;(void)success; // unused warnings prevention
    }
    return 0;
}

ExpectedStdoutTest test_cout
(
    "Case #1: stack unwinding\n"
    " failure\n"
    " exit\n"
    "Case #2: normal exit\n"
    " success\n"
    " exit\n"
);
