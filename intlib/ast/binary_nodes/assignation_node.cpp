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
#include <any>
using namespace std::string_literals;

#include <ale/ast/binary_nodes/AssignationNode.hpp>

#include <intlib/detail/any_conversion.hpp>
#include <intlib/ast/EvaluationContext.hpp>
#include <intlib/ast/Evaluation.hpp>
#include <intlib/ast/interpretation.hpp>
#include <intlib/ast/utils/iterators.hpp>
#include <intlib/logger/macros.hpp>
#include <intlib/ast/utils/evaluation_result_to_string.hpp>
#if defined ALE_LOGGING_MESSAGES
#include <intlib/ast/utils/evaluation_error_to_string.hpp>
#endif

namespace intlib {
namespace ast {

[[nodiscard]] static Evaluation assign_variable(
	EvaluationContext& ctx,
	const std::string& var_name,
	const memory::WrappedAny& value_w
)
{
	INTERPRETER_ENTER_AST_FUNCTION;

	INTERPRETER_PRINT("Going to assign variable '{}'.", var_name);

	if (not ctx.memory.variable_exists(var_name)) {
		INTERPRETER_PRINT(
			"Attempted to assign a value to non-existent variable {}.", var_name
		);
		return make_bad_evaluation(
			Vec{evaluation_error_e::Memory_Variable_Does_Not_Exist},
			Vec{evaluation_function_e::Assignation},
			Vec{std::format(
				"Attempted to assign a value to non-existent variable '{}'",
				var_name
			)}
		);
	}

	auto& var_in_memory = ctx.memory.get_variable(var_name);

	if (var_in_memory.is_constant) {
		INTERPRETER_PRINT(
			"Attempted to assign a value to constant variable {}.", var_name
		);
		return make_bad_evaluation(
			Vec{evaluation_error_e::
					Memory_Attempt_To_Assign_Value_To_Constant_Variable},
			Vec{evaluation_function_e::Assignation},
			Vec{std::format(
				"Attempted to assign a value to constant variable '{}'",
				var_name
			)}
		);
	}

	memory::WrappedAny value_conv_w =
		detail::convert_to_ale_type(value_w, var_in_memory.ale_type);

	if (value_conv_w.type == detail::type_string_cpp<void>) {
		INTERPRETER_PRINT(
			"Could not convert value '{}' to a value of ALE type '{}'.",
			value_w,
			var_in_memory.ale_type
		);
		return make_bad_evaluation(
			Vec{evaluation_error_e::Conversion_Generic},
			Vec{evaluation_function_e::Assignation},
			Vec{std::format(
				"Could not convert value '{}' to a value of type '{}'",
				value_w,
				var_in_memory.ale_type
			)}
		);
	}

	INTERPRETER_PRINT(
		"Value '{}' after conversion to '{}' is: '{}'.",
		value_w,
		var_in_memory.ale_type,
		value_conv_w
	);

	var_in_memory.wrap = std::move(value_conv_w);

	INTERPRETER_PRINT("Successfully assigned variable '{}'.", var_name);

	return make_good_evaluation<EvaluationResult>();
}

[[nodiscard]] static Evaluation assign_multiple_values_rhs(
	EvaluationContext& ctx,
	const std::unique_ptr<ale::ast::Node>& left_child,
	const std::unique_ptr<ale::ast::Node>& right_child
)
{
	EvaluationContext lhs_ctx = new_evaluation_context(ctx.memory);
	auto var_iter = make_name_iterator(lhs_ctx, left_child);
	auto var_iter_pos = var_iter.begin();
	auto var_iter_end = var_iter.end();

	EvaluationContext rhs_ctx = new_evaluation_context(ctx.memory);
	auto value_iter = make_value_iterator(rhs_ctx, right_child);
	auto value_iter_pos = value_iter.begin();
	auto value_iter_end = value_iter.end();

	while (var_iter_pos != var_iter_end and value_iter_pos != value_iter_end) {

		INTERPRETER_PRINT("Going to assign a variable.");

		Evaluation var_eval = *var_iter_pos;
		if (not var_eval) {
			INTERPRETER_PRINT(
				"Something went wrong when retrieving the next variable."
			);
			INTERPRETER_PRINT("Error: '{}'", var_eval.error().errors.at(0));
			return append_error(
				std::move(var_eval.error()),
				evaluation_error_e::List_Iteration,
				evaluation_function_e::Assignation,
				"Something went wrong when retrieving the next variable"
			);
		}

		Evaluation value_eval = *value_iter_pos;
		if (not value_eval) {
			INTERPRETER_PRINT(
				"Something went wrong when computing the next value."
			);
			return append_error(
				std::move(value_eval.error()),
				evaluation_error_e::List_Iteration,
				evaluation_function_e::Assignation,
				"Something went wrong when retrieving the next value"
			);
		}

		const memory::WrappedAny& value_w = *value_eval;
		const memory::WrappedAny *actual_value_w = nullptr;
		if (value_w.type == detail::type_string_cpp<memory::RefVar>) {
			actual_value_w =
				&std::any_cast<memory::RefVar>(value_w.value).get().wrap;
		}
		else if (value_w.type == detail::type_string_cpp<memory::RefConstVar>) {
			actual_value_w =
				&std::any_cast<const memory::RefVar>(value_w.value).get().wrap;
		}
		else {
			actual_value_w = &value_w;
		}

		const memory::WrappedAny& name_w = *var_eval;
#if defined DEBUG
		assert(name_w.type == detail::type_string_cpp<std::string>);
#endif
		const auto& var_name = std::any_cast<const std::string&>(name_w.value);

		INTERPRETER_PRINT("Of name:  '{}'.", var_name);
		INTERPRETER_PRINT("Of value: '{}'.", *actual_value_w);

		Evaluation assignation_eval =
			assign_variable(ctx, var_name, *actual_value_w);

		if (not assignation_eval) {
			INTERPRETER_PRINT("An error occurred.");
			return append_error(
				std::move(var_eval.error()),
				evaluation_error_e::Assignation_Of_Variable,
				evaluation_function_e::Assignation,
				std::format(
					"Something went wrong when assigning  variable '{}'",
					var_name
				)
			);
		}

		++value_iter_pos;
		++var_iter_pos;
	}

	if (var_iter_pos != var_iter_end) {
		INTERPRETER_PRINT("Too many values in the right hand side");
		return make_bad_evaluation(
			Vec{evaluation_error_e::Overfull_Right_Hand_Side_Values},
			Vec{evaluation_function_e::Assignation},
			Vec{"Too many values in the right hand side"s}
		);
	}

	if (value_iter_pos != value_iter_end) {
		INTERPRETER_PRINT("Too many values in the left hand side");
		return make_bad_evaluation(
			Vec{evaluation_error_e::Overfull_Left_Hand_Side_Values},
			Vec{evaluation_function_e::Assignation},
			Vec{"Too many values in the left hand side"s}
		);
	}
	return make_good_evaluation<EvaluationResult>();
}

[[nodiscard]] static Evaluation assign_single_values_rhs(
	EvaluationContext& ctx,
	const std::unique_ptr<ale::ast::Node>& left_child,
	const std::unique_ptr<ale::ast::Node>& right_child
)
{
	EvaluationContext lhs_ctx = new_evaluation_context(ctx.memory);
	auto var_iter = make_name_iterator(lhs_ctx, left_child);
	auto var_iter_pos = var_iter.begin();
	auto var_iter_end = var_iter.end();

	EvaluationContext rhs_ctx = new_evaluation_context(ctx.memory);
	Evaluation rhs_eval = interpret_node(rhs_ctx, right_child);
	if (not rhs_eval) {
		return append_error(
			std::move(rhs_eval.error()),
			evaluation_error_e::Evaluation_Of_Node_Failed,
			evaluation_function_e::Assignation,
			"Something went wrong when evaluating a node"
		);
	}

	const EvaluationResult& rhs_w = *rhs_eval;
	const memory::WrappedAny *actual_rhs_w = nullptr;
	if (rhs_w.type == detail::type_string_cpp<memory::RefVar>) {
		actual_rhs_w = &std::any_cast<memory::RefVar>(rhs_w.value).get().wrap;
	}
	else if (rhs_w.type == detail::type_string_cpp<memory::RefConstVar>) {
		actual_rhs_w =
			&std::any_cast<const memory::RefVar>(rhs_w.value).get().wrap;
	}
	else {
		actual_rhs_w = &rhs_w;
	}

	while (var_iter_pos != var_iter_end) {
		INTERPRETER_PRINT("Going to assign a variable.");

		Evaluation var_eval = *var_iter_pos;
		if (not var_eval) {
			INTERPRETER_PRINT(
				"Something went wrong when retrieving the next variable."
			);
			INTERPRETER_PRINT("Error: '{}'", var_eval.error().errors.at(0));
			return append_error(
				std::move(var_eval.error()),
				evaluation_error_e::List_Iteration,
				evaluation_function_e::Assignation,
				"Something went wrong when retrieving the next variable"
			);
		}

		const memory::WrappedAny& name_w = *var_eval;
#if defined DEBUG
		assert(name_w.type == detail::type_string_cpp<std::string>);
#endif
		const auto& var_name = std::any_cast<const std::string&>(name_w.value);

		INTERPRETER_PRINT("Of name:  '{}'.", var_name);
		INTERPRETER_PRINT("Of value: '{}'.", *actual_rhs_w);

		Evaluation assignation_eval =
			assign_variable(ctx, var_name, *actual_rhs_w);

		if (not assignation_eval) {
			INTERPRETER_PRINT("An error occurred.");
			return append_error(
				std::move(assignation_eval.error()),
				evaluation_error_e::Assignation_Of_Variable,
				evaluation_function_e::Assignation,
				"Something went wrong when retrieving the next variable"
			);
		};
		++var_iter_pos;
	}
	return make_good_evaluation<EvaluationResult>();
}

Evaluation
evaluate(EvaluationContext& ctx, const ale::ast::AssignationNode& assign)
{
	INTERPRETER_ENTER_AST_FUNCTION;

	const auto& left_child = assign.get_left_child();
#if defined DEBUG
	assert(left_child != nullptr);
#endif

	const auto& right_child = assign.get_right_child();
#if defined DEBUG
	assert(right_child != nullptr);
#endif
	const auto right_t = right_child->get_node_type();

	if (right_t == ale::ast::node_type_e::Sequence or
		right_t == ale::ast::node_type_e::Comma_Separated_Group) {

		return assign_multiple_values_rhs(ctx, left_child, right_child);
	}

	return assign_single_values_rhs(ctx, left_child, right_child);
}

} // namespace ast
} // namespace intlib
