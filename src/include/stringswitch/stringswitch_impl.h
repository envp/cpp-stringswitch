#ifndef INCLUDED_STRINGSWITCH_STRINGSWITCH_IMPL_H
#define INCLUDED_STRINGSWITCH_STRINGSWITCH_IMPL_H

#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>

namespace stringswitch::detail {

class Empty {};

// A type tag wrapping a boolean that tracks if a parameter to evaluate the
// strings-switch has been bound yet.
//
// If the wrapped constant is `true`:
//  `StringSwitch::create(...)` was called with a parameter
//
// Otherwise:
//  `StringSwitch::create(...)` was called with no parameters.
template <bool state>
class ParamBoundTag : public std::bool_constant<state> {};

// A type tag wrapping a boolean that tracks if a default has been set on the
// stringswitch.
//
// If a default has been set (by calling `.on_default`), the wrapped boolean is
// `true`, otherwise it is `false`.
template <bool state>
class DefaultBoundTag : public std::bool_constant<state> {};

template <class Result, class ParamStateTag = void, class ResultStateTag = void>
class StringSwitchImpl;

/// Terminal state, that knows about parameters as well as defaults assocaited
/// with the stringswitch.
///
/// This performs the following validations:
///
/// * `on_default` is called at most once
/// * `evaluate(...)` must use the correct number of arguments.
///
/// Allows users to set up cases (using `StringSwitchImpl::when`) or defaults
/// (using `StringSwitchImpl::on_default`).
template <class Result, bool param_given, bool default_given>
class StringSwitchImpl<Result, ParamBoundTag<param_given>,
                       DefaultBoundTag<default_given>> {
public:
  using Param = std::string;
  using EffectiveResultType =
      std::conditional_t<default_given, Result, std::optional<Result>>;

  template <bool state>
  using SelfWithDefault = StringSwitchImpl<Result, ParamBoundTag<param_given>,
                                           DefaultBoundTag<state>>;

  /// Associate the paramter  `label` to the Outcome `result`.
  /// If the parameter used to evaluate the stringswitch matches the label
  /// provided here, `result` will be returned.
  SelfWithDefault<default_given> &when(std::string_view label, Result result) {
    this->d_mapping.emplace(std::make_pair(label, result));
    return *this;
  }

  /// Set a default to use when evaluating the stringswitch.
  SelfWithDefault<true> on_default(Result default_result)
  requires(!default_given)
  {
    return SelfWithDefault<true>{d_mapping, d_param, default_result};
  }

  /// Evaluate the stringswitch with the given parameter.
  EffectiveResultType evaluate(std::string_view param) const
  requires(!param_given)
  {
    return evaluate_impl(std::string(param));
  }

  /// Evaluate the stringswitch using the parameter provided during creation.
  EffectiveResultType evaluate() const
  requires(param_given)
  {
    return evaluate_impl(d_param);
  }

private:
  // Allow a stringswitch with the same parameter state to construct this type.
  friend class StringSwitchImpl<Result, ParamBoundTag<param_given>>;
  // Allow classes with type-tag DefaultBoundTag<false> to cosntruct
  // DefaultBoundTag<true>.
  //
  // This is a transition that happens when `.on_default()` is called the first
  // time.
  friend class StringSwitchImpl<Result, ParamBoundTag<param_given>,
                                DefaultBoundTag<false>>;

  using ParamType = std::string;
  using ParamStorage = std::conditional_t<param_given, ParamType, Empty>;
  using OutcomeStorage = std::conditional_t<default_given, Result, Empty>;
  using MapStorage = std::unordered_map<ParamType, Result>;

  StringSwitchImpl(MapStorage mapping_args, ParamStorage param,
                   OutcomeStorage outcome)
      : d_mapping(mapping_args),
        d_param(param),
        d_default_outcome(outcome) {}

  EffectiveResultType evaluate_impl(const std::string &param) const {
    auto it = d_mapping.find(param);
    if (it != d_mapping.end()) {
      return it->second;
    }
    if constexpr (default_given) {
      return d_default_outcome;
    } else {
      return std::nullopt;
    }
  }

  MapStorage d_mapping;
  ParamStorage d_param;
  OutcomeStorage d_default_outcome;
};

/// An intermediate state in the stringswitch state machine.
///
/// This only has knowledge about whether a paramater to evaluate the string
/// switch exists or not.
///
/// Allows users to set up cases (using `StringSwitchImpl::when`) or defaults
/// (using `StringSwitchImpl::on_default`).
template <class Result, bool param_given>
class StringSwitchImpl<Result, ParamBoundTag<param_given>> {
public:
  using Param = std::string;
  template <bool default_given>

  // Convenience typedef so the return type is somewhat readable
  using StringSwitchWithDefault =
      StringSwitchImpl<Result, ParamBoundTag<param_given>,
                       DefaultBoundTag<default_given>>;

  StringSwitchWithDefault<false> when(std::string_view label, Result &&result) {
    return {{{std::string(label), std::move(result)}}, d_param, {}};
  }

  StringSwitchWithDefault<true> on_default(Result &&result) {
    return {{}, {}, result};
  }

private:
  // The default specialization is the entrypoint and is the only way to reach
  // a state with only parameters bound. Declare it as friend so it is able
  // to call our private constructor.
  friend class StringSwitchImpl<Result>;

  explicit StringSwitchImpl(std::string_view param)
  requires(param_given)
      : d_param(param) {}

  StringSwitchImpl()
  requires(!param_given)
  = default;

  // TODO:
  // This occupies at least 1 extra byte when no parameter was provided, try to
  // eliminate this storage requirement.
  std::conditional_t<param_given, Param, Empty> d_param;
};

/// The default specialization is the entrypoint for `StringSwitch` as a
/// library. It is designed to be the only public creator of the `StringSwitch`
/// family of types.
///
/// Attributes such as cases, and defaults are allowed on types downstream in
/// the state-machine.
template <class Result>
class StringSwitchImpl<Result> {
public:
  template <bool param_given>
  using StringSwitchWithParam =
      StringSwitchImpl<Result, ParamBoundTag<param_given>>;

  static StringSwitchWithParam<true> create(std::string_view param) {
    return StringSwitchWithParam<true>{param};
  }

  static StringSwitchWithParam<false> create() { return {}; }
};

} // namespace stringswitch::detail

#endif // INCLUDED_STRINGSWITCH_STRINGSWITCH_IMPL_H
