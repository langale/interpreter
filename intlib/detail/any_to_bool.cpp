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

#include <optional>

#include <intlib/detail/any_type.hpp>
#include <intlib/detail/macros.hpp>

namespace intlib {
namespace detail {

std::optional<bool> any_to_bool(const memory::WrappedAny& value) noexcept
{
	if (value.type == detail::type_string_cpp<bool>) {
		return std::any_cast<bool>(value.value);
	}

	if (value.type == detail::type_string_cpp<int8_t>) {
		return static_cast<bool>(std::any_cast<int8_t>(value.value));
	}
	if (value.type == detail::type_string_cpp<uint8_t>) {
		return static_cast<bool>(std::any_cast<uint8_t>(value.value));
	}

	if (value.type == detail::type_string_cpp<int16_t>) {
		return static_cast<bool>(std::any_cast<int16_t>(value.value));
	}
	if (value.type == detail::type_string_cpp<uint16_t>) {
		return static_cast<bool>(std::any_cast<uint16_t>(value.value));
	}

	if (value.type == detail::type_string_cpp<int32_t>) {
		return static_cast<bool>(std::any_cast<int32_t>(value.value));
	}
	if (value.type == detail::type_string_cpp<uint32_t>) {
		return static_cast<bool>(std::any_cast<uint32_t>(value.value));
	}

	if (value.type == detail::type_string_cpp<int64_t>) {
		return static_cast<bool>(std::any_cast<int64_t>(value.value));
	}
	if (value.type == detail::type_string_cpp<uint64_t>) {
		return static_cast<bool>(std::any_cast<uint64_t>(value.value));
	}

	if (value.type == detail::type_string_cpp<std::float16_t>) {
		return static_cast<bool>(std::any_cast<std::float16_t>(value.value));
	}
	if (value.type == detail::type_string_cpp<std::float32_t>) {
		return static_cast<bool>(std::any_cast<std::float32_t>(value.value));
	}
	if (value.type == detail::type_string_cpp<std::float64_t>) {
		return static_cast<bool>(std::any_cast<std::float64_t>(value.value));
	}

	return {};
}

} // namespace detail
} // namespace intlib
