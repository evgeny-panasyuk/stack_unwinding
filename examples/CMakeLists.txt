cmake_minimum_required(VERSION 2.8)
project(stack_unwinding_demo)

enable_testing()

if(CMAKE_COMPILER_IS_GNUCXX)
    #add_definitions(-Weffc++ -Wold-style-cast -Wctor-dtor-privacy -Wshadow -Wunreachable-code)#-Wuninitialized
    add_definitions(-ftemplate-depth-16384 -Wall -Werror -Winit-self -Wmissing-include-dirs -Wfloat-equal -Wpointer-arith) 
elseif(MSVC)
    add_definitions("/W4" "/WX")
endif()

include_directories(
    ../
    ../standalone
    ../wild_sutff
)

function(example example_name)
    add_executable(${example_name} ${example_name}.cpp)
    add_test(${example_name} ${example_name})
endfunction()

example(file_throwing_dtor)
example(uncaught_exception_count)
example(scope_guard)
example(scope_actions)
example(example_from_dlang_dot_org)
example(unwinding_aware_destructor)
example(destructor_but_not_terminator)
example(two_stage_destructor)
example(uncaught_exception_count_floating_object)
example(uncaught_exception_count_stress_test)
example(FILE_defer_and_release)

find_package( Boost 1.51.0 )
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    example(boost_scopes)
    example(boost_scopes_cpp11)
endif()
