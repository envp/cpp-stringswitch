#include <iostream>
#include <optional>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <array>

#include "stringswitch/stringswitch.h"

using stringswitch::StringSwitch;

enum class Fruit { k_Apple = 0, k_Mango, k_Orange, k_Invalid = -1 };

std::ostream &operator<<(std::ostream &os, const Fruit &fruit) {
  switch (fruit) {
  case Fruit::k_Apple:
    os << "Fruit::k_Apple";
    break;
  case Fruit::k_Mango:
    os << "Fruit::k_Mango";
    break;
  case Fruit::k_Orange:
    os << "Fruit::k_Orange";
    break;
  case Fruit::k_Invalid:
    os << "Fruit::k_Invalid";
    break;
  }
  return os;
}

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::optional<T> &elem) {
  if (elem.has_value()) {
    os << "(" << elem.value() << ")";
  } else {
    os << "<nullopt>";
  }
  return os;
}

template <typename T>
void assert_equal(const T &left, const T &right) {
  if (left != right) {
    std::stringstream message;
    message << "The operands provided did not compare equal: \n\t" << left
            << " != " << right;
    throw std::runtime_error(message.str());
  }
}

template <typename T>
void assert_true(const T &value) {
  if (!value) {
    std::stringstream message;
    message << "Expected argument to evaluate to true, but got false.";
    throw std::runtime_error(message.str());
  }
}

void test_early_binding_with_default() {
  std::string param = "apple";

  // Parameter can be in constructor
  auto result = StringSwitch<Fruit>::create(param)
                    .when("apple", Fruit::k_Apple)
                    .when("mango", Fruit::k_Mango)
                    .when("orange", Fruit::k_Invalid)
                    .on_default(Fruit::k_Orange)
                    .evaluate();

  assert_equal(result, Fruit::k_Apple);
}

void test_early_binding_without_default() {
  std::string param = "mango";

  // Parameter can be in constructor
  std::optional<Fruit> result = StringSwitch<Fruit>::create(param)
                                    .when("apple", Fruit::k_Apple)
                                    .when("mango", Fruit::k_Mango)
                                    .when("orange", Fruit::k_Invalid)
                                    .evaluate();

  assert_true(result.has_value());
  assert_equal(result.value(), Fruit::k_Mango);
}

void test_early_binding_with_only_default() {
  std::string param = "apple";

  auto result = StringSwitch<Fruit>::create(param)
                    .on_default(Fruit::k_Invalid)
                    .evaluate();

  assert_equal(result, Fruit::k_Invalid);
}

void test_late_binding_with_default() {
  // Parameter can be in constructor
  auto switcher = StringSwitch<Fruit>::create()
                      .when("apple", Fruit::k_Apple)
                      .when("mango", Fruit::k_Mango)
                      .when("orange", Fruit::k_Orange)
                      .on_default(Fruit::k_Invalid);

  assert_equal(switcher.evaluate("apple"), Fruit::k_Apple);
  assert_equal(switcher.evaluate("mango"), Fruit::k_Mango);
  assert_equal(switcher.evaluate("orange"), Fruit::k_Orange);
  assert_equal(switcher.evaluate("bad"), Fruit::k_Invalid);
}

void test_late_binding_without_default() {
  // Parameter can be in constructor
  auto switcher = StringSwitch<Fruit>::create()
                      .when("apple", Fruit::k_Apple)
                      .when("mango", Fruit::k_Mango)
                      .when("orange", Fruit::k_Orange);

  std::array<const char *, 4> args = {"apple", "mango", "orange", "bad"};
  std::array<std::optional<Fruit>, 4> expected = {
      Fruit::k_Apple, Fruit::k_Mango, Fruit::k_Orange, std::nullopt};

  for (size_t idx = 0; idx != args.size(); ++idx) {
    auto outcome = switcher.evaluate(args[idx]);
    assert_equal(outcome, expected[idx]);
  }
}

void test_late_binding_with_only_default() {
  auto switcher = StringSwitch<Fruit>::create().on_default(Fruit::k_Invalid);

  std::array<const char *, 4> args = {"apple", "mango", "orange", "bad"};
  Fruit expected = Fruit::k_Invalid;

  for (size_t idx = 0; idx != args.size(); ++idx) {
    auto outcome = switcher.evaluate(args[idx]);
    assert_equal(outcome, expected);
  }
}

int main() {
  test_early_binding_with_default();
  test_early_binding_without_default();
  test_early_binding_with_only_default();

  test_late_binding_with_default();
  test_late_binding_without_default();
  test_late_binding_with_only_default();
}
