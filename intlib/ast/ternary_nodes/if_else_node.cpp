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

#include <ale/ast/ternary_nodes/IfElseNode.hpp>

#include <intlib/logger/macros.hpp>
#include <intlib/detail/any_type.hpp>
#include <intlib/detail/any_output.hpp>
#include <intlib/detail/any_to_bool.hpp>
#include <intlib/ast/EvaluationContext.hpp>
#include <intlib/ast/interpretation.hpp>

namespace intlib {
namespace ast {

EvaluationResult evaluate(EvaluationContext& ctx, const ale::ast::IfElseNode& v)
{
	INTERPRETER_ENTER_AST_FUNCTION(ale::logger::println);

	const auto& first_child = v.get_first_child();
	const auto& second_child = v.get_second_child();
	const auto& third_child = v.get_third_child();

	if (first_child == nullptr) {
		INTERPRETER_PRINT_LOC(
			ale::logger::println, "Condition of if statement is null."
		);
		return EvaluationError{
			.error = {evaluation_error_e::If_Statement_Condition_Empty},
			.message = {"Condition of if statement is null."}
		};
	}

	EvaluationResult cond_w = interpret_node(ctx, first_child);
	if (not cond_w.has_value()) {
		INTERPRETER_PRINT_LOC(ale::logger::println, "Node evaluation failed.");
		return append_error(
			std::move(cond_w.error()),
			evaluation_error_e::Evaluation_Of_Node_Failed,
			"Node evaluation failed"
		);
	}

	const std::optional cond_bool_w = detail::any_to_bool(*cond_w);
	if (not cond_bool_w) {
		INTERPRETER_PRINT_LOC2(
			ale::logger::println,
			"Unhandled variable type '{}'.",
			detail::get_type_name(*cond_bool_w)
		);
		return EvaluationError{
			.error = {evaluation_error_e::Unhandled_Variable_Type},
			.message = {std::format(
				"Unhandled type '{}'", detail::get_type_name(*cond_bool_w)
			)}
		};
	}

	if (*cond_bool_w) {
		if (second_child == nullptr) {
			INTERPRETER_PRINT_LOC(
				ale::logger::println,
				"Condition is true but the first branch of if statement is "
				"empty."
			);
			return EvaluationError{
				.error = {evaluation_error_e::If_Statement_First_Branch_Empty},
				.message = {"Condition is true but the first branch of if "
							"statement is empty."}
			};
		}

		return interpret_node(ctx, second_child);
	}

	if (third_child == nullptr) {
		INTERPRETER_PRINT_LOC(
			ale::logger::println,
			"Condition is true but the second branch of if statement is empty."
		);
		return EvaluationError{
			.error = {evaluation_error_e::If_Statement_Second_Branch_Empty},
			.message = {"Condition is true but the second branch of if "
						"statement is empty."}
		};
	}

	return interpret_node(ctx, third_child);
}

} // namespace ast
} // namespace intlib
