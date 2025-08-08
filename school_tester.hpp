#ifndef SCHOOL_TESTER_H_
#define SCHOOL_TESTER_H_

#ifndef __cplusplus
#error "Only intended to be included in C++ files"
#endif

#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>

namespace st {
    namespace detail {
        class TestRunner {
        public:
            using TestFunc = void(*)();

            static void addTest(const char* name, TestRunner::TestFunc f);

        private:
            static std::unordered_map<std::string, TestFunc> g_tests;
        };


        
    }
}

namespace st {
    namespace detail {
        void TestRunner::addTest(const char* name, TestRunner::TestFunc f) {
            g_tests.insert({std::string(name), f});
        }
    }
}

#endif // SCHOOL_TESTER_H_