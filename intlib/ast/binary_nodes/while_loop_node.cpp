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
#include <string>
using namespace std::string_literals;

#include <ale/ast/binary_nodes/WhileLoopNode.hpp>

#include <intlib/detail/any_to_bool.hpp>
#include <intlib/ast/EvaluationContext.hpp>
#include <intlib/ast/Evaluation.hpp>
#include <intlib/ast/interpretation.hpp>
#include <intlib/logger/macros.hpp>
#if defined ALE_LOGGING_MESSAGES
#endif

namespace intlib {
namespace ast {

Evaluation evaluate(EvaluationContext& ctx, const ale::ast::WhileLoopNode& v)
{
	INTERPRETER_ENTER_AST_FUNCTION;

	const auto& left_child = v.get_left_child();
	const auto& right_child = v.get_right_child();

	if (left_child == nullptr) {
		INTERPRETER_PRINT("Condition in while loop is missing.");
		return make_bad_evaluation(
			Vec{evaluation_error_e::Node_Is_Malformed},
			Vec{evaluation_function_e::While_Loop},
			Vec{"Condition in while loop is missing"s}
		);
	}

	bool stop = false;
	while (not stop) {
		Evaluation cond_w = interpret_node(ctx, left_child);
		if (not cond_w) {
			INTERPRETER_PRINT("Node evaluation failed.");
			return append_error(
				std::move(cond_w.error()),
				evaluation_error_e::Evaluation_Of_Node_Failed,
				evaluation_function_e::While_Loop,
				"Node evaluation failed"
			);
		}

		const std::optional cond_res_w = detail::any_to_bool(*cond_w);
		if (not cond_res_w) {
			INTERPRETER_PRINT(
				"Could not convert value in while loop condition to a Boolean "
				"value."
			);
			return append_error(
				std::move(cond_w.error()),
				evaluation_error_e::Conversion_To_Bool_Failed,
				evaluation_function_e::While_Loop,
				"Could not convert value in while loop condition to a Boolean "
				"value"
			);
		}

		stop = not * cond_res_w;
		if (*cond_res_w) {

			if (right_child == nullptr) {
				// yes, this may produce infinite loops
				continue;
			}

			Evaluation res_w = interpret_node(ctx, right_child);
			if (not res_w) {
				INTERPRETER_PRINT("Evaluation of while loop body failed.");
				return append_error(
					std::move(cond_w.error()),
					evaluation_error_e::Evaluation_Of_Node_While_Loop_Failed,
					evaluation_function_e::While_Loop,
					"Evaluation of while loop body failed."
				);
			}
		}
	}

	return make_good_evaluation<EvaluationResult>();
}

} // namespace ast
} // namespace intlib
