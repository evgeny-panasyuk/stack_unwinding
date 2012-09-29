stack_unwinding
===============

The stack_unwinding is a small header only C++ library which supplies primitive(class unwinding_indicator) to determining when object destructor is called due to stack-unwinding or due to normal scope leaving.

Throwing Destructors which are not Terminators
==============================================

One of uses cases of this library is to check when throwing exception from destructor may lead to call std::terminate, because destructor was called due to stack unwinding. That will allow use to write destructors, which are not terminators:
```C++
class NotTerminator
{
    stack_unwinding::unwinding_indicator indicator;
public:
    void something_throwable()
    {
        throw 0;
    }
    ~NotTerminator()
    {
            if(indicator.unwinding())
            {
                try { something_throwable(); } catch(...) {}
            }
            else
            {
                something_throwable();
            }
    }
};
```
As the result, we may achieve exactly same effect as manually placing "f.close()" [1] at end of scope automaticly.
```C++
{
   File a,b;
   // ...
   b.close(); // may throw
   a.close(); // may throw
}
```
would became
```C++
{
   File a,b;
   // ...
}
```
Note, there are cases when destructor is not called due to stack unwinding, but throwing exception from it may lead to unintended consequences. For instance, most of C++ code assumes non-throwing destructors. (TODO: add note about built-in arrays, STL, + references to ISO)

Be aware of transitive nature of objects throwing destructors - if some class A aggregates or inherits class B which may throw in destructor, then class A also may throw on destruction, and as the consequence all classes that aggreagate or inherit A may throw on destruction, and so on.

(Aggregation without turning aggregator's destruction into throwable is still possible, but requires explicit managing of construction and destruction of aggregate, for instance with help of placement new and explicit destructor call, or just new/delete. In that case you should swallow all exceptions from destructor.)

As rule of dumb, use throwing destructors only in classes not intended to be aggregated or inherited, i.e. classes which lives only in code scope.

D-style Scope Guards/Actions
============================

More generally this library helps to develop "advanced" Scope Guard [2], which respects exceptions from destructors, and does not require calling of release/commit by hands.
D langauge has scope(success) and scope(failure) [3,4] which are simmilar in something to that "advanced" Scope Guard semantic.

This library has example implementations of "scope(success)" and "scope(failure)":
```C++
{
    scope_action exit = make<scope_exit>( Print("exit") );
    scope_action failure = make<scope_failure>( Print("failure") );
    scope_action success = make<scope_success>( Print("success") );
    throw 1;
}
```

Boost has "Scope Exit" library [5] which in examples shows using of commiting by hands.
Some quote from Boost.ScopeExit manual:
```
Boost.ScopeExit is similar to scope(exit) feature built into the D programming language.

A curious reader may notice that the library does not implement scope(success) and
scope(failure) of the D language. Unfortunately, these are not possible in C++ because
failure or success conditions cannot be determined by calling std::uncaught_exception
(see Guru of the Week #47 for details about std::uncaught_exception and if it has any good use at all).
However, this is not a big problem because these two D's constructs can be expressed in
terms of scope(exit) and a bool commit variable (similarly to some examples presented in the Tutorial section).
```

Implementation details
======================

Currently library is implemented on top of platform-specific implementation of uncaught_exception_count function.
uncaught_exception_count is a function similar to std::uncaught_exception [1] from standard library, but instead of boolean result it returns unsigned int showing current count of uncaught exceptions.

References
==========

1. [Herb Sutter. Uncaught Exceptions](http://www.gotw.ca/gotw/047.htm)
2. [Andrei Alexandrescu, Petru Marginean. Generic: Change the Way You Write Exception-Safe Code - Forever](http://www.drdobbs.com/cpp/generic-change-the-way-you-write-excepti/184403758)
3. [Andrei Alexandrescu. Three Unlikely Successful Features of D](http://channel9.msdn.com/Events/Lang-NEXT/Lang-NEXT-2012/Three-Unlikely-Successful-Features-of-D)
4. [D Programming Language. Scope Guard Statement](http://dlang.org/statement.html#ScopeGuardStatement)
5. [Alexander Nasonov, Lorenzo Caminiti. Boost.ScopeExit](http://www.boost.org/doc/libs/1_51_0/libs/scope_exit/doc/html/index.html)

Caution
=======

TODO: describe pitfalls

Current library status: ALPHA
=============================

* MSVC 2005: tested on x32 and x64, default settings
* MSVC 2008: tested on x32 and x64, default settings
* MSVC 2010: tested on x32 and x64, default settings
* MSVC 2012: tested on x32 and x64, default settings
* GCC 4.1.2: tested on x32 and x64, default settings
* GCC 4.4.6: tested on x32 and x64, default settings
* Clang 3.2:  tested on x32 and x64, default settings

About
======

* Author: Evgeny Panasyuk (e-mail: E?????[dot]P???????[at]gmail.???)
* Initial Release Date: 27/09/2012

License
=======

Copyright (c) 2012, Evgeny Panasyuk

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.