stack_unwinding
===============

The stack_unwinding is a small header-only C++ library which supplies primitive(class unwinding_indicator) to determining when object destructor is called due to stack-unwinding or due to normal scope leaving.

It contains several tools implemented on top of unwinding_indicator:
* Scope Success and Scope Failure in C++, like in D language, without manual ScopeGuard's .commit/.release
* Throwing Destructor which is not Terminator
* Parameterized Unwinding Aware Destructor
* Two Stage Destructor: Deferred Action(possibly throwing) and Release Resources(non-fail, non-throwing) stages

Throwing Destructors which are not Terminators
==============================================

One of uses cases of this library is to check when throwing exception from destructor may lead to call std::terminate, because destructor was called due to stack unwinding. That will allow us to write destructors, which are not terminators:
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
Or shorter version, using macro DESTRUCTOR_BUT_NOT_TERMINATOR:
```C++
class NotTerminator
{
public:
    void something_throwable()
    {
        throw 0;
    }
    DESTRUCTOR_BUT_NOT_TERMINATOR(NotTerminator)
    {
        something_throwable();
    }
};
```
With help of such technique, we may achieve exactly same effect as manually placing "f.close()" [1] at end of scope automaticly.
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
   // b.~File() -  may throw
   // a.~File() -  may throw
}
```

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
Some quote from Boost.ScopeExit manual [6]:
```
Boost.ScopeExit is similar to scope(exit) feature built into the D programming language.

A curious reader may notice that the library does not implement scope(success) and
scope(failure) of the D language. Unfortunately, these are not possible in C++ because
failure or success conditions cannot be determined by calling std::uncaught_exception
(see Guru of the Week #47 for details about std::uncaught_exception and if it has any good use at all).
However, this is not a big problem because these two D's constructs can be expressed in
terms of scope(exit) and a bool commit variable (similarly to some examples presented in the Tutorial section).
```
This library contains example implementations of BOOST_SCOPE_FAILURE and BOOST_SCOPE_SUCCESS (based on Boost.ScopeExit + unwinding_indicator):
```C++
{
    BOOST_SCOPE_EXIT(void) { cout << "exit" << endl; } BOOST_SCOPE_EXIT_END
    BOOST_SCOPE_FAILURE(void) { cout << "failure" << endl; } BOOST_SCOPE_FAILURE_END
    BOOST_SCOPE_SUCCESS(void) { cout << "success" << endl; } BOOST_SCOPE_SUCCESS_END
    throw 1;
}
```
Boost.ScopeExit has following example in manual [5]:
```C++
void world::add_person(person const& a_person) {
    bool commit = false;

    persons_.push_back(a_person);           // (1) direct action
    // Following block is executed when the enclosing scope exits.
    BOOST_SCOPE_EXIT(&commit, &persons_) {
        if(!commit) persons_.pop_back();    // (2) rollback action
    } BOOST_SCOPE_EXIT_END

    // ...                                  // (3) other operations

    commit = true;                          // (4) disable rollback actions
}
```
Using BOOST_SCOPE_FAILURE it would became:
```C++
void world::add_person(person const& a_person) {
    persons_.push_back(a_person);           // (1) direct action
    // Following block is executed when the enclosing scope exits.
    BOOST_SCOPE_FAILURE(&commit, &persons_) {
        persons_.pop_back();                // (2) rollback action
    } BOOST_SCOPE_FAILURE

    // ...                                  // (3) other operations
}
```
Using of ScopeGuard idiom (committing/releasing by hands) becoming even more complicated in face of multiple scope exits: return, break, continue, etc - multiple committing releaseing should be used.
While scope(failure)/scope(success) should be placed only once.

Moreover, it is impossible to place manual committing/releasing somewhere between or after destructor calls, without use of artifical C++ code blocks. While it can be done naturally with scope(failure)/scope(success).

Unwinding Aware Destructor
==========================

Uwinding Aware Destructor is abstration from explicit use of stack_unwinding::unwinding_indicator. It is destructor which takes boolean as parameter, which indicates if it called due to unwinding or not. Pseudocode is:
```C++
struct Foo
{
    ~Foo(bool due_to_unwinding)
    {
        if(due_to_unwinding)
            // ...
        else
            // ...
    }
};
```
Real code using preprocessor macro:
```C++
struct Foo
{
    UNWINDING_AWARE_DESTRUCTOR(Foo,due_to_unwinding)
    {
        if(due_to_unwinding)
            // ...
        else
            // ...
    }
};
```

Two Stage Destructor. Deferred actions
======================================

[Description - TODO]

Deferred Action of object - is action which is executed before Resource Releasing of object if object's destruction happens due to normal scope exit, not unwinding.

Resource Releasing - is last action executed on object's body, it is executed regardless of object's destruction reasons(unwinding or not). Semantically Resource Releasing must not fail.

Main difference between Deferred Action and Resource Releasing is in fact, that Deferred Action may fail and throw exception without risk to trigger std::terminate.

```C++
class RAII_Deferred
{
    bool fail_on_flush;
public:
    // Normal contrustor
    RAII_Deferred(bool fail_on_flush_) : fail_on_flush(fail_on_flush_)
    {
        cout << "acquiring resource" << endl;
    }
    // Release part of destructor.
    // Herb Sutter: "letting go of a resource" must never fail
    // (http://cpp-next.com/archive/2012/08/evil-or-just-misunderstood/)
    TWO_STAGE_DESTRUCTOR_RELEASE(RAII_Deferred)
    {
        cout << "release resource" << endl;
    }
    // Deferred part of destructor. May fail(for instance fflush).
    // Called when object is destroyed due to normal flow, not stack unwinding
    TWO_STAGE_DESTRUCTOR_DEFERRED(RAII_Deferred)
    {
        cout << "flush pending actions on resource" << endl;
        if(fail_on_flush) throw 1;
    }
};
```
Two stage destructor has following semantics:
```C++
~Foo(bool due_to_unwinding)
{
    try
    {
        if(!due_to_unwinding) deferred_part_of_destructor();
    }
    catch(...)
    {
        release_part_of_destructor();
        throw;
    }
    release_part_of_destructor();
}
```
Two-stage destruction clearly partition destructors into two semantically different operations: Dereffed Action and Resource Releasing.
Two-stage destruction has following properties:
* Exceptions are not swallowed, information is not lost.
* Deferred Actions are simillar to ordinary actions below throw statement – because ordinary actions are not executed during stack unwinding also.
* If one of Deferred Actions is failed - other Deferred Actions until next "catch" statement are not executed. In most cases, fail in one of deferred actions means that function/scope/object failed to achieve it's post-conditions, and there is no sense in executing other deferred actions.
* Resource Releasing happens regardless of fail in deferred action.

Let us consider following function:
```C++
void fsome(/* ... */)
{
    OutFile a("a"),b("b");
    a.write("Hello");
    b.write("World");
    // ... - may fail
    b.flush(); // may fail
    a.flush(); // may fail
}
```
In most cases, if b.flush() failed, there is no sense to execute a.flush(), so - b.flush() may safely throw exception, which would be not handled within it's scope.

Also, in most cases, if something failed at "..." and throwed exception - there is no sense to flush writes - only resources should be released.
Exactly same semantics can be achieved with notion of Two-Stage destruction:
```C++
void fsome(/* ... */)
{
    OutFile a("a"),b("b");
    a.write("Hello");
    b.write("World");
    // ... - may fail
    // b.~OutFile():
    //     Deferred Action: flush - called if destruction is not due to unwinding, may throw
    //     Resource Releasing: fclose - called in anycase
    // a.~OutFile()
    //     Deferred Action: flush - called if destruction is not due to unwinding, may throw
    //     Resource Releasing: fclose - called in anycase
}
```

Caution
=======

There are cases when destructor is not called due to stack unwinding, but throwing exception from it may lead to unintended consequences. For instance, most of C++ code assumes non-throwing destructors. (TODO: add note about built-in arrays, STL, + references to ISO)

Be aware of transitive nature of objects throwing destructors - if some class A aggregates or inherits class B which may throw in destructor, then class A also may throw on destruction, and as the consequence all classes that aggreagate or inherit A may throw on destruction, and so on.

(Aggregation without turning aggregator's destruction into throwable is still possible, but requires explicit managing of construction and destruction of aggregate, for instance with help of placement new and explicit destructor call, or just new/delete. In that case you should swallow all exceptions from destructor.)

As rule of dumb, use throwing destructors only in classes not intended to be aggregated or inherited, i.e. classes which lives only in code scope.

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
6. [Alexander Nasonov, Lorenzo Caminiti. Boost.ScopeExit. Note on scope(failure) and scope(success)](http://www.boost.org/doc/libs/1_51_0/libs/scope_exit/doc/html/scope_exit/alternatives.html#scope_exit.alternatives.the_d_programming_language)

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