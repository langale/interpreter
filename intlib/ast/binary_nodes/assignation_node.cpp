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

#include <ale/ast/binary_nodes/SequenceNode.hpp>
#include <ale/ast/binary_nodes/AssignationNode.hpp>
#include <ale/ast/zero_ary_nodes/VariableNode.hpp>
#include <ale/utils/binary_nodes/sequence_node/SequenceNodeIterator.hpp>
#include <ale/ast/n_ary_nodes/CommaSeparatedGroupNode.hpp>

#include <intlib/logger/macros.hpp>
#include <intlib/detail/any_type.hpp>
#include <intlib/detail/any_conversion.hpp>
#if defined ALE_LOGGING_MESSAGES
#include <intlib/detail/any_output.hpp>
#endif
#include <intlib/ast/EvaluationResult.hpp>
#include <intlib/ast/utils/EvaluationResult.hpp>
#include <intlib/ast/utils/variable_names.hpp>
#include <intlib/ast/EvaluationContext.hpp>
#include <intlib/ast/interpretation.hpp>

namespace intlib {
namespace ast {

[[nodiscard]] static EvaluationResult compute_value_from_assignation(
	EvaluationContext& ctx, const ale::ast::AssignationNode& assig
)
{
	INTERPRETER_ENTER_AST_FUNCTION(ale::logger::println);

	const auto& value_node = assig.get_right_child();
	std::any value;
	if (assig.get_node_type() != ale::ast::node_type_e::Declaration_Declare) {
		EvaluationResult res_w = interpret_node(ctx, value_node);
		if (not res_w.has_value()) {
			INTERPRETER_PRINT_LOC(
				ale::logger::println, "Evaluation of node failed."
			);
			return make_bad_evaluation_result(
				std::vector{evaluation_error_e::Evaluation_Of_Node_Failed},
				std::vector<std::string>{"Evaluation of node failed."}
			);
		}
		value = std::move(*res_w);

		INTERPRETER_PRINT_LOC2(
			ale::logger::println,
			"Type returned from node evaluation is: '{}'. Value is: '{}'.",
			detail::get_type_name(value),
			any_view{value}
		);
	}
	return make_good_evaluation_result(std::move(value));
}

[[nodiscard]] static EvaluationResult assign_single_variable(
	EvaluationContext& ctx, const std::string& var_name, const std::any& value
)
{
	INTERPRETER_ENTER_AST_FUNCTION(ale::logger::println);

	INTERPRETER_PRINT_LOC2(
		ale::logger::println, "Going to assign variable '{}'.", var_name
	);

	std::optional<memory::VariableValue> var_in_memory =
		ctx.memory.get_variable(var_name);

	if (not var_in_memory) {
		INTERPRETER_PRINT_LOC2(
			ale::logger::println,
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
	if (var_in_memory->is_constant) {
		INTERPRETER_PRINT_LOC2(
			ale::logger::println,
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

	std::any value_conv =
		detail::any_convert_to_type(value, var_in_memory->type);

	if (detail::is_type<void>(value_conv)) {
		INTERPRETER_PRINT_LOC2(
			ale::logger::println,
			"Could not convert value '{}' to a value of type '{}'.",
			any_view{value},
			var_in_memory->type
		);
	}

	INTERPRETER_PRINT_LOC2(
		ale::logger::println,
		"Value after conversion to '{}' is: '{}'.",
		var_in_memory->type,
		any_view{value_conv}
	);

	[[maybe_unused]] const auto assignation_res =
		ctx.memory.set_variable_value(var_name, std::move(value_conv));

#if defined DEBUG
	assert(assignation_res.has_value());
#endif

	INTERPRETER_PRINT_LOC2(
		ale::logger::println,
		"Successfully assigned variable '{}'.",
		var_name
	);

	return make_good_evaluation_result(std::any{});
}

[[nodiscard]] static EvaluationResult assign_variable(
	EvaluationContext& ctx,
	const std::unique_ptr<ale::ast::Node>& variable_node,
	const std::any& value
)
{
	INTERPRETER_ENTER_AST_FUNCTION(ale::logger::println);

#if defined DEBUG
	assert(variable_node->get_node_type() == ale::ast::node_type_e::Variable);
#endif

	std::string var_name =
		static_cast<const ale::ast::VariableNode *>(variable_node.get())
			->get_variable_name();

	return assign_single_variable(ctx, var_name, value);
}

[[nodiscard]] static EvaluationResult assign_subscripted_variable(
	EvaluationContext& ctx,
	const std::unique_ptr<ale::ast::Node>& variable,
	const std::any& value
)
{
	INTERPRETER_ENTER_AST_FUNCTION(ale::logger::println);

	INTERPRETER_PRINT_LOC(
		ale::logger::println, "Making the name of the variable."
	);

	EvaluationResult res_w = make_subscripted_variable_name(ctx, variable);
	if (not res_w.has_value()) {
		INTERPRETER_PRINT_LOC(
			ale::logger::println, "    Could not make the name of the variable."
		);
		return make_bad_evaluation_result(
			std::vector{evaluation_error_e::Evaluation_Of_Node_Failed},
			std::vector<std::string>{"Evaluation of node failed and the name "
									 "of the variable could not be made."}
		);
	}

	const std::any& name_w = *res_w;
#if defined DEBUG
	assert(detail::is_type<std::string>(name_w));
#endif

	return assign_single_variable(
		ctx, std::any_cast<std::string>(name_w), value
	);
}

[[nodiscard]] static EvaluationResult assign_variable_sequence(
	EvaluationContext& ctx,
	const std::unique_ptr<ale::ast::Node>& sequence,
	const std::any& value
)
{
	INTERPRETER_ENTER_AST_FUNCTION(ale::logger::println);

	INTERPRETER_PRINT_LOC(ale::logger::println, "Going to make list of names.");

	auto res_w = make_sequence_variable_names(ctx, sequence);
	if (not res_w) {
		return make_bad_evaluation_result(
			std::vector{evaluation_error_e::Evaluation_Of_Node_Failed},
			std::vector<std::string>{
				"Evaluation of node failed and the name of the variables in a "
				"sequence could not be made."
			}
		);
	}

	INTERPRETER_PRINT_LOC(
		ale::logger::println, "Successfully made list of names."
	);

	const std::any& list_w = *res_w;

#if defined DEBUG
	assert(detail::is_type<std::vector<std::string>>(list_w));
#endif

	auto list = std::any_cast<std::vector<std::string>>(list_w);

	for (std::string& var_name : list) {
		INTERPRETER_PRINT_LOC2(
			ale::logger::println, "Declare variable with name '{}'.", var_name
		);

		auto assign_res_w = assign_single_variable(ctx, var_name, value);

		INTERPRETER_PRINT_LOC2(
			ale::logger::println,
			"    Successfully assignd variable with name '{}'.",
			var_name
		);

		if (not assign_res_w) {
			return make_bad_evaluation_result(std::move(assign_res_w.error()));
		}
	}

	return make_good_evaluation_result(std::any{});
}

[[nodiscard]] static EvaluationResult assign_variable_sequence(
	EvaluationContext& ctx,
	const ale::ast::AssignationNode& assignation,
	const std::unique_ptr<ale::ast::Node>& sequence
)
{
	INTERPRETER_ENTER_AST_FUNCTION(ale::logger::println);

	const auto& variable_list_node_w = assignation.get_left_child();

#if defined DEBUG
	assert(
		variable_list_node_w->get_node_type() == ale::ast::node_type_e::Sequence
	);
#endif

	EvaluationResult value_w = compute_value_from_assignation(ctx, assignation);
	if (not value_w) {
		return std::move(value_w.error());
	}
	const std::any value = std::move(*value_w);
	return assign_variable_sequence(ctx, sequence, value);
}

[[nodiscard]] static EvaluationResult assign_comma_separated_variables(
	EvaluationContext& ctx, const ale::ast::AssignationNode& assignation
)
{
	INTERPRETER_ENTER_AST_FUNCTION(ale::logger::println);

	const auto& variable_list_node_w = assignation.get_left_child();

#if defined DEBUG
	assert(
		variable_list_node_w->get_node_type() ==
		ale::ast::node_type_e::Comma_Separated_Group
	);
#endif

	EvaluationResult value_w = compute_value_from_assignation(ctx, assignation);
	if (not value_w) {
		return std::move(value_w.error());
	}
	const std::any value = std::move(*value_w);

	const auto variable_list_node =
		static_cast<const ale::ast::CommaSeparatedGroupNode *>(
			variable_list_node_w.get()
		);

	const auto& children = variable_list_node->get_children();

	for (const auto& child : children) {
		if (child->get_node_type() == ale::ast::node_type_e::Variable) {
			auto res_w = assign_variable(ctx, child, value);
			if (not res_w) {
				return make_bad_evaluation_result(std::move(res_w.error()));
			}
		}
		else if (child->get_node_type() == ale::ast::node_type_e::Sequence) {
			auto res_w = assign_variable_sequence(ctx, child, value);
			if (not res_w) {
				return make_bad_evaluation_result(std::move(res_w.error()));
			}
		}
		else if (child->get_node_type() ==
				 ale::ast::node_type_e::Subscripted_Variable) {

			auto res_w = assign_subscripted_variable(ctx, child, value);
			if (not res_w) {
				return make_bad_evaluation_result(std::move(res_w.error()));
			}
		}
	}

	return make_good_evaluation_result(std::any{});
}

EvaluationResult
evaluate(EvaluationContext& ctx, const ale::ast::AssignationNode& assignation)
{
	INTERPRETER_ENTER_AST_FUNCTION(ale::logger::println);

	const auto& left_child = assignation.get_left_child();

#if defined DEBUG
	assert(left_child != nullptr);
#endif

	if (left_child->get_node_type() == ale::ast::node_type_e::Variable) {
		INTERPRETER_PRINT_LOC(ale::logger::println, "Variable.");
		const auto& variable_node = assignation.get_left_child();

		EvaluationResult value_w =
			compute_value_from_assignation(ctx, assignation);
		if (not value_w) {
			return std::move(value_w.error());
		}
		std::any value = std::move(*value_w);

		return assign_variable(ctx, variable_node, std::move(value));
	}

	if (left_child->get_node_type() ==
		ale::ast::node_type_e::Comma_Separated_Group) {
		INTERPRETER_PRINT_LOC(ale::logger::println, "Comma-separated group.");
		return assign_comma_separated_variables(ctx, assignation);
	}

	if (left_child->get_node_type() == ale::ast::node_type_e::Sequence) {
		INTERPRETER_PRINT_LOC(ale::logger::println, "Variable sequence.");
		return assign_variable_sequence(ctx, assignation, left_child);
	}

	if (left_child->get_node_type() ==
		ale::ast::node_type_e::Subscripted_Variable) {

		INTERPRETER_PRINT_LOC(ale::logger::println, "Subscripted variable.");
		EvaluationResult value_w =
			compute_value_from_assignation(ctx, assignation);
		if (not value_w) {
			return std::move(value_w.error());
		}
		std::any value = std::move(*value_w);

		return assign_subscripted_variable(ctx, left_child, value);
	}

	return make_good_evaluation_result(std::any{});
}

} // namespace ast
} // namespace intlib
