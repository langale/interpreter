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
#include <intlib/detail/any_to_bool.hpp>

namespace intlib {
namespace detail {

static constexpr std::array numeric_types{
	"bool",
	"char",
	"unsigned char",
	"signed char",
	"u8",
	"i8",
	"u16",
	"i16",
	"u32",
	"i32",
	"u64",
	"i64",
	"f16",
	"f32",
	"f64"
};

bool is_type_numeric(const std::string& type) noexcept
{
	return std::ranges::find(numeric_types, type) != numeric_types.end();
}

#define OPTIONAL_TO_ANY(func, value)                                           \
	const auto o = func(value);                                                \
	if (o) {                                                                   \
		return std::any{*o};                                                   \
	}                                                                          \
	return {};

std::any any_convert_to_type(const std::any& value_w, const std::string& type)
{
	if (type == "bool") {
		OPTIONAL_TO_ANY(any_to_bool, value_w);
	}

	if (is_type_numeric(type)) {
		if (type == "char") {
			OPTIONAL_TO_ANY(any_to_numeric<char>, value_w);
		}
		if (type == "unsigned char") {
			OPTIONAL_TO_ANY(any_to_numeric<unsigned char>, value_w);
		}
		if (type == "signed char") {
			OPTIONAL_TO_ANY(any_to_numeric<signed char>, value_w);
		}

		if (type == "u8") {
			OPTIONAL_TO_ANY(any_to_numeric<uint8_t>, value_w);
		}
		if (type == "i8") {
			OPTIONAL_TO_ANY(any_to_numeric<int8_t>, value_w);
		}

		if (type == "u16") {
			OPTIONAL_TO_ANY(any_to_numeric<uint16_t>, value_w);
		}
		if (type == "i16") {
			OPTIONAL_TO_ANY(any_to_numeric<int16_t>, value_w);
		}

		if (type == "u32") {
			OPTIONAL_TO_ANY(any_to_numeric<uint32_t>, value_w);
		}
		if (type == "i32") {
			OPTIONAL_TO_ANY(any_to_numeric<int32_t>, value_w);
		}

		if (type == "u64") {
			OPTIONAL_TO_ANY(any_to_numeric<uint64_t>, value_w);
		}
		if (type == "i64") {
			OPTIONAL_TO_ANY(any_to_numeric<int64_t>, value_w);
		}

		if (type == "f16") {
			OPTIONAL_TO_ANY(any_to_numeric<std::float16_t>, value_w);
		}
		if (type == "f32") {
			OPTIONAL_TO_ANY(any_to_numeric<std::float32_t>, value_w);
		}
		if (type == "f64") {
			OPTIONAL_TO_ANY(any_to_numeric<std::float64_t>, value_w);
		}
	}
	return {};
}

} // namespace detail
} // namespace intlib
