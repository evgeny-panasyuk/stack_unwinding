#ifdef _MSC_VER
    #define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <ostream>
#include <cstdio>
#include <stdexcept>
using namespace std;

#include <two_stage_destructor.hpp>

#include "examples_common.hpp"

class File
{
    FILE *file_handle;
    File(const File&);
    File &operator=(const File&);
public:
    explicit File(const char *filename)
    {
        cout << "fopen" << endl;
        if( (file_handle=fopen(filename,"w"))==0 )
            throw runtime_error("fopen failed");
    }
    void puts(const char *str)
    {
        if(fputs(str,file_handle)<0)
            throw runtime_error("fputs failed");
    }
    TWO_STAGE_DESTRUCTOR_RELEASE(File)
    {
        cout << "fclose" << endl;
        fclose(file_handle);
    }
    TWO_STAGE_DESTRUCTOR_DEFERRED(File)
    {
        cout << "fflush" << endl;
        if(fflush(file_handle)!=0)
            throw runtime_error("fflush failed");
    }
};

int main(int,char *[])
{
    try
    {
        File test("out1.txt");
        test.puts("Hello1");
    }
    catch(const exception &e)
    {
        cout << e.what() << endl;
    }
    cout << "-------" << endl;
    try
    {
        File test("out2.txt");
        test.puts("Hello2");
        throw runtime_error("test exception");
    }
    catch(const exception &e)
    {
        cout << e.what() << endl;
    }
    return 0;
}

ExpectedStdoutTest test_cout
(
    "fopen\n"
    "fflush\n"
    "fclose\n"
    "-------\n"
    "fopen\n"
    "fclose\n"
    "test exception\n"
);

