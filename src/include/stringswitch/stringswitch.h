#ifndef INCLUDED_STRINGSWITCH_STRINGSWITCH_H
#define INCLUDED_STRINGSWITCH_STRINGSWITCH_H

#include "stringswitch_impl.h"

namespace stringswitch {

/// A type safe switch-case over strings useful to map strings or string
/// literals to an enum type.
///
/// It tries to perform various validations at compile time to eliminate common
/// misuses, for example:
/// * Not specifying a parameter to evaluate the switch against
/// * Not setting up any cases and defaults to check the parameter against
/// * Trying to set up multiple defaults.
///
/// The semantics of what is "right" matches that of a switch statement:
/// * A parameter is required (either at creation, or evaluation)
/// * At least 1 case or default label must exist.
/// * defaults may not repeat.
///
/// For example, the following is a 'valid' use:
///
/// ```cpp
/// #include <stringswitch/stringswitch.h>
///
/// enum class Fruit { k_Apple, k_Mango, k_Orange, k_Invalid };
///
/// // Valid usage example
/// Fruit from_string(const std::string &name) {
///   return StringSwitch<Fruit>::create(name)      // Uninit
///             .when("apple", Fruit::k_Apple)      // Partial
///             .when("mango", Fruit::k_Mango)      // Partial
///             .when("orange", Fruit::k_Orange)    // Partial
///             .default(Fruit::k_Invalid)          // Complete
///             .evaluate();
/// }
///
/// The follow cases are 'invalid' uses that should produce compilation errors.
///
/// No cases/defaults
/// ```cpp
/// // Should produce a compilation error
/// Fruit from_string(const std::string &name) {
///   return StringSwitch<Fruit>::create(name)
///             .evaluate()
///         //  ^ Sets a parameter to use but provides no cases or defaults.
/// }
/// ```
///
/// Double parameters (both on creation and evaluation
/// ```cpp
/// // Should produce a compilation error
/// Fruit from_string(const std::string &name) {
///   const char *other_name = "...";
///   return StringSwitch<Fruit>::create(name)
///             .evaluate(other_name)
///         //  ^ A paramter was already set. Which one to use is ambiguous.
/// }
/// ```
template <typename Result>
using StringSwitch = detail::StringSwitchImpl<Result>;
} // namespace stringswitch

#endif // INCLUDED_STRINGSWITCH_STRINGSWITCH_H
