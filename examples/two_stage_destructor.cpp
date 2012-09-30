#include <iostream>
#include <ostream>
#include <string>

#include <two_stage_destructor.hpp>

using namespace std;

class RAII_Deffered
{
    bool fail_on_flush;
public:
    RAII_Deffered(bool fail_on_flush_) : fail_on_flush(fail_on_flush_)
    {
        cout << "acquiring resource" << endl;
    }
    TWO_STAGE_DESTRUCTOR_RELEASE(RAII_Deffered)
    {
        cout << "release resource" << endl;
    }
    TWO_STAGE_DESTRUCTOR_DEFERRED(RAII_Deffered)
    {
        cout << "flush pending actions on resource" << endl;
        if(fail_on_flush) throw 1;
    }
};

class RAII_Deffered_out_of_class
{
    bool fail_on_flush;
public:
    RAII_Deffered_out_of_class(bool fail_on_flush_) : fail_on_flush(fail_on_flush_)
    {
        cout << "acquiring resource" << endl;
    }
    TWO_STAGE_DESTRUCTOR_OUF_OF_CLASS_DECL(RAII_Deffered_out_of_class)
};

TWO_STAGE_DESTRUCTOR_RELEASE_OUT_OF_CLASS_DEF(RAII_Deffered_out_of_class)
{
    cout << "release resource" << endl;
}
TWO_STAGE_DESTRUCTOR_DEFERRED_OUT_OF_CLASS_DEF(RAII_Deffered_out_of_class)
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
int main(int,char *[])
{
    test<RAII_Deffered>();
    test<RAII_Deffered_out_of_class>();
    return 0;
}

