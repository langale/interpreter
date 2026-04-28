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
#include <memory>
#include <string>
#include <ranges>
using namespace std::string_literals;

#include <ale/ast/Node.hpp>
#include <ale/ast/binary_nodes/SequenceNode.hpp>
#include <ale/ast/n_ary_nodes/SubscriptedVariableNode.hpp>
#include <ale/utils/IndexIterator.hpp>

#include <intlib/detail/any_type.hpp>
#if defined ALE_LOGGING_MESSAGES
#include <intlib/detail/any_output.hpp>
#endif
#include <intlib/detail/any_to_numeric.hpp>
#include <intlib/ast/interpretation.hpp>
#include <intlib/ast/utils/variable_names.hpp>
#include <intlib/logger/macros.hpp>

namespace intlib {
namespace ast {

#define aleprln ale::logger::println

[[nodiscard]] static EvaluationResult get_indices(
	EvaluationContext& ctx,
	const ale::ast::SubscriptedVariableNode& subscripted_variable
)
{
	INTERPRETER_ENTER_AST_FUNCTION(aleprln);

	Vec<int64_t> indices;
	const auto& children = subscripted_variable.get_children();

	INTERPRETER_PRINT(aleprln, "Variable has {} subindices.", children.size());

	for (const auto& [i, child] : children | std::views::enumerate) {

		INTERPRETER_PRINT(aleprln, "Made index for child {}.", i);

		auto res = interpret_node(ctx, child);
		if (not res.has_value()) {
			return res;
		}

		INTERPRETER_PRINT(aleprln, "Successfully evaluated child.");

		const std::any& val_w = *res;
		std::optional<int64_t> idx_w;

		if (detail::holds_cpp_type<memory::VariableValue>(val_w)) {
			const auto& memory_variable =
				std::any_cast<const memory::VariableValue&>(val_w);
			idx_w = detail::any_to_numeric<int64_t>(memory_variable.value_w);

			if (not idx_w) {
				INTERPRETER_PRINT(
					aleprln,
					"Could not convert node evaluation '{}' into a numeric "
					"int64_t.",
					detail::AnyView{memory_variable.value_w}
				);
			}
		}
		else {
			idx_w = detail::any_to_numeric<int64_t>(val_w);

			if (not idx_w) {
				INTERPRETER_PRINT(
					aleprln,
					"Could not convert node evaluation '{}' into a numeric "
					"int64_t.",
					detail::AnyView{val_w}
				);
			}
		}

		if (not idx_w) {
			return make_bad_evaluation_result(
				Vec{evaluation_error_e::
						Evaluation_Of_Node_Is_Not_A_Numeric_Value},
				Vec{evaluation_function_e::Variable_Names},
				Vec{"Evaluation of node is not a numeric value"s}
			);
		}

		const auto idx = *idx_w;
		INTERPRETER_PRINT(aleprln, "Made index {}.", idx);
		indices.push_back(idx);
	}

	return make_good_evaluation_result<Vec<int64_t>>(std::move(indices));
}

void append_variable_name(std::string& name, const Vec<int64_t>& indices)
{
	for (const int64_t idx : indices) {
		name += "_" + std::to_string(idx);
	}
}

std::string
make_indexed_variable_name(const std::string& name, const Vec<int64_t>& indices)
{
	std::string n = name;
	append_variable_name(n, indices);
	return n;
}

EvaluationResult make_subscripted_variable_name(
	EvaluationContext& ctx,
	const ale::ast::SubscriptedVariableNode& subscripted_variable
)
{
	INTERPRETER_ENTER_AST_FUNCTION(aleprln);

	std::string name = subscripted_variable.get_variable_name();

	if (ctx.sequence_execution_environment.has_value()) {
		INTERPRETER_PRINT(
			aleprln,
			"There is a sequence environment so using the precomputed data."
		);

#if defined DEBUG
		assert(*ctx.sequence_execution_environment != nullptr);
#endif

		const auto& indices_order = subscripted_variable.get_indices_order();

		const auto env = *ctx.sequence_execution_environment;
		for (size_t i = 0; i < subscripted_variable.get_num_children(); ++i) {
#if defined DEBUG
			assert(i < indices_order.size());
#endif

			const size_t depth = indices_order[i];

			const int64_t first_index =
				env->get_first_indices().get_index(depth, name);
			const int64_t plus_distance = env->get_working_distance(depth);
			const int64_t idx = first_index + plus_distance;

			INTERPRETER_PRINT(
				aleprln, "Index '{}' corresponds to depth '{}'.", i, depth
			);
			INTERPRETER_PRINT(aleprln, "First index value: '{}'.", first_index);
			INTERPRETER_PRINT(
				aleprln, "Working distance: '{}'.", plus_distance
			);
			INTERPRETER_PRINT(aleprln, "Resulting index: '{}'.", idx);

			name += "_" + std::to_string(idx);
		}

		INTERPRETER_PRINT(aleprln, "Name constructed '{}'.", name);

		return make_good_evaluation_result<std::string>(std::move(name));
	}

	INTERPRETER_PRINT(
		aleprln,
		"There is no sequence environment so retrieving the indices normally."
	);

	auto res_w = get_indices(ctx, subscripted_variable);
	if (not res_w.has_value()) {
		return res_w;
	}

	INTERPRETER_PRINT(aleprln, "Successfully made indices.");

	std::any indices_w = std::move(*res_w);

	INTERPRETER_PRINT(aleprln, "Make indices for variable {}.", name);

	const auto indices = std::any_cast<Vec<int64_t>&&>(std::move(indices_w));
	append_variable_name(name, indices);

	INTERPRETER_PRINT(aleprln, "Name constructed '{}'.", name);

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
	auto res_left_indices_w = get_indices(ctx, left_subscripted_variable);
	if (not res_left_indices_w.has_value()) {
		return std::move(res_left_indices_w.error());
	}

	// right indices
	const auto& right_subscripted_variable =
		*static_cast<const ale::ast::SubscriptedVariableNode *>(
			right_child.get()
		);
	auto res_right_indices_w = get_indices(ctx, right_subscripted_variable);
	if (not res_right_indices_w.has_value()) {
		return std::move(res_right_indices_w.error());
	}

	std::any left_idxs_w = std::move(*res_left_indices_w);
	std::any right_idxs_w = std::move(*res_right_indices_w);

	INTERPRETER_PRINT(
		aleprln,
		"Type inside left indices: {}.",
		detail::get_type_name(left_idxs_w)
	);
	INTERPRETER_PRINT(
		aleprln,
		"Type inside right indices: {}.",
		detail::get_type_name(right_idxs_w)
	);

	using Veci64 = Vec<int64_t>;

#if defined DEBUG
	assert(detail::holds_cpp_type<Veci64>(left_idxs_w));
	assert(detail::holds_cpp_type<Veci64>(right_idxs_w));
#endif

	return make_good_evaluation_result<ShallowSequenceIndices>(
		std::any_cast<Veci64&&>(std::move(left_idxs_w)),
		std::any_cast<Veci64&&>(std::move(right_idxs_w)),
		std::string_view{left_subscripted_variable.get_variable_name()}
	);
}

} // namespace ast
} // namespace intlib
