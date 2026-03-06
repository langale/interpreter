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
#include <any>

#include <ale/logger/macros.hpp>

#include <intlib/Program.hpp>
#include <intlib/detail/any_to_bool.hpp>

namespace intlib {

EvaluationResult Program::evaluate(const ale::ast::WhileLoopNode& v)
{
	const auto& left_child = v.get_left_child();
	const auto& right_child = v.get_right_child();

	if (left_child == nullptr) {
		ALE_PRINT_LOC(
			ale::logger::println, "Condition in while loop is missing."
		);
		return {};
	}

	bool stop = false;
	while (not stop) {
		EvaluationResult cond = interpret_node(left_child);
		if (not cond) {
			ALE_PRINT_LOC(ale::logger::println, "Node evaluation failed.");
			return append_error(
				std::move(cond.error()),
				evaluation_error_e::Evaluation_Of_Node_Failed,
				"Node evaluation failed"
			);
		}

		const std::optional<bool> cond_bool = detail::any_to_bool(*cond);
		if (not cond_bool) {
			ALE_PRINT_LOC(
				ale::logger::println,
				"Could not convert value in while loop condition to a Boolean "
				"value."
			);
			return append_error(
				std::move(cond.error()),
				evaluation_error_e::Conversion_To_Bool_Failed,
				"Could not convert value in while loop condition to a Boolean "
				"value."
			);
		}

		stop = not *cond_bool;
		if (*cond_bool) {
			// yes, this may produce infinite loops
			if (right_child == nullptr) {
				continue;
			}

			EvaluationResult r = interpret_node(right_child);
			if (not r) {
				ALE_PRINT_LOC(
					ale::logger::println,
					"Evaluation of while loop body failed."
				);
				return append_error(
					std::move(cond.error()),
					evaluation_error_e::Evaluation_Of_Node_While_Loop_Failed,
					"Evaluation of while loop body failed."
				);
			}
		}
	}

	return std::any{};
}

} // namespace intlib
