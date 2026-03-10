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

#include <optional>
#include <stdfloat>

#include <intlib/detail/any_type.hpp>
#include <intlib/detail/concepts.hpp>

namespace intlib {
namespace detail {

template <numeric_c to_type_t>
[[nodiscard]] std::any any_to_numeric(const std::any& a) noexcept
{
	const std::string name = get_type_name(a);

	if (is_builtin_type<uint8_t>(name)) {
		return static_cast<to_type_t>(std::any_cast<uint8_t>(a));
	}
	if (is_builtin_type<int8_t>(name)) {
		return static_cast<to_type_t>(std::any_cast<int8_t>(a));
	}

	if (is_builtin_type<uint16_t>(name)) {
		return static_cast<to_type_t>(std::any_cast<uint16_t>(a));
	}
	if (is_builtin_type<int16_t>(name)) {
		return static_cast<to_type_t>(std::any_cast<int16_t>(a));
	}

	if (is_builtin_type<uint32_t>(name)) {
		return static_cast<to_type_t>(std::any_cast<uint32_t>(a));
	}
	if (is_builtin_type<int32_t>(name)) {
		return static_cast<to_type_t>(std::any_cast<int32_t>(a));
	}

	if (is_builtin_type<uint64_t>(name)) {
		return static_cast<to_type_t>(std::any_cast<uint64_t>(a));
	}
	if (is_builtin_type<int64_t>(name)) {
		return static_cast<to_type_t>(std::any_cast<int64_t>(a));
	}

	if (is_builtin_type<std::float16_t>(name)) {
		return static_cast<to_type_t>(std::any_cast<std::float16_t>(a));
	}
	if (is_builtin_type<std::float32_t>(name)) {
		return static_cast<to_type_t>(std::any_cast<std::float32_t>(a));
	}
	if (is_builtin_type<std::float64_t>(name)) {
		return static_cast<to_type_t>(std::any_cast<std::float64_t>(a));
	}

	return {};
}

} // namespace detail
} // namespace intlib
