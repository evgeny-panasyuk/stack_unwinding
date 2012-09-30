#include <iostream>
#include <ostream>
#include <string>

#include <destructor_but_not_terminator.hpp>

using namespace std;

struct DestructorInClass
{
    DESTRUCTOR_BUT_NOT_TERMINATOR(DestructorInClass)
    {
        throw 1;
    }
};

struct DestructorOutOfClass
{
    DESTRUCTOR_BUT_NOT_TERMINATOR_OUF_OF_CLASS_DECL(DestructorOutOfClass);
};

DESTRUCTOR_BUT_NOT_TERMINATOR_OUT_OF_CLASS_DEF(DestructorOutOfClass)
{
    throw 2;
}

int main(int,char *[])
{
    try
    {
       DestructorOutOfClass a;
       DestructorInClass b;
    }
    catch(int i) { cout << i; }
    try
    {
       DestructorInClass b;
       DestructorOutOfClass a;
    }
    catch(int i) { cout << i; }
    try
    {
       DestructorOutOfClass a;
       DestructorInClass b;
       throw 3;
    }
    catch(int i) { cout << i; }

    return 0;
}

