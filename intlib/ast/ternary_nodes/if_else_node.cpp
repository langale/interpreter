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

#include <ale/ast/ternary_nodes/IfElseNode.hpp>

#include <intlib/detail/any_to_bool.hpp>
#include <intlib/ast/EvaluationContext.hpp>
#include <intlib/ast/interpretation.hpp>
#include <intlib/logger/macros.hpp>
#include <intlib/ast/utils/evaluation_result_to_string.hpp>

namespace intlib {
namespace ast {

Evaluation evaluate(EvaluationContext& ctx, const ale::ast::IfElseNode& v)
{
	INTERPRETER_ENTER_AST_FUNCTION;

	const auto& condition = v.get_first_child();
	const auto& branch_if = v.get_second_child();
	const auto& branch_else = v.get_third_child();

	if (condition == nullptr) {
		INTERPRETER_PRINT("Condition of if statement is null.");
		return make_bad_evaluation(
			Vec{evaluation_error_e::If_Statement_Condition_Empty},
			Vec{evaluation_function_e::If_Else},
			Vec{"Condition of if statement is null"s}
		);
	}

	Evaluation condition_eval = interpret_node(ctx, condition);
	if (not condition_eval) {
		INTERPRETER_PRINT("Node evaluation failed.");
		return append_error(
			std::move(condition_eval.error()),
			evaluation_error_e::Evaluation_Of_Node_Failed,
			evaluation_function_e::If_Else,
			"Node evaluation failed"
		);
	}

	const EvaluationResult& condition_res = *condition_eval;
	const std::optional condition_value_w = detail::any_to_bool(condition_res);
	if (not condition_value_w) {
		INTERPRETER_PRINT("Unhandled variable type '{}'.", condition_res);
		return make_bad_evaluation(
			Vec{evaluation_error_e::Conversion_To_Bool_Failed},
			Vec{evaluation_function_e::If_Else},
			Vec{std::format(
				"Conversion to bool failed for value '{}'", condition_res
			)}
		);
	}

	if (*condition_value_w) {
		if (branch_if == nullptr) {
			INTERPRETER_PRINT(
				"Condition is true but first branch of 'if' is empty."
			);
			return make_bad_evaluation(
				Vec{evaluation_error_e::Node_Is_Malformed},
				Vec{evaluation_function_e::If_Else},
				Vec{"Condition is true but first branch of 'if' is empty"s}
			);
		}

		return interpret_node(ctx, branch_if);
	}

	if (branch_else == nullptr) {
		INTERPRETER_PRINT(
			"Condition is true but the second branch of if statement is empty."
		);
		return make_bad_evaluation(
			Vec{evaluation_error_e::Node_Is_Malformed},
			Vec{evaluation_function_e::If_Else},
			Vec{"Condition is true but second branch of 'if' is empty"s}
		);
	}

	return interpret_node(ctx, branch_else);
}

} // namespace ast
} // namespace intlib
