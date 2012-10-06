#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include <numeric>
#include <algorithm>
#include <functional>

#include <stack_unwinding.hpp>

#include "examples_common.hpp"

using namespace std;

vector<unsigned> exception_counts;
const unsigned test_count=1024;

struct ExpCountSaver
{
    ~ExpCountSaver()
    {
        exception_counts.push_back(stack_unwinding::uncaught_exception_count());
    }
};


template<int n>
struct ExptSwallower
{
    ~ExptSwallower()
    {
        try
        {
            ExpCountSaver p;
            ExptSwallower<n-1> a;
            throw 1;

            suppress_unused_warning(p,a);
        }catch(int){}
    }
};
template<>
struct ExptSwallower<0>
{
    ~ExptSwallower() {}
};
int main(int,char *[])
{
    exception_counts.reserve(test_count);
    try
    {
        ExptSwallower<test_count> test;

        suppress_unused_warning(test);
    } catch(int) {}
    if(exception_counts.size()!=test_count)
        return 1;
    adjacent_difference(exception_counts.rbegin(),exception_counts.rend(),exception_counts.rbegin());
    if
    (
        find_if(exception_counts.begin(),exception_counts.end(), bind2nd(not_equal_to<unsigned>(),1) )
            !=
        exception_counts.end()
    )
        return 2;
    return 0;
}
