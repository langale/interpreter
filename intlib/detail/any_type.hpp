/*********************************************************************
 *
 * ALE language -- an interpreted programming language
 * Copyright (C) 2024 - 2026 Lluís Alemany Puig
 *
 * This file is part of the implementation of ALE. The full code is
 * available at:
 *
 *     https://github.com/langale/ale
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Contact:
 *
 *     Lluís Alemany Puig
 *     lluis.alemany.puig@gmail.com
 *     https://github.com/lluisalemanypuig
 *
 ********************************************************************/

#pragma once

#include <string>
#include <any>

#include <intlib/detail/type_string_cpp.hpp>

namespace intlib {
namespace detail {

/**
 * @brief De-mangles the type name of a std::any value.
 *
 * @param name Input name from type().name()
 * @returns A more human-readable string for the name of a std::any.
 */
[[nodiscard]] std::string demangle_name_type(const char *name);

/// Returns a 'standardized' name for 'a'.
[[nodiscard]] std::string get_type_name(const std::any& value_w);

/// Constant expression to check whether a given type is a built-in type or not.
template <typename type_t>
static constexpr bool is_cpp_builtin_type_v =
	std::is_same_v<type_t, bool> or std::is_same_v<type_t, char> or
	std::is_same_v<type_t, unsigned char> or
	std::is_same_v<type_t, signed char> or std::is_same_v<type_t, int8_t> or
	std::is_same_v<type_t, uint8_t> or std::is_same_v<type_t, int16_t> or
	std::is_same_v<type_t, uint16_t> or std::is_same_v<type_t, int32_t> or
	std::is_same_v<type_t, uint32_t> or std::is_same_v<type_t, int64_t> or
	std::is_same_v<type_t, uint64_t> or
	std::is_same_v<type_t, std::float16_t> or
	std::is_same_v<type_t, std::float32_t> or std::is_same_v<type_t, float> or
	std::is_same_v<type_t, std::float64_t> or std::is_same_v<type_t, double> or
	std::is_void_v<type_t>;

/**
 * @brief Checks whether an input string corresponds to a given built-in type.
 * @tparam type_t Given type.
 * @param name Name of the type.
 * @returns Whether the input string corresponds to type @e T.
 */
template <typename type_t>
[[nodiscard]] constexpr bool
is_cpp_basic_type(const std::string_view name) noexcept
{
	static_assert(is_cpp_builtin_type_v<type_t>);
	return name == cpp_type_string<type_t>;
}

/**
 * @brief Does the given string represent a C++ built-in type?
 * @tparam type_t Given type.
 * @param name String to be checked.
 * @returns Whether the input string contains a C++ built-in type.
 */
template <typename type_t>
[[nodiscard]] bool is_cpp_type(const std::string_view name)
{
	if constexpr (is_cpp_builtin_type_v<type_t>) {
		return is_cpp_basic_type<type_t>(name);
	}

	return name == demangle_name_type(typeid(type_t).name());
}

/**
 * @brief Checks whether the input std::any holds a specific given type.
 * @tparam type_t Given type.
 * @param value_w Input std::any.
 * @returns True or false depending on whether the input std::any holds a value
 * of the given type @e T.
 */
template <typename type_t>
[[nodiscard]] bool holds_cpp_type(const std::any& value_w)
{
	const std::string name = get_type_name(value_w);
	return is_cpp_type<type_t>(name);
}

} // namespace detail
} // namespace intlib
