cmake_minimum_required(VERSION 2.8)
project(stack_unwinding_demo)

enable_testing()

if(CMAKE_COMPILER_IS_GNUCXX)
    #add_definitions(-Wall -Weffc++ -Wold-style-cast -Wctor-dtor-privacy -Winit-self -Wmissing-include-dirs -Wfloat-equal -Wshadow -Wpointer-arith -Wunreachable-code)#-Wuninitialized
elseif(MSVC)
    add_definitions("/W4" "/WX")
endif()

include_directories("../")

function(example example_name)
    add_executable(${example_name} ${example_name}.cpp)
    add_test(${example_name} ${example_name})
endfunction()

example(file_throwing_dtor)
example(uncaught_exception_count)
example(scope_guard)
example(scope_actions)
example(example_from_dlang_dot_org)


