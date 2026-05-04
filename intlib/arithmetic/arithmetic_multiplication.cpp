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

#include <any>

#include <intlib/logger/macros.hpp>
#include <intlib/detail/macros.hpp>
#include <intlib/memory/utils/wrapped_any_to_string.hpp>
#include <intlib/arithmetic/utils.hpp>

namespace intlib {
namespace arithmetic {

std::optional<WrappedAny>
arithmetic_multiplication(const WrappedAny& left_w, const WrappedAny& right_w)
{
	INTERPRETER_ENTER_ARITHMETIC_FUNCTION;

	INTERPRETER_PRINT("Operating {} * {}.", left_w, right_w);

	if (left_w.type == detail::type_string_cpp<uint64_t>) {
		const auto left = std::any_cast<uint64_t>(left_w.value);
		INTERPRETER_PRINT("First component is uint64_t: {}.", left);

		if (right_w.type == detail::type_string_cpp<uint64_t>) {
			const auto right = std::any_cast<uint64_t>(right_w.value);
			INTERPRETER_PRINT("Second component is uint64_t: {}.", right);

			const auto res = left * right;
			return make_uint64(res);
		}

		if (right_w.type == detail::type_string_cpp<int64_t>) {
			const auto right = std::any_cast<int64_t>(right_w.value);
			INTERPRETER_PRINT("Second component is int64_t: {}.", right);

			const auto res = detail::to_int64(left) * right;
			return make_int64(res);
		}

		if (right_w.type == detail::type_string_cpp<std::float64_t>) {
			const auto right = std::any_cast<std::float64_t>(right_w.value);
			INTERPRETER_PRINT("Second component is std::float64_t: {}.", right);

			const auto res = detail::to_float64(left) * right;
			return make_float64(res);
		}

		INTERPRETER_PRINT("Parameter right_w '{}'.", right_w);
	}

	if (left_w.type == detail::type_string_cpp<int64_t>) {
		const auto left = std::any_cast<int64_t>(left_w.value);
		INTERPRETER_PRINT("First component is int64_t: {}.", left);

		if (right_w.type == detail::type_string_cpp<uint64_t>) {
			const auto right = std::any_cast<uint64_t>(right_w.value);
			INTERPRETER_PRINT("Second component is uint64_t: {}.", right);

			const auto res = left * detail::to_int64(right);
			return make_int64(res);
		}

		if (right_w.type == detail::type_string_cpp<int64_t>) {
			const auto right = std::any_cast<int64_t>(right_w.value);
			INTERPRETER_PRINT("Second component is int64_t: {}.", right);

			const auto res = left * right;
			return make_int64(res);
		}

		if (right_w.type == detail::type_string_cpp<std::float64_t>) {
			const auto right = std::any_cast<std::float64_t>(right_w.value);
			INTERPRETER_PRINT("Second component is std::float64_t: {}.", right);

			const auto res = detail::to_float64(left) * right;
			return make_float64(res);
		}

		INTERPRETER_PRINT("Parameter right_w '{}'.", right_w);
	}

	if (left_w.type == detail::type_string_cpp<std::float64_t>) {
		const auto left = std::any_cast<std::float64_t>(left_w.value);
		INTERPRETER_PRINT("First component is std::float64_t: {}.", left);

		if (right_w.type == detail::type_string_cpp<uint64_t>) {
			const auto right = std::any_cast<uint64_t>(right_w.value);
			INTERPRETER_PRINT("Second component is uint64_t: {}.", right);

			const auto res = left * detail::to_float64(right);
			return make_float64(res);
		}

		if (right_w.type == detail::type_string_cpp<int64_t>) {
			const auto right = std::any_cast<int64_t>(right_w.value);
			INTERPRETER_PRINT("Second component is int64_t: {}.", right);

			const auto res = left * detail::to_float64(right);
			return make_float64(res);
		}

		if (right_w.type == detail::type_string_cpp<std::float64_t>) {
			const auto right = std::any_cast<std::float64_t>(right_w.value);
			INTERPRETER_PRINT("Second component is std::float64_t: {}.", right);

			const auto res = left * right;
			return make_float64(res);
		}

		INTERPRETER_PRINT("Parameter right_w '{}'.", right_w);
	}

	INTERPRETER_PRINT("Parameter left_w '{}'.", left_w);

	return {};
}

} // namespace arithmetic
} // namespace intlib
