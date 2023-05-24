// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef UTILS_CONVERSIONS_HPP
#define UTILS_CONVERSIONS_HPP

#include <variant>

#include <nlohmann/json.hpp>

#include <utils/types.hpp>

namespace Everest {
using json = nlohmann::json;

namespace detail {

template <typename FundamentalType> struct FundamentalTypeTrait {
    static const auto type = nlohmann::json::value_t::discarded;
};

template <> struct FundamentalTypeTrait<nullptr_t> {
    static const auto type = nlohmann::json::value_t::null;
};

template <> struct FundamentalTypeTrait<bool> {
    static const auto type = nlohmann::json::value_t::boolean;
};

template <> struct FundamentalTypeTrait<int> {
    static const auto type = nlohmann::json::value_t::number_integer;
};

template <> struct FundamentalTypeTrait<unsigned int> {
    static const auto type = nlohmann::json::value_t::number_unsigned;
};

template <> struct FundamentalTypeTrait<double> {
    static const auto type = nlohmann::json::value_t::number_float;
};

template <> struct FundamentalTypeTrait<std::string> {
    static const auto type = nlohmann::json::value_t::string;
};

template <> struct FundamentalTypeTrait<nlohmann::json::array_t> {
    static const auto type = nlohmann::json::value_t::array;
};

template <> struct FundamentalTypeTrait<nlohmann::json::object_t> {
    static const auto type = nlohmann::json::value_t::object;
};

template <typename T> bool json_to_variant_impl(T& to, const nlohmann::json& from) noexcept {
    return false;
}

template <typename VariantType, typename CurrentType, typename... Rest>
bool json_to_variant_impl(VariantType& to, const nlohmann::json& from) noexcept {
    if (from.type() == FundamentalTypeTrait<CurrentType>::type) {
        to = from.get<CurrentType>();
        return true;
    }

    return json_to_variant_impl<VariantType, Rest...>(to, from);
}

} // namespace detail

template <typename... Ts> static std::variant<Ts...> json_to_variant(const nlohmann::json& j) {
    std::variant<Ts...> var;

    if (detail::json_to_variant_impl<std::variant<Ts...>, Ts...>(var, j)) {
        return var;
    }

    throw std::runtime_error("The given json object doesn't contain any type, the std::variant is aware of");
}

template <typename T> nlohmann::json variant_to_json(T variant) {
    return std::visit([](auto&& arg) -> nlohmann::json { return arg; }, variant);
}

} // namespace Everest

#endif // UTILS_CONVERSIONS_HPP
