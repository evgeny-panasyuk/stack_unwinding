#include <iostream>
#include <ostream>
#include <string>

#include <unwinding_aware_destructor.hpp>

using namespace std;

class DestructorInClass
{
public:
    UNWINDING_AWARE_DESTRUCTOR(DestructorInClass,unwinding)
    {
        cout << "DestructorInClass, unwinding: " << ( unwinding ? "true" : "false" ) << endl;
    }
};

class DestructorOutOfClass
{
public:
    UNWINDING_AWARE_DESTRUCTOR_OUF_OF_CLASS_DECL(DestructorOutOfClass,unwinding);
};

UNWINDING_AWARE_DESTRUCTOR_OUT_OF_CLASS_DEF(DestructorOutOfClass,unwinding)
{
    cout << "DestructorOutOfClass, unwinding: " << ( unwinding ? "is happening" : "is not happening" ) << endl;
}

int main(int,char *[])
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

