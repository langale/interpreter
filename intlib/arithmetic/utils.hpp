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

#include <ale/detail/make_optional.hpp>

#include <intlib/memory/WrappedAny.hpp>
#include <intlib/detail/type_string_cpp.hpp>
#include <intlib/detail/macros.hpp>
#include <intlib/arithmetic/definitions.hpp>

namespace intlib {
namespace arithmetic {

[[nodiscard]] inline std::optional<WrappedAny> make_uint64(const uint64_t value)
{
	return ale::detail::make_optional<
		WrappedAny>(value, intlib::detail::type_string_cpp<uint64_t>);
}

[[nodiscard]] inline std::optional<WrappedAny> make_int64(const int64_t value)
{
	if (value < 0) {
		return ale::detail::make_optional<
			memory::
				WrappedAny>(value, intlib::detail::type_string_cpp<int64_t>);
	}
	return ale::detail::make_optional<
		memory::
			WrappedAny>(detail::to_uint64(value), intlib::detail::type_string_cpp<uint64_t>);
}

[[nodiscard]] inline std::optional<WrappedAny>
make_float64(const std::float64_t value)
{
	return ale::detail::make_optional<
		memory::
			WrappedAny>(value, intlib::detail::type_string_cpp<std::float64_t>);
}

} // namespace arithmetic
} // namespace intlib
