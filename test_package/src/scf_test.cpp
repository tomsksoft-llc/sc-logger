#include <gtest/gtest.h>
#include <scf/scf.h>
#include <vector>

// User types for the UserTypeTest test

struct UserType1 {
    std::string data{};
    int number = 0;
};

std::string ToString(const UserType1 &arg) {
    return "{ " + arg.data + ", "  + std::to_string(arg.number) + " }";
}

class UserType2 {
public:
    explicit UserType2(int num) : number(num) {
    }

    int get_number() const { return number; }

private:
    int number = 0;
};

std::string ToString(const UserType2 &arg) {
    return "{ " + std::to_string(arg.get_number()) + " }";
}

using UserType3 = std::vector<UserType2>;

std::string ToString(const UserType3 &arg) {
    std::stringstream ss;
    ss << "{ ";
    for (const auto &v : arg) {
        ss << ToString(v) << " ";
    }

    ss << "}";
    return ss.str();
}

// Tests

TEST(ScfTest, CommonTest) {
  const std::string arg1 = "world";
  const std::size_t arg2 = 17;
  const bool arg3 = true;

  const std::string result
      = SCFormat("Hello, %s. Required C++ standard - %d, it's %b", arg1, arg2, arg3);
  const auto origin = "Hello, world. Required C++ standard - 17, it's true";
  ASSERT_TRUE(result == origin);
}

TEST(ScfTest, EmptyFormatTest) {
  const std::string result = SCFormat("");
  const std::string origin;
  ASSERT_TRUE(result == origin);
}

TEST(ScfTest, StringTest) {
  const std::string arg1 = "string";
  const std::string_view arg2 = "string_view";
  const auto arg3 = "literal";
  const char *arg4 = "pointer";
  const auto arg5 = "";

  const std::string result
      = SCFormat("'%s', '%s', '%s', '%s', '%s' - strings",
                 arg1, arg2, arg3, arg4, arg5);
  const auto origin = "'string', 'string_view', 'literal', 'pointer', '' - strings";

  ASSERT_TRUE(result == origin);
}

TEST(ScfTest, IntegerTest) {
  const char arg1 = -128;
  const unsigned char arg2 = 255;
  const short arg3 = -32767;
  const unsigned short arg4 = 65535;
  const long arg5 = -2147483647;
  const unsigned long arg6 = 4294967295;
  const long long arg7 = -9223372036854775807;

  const auto result
      = SCFormat("%d, %d, %d, %d, %d, %d, %d - integers",
                 arg1, arg2, arg3, arg4, arg5, arg6, arg7);
  const auto origin = "-128, 255, -32767, 65535, -2147483647, "
                      "4294967295, -9223372036854775807 - integers";

  ASSERT_TRUE(result == origin);
}

TEST(ScfTest, CharTest) {
  const char arg1 = 'c';
  const unsigned char arg2 = 'u';

  const auto result
      = SCFormat("%c, %c - chars", arg1, arg2);
  const auto origin = "c, u - chars";

  ASSERT_TRUE(result == origin);
}

TEST(ScfTest, BooleanTest) {
  const bool arg1 = true;
  const bool arg2 = false;

  const auto result
      = SCFormat("%b, %b - booleans", arg1, arg2);
  const auto origin = "true, false - booleans";

  ASSERT_TRUE(result == origin);
}

TEST(ScfTest, FloatTest) {
  const float arg1 = 3.14f;
  const double arg2 = 3.141593;
  const long double arg3 = 3.141593;

  const auto result
      = SCFormat("%f, %f, %f - floats", arg1, arg2, arg3);
  const auto origin = "3.140000, 3.141593, 3.141593 - floats";

  ASSERT_TRUE(result == origin);
}

TEST(ScfTest, TypePackTest) {
  scf::detail::TypePack<int, bool> val1;
  scf::detail::TypePack<int, bool, double> val2;
  scf::detail::TypePack<bool, double> val3;
  scf::detail::TypePack val4;
  scf::detail::TypePack<double> val5;

  static_assert(scf::detail::PushBack<double>(val1) == val2);
  static_assert(scf::detail::PopFront(val2) == val3);
  static_assert(scf::detail::PushBack<double>(val4) == val5);

  static_assert(scf::detail::Size(val1) == 2);
  static_assert(scf::detail::Size(val2) == 3);
  static_assert(scf::detail::Size(val3) == 2);
  static_assert(scf::detail::Size(val4) == 0);
  static_assert(scf::detail::Size(val5) == 1);

  static_assert(scf::detail::Contains<int>(val1));
  static_assert(scf::detail::Contains<bool>(val1));
  static_assert(scf::detail::Contains<double>(val2));
  static_assert(!scf::detail::Contains<int>(val3));
  static_assert(!scf::detail::Contains<bool>(val4));
  static_assert(scf::detail::Contains<double>(val5));
}

TEST(ScfTest, UserTypeTest) {
    UserType1 arg1{"sample", 123};
    UserType2 arg2(10);
    UserType3 arg3;

    for (auto i = 0; i < 5; ++i) {
        arg3.emplace_back(i);
    }

    const auto result = SCFormat("%U, %U, %U - user types", arg1, arg2, arg3);
    const auto origin = "{ sample, 123 }, { 10 }, { { 0 } { 1 } { 2 } { 3 } { 4 } } - user types";
    ASSERT_TRUE(result == origin);
}
