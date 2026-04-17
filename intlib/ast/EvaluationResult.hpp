/*********************************************************************
 *
 * ALE interpreter library -- the base utilities for a command line utility
 * to run Programs written in ALE
 *
 *     Copyright (C) 2024 - 2026 Lluís Alemany Puig
 *
 * This file is part of the implementation of an interpreter for ALE.
 * The full code is available at:
 *
 *     https://github.com/langale/interpreter
 *
 * This Program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this Program.  If not, see <https://www.gnu.org/licenses/>.
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

namespace intlib {
namespace ast {

struct EvaluationError {
	std::vector<evaluation_error_e> error;
	std::vector<std::string> message;
};

[[nodiscard]] EvaluationError append_error(
	EvaluationError&& err, const evaluation_error_e t, std::string&& msg
);

using EvaluationResult = std::expected<std::any, EvaluationError>;

template <typename content_t, typename... params_t>
EvaluationResult make_good_evaluation_result(params_t&&...params)
{
	if constexpr (std::is_same_v<content_t, std::any>) {
		return ale::detail::make_expected<
			ale::detail::expected_type_e::Good,
			EvaluationResult>(std::forward<params_t>(params)...);
	}
	else {
		return ale::detail::
			make_expected<ale::detail::expected_type_e::Good, EvaluationResult>(
				std::in_place_type_t<content_t>{},
				std::forward<params_t>(params)...
			);
	}
}

template <typename... params_t>
EvaluationResult make_bad_evaluation_result(params_t&&...params)
{
	return ale::detail::make_expected<
		ale::detail::expected_type_e::Bad,
		EvaluationResult>(std::forward<params_t>(params)...);
}

} // namespace ast
} // namespace intlib
