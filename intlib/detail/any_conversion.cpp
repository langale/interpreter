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
#include <any>

#include <ale/logger/Logger.hpp>

#include <intlib/detail/any_to_numeric.hpp>
#include <intlib/detail/any_to_bool.hpp>
#include <intlib/detail/type_string_ale.hpp>
#include <intlib/logger/macros.hpp>

namespace intlib {
namespace detail {

#define aleprln ale::logger::println

#define OPTIONAL_TO_ANY(func, value)                                           \
	const auto o = func(value);                                                \
	if (o) {                                                                   \
		return std::any{*o};                                                   \
	}                                                                          \
	return {};

#define TEST(test_ale_type)                                                    \
	if (to_ale_type == (test_ale_type)) {                                      \
		using CppType = AleToCpp_t<test_ale_type>;                             \
		OPTIONAL_TO_ANY(any_to_numeric<CppType>, value_w);                     \
	}

std::any
convert_to_ale_type(const std::any& value_w, const std::string_view to_ale_type)
{
	INTERPRETER_ENTER_DETAIL_FUNCTION(aleprln);

	INTERPRETER_PRINT(
		aleprln,
		"Convert std::any object to a value of ALE type '{}'.",
		to_ale_type
	);

	if (to_ale_type == bool_ale) {
		OPTIONAL_TO_ANY(any_to_bool, value_w);
	}

	if (is_ale_type_numeric(to_ale_type)) {
		TEST(i8_ale);
		TEST(u8_ale);

		TEST(i16_ale);
		TEST(u16_ale);

		TEST(i32_ale);
		TEST(u32_ale);

		TEST(i64_ale);
		TEST(u64_ale);

		TEST(f16_ale);
		TEST(f32_ale);
		TEST(f64_ale);
	}

	INTERPRETER_PRINT(
		aleprln, "Unhandled conversion to type '{}'.", to_ale_type
	);

	return {};
}

} // namespace detail
} // namespace intlib
