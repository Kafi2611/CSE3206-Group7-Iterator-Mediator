#pragma once
/**
 * @file   TestFramework.hpp
 * @brief  A tiny, dependency-free unit-test helper (no GoogleTest needed,
 *         so the tests build on any lab PC with just g++).
 *
 * Usage:
 *   TEST_CASE(friends_are_sorted) { CHECK_EQ(a, b); }
 *   int main() { return campus::test::runAll(); }
 */

#include <exception>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace campus::test {

struct TestCase {
    std::string           name;
    std::function<void()> body;
};

inline std::vector<TestCase>& registry() {
    static std::vector<TestCase> tests;
    return tests;
}

struct Registrar {
    Registrar(const char* name, std::function<void()> body) {
        registry().push_back({name, std::move(body)});
    }
};

/// Thrown by a failing CHECK; caught and reported by runAll().
struct AssertionFailure : std::runtime_error {
    using std::runtime_error::runtime_error;
};

inline std::string location(const char* file, int line) {
    std::ostringstream out;
    out << file << ':' << line;
    return out.str();
}

inline int runAll() {
    int failed = 0;
    for (const TestCase& test : registry()) {
        try {
            test.body();
            std::cout << "[  PASS  ] " << test.name << '\n';
        } catch (const std::exception& error) {
            ++failed;
            std::cout << "[  FAIL  ] " << test.name << "\n           " << error.what() << '\n';
        }
    }
    std::cout << "\n" << registry().size() - static_cast<std::size_t>(failed) << " passed, "
              << failed << " failed, " << registry().size() << " total\n";
    return failed == 0 ? 0 : 1;
}

}  // namespace campus::test

#define TEST_CASE(name)                                                         \
    static void name();                                                         \
    static const campus::test::Registrar registrar_##name(#name, name);        \
    static void name()

#define CHECK(condition)                                                        \
    do {                                                                        \
        if (!(condition)) {                                                     \
            throw campus::test::AssertionFailure(                               \
                campus::test::location(__FILE__, __LINE__) + "  CHECK(" #condition ") failed"); \
        }                                                                       \
    } while (false)

#define CHECK_EQ(actual, expected)                                              \
    do {                                                                        \
        const auto& actualValue_   = (actual);                                  \
        const auto& expectedValue_ = (expected);                                \
        if (!(actualValue_ == expectedValue_)) {                                \
            std::ostringstream message_;                                        \
            message_ << campus::test::location(__FILE__, __LINE__)              \
                     << "  CHECK_EQ(" #actual ", " #expected ") failed: got '"  \
                     << actualValue_ << "', expected '" << expectedValue_ << "'"; \
            throw campus::test::AssertionFailure(message_.str());               \
        }                                                                       \
    } while (false)

#define CHECK_THROWS_AS(statement, ExceptionType)                               \
    do {                                                                        \
        bool caught_ = false;                                                   \
        try {                                                                   \
            statement;                                                          \
        } catch (const ExceptionType&) {                                        \
            caught_ = true;                                                     \
        }                                                                       \
        if (!caught_) {                                                         \
            throw campus::test::AssertionFailure(                               \
                campus::test::location(__FILE__, __LINE__) +                    \
                "  expected " #ExceptionType " from: " #statement);             \
        }                                                                       \
    } while (false)
