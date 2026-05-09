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

#include <ale/detail/make_optional.hpp>

#include <intlib/detail/any_to_numeric.hpp>
#include <intlib/detail/any_to_bool.hpp>
#include <intlib/detail/type_string_ale.hpp>
#include <intlib/detail/type_traits_ale.hpp>
#include <intlib/detail/type_string_cpp.hpp>
#include <intlib/logger/macros.hpp>
#if defined ALE_LOGGING_MESSAGES
#include <intlib/memory/utils/wrapped_any_to_string.hpp>
#endif

namespace intlib {
namespace detail {

#define OPTIONAL_TO_ANY(func, value, CppType)                                  \
	const auto o = func(value);                                                \
	if (o) {                                                                   \
		return {*o, detail::type_string_cpp<CppType>};                         \
	}                                                                          \
	return {std::any{}, detail::type_string_cpp<void>};

#define TEST(test_ale_type)                                                                     \
	if (ale_type == (test_ale_type)) {                                                          \
		INTERPRETER_PRINT("Found a match at type '{}'.", test_ale_type);                        \
		using CppType = AleToCpp_t<test_ale_type>;                                              \
		INTERPRETER_PRINT("Corresponding C++ type is '{}'.", detail::type_string_cpp<CppType>); \
		OPTIONAL_TO_ANY(any_to_numeric<CppType>, value, CppType);                               \
	}

memory::WrappedAny convert_to_ale_type(
	const memory::WrappedAny& value, const std::string_view ale_type
)
{
	INTERPRETER_ENTER_DETAIL_FUNCTION;

	INTERPRETER_PRINT(
		"Convert '{}' to a value of ALE type '{}'.", value, ale_type
	);

	if (ale_type == ale_bool) {
		OPTIONAL_TO_ANY(any_to_bool, value, bool);
	}

	if (is_ale_type_numeric(ale_type)) {
		TEST(ale_i8);
		TEST(ale_u8);

		TEST(ale_i16);
		TEST(ale_u16);

		TEST(ale_i32);
		TEST(ale_u32);

		TEST(ale_i64);
		TEST(ale_u64);

		TEST(ale_f16);
		TEST(ale_f32);
		TEST(ale_f64);
	}

	INTERPRETER_PRINT("Unhandled conversion to type '{}'.", ale_type);

	return {};
}

} // namespace detail
} // namespace intlib
