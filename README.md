# school-tester

C and C++ header-only testing library for assistance as a teaching resource.

## Inlcusion

As a header-only library, only `school_tester.hpp` is required. It should be included with some `.cpp` file that gets compiled with the program, typically the main program/tester. No extra build steps are necessary, and it will compile with whatever flags. As many schools use **C++11**, that is the minimum language standard supported.

### Targets

Tested on:

- X86_64 MSVC
- ARM Apple Clang
- X86_64 Linux G++

## Usage

There are various macros and classes available.

### `TEST_CASE(testName)`

Defines a test case, and adds it to the global list of test cases before `main()`.

```C++
TEST_CASE("some test name") {
    // ... test body
}
```

### `CHECK(condition)`

Asserts that a condition is true. If it's false, ends the test and marks it as failed.

```C++
TEST_CASE("two numbers equal") {
    bool flag = true;
    CHECK(flag); // true, so test continues
    flag = false;
    CHECK(flag); // false, so test ends here
    // nothing below here gets executed
}
```

Also works with any type that can be cast to `bool`.

```C++
TEST_CASE("two numbers equal") {
    std::ifstream file;
    file.open("hello_world.txt");
    CHECK(file);
}
```

### `CHECK_FALSE(condition)`

Asserts that a condition is false. If it's true, ends the test and marks it as failed.

```C++
TEST_CASE("two numbers equal") {
    bool flag = false;
    CHECK_FALSE(flag); // false, so test continues
    flag = true;
    CHECK_FALSE(flag); // true, so test ends here
    // nothing below here gets executed
}
```

Also works with any type that can be cast to `bool`.

```C++
TEST_CASE("two numbers equal") {
    std::ifstream file;
    file.open("file_that_doesnt_exist.txt");
    CHECK_FALSE(file);
}
```

### `CHECK_EQ(lhs, rhs)`

Asserts that two values are equal. If they are not equal, ends the test and marks it as failed.

```C++
TEST_CASE("two numbers equal") {
    int num = 10;
    CHECK_EQ(num, 10); // equal, so test continues
    num = 9;
    CHECK_EQ(num, 10); // not equal, so test ends here
    // nothing below here gets executed
}
```

Works for any type that implements `bool operator==`, accepted whatever types as the right hand argument that the operator overload supports.

```C++
TEST_CASE("two numbers equal") {
    std::string str = "hello world";
    CHECK_EQ(str, "hello world");
}
```

### `CHECK_NE(lhs, rhs)`

Asserts that two values are not equal. If they are equal, ends the test and marks it as failed.

```C++
TEST_CASE("two numbers equal") {
    int num = 9;
    CHECK_NE(num, 10); // not equal, so test continues
    num = 10;
    CHECK_NE(num, 10); // equal, so test ends here
    // nothing below here gets executed
}
```

Works for any type that implements `bool operator==`, accepted whatever types as the right hand argument that the operator overload supports.

```C++
TEST_CASE("two numbers equal") {
    std::string str = "hello world";
    CHECK_NE(str, "good morning world");
}
```

### `CHECK_LT(lhs, rhs)`

Asserts that the first argument is less than the second argument. If not, ends the test and marks it as failed.

```C++
TEST_CASE("two numbers equal") {
    int num = 9;
    CHECK_LT(num, 10); // is less, so test continues
    num = 10;
    CHECK_LT(num, 10); // is not less, so test ends here
    // nothing below here gets executed
}
```

Works for any type that implements `bool operator<`, accepted whatever types as the right hand argument that the operator overload supports.

```C++
TEST_CASE("two numbers equal") {
    std::string str = "hello world";
    CHECK_LT(str, "wello world");
}
```

### `CHECK_LE(lhs, rhs)`

Asserts that the first argument is less than or equal to the second argument. If not, ends the test and marks it as failed.

```C++
TEST_CASE("two numbers equal") {
    int num = 9;
    CHECK_LE(num, 10); // is less or equal, so test continues
    num = 10;
    CHECK_LE(num, 10); // is less or equal, so test continues
    num = 11;
    CHECK_LE(num, 10); // is not less and not equal, so test ends here
    // nothing below here gets executed
}
```

Works for any type that implements `bool operator<=`, accepted whatever types as the right hand argument that the operator overload supports.

```C++
TEST_CASE("two numbers equal") {
    std::string str = "hello world";
    CHECK_LE(str, "wello world");
}
```

### `CHECK_GT(lhs, rhs)`

Asserts that the first argument is greater than the second argument. If not, ends the test and marks it as failed.

```C++
TEST_CASE("two numbers equal") {
    int num = 11;
    CHECK_GT(num, 10); // is greater, so test continues
    num = 10;
    CHECK_GT(num, 10); // is not greater, so test ends here
    // nothing below here gets executed
}
```

Works for any type that implements `bool operator>`, accepted whatever types as the right hand argument that the operator overload supports.

```C++
TEST_CASE("two numbers equal") {
    std::string str = "hello world";
    CHECK_LT(str, "aello world");
}
```

### `CHECK_GE(lhs, rhs)`

Asserts that the first argument is greater than or equal to the second argument. If not, ends the test and marks it as failed.

```C++
TEST_CASE("two numbers equal") {
    int num = 11;
    CHECK_GE(num, 10); // is greater or equal, so test continues
    num = 10;
    CHECK_GE(num, 10); // is greater or equal, so test continues
    num = 9;
    CHECK_GE(num, 10); // is not greater and not equal, so test ends here
    // nothing below here gets executed
}
```

Works for any type that implements `bool operator>=`, accepted whatever types as the right hand argument that the operator overload supports.

```C++
TEST_CASE("two numbers equal") {
    std::string str = "hello world";
    CHECK_GE(str, "aello world");
}
```
