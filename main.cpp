#include "school_tester.hpp"
#include <iostream>

void testFunction() {
    throw 0;
}

TEST_CASE("some other test") {
    std::cout << "test case!\n";
}


int main() {
    // st::TestConfig defaultConfig{};
    // defaultConfig.useTestFile = true;
    // defaultConfig.testFileName = "../test_cases.txt";
    // (void)st::runTests(defaultConfig);

    bool dbg = st::detail::isDebuggerAttached();
    std::cout << dbg << std::endl;

    return 0;
}