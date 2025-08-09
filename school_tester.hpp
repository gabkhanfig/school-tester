#ifndef SCHOOL_TESTER_H_
#define SCHOOL_TESTER_H_

#ifndef __cplusplus
#error "Only intended to be included in C++ files"
#endif

#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>

namespace st {
    class TestConfig {

    };

    class TestReport {

    };

    TestReport runTests(const TestConfig& config);

    namespace detail {
        using TestFunc = void(*)();

        class TestInfo {
        public:
            std::string name;
            TestFunc    func;
        };
            
        void addTest(const char* name, TestFunc f); 
        bool runTest(const std::string& name, bool catchExceptions);
        bool runAllTests();
    }
}

#define _ST_GENERATE_TEST_NAME SCHOOL_TEST_##__COUNTER__

#define _ST_REGISTER_TEST(f, testName) \
static void f(); \
static const int _ST_IGNORE_##__COUNTER__ \
    = [](){st::detail::addTest(testName, f); return 0; }();\
static void f()

#define TEST_CASE(testName) _ST_REGISTER_TEST(_ST_GENERATE_TEST_NAME, testName)

namespace st {
    TestReport runTests(const TestConfig& config) {
        (void)config;
        detail::runAllTests();
        return TestReport();
    }

    namespace detail {   
        static std::unordered_map<std::string, TestInfo>& getAllTests() {
            static std::unordered_map<std::string, TestInfo> allTests{};
            return allTests;
        }

        static std::vector<std::string>& getTestOrder() {
            static std::vector<std::string> orderedTests{};
            return orderedTests;
        }

        static std::unordered_map<std::string, bool>& getRanTests() {
            static std::unordered_map<std::string, bool> ranTests{};
            return ranTests;
        }

        void addTest(const char* name, TestFunc f) {
            const std::string nameStr = name;
            TestInfo info{nameStr, f};
            getAllTests().insert({nameStr, std::move(info)});
            getTestOrder().push_back(nameStr);
            std::cerr << "added test " << name << std::endl;
        }

        bool runTest(const std::string &name, bool catchExceptions)
        {
            auto setSuccessStatus = [name](bool success) {
                auto& ranTests = getRanTests();
                auto found = ranTests.find(name);
                if(found != ranTests.end()) {
                    ranTests.insert({name, success});
                } else {
                    std::terminate();
                }
            };

            auto& allTests = getAllTests();
            const auto found = allTests.find(name);
            if(found == allTests.end()) {
                return false;
            }

            if(catchExceptions) {
                try {
                    TestInfo& info = (*found).second;
                    info.func();
                } catch(...) {
                    std::cerr << "Caught exception from test [" << name << ']' << std::endl;
                    return false;
                }
                
                return true;
            } 
            else {
                TestInfo& info = (*found).second;
                info.func();
                return true;
            }
            
        }

        bool runAllTests()
        {
            bool success = true;
            const auto& testOrder = getTestOrder();
            for(const std::string& testName : testOrder) {
                if(!runTest(testName, true)) {
                    success = false;
                }
            }
            return success;
        }
    }
}

#endif // SCHOOL_TESTER_H_