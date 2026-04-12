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

#include <optional>
#include <ranges>
#include <any>

#include <ale/ast/binary_nodes/SequenceNode.hpp>
#include <ale/ast/binary_nodes/DeclarationNode.hpp>
#include <ale/ast/zero_ary_nodes/VariableNode.hpp>
#include <ale/utils/IndexIterator.hpp>
#include <ale/ast/n_ary_nodes/CommaSeparatedGroupNode.hpp>

#include <intlib/logger/macros.hpp>
#include <intlib/detail/any_type.hpp>
#include <intlib/detail/any_output.hpp>
#include <intlib/detail/any_conversion.hpp>
#include <intlib/ast/EvaluationContext.hpp>
#include <intlib/ast/EvaluationResult.hpp>
#include <intlib/ast/interpretation.hpp>
#include <intlib/ast/utils/variable_names.hpp>

namespace intlib {
namespace ast {

[[nodiscard]] static EvaluationResult compute_value_from_declaration(
	EvaluationContext& ctx, const ale::ast::DeclarationNode& decl
)
{
	INTERPRETER_ENTER_AST_FUNCTION(ale::logger::println);

	const auto& value_node = decl.get_right_child();
	std::any value_w;
	if (decl.get_node_type() != ale::ast::node_type_e::Declaration_Declare) {
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
		value_w = std::move(*res_w);

		INTERPRETER_PRINT_LOC2(
			ale::logger::println,
			"Type returned from node evaluation is: '{}'. Value is: '{}'.",
			detail::get_type_name(value_w),
			any_view{value_w}
		);
	}
	return make_good_evaluation_result<std::any>(std::move(value_w));
}

[[nodiscard]] static EvaluationResult declare_single_variable(
	EvaluationContext& ctx,
	const ale::ast::node_type_e t,
	std::string&& var_name,
	std::string&& var_type,
	const std::any& value_w
)
{
	INTERPRETER_ENTER_AST_FUNCTION(ale::logger::println);

#if defined ALE_LOGGING_MESSAGES
	const std::string variable_name_copy = var_name;
#endif

	INTERPRETER_PRINT_LOC2(
		ale::logger::println,
		"Going to declare variable '{}'.",
		variable_name_copy
	);

	if (ctx.memory.variable_exists_shallow(var_name)) {
		INTERPRETER_PRINT_LOC2(
			ale::logger::println, "Attempt to redeclare variable {}.", var_name
		);
		return make_bad_evaluation_result(
			std::vector{evaluation_error_e::Memory_Variable_Already_Exists},
			std::vector{std::format("Redeclaration of variable {}.", var_name)}
		);
	}

	// This is a 'declare' node.
	if (t == ale::ast::node_type_e::Declaration_Declare) {
		ctx.memory.declare_variable(
			std::move(var_name), {}, std::move(var_type)
		);
		return make_good_evaluation_result<std::any>();
	}

	std::any value_conv_w = detail::any_convert_to_type(value_w, var_type);

	INTERPRETER_PRINT_LOC2(
		ale::logger::println,
		"Value after conversion to '{}' is: '{}'.",
		var_type,
		any_view{value_conv_w}
	);

	if (t == ale::ast::node_type_e::Declaration_Const) {
		ctx.memory.declare_constant_variable(
			std::move(var_name), std::move(value_conv_w), std::move(var_type)
		);
	}
	else {
		ctx.memory.declare_variable(
			std::move(var_name), std::move(value_conv_w), std::move(var_type)
		);
	}

	INTERPRETER_PRINT_LOC2(
		ale::logger::println,
		"Successfully declared variable '{}'.",
		variable_name_copy
	);

	return make_good_evaluation_result<std::any>();
}

[[nodiscard]] static EvaluationResult declare_variable(
	EvaluationContext& ctx,
	const ale::ast::DeclarationNode& decl,
	const std::unique_ptr<ale::ast::Node>& variable_node,
	const std::any& value_w
)
{
	INTERPRETER_ENTER_AST_FUNCTION(ale::logger::println);

#if defined DEBUG
	assert(variable_node->get_node_type() == ale::ast::node_type_e::Variable);
#endif

	std::string var_name =
		static_cast<const ale::ast::VariableNode *>(variable_node.get())
			->get_variable_name();

	std::string var_type = decl.get_variable_type();
	return declare_single_variable(
		ctx,
		decl.get_node_type(),
		std::move(var_name),
		std::move(var_type),
		value_w
	);
}

[[nodiscard]] static EvaluationResult declare_subscripted_variable(
	EvaluationContext& ctx,
	const ale::ast::DeclarationNode& decl,
	const std::unique_ptr<ale::ast::Node>& variable,
	const std::any& value_w
)
{
	INTERPRETER_ENTER_AST_FUNCTION(ale::logger::println);

	INTERPRETER_PRINT_LOC(
		ale::logger::println, "Making the name of the variable."
	);

	EvaluationResult res_w = make_subscripted_variable_name(ctx, variable);
	if (not res_w.has_value()) {
		INTERPRETER_PRINT_LOC(
			ale::logger::println, "Could not make the name of the variable."
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

	std::string var_type = decl.get_variable_type();
	return declare_single_variable(
		ctx,
		decl.get_node_type(),
		std::any_cast<std::string>(name_w),
		std::move(var_type),
		value_w
	);
}

[[nodiscard]] static EvaluationResult declare_variable_sequence(
	EvaluationContext& ctx,
	const ale::ast::DeclarationNode& decl,
	const std::unique_ptr<ale::ast::Node>& sequence,
	const std::any& value_w
)
{
	INTERPRETER_ENTER_AST_FUNCTION(ale::logger::println);

	INTERPRETER_PRINT_LOC(ale::logger::println, "Going to make list of names.");

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

	INTERPRETER_PRINT_LOC(
		ale::logger::println, "Successfully made list of names."
	);

	std::any idxs_w = std::move(*res_w);

#if defined DEBUG
	assert(detail::is_type<ShallowSequenceIndices>(idxs_w));
#endif

	auto idxs = std::any_cast<ShallowSequenceIndices&&>(std::move(idxs_w));
	const std::string& base_name = idxs.base_name;

	INTERPRETER_PRINT_LOC(
		ale::logger::println, "    Constructed SequenceNodeIterator."
	);

	ale::utils::IndexIterator iter(std::move(idxs.left), std::move(idxs.right));
	while (not iter.end()) {
		const auto& indices = iter.get_current_indices();

		std::string var_name = base_name;
		append_variable_name(var_name, indices);

		INTERPRETER_PRINT_LOC2(
			ale::logger::println, "Variable name: {}.", var_name
		);

		std::string var_type = decl.get_variable_type();
		auto declare_res_w = declare_single_variable(
			ctx,
			decl.get_node_type(),
			std::move(var_name),
			std::move(var_type),
			value_w
		);

		INTERPRETER_PRINT_LOC2(
			ale::logger::println,
			"    Successfully assigned variable with name '{}'.",
			var_name
		);

		if (not declare_res_w) {
			return make_bad_evaluation_result(std::move(declare_res_w.error()));
		}

		iter.next_indices();
	}

	return make_good_evaluation_result<std::any>();
}

[[nodiscard]] static EvaluationResult declare_variable_sequence(
	EvaluationContext& ctx,
	const ale::ast::DeclarationNode& decl,
	const std::unique_ptr<ale::ast::Node>& sequence
)
{
	INTERPRETER_ENTER_AST_FUNCTION(ale::logger::println);

	const auto& variable_list_node_w = decl.get_left_child();

#if defined DEBUG
	assert(
		variable_list_node_w->get_node_type() == ale::ast::node_type_e::Sequence
	);
#endif

	EvaluationResult res_compute_w = compute_value_from_declaration(ctx, decl);
	if (not res_compute_w) {
		return std::move(res_compute_w.error());
	}
	const std::any value_w = std::move(*res_compute_w);
	return declare_variable_sequence(ctx, decl, sequence, value_w);
}

[[nodiscard]] static EvaluationResult declare_comma_separated_variables(
	EvaluationContext& ctx, const ale::ast::DeclarationNode& decl
)
{
	INTERPRETER_ENTER_AST_FUNCTION(ale::logger::println);

	const auto& variable_list_node_w = decl.get_left_child();

#if defined DEBUG
	assert(
		variable_list_node_w->get_node_type() ==
		ale::ast::node_type_e::Comma_Separated_Group
	);
#endif

	EvaluationResult res_compute_w = compute_value_from_declaration(ctx, decl);
	if (not res_compute_w) {
		return std::move(res_compute_w.error());
	}
	const std::any value_w = std::move(*res_compute_w);

	const auto& variable_list_node =
		*static_cast<const ale::ast::CommaSeparatedGroupNode *>(
			variable_list_node_w.get()
		);

	const auto& children = variable_list_node.get_children();

	for (const auto& child : children) {
		if (child->get_node_type() == ale::ast::node_type_e::Variable) {
			auto res_w = declare_variable(ctx, decl, child, value_w);
			if (not res_w) {
				return make_bad_evaluation_result(std::move(res_w.error()));
			}
		}
		else if (child->get_node_type() == ale::ast::node_type_e::Sequence) {
			auto res_w = declare_variable_sequence(ctx, decl, child, value_w);
			if (not res_w) {
				return make_bad_evaluation_result(std::move(res_w.error()));
			}
		}
		else if (child->get_node_type() ==
				 ale::ast::node_type_e::Subscripted_Variable) {

			auto res_w =
				declare_subscripted_variable(ctx, decl, child, value_w);
			if (not res_w) {
				return make_bad_evaluation_result(std::move(res_w.error()));
			}
		}
	}

	return make_good_evaluation_result<std::any>();
}

EvaluationResult
evaluate(EvaluationContext& ctx, const ale::ast::DeclarationNode& decl)
{
	INTERPRETER_ENTER_AST_FUNCTION(ale::logger::println);

	const auto& left_child = decl.get_left_child();

#if defined DEBUG
	assert(left_child != nullptr);
#endif

	if (left_child->get_node_type() == ale::ast::node_type_e::Variable) {
		INTERPRETER_PRINT_LOC(ale::logger::println, "Variable.");
		const auto& variable_node = decl.get_left_child();

		EvaluationResult res_w = compute_value_from_declaration(ctx, decl);
		if (not res_w) {
			return make_bad_evaluation_result(std::move(res_w.error()));
		}
		const std::any value_w = std::move(*res_w);
		return declare_variable(ctx, decl, variable_node, value_w);
	}

	if (left_child->get_node_type() ==
		ale::ast::node_type_e::Comma_Separated_Group) {
		INTERPRETER_PRINT_LOC(ale::logger::println, "Comma-separated group.");
		return declare_comma_separated_variables(ctx, decl);
	}

	if (left_child->get_node_type() == ale::ast::node_type_e::Sequence) {
		INTERPRETER_PRINT_LOC(ale::logger::println, "Variable sequence.");
		return declare_variable_sequence(ctx, decl, left_child);
	}

	if (left_child->get_node_type() ==
		ale::ast::node_type_e::Subscripted_Variable) {

		INTERPRETER_PRINT_LOC(ale::logger::println, "Subscripted variable.");
		EvaluationResult res_w = compute_value_from_declaration(ctx, decl);
		if (not res_w) {
			return make_bad_evaluation_result(std::move(res_w.error()));
		}
		const std::any value_w = std::move(*res_w);
		return declare_subscripted_variable(ctx, decl, left_child, value_w);
	}

	return make_good_evaluation_result<std::any>();
}

} // namespace ast
} // namespace intlib
