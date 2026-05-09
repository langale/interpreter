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

#include <ale/ast/n_ary_nodes/SubscriptedVariableNode.hpp>

#include <intlib/detail/any_to_numeric.hpp>
#include <intlib/ast/interpretation.hpp>
#include <intlib/ast/utils/variable_names.hpp>
#include <intlib/logger/macros.hpp>
#if defined ALE_LOGGING_MESSAGES
#include <intlib/memory/utils/variable_to_string.hpp>
#include <intlib/memory/utils/wrapped_any_to_string.hpp>
#endif

namespace intlib {
namespace ast {

[[nodiscard]] static Evaluation get_indices(
	EvaluationContext& ctx,
	const ale::ast::SubscriptedVariableNode& subscripted_variable
)
{
	INTERPRETER_ENTER_AST_FUNCTION;

	Vec<int64_t> indices;
	const auto& children = subscripted_variable.get_children();

	INTERPRETER_PRINT("Variable has {} subindices.", children.size());

	for (const auto& [i, child] : children | std::views::enumerate) {

		INTERPRETER_PRINT("Made index for child {}.", i);

		Evaluation eval = interpret_node(ctx, child);
		if (not eval.has_value()) {
			return eval;
		}

		INTERPRETER_PRINT("Successfully evaluated child.");

		const memory::WrappedAny& val_w = *eval;
		std::optional<int64_t> idx_w;

		if (val_w.type == detail::type_string_cpp<memory::Variable>) {
			const auto& memory_variable =
				std::any_cast<const memory::Variable&>(val_w.value);

			idx_w = detail::any_to_numeric<int64_t>(memory_variable);

			if (not idx_w) {
				INTERPRETER_PRINT(
					"Could not convert variable '{}' into a numeric int64_t.",
					memory_variable
				);
			}
		}
		else {
			idx_w = detail::any_to_numeric<int64_t>(val_w);

			if (not idx_w) {
				INTERPRETER_PRINT(
					"Could not convert value '{}' into a numeric int64_t.",
					val_w
				);
			}
		}

		if (not idx_w) {
			return make_bad_evaluation(
				Vec{evaluation_error_e::
						Evaluation_Of_Node_Is_Not_A_Numeric_Value},
				Vec{evaluation_function_e::Variable_Names},
				Vec{"Evaluation of node is not a numeric value"s}
			);
		}

		const auto idx = *idx_w;
		INTERPRETER_PRINT("Made index {}.", idx);
		indices.push_back(idx);
	}

	return make_good_evaluation<EvaluationResult>(std::move(indices), "");
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

Evaluation make_subscripted_variable_name(
	EvaluationContext& ctx,
	const ale::ast::SubscriptedVariableNode& subscripted_variable
)
{
	INTERPRETER_ENTER_AST_FUNCTION;

	std::string name = subscripted_variable.get_variable_name();

	if (ctx.sequence_execution_environment.has_value()) {
		INTERPRETER_PRINT(
			"There is a sequence environment so using the precomputed data."
		);

#if defined DEBUG
		assert(*ctx.sequence_execution_environment != nullptr);
#endif

		const auto N = subscripted_variable.get_num_children();
		const auto& indices_order = subscripted_variable.get_indices_order();
		std::vector<int64_t> indices(N);

		const auto env = *ctx.sequence_execution_environment;
		for (size_t i = 0; i < N; ++i) {
#if defined DEBUG
			assert(i < indices_order.size());
#endif

			const size_t depth = indices_order[i];

			const int64_t first_index =
				env->get_first_indices().get_index(depth, name);
			const int64_t plus_distance = env->get_working_distance(depth);
			const int64_t idx = first_index + plus_distance;

			INTERPRETER_PRINT(
				"Index '{}' corresponds to depth '{}'.", i, depth
			);
			INTERPRETER_PRINT("First index value: '{}'.", first_index);
			INTERPRETER_PRINT("Working distance: '{}'.", plus_distance);
			INTERPRETER_PRINT("Resulting index: '{}'.", idx);

			indices[depth] = idx;
		}

		append_variable_name(name, indices);

		INTERPRETER_PRINT("Name constructed '{}'.", name);

		return make_good_evaluation<
			EvaluationResult>(std::move(name), detail::type_string_cpp<std::string>);
	}

	INTERPRETER_PRINT(
		"There is no sequence environment so retrieving the indices normally."
	);

	Evaluation eval = get_indices(ctx, subscripted_variable);
	if (not eval.has_value()) {
		return eval;
	}

	INTERPRETER_PRINT("Successfully made indices.");

	memory::WrappedAny indices_w = std::move(*eval);

	INTERPRETER_PRINT("Make indices for variable {}.", name);

	const auto indices =
		std::any_cast<Vec<int64_t>&&>(std::move(indices_w.value));
	append_variable_name(name, indices);

	INTERPRETER_PRINT("Name constructed '{}'.", name);

	return make_good_evaluation<
		EvaluationResult>(std::move(name), detail::type_string_cpp<std::string>);
}

} // namespace ast
} // namespace intlib
