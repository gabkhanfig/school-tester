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
#include <sstream>
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

    TestReport runTests(const TestConfig& config);

    class TestException : public std::exception {
    public:

        TestException(std::string condition, const char* file, int line, const char* extra = "");

        virtual const char* what() const noexcept override;

        template<typename Lhs, typename Rhs>
        static std::string generateComparisonString(const Lhs& lhs, const Rhs& rhs, const char* comparison);


    private:
        std::string what_;
        std::string condition_;
        const char* file_;
        int line_;
    };
}

#define _ST_CONCAT_IMPL(a, b) a##b
#define _ST_CONCAT(a, b) _ST_CONCAT_IMPL(a, b)
#define _ST_GENERATE_TEST_FUNC _ST_CONCAT(SCHOOL_TEST_, __COUNTER__)

#define _ST_REGISTER_TEST(f, testName)                      \
static void f();                                            \
static const int _ST_CONCAT(_ST_IGNORE_, __COUNTER__)       \
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
        throw st::TestException(#condition, __FILE__, __LINE__, "Expected to be true"); \
    }   \
} while(false)

#define CHECK_FALSE(condition) \
do { \
    if(condition) { \
        if(st::detail::currentContext().shouldDebugBreak) { \
            _ST_DEBUG_BREAK(); \
        } \
        throw st::TestException(std::string("!") + #condition, __FILE__, __LINE__, "Expected to be false"); \
    }   \
} while(false)

#define CHECK_EQ(lhs, rhs) \
do { \
    if((lhs) != (rhs)) { \
        if(st::detail::currentContext().shouldDebugBreak) { \
            _ST_DEBUG_BREAK(); \
        } \
        throw st::TestException(    \
            st::TestException::generateComparisonString(lhs, rhs, "=="), __FILE__, __LINE__, "Expected to be equal"); \
    }   \
} while(false)

#define CHECK_NE(lhs, rhs) \
do { \
    if((lhs) == (rhs)) { \
        if(st::detail::currentContext().shouldDebugBreak) { \
            _ST_DEBUG_BREAK(); \
        } \
        throw st::TestException(    \
            st::TestException::generateComparisonString(lhs, rhs, "!="), __FILE__, __LINE__, "Expected to be not equal"); \
    }   \
} while(false)

#define CHECK_LT(lhs, rhs) \
do { \
    if((lhs) >= (rhs)) { \
        if(st::detail::currentContext().shouldDebugBreak) { \
            _ST_DEBUG_BREAK(); \
        } \
        throw st::TestException(    \
            st::TestException::generateComparisonString(lhs, rhs, "<"), __FILE__, __LINE__, "Expected to be less than"); \
    }   \
} while(false)

#define CHECK_LE(lhs, rhs) \
do { \
    if((lhs) > (rhs)) { \
        if(st::detail::currentContext().shouldDebugBreak) { \
            _ST_DEBUG_BREAK(); \
        } \
        throw st::TestException(    \
            st::TestException::generateComparisonString(lhs, rhs, "<="), __FILE__, __LINE__, "Expected to be less than or equal to"); \
    }   \
} while(false)

#define CHECK_GT(lhs, rhs) \
do { \
    if((lhs) <= (rhs)) { \
        if(st::detail::currentContext().shouldDebugBreak) { \
            _ST_DEBUG_BREAK(); \
        } \
        throw st::TestException(    \
            st::TestException::generateComparisonString(lhs, rhs, ">"), __FILE__, __LINE__, "Expected to be greater than"); \
    }   \
} while(false)

#define CHECK_GE(lhs, rhs) \
do { \
    if((lhs) < (rhs)) { \
        if(st::detail::currentContext().shouldDebugBreak) { \
            _ST_DEBUG_BREAK(); \
        } \
        throw st::TestException(    \
            st::TestException::generateComparisonString(lhs, rhs, ">="), __FILE__, __LINE__, "Expected to be greater than or equal to"); \
    }   \
} while(false)

namespace st {
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
    } // namespace detail

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

    st::TestException::TestException(std::string condition, const char* file, int line, const char* extra)
        : condition_(std::move(condition)), file_(file), line_(line)
    {
        what_ = std::string(file_) + ':' + std::to_string(line_) + ": ERROR: " + "Check failed [ " + condition_ + " ]. " + extra;
    }

    inline const char *TestException::what() const noexcept
    {
        return this->what_.c_str();
    }

    template <typename Lhs, typename Rhs>
    inline std::string TestException::generateComparisonString(const Lhs &lhs, const Rhs &rhs, const char *comparison)
    {
        std::ostringstream os;
        os << lhs << ' ' << comparison << ' ' << rhs;
        return os.str();
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
} // namespace st

#endif // SCHOOL_TESTER_H_