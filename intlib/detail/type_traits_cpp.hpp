/*********************************************************************
 *
 * ALE interpreter library -- the base utilities for a command line utility
 * to run programs written in ALE
 *
 *     Copyright (C) 2024 - 2026 Lluís Alemany Puig
 *
 * This file is part of the implementation of an interpreter for ALE.
 * The full code is available at:
 *
 *     https://github.com/langale/interpreter
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

#include <string_view>
#include <cstdint>
#include <stdfloat>

#include <intlib/detail/type_string_cpp.hpp>

namespace intlib {
namespace detail {

template <typename type_t, typename... params_t>
[[nodiscard]] constexpr bool is_in_pack(const std::string_view t) noexcept
{
	const bool first = t == type_string_cpp<type_t>;
	if constexpr (sizeof...(params_t) > 0) {
		return first or is_in_pack<params_t...>(t);
	}
	return first;
}

[[nodiscard]] constexpr bool is_integral(const std::string_view t) noexcept
{
	return is_in_pack<
		bool,
		char,
		unsigned char,
		signed char,
		uint8_t,
		int8_t,
		uint16_t,
		int16_t,
		uint32_t,
		int32_t,
		uint64_t,
		int64_t>(t);
}

[[nodiscard]] constexpr bool is_decimal(const std::string_view t) noexcept
{
	return is_in_pack<
		float,
		double,
		std::float16_t,
		std::float32_t,
		std::float64_t>(t);
}

} // namespace detail
} // namespace intlib
