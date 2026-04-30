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

#include <cmath>
#include <any>

#include <intlib/logger/macros.hpp>
#include <intlib/detail/macros.hpp>
#include <intlib/detail/any_type.hpp>

namespace intlib {
namespace arithmetic {

#define aleprln ale::logger::println

std::any arithmetic_exponentiation(
	const std::any& left_w, const std::any& right_w
) noexcept
{
	INTERPRETER_ENTER_ARITHMETIC_FUNCTION(aleprln);

	const std::string left_type = detail::get_type_name(left_w);
	const std::string right_type = detail::get_type_name(right_w);

	if (detail::is_cpp_type<uint64_t>(left_type)) {
		const auto left = std::any_cast<uint64_t>(left_w);
		INTERPRETER_PRINT(aleprln, "First component is uint64_t: {}.", left);

		if (detail::is_cpp_type<uint64_t>(right_type)) {
			const auto right = std::any_cast<uint64_t>(right_w);
			INTERPRETER_PRINT(
				aleprln, "Second component is uint64_t: {}.", right
			);

			return detail::to_float64(std::pow(left, right));
		}

		if (detail::is_cpp_type<int64_t>(right_type)) {
			const auto right = std::any_cast<int64_t>(right_w);
			INTERPRETER_PRINT(
				aleprln, "Second component is int64_t: {}.", right
			);

			return detail::to_float64(std::pow(detail::to_int64(left), right));
		}

		if (detail::is_cpp_type<std::float64_t>(right_type)) {
			const auto bd = std::any_cast<std::float64_t>(right_w);
			INTERPRETER_PRINT(
				aleprln, "Second component is std::float64_t: {}.", bd
			);

			return detail::to_float64(std::pow(detail::to_float64(left), bd));
		}

		INTERPRETER_PRINT(
			aleprln,
			"Parameter right type {} is not handled.",
			detail::get_type_name(right_w)
		);
	}

	if (detail::is_cpp_type<int64_t>(left_type)) {
		const auto left = std::any_cast<int64_t>(left_w);
		INTERPRETER_PRINT(aleprln, "First component is int64_t: {}.", left);

		if (detail::is_cpp_type<uint64_t>(right_type)) {
			const auto right = std::any_cast<uint64_t>(right_w);
			INTERPRETER_PRINT(
				aleprln, "Second component is uint64_t: {}.", right
			);

			return detail::to_float64(std::pow(left, detail::to_int64(right)));
		}

		if (detail::is_cpp_type<int64_t>(right_type)) {
			const auto right = std::any_cast<int64_t>(right_w);
			INTERPRETER_PRINT(
				aleprln, "Second component is int64_t: {}.", right
			);

			return detail::to_float64(std::pow(left, right));
		}

		if (detail::is_cpp_type<std::float64_t>(right_type)) {
			const auto bd = std::any_cast<std::float64_t>(right_w);
			INTERPRETER_PRINT(
				aleprln, "Second component is std::float64_t: {}.", bd
			);

			return detail::to_float64(std::pow(detail::to_float64(left), bd));
		}

		INTERPRETER_PRINT(
			aleprln,
			"Parameter right type {} is not handled.",
			detail::get_type_name(right_w)
		);
	}

	if (detail::is_cpp_type<std::float64_t>(left_type)) {
		const auto left = std::any_cast<std::float64_t>(left_w);
		INTERPRETER_PRINT(
			aleprln, "First component is std::float64_t: {}.", left
		);

		if (detail::is_cpp_type<uint64_t>(right_type)) {
			const auto right = std::any_cast<uint64_t>(right_w);
			INTERPRETER_PRINT(
				aleprln, "Second component is uint64_t: {}.", right
			);

			return detail::to_float64(std::pow(left, detail::to_float64(right))
			);
		}

		if (detail::is_cpp_type<int64_t>(right_type)) {
			const auto right = std::any_cast<int64_t>(right_w);
			INTERPRETER_PRINT(
				aleprln, "Second component is int64_t: {}.", right
			);

			return detail::to_float64(std::pow(left, detail::to_float64(right))
			);
		}

		if (detail::is_cpp_type<std::float64_t>(right_type)) {
			const auto bd = std::any_cast<std::float64_t>(right_w);
			INTERPRETER_PRINT(
				aleprln, "Second component is std::float64_t: {}.", bd
			);

			return detail::to_float64(std::pow(left, bd));
		}

		INTERPRETER_PRINT(
			aleprln,
			"Parameter right type {} is not handled.",
			detail::get_type_name(right_w)
		);
	}

	INTERPRETER_PRINT(
		aleprln,
		"Parameter left type {} is not handled.",
		detail::get_type_name(left_w)
	);

	return {};
}

} // namespace arithmetic
} // namespace intlib
