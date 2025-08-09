#include "school_tester.hpp"
#include <iostream>

void testFunction() {
    throw 0;
}

TEST_CASE("some other test") {
    std::cout << "test case!\n";
}


int main() {
    st::TestConfig defaultConfig{};
    (void)st::runTests(defaultConfig);
    return 0;
}