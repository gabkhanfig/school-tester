#include "school_tester.hpp"
#include <iostream>

void testFunction() {
    bool a = false;
    do { 
        if((a) == false) { 
            if(st::detail::currentContext().shouldDebugBreak) { 
                _ST_DEBUG_BREAK(); 
            } 
            throw st::TestException("a", "something", 1);
        }   
    } while(false);

}

TEST_CASE("some other test") {
    std::cout << "test case!\n";
}

TEST_CASE("two numbers equal") {
    std::string str = "hello world";
    CHECK_GE(str, "wello world");
}

int main() {
    st::TestConfig defaultConfig{};
    // defaultConfig.useTestFile = true;
    // defaultConfig.testFileName = "../test_cases.txt";
    (void)st::runTests(defaultConfig);

    bool dbg = st::detail::isDebuggerAttached();
    std::cout << dbg << std::endl;
    
    int a = 5;
    try {
        //testFunction();
        CHECK_GE(a, 6);
    } catch(st::TestException& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}