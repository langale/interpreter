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

#include <cxxabi.h>
#include <memory>
#include <string>
#include <any>

namespace intlib {
namespace detail {

/**
 * @brief Demangles the type name of a std::any value.
 *
 * Copied from ChatGPT.
 * @param name Input name from type().name()
 * @returns A more human-readable string for the name of a std::any.
 */
[[nodiscard]] inline std::string demangle_name_type(const char *name) noexcept
{
	int status = -4;
	std::unique_ptr<char, void (*)(void *)> res{
		abi::__cxa_demangle(name, nullptr, nullptr, &status), std::free
	};
	return (status == 0) ? res.get() : name;
}

/// Returns a 'standardized' name for 'a'.
[[nodiscard]] inline std::string get_name(const std::any& a) noexcept
{
	return demangle_name_type(a.type().name());
}

/**
 * @brief Checks whether an input string corresponds to a given type.
 * @tparam type_t Given type.
 * @param name Name of the type.
 * @returns True or false depending on whether the input string corresponds to
 * type @e T.
 */
template <typename type_t>
[[nodiscard]] bool is_type(const std::string& name) noexcept
{
	if constexpr (std::is_same_v<type_t, bool>) {
		return name == "bool";
	}
	else if constexpr (std::is_same_v<type_t, int64_t>) {
		return name == "long";
	}
	else if constexpr (std::is_same_v<type_t, uint64_t>) {
		return name == "unsigned long";
	}
	else if constexpr (std::is_same_v<type_t, double>) {
		return name == "double";
	}
	else if constexpr (std::is_same_v<type_t, std::string>) {
		return name == "std::__cxx11::basic_string<char, "
					   "std::char_traits<char>, std::allocator<char> >";
	}
	else if constexpr (std::is_void_v<type_t>) {
		return name == "void";
	}
	else {
		static_assert(false);
	}
}

/**
 * @brief Checks whether the input std::any holds a specific given type.
 * @tparam type_t Given type.
 * @param a Input std::any.
 * @returns True or false depending on whether the input std::any holds the a value
 * of the given type @e T.
 */
template <typename type_t>
[[nodiscard]] bool is_type(const std::any& a) noexcept
{
	const std::string name = demangle_name_type(a.type().name());
	return is_type<type_t>(name);
}

#define UNHANDLED_ANY(os, a)                                                   \
	os << ERROR_LOCATION << "Unhandled type '" << intlib::detail::get_name(a)  \
	   << "'.\n"

} // namespace detail
} // namespace intlib
