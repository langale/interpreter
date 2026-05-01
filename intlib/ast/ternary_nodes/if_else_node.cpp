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

#include <intlib/logger/macros.hpp>
#include <intlib/detail/any_type.hpp>
#include <intlib/detail/any_output.hpp>
#include <intlib/detail/any_to_bool.hpp>
#include <intlib/ast/EvaluationContext.hpp>
#include <intlib/ast/interpretation.hpp>

namespace intlib {
namespace ast {

#define aleprln ale::logger::println

Evaluation evaluate(EvaluationContext& ctx, const ale::ast::IfElseNode& v)
{
	INTERPRETER_ENTER_AST_FUNCTION(aleprln);

	const auto& first_child = v.get_first_child();
	const auto& second_child = v.get_second_child();
	const auto& third_child = v.get_third_child();

	if (first_child == nullptr) {
		INTERPRETER_PRINT(aleprln, "Condition of if statement is null.");
		return make_bad_evaluation(
			Vec{evaluation_error_e::If_Statement_Condition_Empty},
			Vec{evaluation_function_e::If_Else},
			Vec{"Condition of if statement is null"s}
		);
	}

	Evaluation cond_res = interpret_node(ctx, first_child);
	if (not cond_res.has_value()) {
		INTERPRETER_PRINT(aleprln, "Node evaluation failed.");
		return append_error(
			std::move(cond_res.error()),
			evaluation_error_e::Evaluation_Of_Node_Failed,
			evaluation_function_e::If_Else,
			"Node evaluation failed"
		);
	}

	const EvaluationResult& cond = *cond_res;
	const std::optional cond_bool_w = detail::any_to_bool(cond);
	if (not cond_bool_w) {
		INTERPRETER_PRINT(
			aleprln,
			"Unhandled variable type '{}'.",
			detail::get_type_name(*cond_bool_w)
		);
		return make_bad_evaluation(
			Vec{evaluation_error_e::Conversion_To_Bool_Failed},
			Vec{evaluation_function_e::If_Else},
			Vec{std::format(
				"Conversion to bool failed for type '{}'",
				detail::get_type_name(*cond_bool_w)
			)}
		);
	}

	if (*cond_bool_w) {
		if (second_child == nullptr) {
			INTERPRETER_PRINT(
				aleprln, "Condition is true but first branch of 'if' is empty."
			);
			return make_bad_evaluation(
				Vec{evaluation_error_e::Node_Is_Malformed},
				Vec{evaluation_function_e::If_Else},
				Vec{"Condition is true but first branch of 'if' is empty"s}
			);
		}

		return interpret_node(ctx, second_child);
	}

	if (third_child == nullptr) {
		INTERPRETER_PRINT(
			aleprln,
			"Condition is true but the second branch of if statement is empty."
		);
		return make_bad_evaluation(
			Vec{evaluation_error_e::Node_Is_Malformed},
			Vec{evaluation_function_e::If_Else},
			Vec{"Condition is true but second branch of 'if' is empty"s}
		);
	}

	return interpret_node(ctx, third_child);
}

} // namespace ast
} // namespace intlib
