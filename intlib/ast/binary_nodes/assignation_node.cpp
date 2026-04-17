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

#if defined DEBUG
#include <cassert>
#endif
#include <generator>
#include <optional>
#include <string>
#include <ranges>
#include <any>
using namespace std::string_literals;

#include <ale/ast/binary_nodes/AssignationNode.hpp>
#include <ale/utils/IndexIterator.hpp>

#include <intlib/logger/macros.hpp>
#include <intlib/detail/any_type.hpp>
#include <intlib/detail/any_output.hpp>
#include <intlib/detail/any_conversion.hpp>
#include <intlib/ast/EvaluationContext.hpp>
#include <intlib/ast/EvaluationResult.hpp>
#include <intlib/ast/interpretation.hpp>
#include <intlib/ast/utils/iterators.hpp>

namespace intlib {
namespace ast {

#define aleprln ale::logger::println

[[nodiscard]] static EvaluationResult assign_variable(
	EvaluationContext& ctx, const std::string& var_name, const std::any& value_w
)
{
	INTERPRETER_ENTER_AST_FUNCTION(aleprln);

	INTERPRETER_PRINT_LOC(aleprln, "Going to assign variable '{}'.", var_name);

	if (not ctx.memory.variable_exists(var_name)) {
		INTERPRETER_PRINT_LOC(
			aleprln,
			"Attempted to assign a value to non-existent variable {}.",
			var_name
		);
		return make_bad_evaluation_result(
			std::vector{evaluation_error_e::Memory_Variable_Does_Not_Exist},
			std::vector{std::format(
				"Attempted to assign a value to non-existent variable {}.",
				var_name
			)}
		);
	}

	auto& var_in_memory = ctx.memory.get_variable(var_name);

	if (var_in_memory.is_constant) {
		INTERPRETER_PRINT_LOC(
			aleprln,
			"Attempted to assign a value to constant variable {}.",
			var_name
		);
		return make_bad_evaluation_result(
			std::vector{
				evaluation_error_e::
					Memory_Attempt_To_Assign_Value_To_Constant_Variable
			},
			std::vector{std::format(
				"Attempted to assign a value to constant variable {}.", var_name
			)}
		);
	}

	std::any value_conv_w =
		detail::any_convert_to_type(value_w, var_in_memory.type);

	if (detail::is_type<void>(value_conv_w)) {
		INTERPRETER_PRINT_LOC(
			aleprln,
			"Could not convert value '{}' to a value of type '{}'.",
			any_view{value_w},
			var_in_memory.type
		);
	}

	INTERPRETER_PRINT_LOC(
		aleprln,
		"Value after conversion to '{}' is: '{}'.",
		var_in_memory.type,
		any_view{value_conv_w}
	);

	var_in_memory.value_w = std::move(value_conv_w);

	INTERPRETER_PRINT_LOC(
		aleprln, "Successfully assigned variable '{}'.", var_name
	);

	return make_good_evaluation_result<std::any>();
}

EvaluationResult
evaluate(EvaluationContext& ctx, const ale::ast::AssignationNode& assign)
{
	INTERPRETER_ENTER_AST_FUNCTION(aleprln);

	const auto& left_child = assign.get_left_child();
#if defined DEBUG
	assert(left_child != nullptr);
#endif

	auto var_iter = make_name_iterator(ctx, left_child);
	auto var_iter_pos = var_iter.begin();
	auto var_iter_end = var_iter.end();

	const auto& right_child = assign.get_right_child();
#if defined DEBUG
	assert(right_child != nullptr);
#endif
	const auto right_t = right_child->get_node_type();

	if (right_t == ale::ast::node_type_e::Sequence or
		right_t == ale::ast::node_type_e::Comma_Separated_Group) {

		auto value_iter = make_value_iterator(ctx, right_child);
		auto value_iter_pos = value_iter.begin();
		auto value_iter_end = value_iter.end();

		while (var_iter_pos != var_iter_end and
			   value_iter_pos != value_iter_end) {

			INTERPRETER_PRINT_LOC(aleprln, "Going to declare a variable.");

			EvaluationResult var_res = *var_iter_pos;
			if (not var_res) {
				INTERPRETER_PRINT_LOC(
					aleprln,
					"Something went wrong when retrieving the next variable."
				);
				INTERPRETER_PRINT_LOC(
					aleprln, "Error: '{}'", var_res.error().error[0]
				);
				return var_res;
			}

			EvaluationResult value_res = *value_iter_pos;
			if (not value_res) {
				INTERPRETER_PRINT_LOC(
					aleprln,
					"Something went wrong when computing the next value."
				);
				return value_res;
			}

			std::any var_name_w = std::move(*var_res);
			std::any value_w = std::move(*value_res);

			const std::string& var_name =
				*std::any_cast<std::string>(&var_name_w);

			INTERPRETER_PRINT_LOC(aleprln, "Of name:  '{}'.", var_name);
			INTERPRETER_PRINT_LOC(
				aleprln, "Of value: '{}'.", any_view{value_w}
			);

			EvaluationResult assign_res =
				assign_variable(ctx, var_name, value_w);
			if (not assign_res) {
				return assign_res;
			}

			++value_iter_pos;
			++var_iter_pos;
		}

		if (var_iter_pos != var_iter_end) {
			INTERPRETER_PRINT_LOC(
				aleprln, "Too many values in the right hand side"
			);
			return make_bad_evaluation_result();
		}

		if (value_iter_pos != value_iter_end) {
			INTERPRETER_PRINT_LOC(
				aleprln, "Too many values in the left hand side"
			);
			return make_bad_evaluation_result();
		}
	}
	else {

		EvaluationResult compute_res = interpret_node(ctx, right_child);
		if (not compute_res) {
			return compute_res;
		}

		std::any value_w = std::move(*compute_res);

		while (var_iter_pos != var_iter_end) {
			INTERPRETER_PRINT_LOC(aleprln, "Going to declare a variable.");

			EvaluationResult var_res = *var_iter_pos;
			if (not var_res) {
				INTERPRETER_PRINT_LOC(
					aleprln,
					"Something went wrong when retrieving the next variable."
				);
				INTERPRETER_PRINT_LOC(
					aleprln, "Error: '{}'", var_res.error().error[0]
				);
				return var_res;
			}

			std::any var_name_w = std::move(*var_res);
			const std::string& var_name =
				*std::any_cast<std::string>(&var_name_w);

			INTERPRETER_PRINT_LOC(aleprln, "Of name:  '{}'.", var_name);
			INTERPRETER_PRINT_LOC(
				aleprln, "Of value: '{}'.", any_view{value_w}
			);

			EvaluationResult assign_res =
				assign_variable(ctx, var_name, value_w);
			if (not assign_res) {
				return assign_res;
			};
			++var_iter_pos;
		}
	}

	return make_good_evaluation_result<std::any>();
}

} // namespace ast
} // namespace intlib
