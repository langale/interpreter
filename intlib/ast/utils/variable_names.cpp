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
using namespace std::string_literals;

#include <ale/ast/Node.hpp>
#include <ale/ast/binary_nodes/SequenceNode.hpp>
#include <ale/ast/n_ary_nodes/SubscriptedVariableNode.hpp>
#include <ale/utils/IndexIterator.hpp>

#include <intlib/ast/interpretation.hpp>
#include <intlib/ast/utils/variable_names.hpp>
#include <intlib/detail/any_type.hpp>
#if defined ALE_LOGGING_MESSAGES
#include <intlib/detail/any_output.hpp>
#endif
#include <intlib/detail/any_to_numeric.hpp>
#include <intlib/logger/macros.hpp>

namespace intlib {
namespace ast {

#define aleprln ale::logger::println

[[nodiscard]] EvaluationResult get_indices(
	EvaluationContext& ctx,
	const ale::ast::SubscriptedVariableNode& subscripted_variable
)
{
	INTERPRETER_ENTER_AST_FUNCTION(aleprln);

	std::vector<int64_t> indices;
	const auto& children = subscripted_variable.get_children();

	INTERPRETER_PRINT_LOC(
		aleprln, "Variable has {} subindices.", children.size()
	);

	for (const auto& [i, child] : children | std::views::enumerate) {

		INTERPRETER_PRINT_LOC(aleprln, "Made index for child {}.", i);

		auto val_int_w = interpret_node(ctx, child);
		if (not val_int_w.has_value()) {
			return std::move(val_int_w.error());
		}

		INTERPRETER_PRINT_LOC(aleprln, "Successfully evaluated child.");

		const std::any& val_w = *val_int_w;
		std::optional<int64_t> idx_w;

		if (detail::is_type<memory::VariableValue>(val_w)) {
			const auto& memory_variable =
				std::any_cast<const memory::VariableValue&>(val_w);
			idx_w = detail::any_to_numeric<int64_t>(memory_variable.value_w);

			if (not idx_w) {
				INTERPRETER_PRINT_LOC(
					aleprln,
					"Could not convert node evaluation '{}' into a numeric "
					"int64_t.",
					any_view{memory_variable.value_w}
				);
			}
		}
		else {
			idx_w = detail::any_to_numeric<int64_t>(val_w);

			if (not idx_w) {
				INTERPRETER_PRINT_LOC(
					aleprln,
					"Could not convert node evaluation '{}' into a numeric "
					"int64_t.",
					any_view{val_w}
				);
			}
		}

		if (not idx_w) {
			return make_bad_evaluation_result(
				std::vector{evaluation_error_e::
								Evaluation_Of_Node_Is_Not_A_Numeric_Value},
				std::vector{"Evaluation of node is not a numeric value."s}
			);
		}

		const auto idx = *idx_w;
		INTERPRETER_PRINT_LOC(aleprln, "Made index {}.", idx);
		indices.push_back(idx);
	}

	return make_good_evaluation_result<std::vector<int64_t>>(
		std::move(indices)
	);
}

void append_variable_name(std::string& name, const std::vector<int64_t>& idxs)
{
	for (const int64_t idx : idxs) {
		name += "_" + std::to_string(idx);
	}
}

std::string make_indexed_variable_name(
	const std::string& name, const std::vector<int64_t>& indices
)
{
	std::string n = name;
	append_variable_name(n, indices);
	return n;
}

const std::string&
get_variable_name(const ale::ast::SequenceNode& sequence) noexcept
{
	return static_cast<ale::ast::SubscriptedVariableNode *>(
			   sequence.get_left_child().get()
	)
		->get_variable_name();
}

EvaluationResult make_subscripted_variable_name(
	EvaluationContext& ctx,
	const ale::ast::SubscriptedVariableNode& subscripted_variable
)
{
	INTERPRETER_ENTER_AST_FUNCTION(aleprln);

	auto res_w = get_indices(ctx, subscripted_variable);
	if (not res_w.has_value()) {
		return make_bad_evaluation_result(std::move(res_w.error()));
	}

	INTERPRETER_PRINT_LOC(aleprln, "Successfully made indices.");

	std::any idxs_w = std::move(*res_w);

	std::string name = subscripted_variable.get_variable_name();

	INTERPRETER_PRINT_LOC(aleprln, "Make indices for variable {}.", name);

	auto idxs = std::any_cast<std::vector<int64_t>&&>(std::move(idxs_w));
	append_variable_name(name, idxs);

	INTERPRETER_PRINT_LOC(aleprln, "Name constructed '{}'.", name);

	return make_good_evaluation_result<std::string>(std::move(name));
}

EvaluationResult make_shallow_sequence_indices(
	EvaluationContext& ctx, const ale::ast::SequenceNode& sequence_comma
)
{
	INTERPRETER_ENTER_AST_FUNCTION(aleprln);

#if defined DEBUG
	assert(sequence_comma.get_operator_type().has_value());
	assert(
		sequence_comma.get_operator_type() ==
		ale::ast::node_type_e::Comma_Separated_Group
	);
#endif

	const auto& left_child = sequence_comma.get_left_child();
	const auto& right_child = sequence_comma.get_right_child();

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
	const auto& left_subscripted_variable =
		*static_cast<const ale::ast::SubscriptedVariableNode *>(
			left_child.get()
		);
	auto res_left_idxs_w = get_indices(ctx, left_subscripted_variable);
	if (not res_left_idxs_w.has_value()) {
		return std::move(res_left_idxs_w.error());
	}

	// right indices
	const auto& right_subscripted_variable =
		*static_cast<const ale::ast::SubscriptedVariableNode *>(
			right_child.get()
		);
	auto res_right_idxs_w = get_indices(ctx, right_subscripted_variable);
	if (not res_right_idxs_w.has_value()) {
		return std::move(res_right_idxs_w.error());
	}

	std::any left_idxs_w = std::move(*res_left_idxs_w);
	std::any right_idxs_w = std::move(*res_right_idxs_w);

	INTERPRETER_PRINT_LOC(
		aleprln,
		"Type inside left indices: {}.",
		detail::get_type_name(left_idxs_w)
	);
	INTERPRETER_PRINT_LOC(
		aleprln,
		"Type inside right indices: {}.",
		detail::get_type_name(right_idxs_w)
	);

	using Veci64 = std::vector<int64_t>;

#if defined DEBUG
	assert(detail::is_type<Veci64>(left_idxs_w));
	assert(detail::is_type<Veci64>(right_idxs_w));
#endif

	ShallowSequenceIndices idxs_limits{
		.left = std::any_cast<Veci64&&>(std::move(left_idxs_w)),
		.right = std::any_cast<Veci64&&>(std::move(right_idxs_w)),
		.base_name = left_subscripted_variable.get_variable_name()
	};
	return make_good_evaluation_result<ShallowSequenceIndices>(
		std::move(idxs_limits)
	);
}

} // namespace ast
} // namespace intlib
