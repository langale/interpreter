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
#include <intlib/detail/type_string_ale.hpp>
#include <intlib/memory/VariableValue.hpp>

namespace intlib {
namespace detail {

template <Numeric_c cpp_t>
[[nodiscard]] std::optional<cpp_t> any_to_numeric(const memory::WrappedAny& wrap
) noexcept
{
	if (wrap.type == type_string_cpp<memory::RefMemVar>) {
		return any_to_numeric<cpp_t>(
			std::any_cast<memory::RefMemVar>(wrap.value).get().wrap
		);
	}
	if (wrap.type == type_string_cpp<memory::RefConstMemVar>) {
		return any_to_numeric<cpp_t>(
			std::any_cast<memory::RefConstMemVar>(wrap.value).get().wrap
		);
	}

	if (wrap.type == type_string_cpp<bool>) {
		return static_cast<cpp_t>(std::any_cast<bool>(wrap.value));
	}

	if (wrap.type == type_string_cpp<char>) {
		return static_cast<cpp_t>(std::any_cast<char>(wrap.value));
	}
	if (wrap.type == type_string_cpp<unsigned char>) {
		return static_cast<cpp_t>(std::any_cast<unsigned char>(wrap.value));
	}
	if (wrap.type == type_string_cpp<signed char>) {
		return static_cast<cpp_t>(std::any_cast<signed char>(wrap.value));
	}

	if (wrap.type == type_string_cpp<uint8_t>) {
		return static_cast<cpp_t>(std::any_cast<uint8_t>(wrap.value));
	}
	if (wrap.type == type_string_cpp<int8_t>) {
		return static_cast<cpp_t>(std::any_cast<int8_t>(wrap.value));
	}

	if (wrap.type == type_string_cpp<uint16_t>) {
		return static_cast<cpp_t>(std::any_cast<uint16_t>(wrap.value));
	}
	if (wrap.type == type_string_cpp<int16_t>) {
		return static_cast<cpp_t>(std::any_cast<int16_t>(wrap.value));
	}

	if (wrap.type == type_string_cpp<uint32_t>) {
		return static_cast<cpp_t>(std::any_cast<uint32_t>(wrap.value));
	}
	if (wrap.type == type_string_cpp<int32_t>) {
		return static_cast<cpp_t>(std::any_cast<int32_t>(wrap.value));
	}

	if (wrap.type == type_string_cpp<uint64_t>) {
		return static_cast<cpp_t>(std::any_cast<uint64_t>(wrap.value));
	}
	if (wrap.type == type_string_cpp<int64_t>) {
		return static_cast<cpp_t>(std::any_cast<int64_t>(wrap.value));
	}

	if (wrap.type == type_string_cpp<std::float16_t>) {
		return static_cast<cpp_t>(std::any_cast<std::float16_t>(wrap.value));
	}
	if (wrap.type == type_string_cpp<std::float32_t>) {
		return static_cast<cpp_t>(std::any_cast<std::float32_t>(wrap.value));
	}
	if (wrap.type == type_string_cpp<std::float64_t>) {
		return static_cast<cpp_t>(std::any_cast<std::float64_t>(wrap.value));
	}

	return {};
}

template <Numeric_c cpp_t>
[[nodiscard]] std::optional<cpp_t> any_to_numeric(const memory::RefMemVar& value
) noexcept
{
	return any_to_numeric<cpp_t>(value.get().wrap);
}

template <Numeric_c cpp_t>
[[nodiscard]] std::optional<cpp_t>
any_to_numeric(const memory::RefConstMemVar& value) noexcept
{
	return any_to_numeric<cpp_t>(value.get().wrap);
}

} // namespace detail
} // namespace intlib
