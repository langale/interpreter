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

#include <expected>
#include <vector>
#include <string>
#include <any>

#include <ale/detail/make_expected.hpp>

#include <intlib/ast/utils/evaluation_error_enum.hpp>
#include <intlib/ast/utils/evaluation_function_enum.hpp>
#include <intlib/ast/EvaluationResult.hpp>
#include <intlib/ast/EvaluationError.hpp>
#include <intlib/detail/type_string_cpp.hpp>

namespace intlib {
namespace ast {

template <typename type_t>
using Vec = std::vector<type_t>;

using Evaluation = std::expected<EvaluationResult, EvaluationError>;

[[nodiscard]] Evaluation append_error(
	EvaluationError&& err,
	const evaluation_error_e t,
	const evaluation_function_e f,
	std::string&& msg
);

template <typename type_t>
using Vec = std::vector<type_t>;

template <typename content_t, typename... params_t>
[[nodiscard]] Evaluation make_good_evaluation(params_t&&...params)
{
	if constexpr (std::is_same_v<content_t, EvaluationResult>) {
		if constexpr (sizeof...(params) > 0) {
			return ale::detail::make_expected<
				ale::detail::expected_type_e::Good,
				Evaluation>(std::forward<params_t>(params)...);
		}
		else {
			return ale::detail::make_expected<
				ale::detail::expected_type_e::Good,
				Evaluation>(std::any{}, detail::cpp_type_string<void>);
		}
	}
	else {
		return ale::detail::
			make_expected<ale::detail::expected_type_e::Good, Evaluation>(
				std::in_place_type_t<content_t>{},
				std::forward<params_t>(params)...
			);
	}
}

template <typename... params_t>
[[nodiscard]] Evaluation make_bad_evaluation(params_t&&...params)
{
	return ale::detail::make_expected<
		ale::detail::expected_type_e::Bad,
		Evaluation>(std::forward<params_t>(params)...);
}

} // namespace ast
} // namespace intlib
