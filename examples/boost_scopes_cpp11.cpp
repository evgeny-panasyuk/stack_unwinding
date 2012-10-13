#include <boost/scope_exit.hpp>
#include <boost/scope_failure.hpp>
#include <boost/scope_success.hpp>
#include <iostream>
#include <ostream>

#include "examples_common.hpp"

using namespace std;

#ifdef BOOST_NO_LAMBDAS
int main(int,char *[])
#else

#define SCOPE_ALL_REF_exit BOOST_SCOPE_EXIT_ALL(&)
#define SCOPE_ALL_REF_failure SCOPE_FAILURE_ALL(&)
#define SCOPE_ALL_REF_success SCOPE_SUCCESS_ALL(&)

#define scope(when) SCOPE_ALL_REF_##when

int main(int,char *[])
{
    try
    {
		int some_var=1;
        cout << "Case #1: stack unwinding" << endl;
        scope(exit)
		{
			cout << "exit " << some_var << endl;
			++some_var;
		};
        scope(failure)
		{
			cout << "failure " << some_var  << endl;
			++some_var;
		};
        scope(success)
		{
			cout << "success " << some_var  << endl;
			++some_var;
		};
        throw 1;
    } catch(int){}
    {
        int some_var=1;
        cout << "Case #2: normal exit" << endl;
        scope(exit)
		{
			cout << "exit " << some_var << endl;
			++some_var;
		};
        scope(failure)
		{
			cout << "failure " << some_var  << endl;
			++some_var;
		};
        scope(success)
		{
			cout << "success " << some_var  << endl;
			++some_var;
		};
    }
    return 0;
}

ExpectedStdoutTest test_cout
(
        "Case #1: stack unwinding\n"
        "failure 1\n"
        "exit 2\n"
        "Case #2: normal exit\n"
        "success 1\n"
        "exit 2\n"
);

#endif
