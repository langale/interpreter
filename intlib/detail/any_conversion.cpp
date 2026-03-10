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

#include <string>
#include <array>
#include <any>

#include <intlib/detail/any_to_numeric.hpp>

namespace intlib {
namespace detail {

static constexpr std::array numeric_types{
	"u8", "i8", "u16", "i16", "u32", "i32", "u64", "i64", "f16", "f32", "f64"
};

bool is_type_numeric(const std::string& type) noexcept
{
	return std::ranges::find(numeric_types, type) != numeric_types.end();
}

std::any any_convert_to_type(const std::any& value, const std::string& type)
{
	if (is_type_numeric(type)) {
		if (type == "u8") {
			return any_to_numeric<uint8_t>(value);
		}
		if (type == "i8") {
			return any_to_numeric<int8_t>(value);
		}

		if (type == "u16") {
			return any_to_numeric<uint16_t>(value);
		}
		if (type == "i16") {
			return any_to_numeric<int16_t>(value);
		}

		if (type == "u32") {
			return any_to_numeric<uint32_t>(value);
		}
		if (type == "i32") {
			return any_to_numeric<int32_t>(value);
		}

		if (type == "u64") {
			return any_to_numeric<uint64_t>(value);
		}
		if (type == "i64") {
			return any_to_numeric<int64_t>(value);
		}

		if (type == "f16") {
			return any_to_numeric<std::float16_t>(value);
		}
		if (type == "f32") {
			return any_to_numeric<std::float32_t>(value);
		}
		if (type == "f64") {
			return any_to_numeric<std::float64_t>(value);
		}
	}
	return {};
}

} // namespace detail
} // namespace intlib
