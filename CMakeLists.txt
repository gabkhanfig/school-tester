cmake_minimum_required (VERSION 3.19)

set(CMAKE_CXX_STANDARD 11)

project("SchoolTester")

add_executable(SchoolTester "main.cpp")

if(MSVC)
    target_compile_options(SchoolTester PRIVATE /W4 /WX)
    target_link_libraries(SchoolTester PRIVATE dbghelp)
    target_compile_options(SchoolTester PRIVATE /EHsc /Zi) 
    # target_compile_options(SchoolTester PRIVATE /fsanitize=address)
else()
    target_compile_options(SchoolTester PRIVATE -Wall -Wextra -Wpedantic -Werror)
    target_compile_options(SchoolTester PRIVATE -fsanitize=address)
    set(CMAKE_EXE_LINKER_FLAGS "-fsanitize=address")
endif()

