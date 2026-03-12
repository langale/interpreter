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
#include <memory>
#include <string>
#include <ranges>

#include <ale/ast/Node.hpp>
#include <ale/ast/binary_nodes/SequenceNode.hpp>
#include <ale/ast/n_ary_nodes/SubscriptedVariableNode.hpp>
#include <ale/utils/binary_nodes/sequence_node/SequenceNodeIterator.hpp>

#include <intlib/ast/interpretation.hpp>
#include <intlib/ast/utils/EvaluationResult.hpp>
#include <intlib/detail/any_type.hpp>
#if defined ALE_LOGGING_MESSAGES
#include <intlib/detail/any_output.hpp>
#endif
#include <intlib/detail/any_to_numeric.hpp>
#include <intlib/logger/macros.hpp>

namespace intlib {
namespace ast {

/// TODO: try using C++23 std::generator

[[nodiscard]] EvaluationResult get_indices(
	EvaluationContext& ctx,
	const ale::ast::SubscriptedVariableNode * const subscripted_variable
)
{
	INTERPRETER_ENTER_AST_FUNCTION(ale::logger::println);

	std::vector<int64_t> indices;
	const auto& children = subscripted_variable->get_children();

	INTERPRETER_PRINT_LOC2(
		ale::logger::println, "Variable has {} subindices.", children.size()
	);

	for (const auto& [i, child] : children | std::views::enumerate) {

		INTERPRETER_PRINT_LOC2(
			ale::logger::println, "Made index for child {}.", i
		);

		auto val_w = interpret_node(ctx, child);
		if (not val_w.has_value()) {
			return std::move(val_w.error());
		}

		INTERPRETER_PRINT_LOC(
			ale::logger::println, "Successfully evaluated child."
		);

		const std::any& val = *val_w;
		const std::any idx_w = detail::any_to_numeric<int64_t>(val);

		if (not idx_w.has_value()) {
			INTERPRETER_PRINT_LOC2(
				ale::logger::println,
				"Could not convert node evaluation '{}' into a numeric "
				"int64_t.",
				any_view{val}
			);

			return make_bad_evaluation_result(
				std::vector{evaluation_error_e::
								Evaluation_Of_Node_Is_Not_A_Numeric_Value},
				std::vector<std::string>{
					"Evaluation of node is not a numeric value."
				}
			);
		}

		const auto idx = std::any_cast<int64_t>(idx_w);
		INTERPRETER_PRINT_LOC2(ale::logger::println, "Made index {}.", idx);
		indices.push_back(idx);
	}

	return make_good_evaluation_result(std::move(indices));
}

EvaluationResult make_subscripted_variable_name(
	EvaluationContext& ctx,
	const std::unique_ptr<ale::ast::Node>& subscripted_variable_w
)
{
	INTERPRETER_ENTER_AST_FUNCTION(ale::logger::println);

#if defined DEBUG
	assert(
		subscripted_variable_w->get_node_type() ==
		ale::ast::node_type_e::Subscripted_Variable
	);
#endif

	const auto& subscripted_variable =
		static_cast<const ale::ast::SubscriptedVariableNode *>(
			subscripted_variable_w.get()
		);

	std::string name = subscripted_variable->get_variable_name();

	INTERPRETER_PRINT_LOC2(
		ale::logger::println, "Make indices for variable {}.", name
	);

	auto res_w = get_indices(ctx, subscripted_variable);
	if (not res_w.has_value()) {
		return std::move(res_w.error());
	}

	INTERPRETER_PRINT_LOC(ale::logger::println, "Successfully made indices.");

	std::any idxs_w = std::move(*res_w);
	auto idxs = std::any_cast<std::vector<int64_t>&&>(std::move(idxs_w));

	for (const int64_t idx : idxs) {
		name += "_" + std::to_string(idx);
	}

	INTERPRETER_PRINT_LOC2(
		ale::logger::println, "Name constructed '{}'.", name
	);

	return make_good_evaluation_result(std::move(name));
}

EvaluationResult make_sequence_variable_names(
	EvaluationContext& ctx, const std::unique_ptr<ale::ast::Node>& sequence_w
)
{
	INTERPRETER_ENTER_AST_FUNCTION(ale::logger::println);

#if defined DEBUG
	assert(sequence_w->get_node_type() == ale::ast::node_type_e::Sequence);
#endif

	auto sequence =
		static_cast<const ale::ast::SequenceNode *>(sequence_w.get());

	const auto& left_child = sequence->get_left_child();
	const auto& right_child = sequence->get_right_child();

#if defined DEBUG
	assert(
		left_child->get_node_type() ==
		ale::ast::node_type_e::Subscripted_Variable
	);

	assert(
		right_child->get_node_type() ==
		ale::ast::node_type_e::Subscripted_Variable
	);
#endif

	// left indices
	auto left_subscripted_variable =
		static_cast<const ale::ast::SubscriptedVariableNode *>(
			left_child.get()
		);
	auto left_idxs_w = get_indices(ctx, left_subscripted_variable);
	if (not left_idxs_w.has_value()) {
		return std::move(left_idxs_w.error());
	}

	// right indices
	auto right_subscripted_variable =
		static_cast<const ale::ast::SubscriptedVariableNode *>(
			right_child.get()
		);
	auto right_idxs_w = get_indices(ctx, right_subscripted_variable);
	if (not right_idxs_w.has_value()) {
		return std::move(right_idxs_w.error());
	}

	const std::any& left_idxs = *left_idxs_w;
	const std::any& right_idxs = *right_idxs_w;

	std::vector<std::string> names;

	INTERPRETER_PRINT_LOC(
		ale::logger::println, "Going to construct SequenceNodeIterator."
	);
	INTERPRETER_PRINT_LOC2(
		ale::logger::println,
		"    Type inside left indices: {}.",
		detail::get_type_name(left_idxs)
	);
	INTERPRETER_PRINT_LOC2(
		ale::logger::println,
		"    Type inside right indices: {}.",
		detail::get_type_name(right_idxs)
	);

#if defined DEBUG
	assert(detail::is_type<std::vector<int64_t>>(left_idxs));
	assert(detail::is_type<std::vector<int64_t>>(right_idxs));
#endif

	ale::utils::SequenceNodeIterator iter(
		std::any_cast<std::vector<int64_t>>(left_idxs),
		std::any_cast<std::vector<int64_t>>(right_idxs)
	);

	INTERPRETER_PRINT_LOC(
		ale::logger::println, "    Constructed SequenceNodeIterator."
	);

	while (not iter.end()) {
		const auto& idxs = iter.get_current_indices();

		std::string name = left_subscripted_variable->get_variable_name();
		for (const int64_t idx : idxs) {
			name += "_" + std::to_string(idx);
		}

		INTERPRETER_PRINT_LOC2(
			ale::logger::println, "Variable name: {}.", name
		);

		names.push_back(std::move(name));
		iter.next_indices();
	}

	return make_good_evaluation_result(std::move(names));
}

} // namespace ast
} // namespace intlib
