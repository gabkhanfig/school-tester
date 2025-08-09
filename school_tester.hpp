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
#include <cstring>
#include <exception>
#if defined(_MSC_VER) || defined (_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <debugapi.h>
#elif defined(__APPLE__) 
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/sysctl.h>
#elif defined (__GNUC__)
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#endif

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

    class TestException : public std::exception {
    public:
        TestException(const char* condition, const char* file, int line);

        virtual const char* what() const noexcept override;

    private:
        std::string what_;
        const char* condition_;
        const char* file_;
        int line_;
    };

    TestReport runTests(const TestConfig& config);

    namespace detail {
        using TestFunc = void(*)();

        struct TestInfo {
            std::string name;
            TestFunc    func;
        };

        struct TestContext {
            bool shouldDebugBreak = false;
            const TestInfo* currentTest = nullptr;
        };
            
        static void addTest(const char* name, TestFunc f); 
        static bool runTest(const std::string& name, bool catchExceptions);
        static std::vector<std::string> testsToRun(const TestConfig& config);
        static TestContext& currentContext();
        static bool isDebuggerAttached();
    }
}

#define _ST_GENERATE_TEST_FUNC SCHOOL_TEST_##__COUNTER__

#define _ST_REGISTER_TEST(f, testName)                      \
static void f();                                            \
static const int _ST_IGNORE_##__COUNTER__                   \
    = [](){st::detail::addTest(testName, f); return 0; }(); \
static void f()

#if defined(_MSC_VER) || defined (_WIN32)
#define _ST_DEBUG_BREAK() DebugBreak()
#elif defined(__APPLE__) || defined (__GNUC__)
#define _ST_DEBUG_BREAK() __builtin_trap()
#endif

#define TEST_CASE(testName) _ST_REGISTER_TEST(_ST_GENERATE_TEST_FUNC, testName)

#define CHECK(condition) \
do { \
    if((condition) == false) { \
        if(st::detail::currentContext().shouldDebugBreak) { \
            _ST_DEBUG_BREAK(); \
        } \
        throw st::TestException(#condition, __FILE__, __LINE__); \
    }   \
} while(false)

namespace st {
    TestReport runTests(const TestConfig& config) {
        const detail::TestContext oldContext = detail::currentContext();

        const bool debuggerAttached = detail::isDebuggerAttached();
        detail::currentContext().shouldDebugBreak = debuggerAttached;

        const auto tests = detail::testsToRun(config);
        for(const std::string& testName : tests) {
            (void)detail::runTest(testName, true);
        }

        detail::currentContext() = oldContext;

        return TestReport();
    }

    st::TestException::TestException(const char* condition, const char* file, int line)
        : condition_(condition), file_(file), line_(line)
    {
        what_ = std::string(file) + ':' + std::to_string(line) + ": ERROR: " + "Check failed [ " + condition + " ]";
    }

    inline const char *TestException::what() const noexcept
    {
        return this->what_.c_str();
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
                    detail::currentContext().currentTest = &info;
                    info.func();
                } catch(...) {
                    std::cerr << "Caught exception from test [" << name << ']' << std::endl;
                    detail::currentContext().currentTest = nullptr;
                    return false;
                }
                
                detail::currentContext().currentTest = nullptr;
                return true;
            } 
            else {
                TestInfo& info = (*found).second;
                detail::currentContext().currentTest = &info;
                info.func();
                detail::currentContext().currentTest = nullptr;
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
                std::cerr << "FAILED TO OPEN TEST CASES FILE: " << config.testFileName << std::endl;
                return std::vector<std::string>();
            }

            std::vector<std::string> tests;

            std::string line;
            while(std::getline(testCases, line)) { 
                const auto found = getAllTests().find(line);
                if(found == getAllTests().end()) {
                    std::cerr << "FAILED TO FIND TEST [" << line << ']' << std::endl;
                } else {
                    tests.push_back(line);
                }
            }

            testCases.close();
            return tests;
        }

        static bool isDebuggerAttached()
        {
            #if defined(_MSC_VER) || defined (_WIN32)
            return IsDebuggerPresent();
            #elif defined(__APPLE__)
            // https://developer.apple.com/library/archive/qa/qa1361/_index.html
            int junk;
            int mib[4];
            struct kinfo_proc info;
            size_t size;

            info.kp_proc.p_flag = 0;

            mib[0] = CTL_KERN;
            mib[1] = KERN_PROC;
            mib[2] = KERN_PROC_PID;
            mib[3] = getpid();

            size = sizeof(info);
            junk = sysctl(mib, sizeof(mib) / sizeof(*mib), &info, &size, NULL, 0);
            assert(junk == 0);

            return ((info.kp_proc.p_flag & P_TRACED) != 0);
            #elif defined (__GNUC__)
            // https://stackoverflow.com/questions/3596781/how-to-detect-if-the-current-process-is-being-run-by-gdb
            char buf[4096];

            const int status_fd = open("/proc/self/status", O_RDONLY);
            if (status_fd == -1)
                return false;

            const ssize_t num_read = read(status_fd, buf, sizeof(buf) - 1);
            close(status_fd);

            if (num_read <= 0)
                return false;

            buf[num_read] = '\0';
            constexpr char tracerPidString[] = "TracerPid:";
            const auto tracer_pid_ptr = strstr(buf, tracerPidString);
            if (!tracer_pid_ptr)
                return false;

            for (const char* characterPtr = tracer_pid_ptr + sizeof(tracerPidString) - 1; characterPtr <= buf + num_read; ++characterPtr)
            {
                if (isspace(*characterPtr))
                    continue;
                else
                    return isdigit(*characterPtr) != 0 && *characterPtr != '0';
            }

            return false;
            #else
            #error "Unsupported architecture"
            #endif
        }

        TestContext& currentContext()
        {
            static TestContext context = [](){
                TestContext c;
                c.shouldDebugBreak = isDebuggerAttached();
                return c;
            }();
            return context;
        }
    }
}

#endif // SCHOOL_TESTER_H_