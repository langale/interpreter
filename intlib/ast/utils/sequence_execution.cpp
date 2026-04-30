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
#include <vector>

#include <ale/ast/UnaryNode.hpp>
#include <ale/ast/BinaryNode.hpp>
#include <ale/ast/TernaryNode.hpp>
#include <ale/ast/NAryNode.hpp>
#include <ale/ast/binary_nodes/SequenceNode.hpp>
#include <ale/ast/n_ary_nodes/SubscriptedVariableNode.hpp>
#include <ale/ast/utils/node_type_enum.hpp>
#include <ale/ast/utils/node_is_type.hpp>

#include <intlib/logger/macros.hpp>
#include <intlib/ast/EvaluationResult.hpp>
#include <intlib/ast/SequenceIndices.hpp>
#include <intlib/ast/SequenceExecutionEnvironment.hpp>
#include <intlib/ast/interpretation.hpp>
#include <intlib/ast/utils/variable_names.hpp>
#include <intlib/detail/any_type.hpp>
#include <intlib/detail/any_to_numeric.hpp>
#include <intlib/detail/any_output.hpp>

namespace intlib {
namespace ast {

#define aleprln ale::logger::println

[[nodiscard]] static bool
check_equal_anys(const std::any& l, const std::any& r) noexcept
{
	const bool is_l_uint64 = detail::holds_cpp_type<uint64_t>(l);
	const bool is_l_int64 = detail::holds_cpp_type<int64_t>(l);
	const bool is_r_uint64 = detail::holds_cpp_type<uint64_t>(r);
	const bool is_r_int64 = detail::holds_cpp_type<int64_t>(r);
	if (is_l_uint64 and is_r_uint64) {
		return std::any_cast<uint64_t>(l) == std::any_cast<uint64_t>(r);
	}
	if (is_l_int64 and is_r_int64) {
		return std::any_cast<int64_t>(l) == std::any_cast<int64_t>(r);
	}
	return false;
}

[[nodiscard]] static EvaluationResult add_indices_from_subvar(
	EvaluationContext& ctx,
	const std::unique_ptr<ale::ast::Node>& n,
	const size_t d,
	SequenceIndices& indices
)
{
	const auto& var_subs =
		*static_cast<ale::ast::SubscriptedVariableNode *>(n.get());

	const auto& children = var_subs.get_children();
	if (d < children.size()) {
		const std::string_view name = var_subs.get_variable_name();
		const size_t i = var_subs.get_indices_order()[d];
		const auto& child = children[i];
		const bool has_index = indices.has_index(d, name);

		EvaluationResult res = interpret_node(ctx, child);
		if (not res.has_value()) {
			return res;
		}

		auto new_index_w = std::move(*res);
		if (not has_index) {
			if (detail::holds_cpp_type<int64_t>(new_index_w)) {
				const int64_t idx = std::any_cast<int64_t>(new_index_w);
				INTERPRETER_PRINT(
					aleprln,
					"At depth '{}', for variable '{}', set index '{}'.",
					d,
					name,
					idx
				);
				indices.set_index(d, name, idx);
			}
			else if (detail::holds_cpp_type<uint64_t>(new_index_w)) {
				const uint64_t idx = std::any_cast<uint64_t>(new_index_w);
				INTERPRETER_PRINT(
					aleprln,
					"At depth '{}', for variable '{}', set index '{}'.",
					d,
					name,
					idx
				);
				indices.set_index(d, name, static_cast<int64_t>(idx));
			}
			else {
				const std::any idx_w =
					detail::any_to_numeric<int64_t>(new_index_w);
				const int64_t idx = std::any_cast<int64_t>(idx_w);
				INTERPRETER_PRINT(
					aleprln,
					"At depth '{}', for variable '{}', set index '{}'.",
					d,
					name,
					idx
				);
				indices.set_index(d, name, idx);
			}
		}
		else {
			const std::any& known_index_w = indices.get_index(d, name);
			if (not check_equal_anys(new_index_w, known_index_w)) {
				INTERPRETER_PRINT(
					aleprln,
					"Mismatch between known index value '{}' and new "
					"computed index value '{}', for variable '{}'",
					detail::AnyView{known_index_w},
					detail::AnyView{new_index_w},
					name
				);

				return make_bad_evaluation_result(
					Vec{evaluation_error_e::Sequence_Environment_Index_Mismatch
					},
					Vec{evaluation_function_e::
							Sequence_Execution_Environment_Construction},
					Vec{std::format(
						"Mismatch between known index value '{}' and new "
						"computed index value '{}', for variable '{}'",
						detail::AnyView{known_index_w},
						detail::AnyView{new_index_w},
						name
					)}
				);
			}
		}
	}
	return make_good_evaluation_result<std::any>();
}

enum class indices_type_e {
	first,
	last
};

template <indices_type_e indices_type>
[[nodiscard]] static EvaluationResult add_indices(
	EvaluationContext& ctx,
	const std::unique_ptr<ale::ast::Node>& n,
	const size_t d,
	SequenceIndices& indices
)
{
	const auto t = n->get_node_type();
	if (t != ale::ast::node_type_e::Subscripted_Variable) {

		if (ale::ast::is_node_unary(t)) {
			const auto& unary = *static_cast<ale::ast::UnaryNode *>(n.get());
			return add_indices<indices_type>(
				ctx, unary.get_child(), d, indices
			);
		}

		if (ale::ast::is_node_binary(t)) {
			const auto& binary = *static_cast<ale::ast::BinaryNode *>(n.get());
			if (t == ale::ast::node_type_e::Sequence) {
				if constexpr (indices_type == indices_type_e::first) {
					return add_indices<indices_type>(
						ctx, binary.get_left_child(), d + 1, indices
					);
				}
				else {
					return add_indices<indices_type>(
						ctx, binary.get_right_child(), d + 1, indices
					);
				}
			}

			auto res1 = add_indices<indices_type>(
				ctx, binary.get_left_child(), d, indices
			);
			if (not res1.has_value()) {
				return res1;
			}
			auto res2 = add_indices<indices_type>(
				ctx, binary.get_right_child(), d, indices
			);
			if (not res2.has_value()) {
				return res2;
			}
			return make_good_evaluation_result<std::any>();
		}

		if (ale::ast::is_node_ternary(t)) {
			const auto& ternary =
				*static_cast<ale::ast::TernaryNode *>(n.get());
			auto res1 = add_indices<indices_type>(
				ctx, ternary.get_first_child(), d, indices
			);
			if (not res1.has_value()) {
				return res1;
			}
			auto res2 = add_indices<indices_type>(
				ctx, ternary.get_second_child(), d, indices
			);
			if (not res2.has_value()) {
				return res2;
			}
			auto res3 = add_indices<indices_type>(
				ctx, ternary.get_third_child(), d, indices
			);
			if (not res3.has_value()) {
				return res3;
			}
			return make_good_evaluation_result<std::any>();
		}

		if (ale::ast::is_node_n_ary(t)) {
			const auto& nary = *static_cast<ale::ast::NAryNode *>(n.get());
			for (const auto& child : nary.get_children()) {
				auto res = add_indices<indices_type>(ctx, child, d, indices);
				if (not res.has_value()) {
					return res;
				}
			}
			return make_good_evaluation_result<std::any>();
		}

		// Node is zero-ary. Ignore
		return make_good_evaluation_result<std::any>();
	}

	return add_indices_from_subvar(ctx, n, d, indices);
}

[[nodiscard]] static const std::unique_ptr<ale::ast::Node>&
get_expression(const ale::ast::SequenceNode& seq) noexcept
{
	const auto& next_child = seq.get_left_child();
	if (next_child->get_node_type() == ale::ast::node_type_e::Sequence) {
		const auto& next_seq =
			*static_cast<const ale::ast::SequenceNode *>(next_child.get());
		return get_expression(next_seq);
	}
	return next_child;
}

EvaluationResult make_sequence_execution_environment(
	EvaluationContext& ctx, const ale::ast::SequenceNode& seq
)
{
#if defined DEBUG
	assert(seq.get_depth() == 0);
#endif

	SequenceExecutionEnvironment env;

	INTERPRETER_PRINT(aleprln, "Going to extract first indices.");

	const auto& left = seq.get_left_child();
	auto res_left = add_indices<indices_type_e::first>(
		ctx, left, 0, env.get_first_indices()
	);
	if (not res_left.has_value()) {
		return res_left;
	}

	INTERPRETER_PRINT(aleprln, "Going to extract last indices.");

	const auto& right = seq.get_right_child();
	auto res_right = add_indices<indices_type_e::last>(
		ctx, right, 0, env.get_last_indices()
	);
	if (not res_right.has_value()) {
		return res_right;
	}

	INTERPRETER_PRINT(aleprln, "Going to check correctness.");

	// check correctness: variables match
	const auto depth_start = env.get_first_indices().depth();
	const auto depth_end = env.get_first_indices().depth();
	if (depth_start != depth_end) {
		return make_bad_evaluation_result(
			Vec{evaluation_error_e::Sequence_Environment_Mismatch_Depth},
			Vec{evaluation_function_e::
					Sequence_Execution_Environment_Construction},
			Vec{std::format(
				"Mismatch in depth. Left indices: '{}'. Right indices: '{}'",
				depth_start,
				depth_end
			)}
		);
	}

	// check correctness: correct distances
	auto res = env.make_distances();
	if (not res) {
		return res;
	}

	const auto& expression = get_expression(seq);
	env.set_expression(&expression);

	return make_good_evaluation_result<SequenceExecutionEnvironment>(
		std::move(env)
	);
}

[[nodiscard]] static std::generator<EvaluationResult> enumerate_values_sequence(
	EvaluationContext& ctx,
	SequenceExecutionEnvironment& env,
	const size_t depth
)
{
	if (depth == env.get_depth()) {
		ctx.sequence_execution_environment = std::optional{&env};
		EvaluationResult res = interpret_node(ctx, env.get_expression());
		ctx.sequence_execution_environment = {};
		co_yield std::move(res);
		co_return;
	}

#if defined DEBUG
	assert(env.get_distance(depth) > 0);
#endif

	const int64_t distance = env.get_distance(depth);
	for (int64_t i = 0; i < distance; ++i) {
		env.set_working_distance(depth, i);

		auto gen = enumerate_values_sequence(ctx, env, depth + 1);
		auto pos = gen.begin();
		const auto end = gen.end();
		while (pos != end) {
			EvaluationResult res = *pos;
			if (not res) {
				co_yield std::move(res);
				co_return;
			}
			co_yield std::move(res);
			++pos;
		}
	}

	co_return;
}

std::generator<EvaluationResult> enumerate_values_sequence(
	EvaluationContext& ctx, SequenceExecutionEnvironment& env
)
{
	auto gen = enumerate_values_sequence(ctx, env, 0);
	auto pos = gen.begin();
	const auto end = gen.end();
	while (pos != end) {
		EvaluationResult res = *pos;
		if (not res) {
			co_yield std::move(res);
			co_return;
		}
		co_yield std::move(res);
		++pos;
	}
}

[[nodiscard]] static std::generator<EvaluationResult> enumerate_names_sequence(
	EvaluationContext& ctx,
	SequenceExecutionEnvironment& env,
	const size_t depth
)
{
	if (depth == env.get_depth()) {
		const auto& expr = env.get_expression();
#if defined DEBUG
		assert(
			expr->get_node_type() == ale::ast::node_type_e::Subscripted_Variable
		);
#endif

		const ale::ast::SubscriptedVariableNode& sub =
			*static_cast<const ale::ast::SubscriptedVariableNode *>(expr.get());

		ctx.sequence_execution_environment = {&env};
		EvaluationResult res = make_subscripted_variable_name(ctx, sub);
		ctx.sequence_execution_environment = {};

		co_yield std::move(res);
		co_return;
	}

#if defined DEBUG
	assert(env.get_distance(depth) > 0);
#endif

	const int64_t distance = env.get_distance(depth);
	for (int64_t i = 0; i < distance; ++i) {
		env.set_working_distance(depth, i);

		auto gen = enumerate_names_sequence(ctx, env, depth + 1);
		auto pos = gen.begin();
		const auto end = gen.end();
		while (pos != end) {
			EvaluationResult res = *pos;
			if (not res) {
				co_yield std::move(res);
				co_return;
			}
			co_yield std::move(res);
			++pos;
		}
	}

	co_return;
}

std::generator<EvaluationResult> enumerate_names_sequence(
	EvaluationContext& ctx, SequenceExecutionEnvironment& env
)
{
#if defined DEBUG
	assert(
		env.get_expression()->get_node_type() ==
		ale::ast::node_type_e::Subscripted_Variable
	);
#endif

	INTERPRETER_PRINT(
		aleprln, "Going to enumerate the variable names of a sequence."
	);

	auto gen = enumerate_names_sequence(ctx, env, 0);
	auto pos = gen.begin();
	const auto end = gen.end();
	while (pos != end) {
		EvaluationResult res = *pos;
		if (not res) {
			co_yield std::move(res);
			co_return;
		}
		co_yield std::move(res);
		++pos;
	}
}

} // namespace ast
} // namespace intlib
