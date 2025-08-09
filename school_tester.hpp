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
#include <utility>
#include <cstdlib>
#include <streambuf>

namespace st {
    struct TestConfig {
        bool useTestFile = false;
        std::string testFileName;
    
        TestConfig() = default;

        /// Initialize configuration to read a specific test file name, reading
        /// the test(s) to run, and in which order.
        TestConfig(std::string inTestFileName)
            : useTestFile(true), testFileName(std::move(inTestFileName))
        {}
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
            
        static void addTest(const char* name, TestFunc f); 
        static bool runTest(const std::string& name, bool catchExceptions);
        static std::vector<std::string> testsToRun(const TestConfig& config);
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
        const auto tests = detail::testsToRun(config);
        for(const std::string& testName : tests) {
            (void)detail::runTest(testName, true);
        }
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

        static void addTest(const char* name, TestFunc f) {
            const std::string nameStr = name;
            TestInfo info{nameStr, f};
            getAllTests().insert({nameStr, std::move(info)});
            getTestOrder().push_back(nameStr);
            std::cerr << "added test " << name << std::endl;
        }

        static bool runTest(const std::string &name, bool catchExceptions)
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

        static std::vector<std::string> testsToRun(const TestConfig &config)
        {
            if(!config.useTestFile) {
                return getTestOrder();
            }

            std::ifstream testCases;
            testCases.open(config.testFileName);
            if(!testCases.is_open()) {
                // todo handle this
            }

            std::vector<std::string> tests;

            std::string line;
            while(std::getline(testCases, line)) { 
                const auto found = getAllTests().find(line);
                if(found == getAllTests().end()) {
                    std::cerr << "FAILED TO FIND TEST [" << line << ']' << std::endl;
                }
            }
            std::cout << "-\n";

            testCases.close();
            return tests;
        }
    }
}

#endif // SCHOOL_TESTER_H_