// Copyright András Vukics 2006–2026. Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE.txt)
#pragma once

/// @brief Perfect-forwarding macro. Expands to `std::forward<decltype(x)>(x)`, avoiding the need to repeat the type.
#define FWD(x) std::forward<decltype(x)>(x)

#include <boost/hana.hpp>
/// @brief Namespace alias for Boost.Hana, used for compile-time sequences and heterogeneous containers.
namespace hana=boost::hana;

#include <boost/json.hpp>

#include <type_traits>


namespace cppqedutils {

/// @brief Namespace alias for Boost.JSON, used for structured logging and JSON serialization.
namespace json = boost::json ;
/// @brief Alias for a JSON object used as a structured log tree.
/// Passed through the trajectory and structure layers to accumulate human-readable, machine-parseable log data.
using LogTree = json::object ;

/// @brief Serializes any JSON-convertible value to a string via `json::value_from`.
/// @param v Any value for which `boost::json::value_from` is defined.
/// @return A JSON string representation of @p v.
std::string toStringJSON(auto&& v) {return json::serialize( json::value_from( FWD(v) ) ) ;}

} // cppqedutils


namespace cppqedutils {

/// @brief Primary template for the pass-by-value trait.
/// Specializations are `true` for view types (e.g. `MultiArrayView`) and `false` for owning types (e.g. `MultiArray`).
/// The primary template returns `std::nullopt` to signal the absence of a specialization.
template <typename T>
constexpr auto passByValue_v=std::nullopt;


/// @brief Metafunction yielding the reference type of @p State. Defaults to `State&`.
/// Specialized for view types to return the view itself, since views are already reference-like.
/// @note Template aliases cannot be partially specialized, hence the metafunction class.
template <typename State> struct ReferenceMF : std::type_identity<std::add_lvalue_reference_t<State>> {};

/// @brief Convenience alias for `ReferenceMF<State>::type`.
template <typename State> using Reference = typename ReferenceMF<State>::type;

/// @brief Metafunction yielding the const-reference type of @p State. Defaults to `const State&`.
/// Specialized for view types to return a const view.
template <typename State> struct ConstReferenceMF : std::type_identity<std::add_lvalue_reference_t<std::add_const_t<State>>> {};

/// @brief Convenience alias for `ConstReferenceMF<State>::type`.
template <typename State> using ConstReference = typename ConstReferenceMF<State>::type;

} // cppqedutils