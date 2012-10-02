#include <boost/config.hpp>
#ifdef BOOST_NO_VARIADIC_MACROS
#   error "variadic macros required"
#else

#include <boost/scope_exit.hpp>
#include <boost/scope_failure.hpp>
#include <boost/scope_success.hpp>

#include <iostream>
#include <ostream>

#include "examples_common.hpp"

using namespace std;

int main(int,char *[])
{
    try
    {
        cout << "Case #1: stack unwinding" << endl;
        BOOST_SCOPE_EXIT(void) { cout << "exit" << endl; } BOOST_SCOPE_EXIT_END
        BOOST_SCOPE_FAILURE(void) { cout << "failure" << endl; } BOOST_SCOPE_FAILURE_END
        BOOST_SCOPE_SUCCESS(void) { cout << "success" << endl; } BOOST_SCOPE_SUCCESS_END
        throw 1;
    } catch(int){}
    {
        cout << "Case #2: normal exit" << endl;
        BOOST_SCOPE_EXIT(void) { cout << "exit" << endl; } BOOST_SCOPE_EXIT_END
        BOOST_SCOPE_FAILURE(void) { cout << "failure" << endl; } BOOST_SCOPE_FAILURE_END
        BOOST_SCOPE_SUCCESS(void) { cout << "success" << endl; } BOOST_SCOPE_SUCCESS_END
    }
    return 0;
}

ExpectedStdoutTest test_cout
(
        "Case #1: stack unwinding\n"
        "failure\n"
        "exit\n"
        "Case #2: normal exit\n"
        "success\n"
        "exit\n"
);


#endif
