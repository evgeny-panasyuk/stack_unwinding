#include <iostream>
#include <ostream>
#include <string>

#include <unwinding_aware_destructor.hpp>

#include "examples_common.hpp"

using namespace std;

struct DestructorInClass
{
    UNWINDING_AWARE_DESTRUCTOR(DestructorInClass,unwinding)
    {
        cout << "DestructorInClass, unwinding: " << ( unwinding ? "true" : "false" ) << endl;
    }
};

struct DestructorOutOfClass
{
    UNWINDING_AWARE_DESTRUCTOR_OUF_OF_CLASS_DECL(DestructorOutOfClass,unwinding);
};

UNWINDING_AWARE_DESTRUCTOR_OUT_OF_CLASS_DEF(DestructorOutOfClass,unwinding)
{
    cout << "DestructorOutOfClass, unwinding: " << ( unwinding ? "is happening" : "is not happening" ) << endl;
}

int main()
{
    try
    {
       DestructorOutOfClass a;
       DestructorInClass b;
    }
    catch(int) {}
    try
    {
       DestructorOutOfClass a;
       DestructorInClass b;
       throw 1;
    }
    catch(int) {}
    return 0;
}

ExpectedStdoutTest test_cout
(
    "DestructorInClass, unwinding: false\n"
    "DestructorOutOfClass, unwinding: is not happening\n"
    "DestructorInClass, unwinding: true\n"
    "DestructorOutOfClass, unwinding: is happening\n"
);

