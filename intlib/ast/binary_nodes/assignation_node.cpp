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
#include <ale/ast/n_ary_nodes/CommaSeparatedGroupNode.hpp>
#include <ale/utils/IndexIterator.hpp>

#include <intlib/logger/macros.hpp>
#include <intlib/detail/any_type.hpp>
#include <intlib/detail/any_conversion.hpp>
#if defined ALE_LOGGING_MESSAGES
#include <intlib/detail/any_output.hpp>
#endif
#include <intlib/ast/EvaluationResult.hpp>
#include <intlib/ast/utils/variable_names.hpp>
#include <intlib/ast/EvaluationContext.hpp>
#include <intlib/ast/interpretation.hpp>

namespace intlib {
namespace ast {

#define aleprln ale::logger::println

[[nodiscard]] static EvaluationResult compute_value_from_assignation(
	EvaluationContext& ctx, const ale::ast::AssignationNode& assignation
)
{
	INTERPRETER_ENTER_AST_FUNCTION(aleprln);

	const auto& value_node = assignation.get_right_child();
	std::any value_w;
	if (assignation.get_node_type() !=
		ale::ast::node_type_e::Declaration_Declare) {

		EvaluationResult res_w = interpret_node(ctx, value_node);
		if (not res_w.has_value()) {
			INTERPRETER_PRINT_LOC(aleprln, "Evaluation of node failed.");
			return make_bad_evaluation_result(
				std::vector{evaluation_error_e::Evaluation_Of_Node_Failed},
				std::vector<std::string>{"Evaluation of node failed."}
			);
		}
		value_w = std::move(*res_w);

		INTERPRETER_PRINT_LOC(
			aleprln,
			"Type returned from node evaluation is: '{}'. Value is: '{}'.",
			detail::get_type_name(value_w),
			any_view{value_w}
		);
	}
	return make_good_evaluation_result<std::any>(std::move(value_w));
}

[[nodiscard]] static EvaluationResult assign_single_variable(
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

[[nodiscard]] static EvaluationResult assign_variable(
	EvaluationContext& ctx,
	const std::unique_ptr<ale::ast::Node>& variable_node,
	const std::any& value_w
)
{
	INTERPRETER_ENTER_AST_FUNCTION(aleprln);

#if defined DEBUG
	assert(variable_node->get_node_type() == ale::ast::node_type_e::Variable);
#endif

	std::string var_name =
		static_cast<const ale::ast::VariableNode *>(variable_node.get())
			->get_variable_name();

	return assign_single_variable(ctx, var_name, value_w);
}

[[nodiscard]] static EvaluationResult assign_subscripted_variable(
	EvaluationContext& ctx,
	const std::unique_ptr<ale::ast::Node>& variable,
	const std::any& value_w
)
{
	INTERPRETER_ENTER_AST_FUNCTION(aleprln);

	INTERPRETER_PRINT_LOC(aleprln, "Making the name of the variable.");

	EvaluationResult res_w = make_subscripted_variable_name(ctx, variable);
	if (not res_w.has_value()) {
		INTERPRETER_PRINT_LOC(
			aleprln, "    Could not make the name of the variable."
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

	const auto& s = std::any_cast<const std::string&>(name_w);
	return assign_single_variable(ctx, s, value_w);
}

[[nodiscard]] static EvaluationResult assign_variable_sequence(
	EvaluationContext& ctx,
	const std::unique_ptr<ale::ast::Node>& sequence,
	const std::any& value_w
)
{
	INTERPRETER_ENTER_AST_FUNCTION(aleprln);

	INTERPRETER_PRINT_LOC(aleprln, "Going to make list of names.");

	auto res_w = make_shallow_sequence_indices(ctx, sequence);
	if (not res_w) {
		return make_bad_evaluation_result(
			std::vector{evaluation_error_e::Evaluation_Of_Node_Failed},
			std::vector<std::string>{
				"Evaluation of node failed and the name of the variables in a "
				"sequence could not be made."
			}
		);
	}

	INTERPRETER_PRINT_LOC(aleprln, "Successfully made list of names.");

	std::any idxs_w = std::move(*res_w);

#if defined DEBUG
	assert(detail::is_type<ShallowSequenceIndices>(idxs_w));
#endif

	auto idxs = std::any_cast<ShallowSequenceIndices&&>(std::move(idxs_w));
	const std::string& base_name = idxs.base_name;

	INTERPRETER_PRINT_LOC(aleprln, "    Constructed SequenceNodeIterator.");

	ale::utils::IndexIterator iter(std::move(idxs.left), std::move(idxs.right));
	while (not iter.end()) {
		const auto& indices = iter.get_current_indices();

		std::string var_name = base_name;
		append_variable_name(var_name, indices);

		INTERPRETER_PRINT_LOC(aleprln, "Variable name: {}.", var_name);

		auto assign_res_w = assign_single_variable(ctx, var_name, value_w);

		INTERPRETER_PRINT_LOC(
			aleprln,
			"    Successfully assigned variable with name '{}'.",
			var_name
		);

		if (not assign_res_w) {
			return make_bad_evaluation_result(std::move(assign_res_w.error()));
		}

		iter.next_indices();
	}

	return make_good_evaluation_result<std::any>();
}

[[nodiscard]] static EvaluationResult assign_variable_sequence(
	EvaluationContext& ctx,
	const ale::ast::AssignationNode& assignation,
	const std::unique_ptr<ale::ast::Node>& sequence
)
{
	INTERPRETER_ENTER_AST_FUNCTION(aleprln);

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
	return assign_variable_sequence(ctx, sequence, *value_w);
}

[[nodiscard]] static EvaluationResult assign_comma_separated_variables(
	EvaluationContext& ctx, const ale::ast::AssignationNode& assignation
)
{
	INTERPRETER_ENTER_AST_FUNCTION(aleprln);

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

	return make_good_evaluation_result<std::any>();
}

EvaluationResult
evaluate(EvaluationContext& ctx, const ale::ast::AssignationNode& assignation)
{
	INTERPRETER_ENTER_AST_FUNCTION(aleprln);

	const auto& left_child = assignation.get_left_child();

#if defined DEBUG
	assert(left_child != nullptr);
#endif

	if (left_child->get_node_type() == ale::ast::node_type_e::Variable) {
		INTERPRETER_PRINT_LOC(aleprln, "Variable.");
		const auto& variable_node = assignation.get_left_child();

		EvaluationResult res_compute_w =
			compute_value_from_assignation(ctx, assignation);
		if (not res_compute_w) {
			return std::move(res_compute_w.error());
		}
		return assign_variable(ctx, variable_node, *res_compute_w);
	}

	if (left_child->get_node_type() ==
		ale::ast::node_type_e::Comma_Separated_Group) {
		INTERPRETER_PRINT_LOC(aleprln, "Comma-separated group.");
		return assign_comma_separated_variables(ctx, assignation);
	}

	if (left_child->get_node_type() == ale::ast::node_type_e::Sequence) {
		INTERPRETER_PRINT_LOC(aleprln, "Variable sequence.");
		return assign_variable_sequence(ctx, assignation, left_child);
	}

	if (left_child->get_node_type() ==
		ale::ast::node_type_e::Subscripted_Variable) {

		INTERPRETER_PRINT_LOC(aleprln, "Subscripted variable.");
		EvaluationResult res_compute_w =
			compute_value_from_assignation(ctx, assignation);

		if (not res_compute_w) {
			return std::move(res_compute_w.error());
		}
		return assign_subscripted_variable(ctx, left_child, *res_compute_w);
	}

	return make_good_evaluation_result<std::any>();
}

} // namespace ast
} // namespace intlib
