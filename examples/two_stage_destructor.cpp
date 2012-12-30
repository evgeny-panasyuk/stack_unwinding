#include <iostream>
#include <ostream>
#include <string>

#include <two_stage_destructor.hpp>

#include "examples_common.hpp"

using namespace std;

class RAII_Deferred
{
    bool fail_on_flush;
public:
    RAII_Deferred(bool fail_on_flush_) : fail_on_flush(fail_on_flush_)
    {
        cout << "acquiring resource" << endl;
    }
    TWO_STAGE_DESTRUCTOR_RELEASE(RAII_Deferred)
    {
        cout << "release resource" << endl;
    }
    TWO_STAGE_DESTRUCTOR_DEFERRED(RAII_Deferred)
    {
        cout << "flush pending actions on resource" << endl;
        if(fail_on_flush) throw 1;
    }
};

class RAII_Deferred_out_of_class
{
    bool fail_on_flush;
public:
    RAII_Deferred_out_of_class(bool fail_on_flush_) : fail_on_flush(fail_on_flush_)
    {
        cout << "acquiring resource" << endl;
    }
    TWO_STAGE_DESTRUCTOR_OUF_OF_CLASS_DECL(RAII_Deferred_out_of_class)
};

TWO_STAGE_DESTRUCTOR_RELEASE_OUT_OF_CLASS_DEF(RAII_Deferred_out_of_class)
{
    cout << "release resource" << endl;
}
TWO_STAGE_DESTRUCTOR_DEFERRED_OUT_OF_CLASS_DEF(RAII_Deferred_out_of_class)
{
    cout << "flush pending actions on resource" << endl;
    if(fail_on_flush) throw 1;
}

template<class Test>
void test()
{
    cout << "--------" << endl;
    try
    {
        Test a(false);
    }
    catch(int i) { cout << i << endl; }
    try
    {
        Test a(true);
    }
    catch(int i) { cout << i << endl; }
    try
    {
        Test a(false);
        throw 2;
    }
    catch(int i) { cout << i << endl; }
}
int main()
{
    test<RAII_Deferred>();
    test<RAII_Deferred_out_of_class>();
    return 0;
}

ExpectedStdoutTest test_cout
(
    "--------\n"
    "acquiring resource\n"
    "flush pending actions on resource\n"
    "release resource\n"
    "acquiring resource\n"
    "flush pending actions on resource\n"
    "release resource\n"
    "1\n"
    "acquiring resource\n"
    "release resource\n"
    "2\n"
    "--------\n"
    "acquiring resource\n"
    "flush pending actions on resource\n"
    "release resource\n"
    "acquiring resource\n"
    "flush pending actions on resource\n"
    "release resource\n"
    "1\n"
    "acquiring resource\n"
    "release resource\n"
    "2\n"
);
