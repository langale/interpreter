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
#if defined ALE_LOGGING_MESSAGES
#include <ale/ast/utils/node_type_to_string.hpp>
#include <ale/ast/utils/xml_tree.hpp>
#endif

#include <intlib/ast/Evaluation.hpp>
#include <intlib/ast/SequenceIndices.hpp>
#include <intlib/ast/SequenceExecutionEnvironment.hpp>
#include <intlib/ast/interpretation.hpp>
#include <intlib/ast/evaluation.hpp>
#include <intlib/ast/utils/variable_names.hpp>
#include <intlib/detail/any_to_numeric.hpp>
#include <intlib/detail/macros.hpp>
#include <intlib/logger/macros.hpp>
#include <intlib/ast/utils/evaluation_result_to_string.hpp>

namespace intlib {
namespace ast {

[[nodiscard]] static bool
check_equal_anys(const memory::WrappedAny& l, const int64_t r)
{
	const bool is_l_uint64 = l.type == detail::type_string_cpp<uint64_t>;
	const bool is_l_int64 = l.type == detail::type_string_cpp<int64_t>;
	if (is_l_uint64) {
		return std::any_cast<uint64_t>(l.value) == detail::to_uint64(r);
	}
	if (is_l_int64) {
		return std::any_cast<int64_t>(l.value) == r;
	}
	return false;
}

[[nodiscard]] static Evaluation add_indices_from_subvar(
	EvaluationContext& ctx,
	const std::unique_ptr<ale::ast::Node>& n,
	SequenceIndices& indices
)
{
	const auto& var_subs =
		*static_cast<ale::ast::SubscriptedVariableNode *>(n.get());

	const auto& children = var_subs.get_children();
	const size_t N = children.size();

	indices.reserve(N);

	for (size_t i = 0; i < N; ++i) {
		const std::string_view name = var_subs.get_variable_name();

		const size_t depth = var_subs.get_indices_order()[i];
		const auto& child = children[depth];
		const bool has_index = indices.has_index(depth, name);

		Evaluation eval = interpret_node(ctx, child);
		if (not eval.has_value()) {
			return eval;
		}

		EvaluationResult new_index_res = std::move(*eval);

		INTERPRETER_PRINT(
			"At subscripted variable '{}', child {} is the {}th index, with "
			"value {}.",
			name,
			i,
			depth,
			new_index_res
		);

		if (not has_index) {
			if (new_index_res.type == detail::type_string_cpp<int64_t>) {
				const auto idx = std::any_cast<int64_t>(new_index_res.value);
				INTERPRETER_PRINT(
					"At depth '{}', for variable '{}', set index '{}'.",
					depth,
					name,
					idx
				);
				indices.set_index(depth, name, idx);
			}
			else if (new_index_res.type == detail::type_string_cpp<uint64_t>) {
				const auto idx = std::any_cast<uint64_t>(new_index_res.value);

				INTERPRETER_PRINT(
					"At depth '{}', for variable '{}', set index '{}'.",
					depth,
					name,
					idx
				);
				indices.set_index(depth, name, detail::to_int64(idx));
			}
			else {
				const std::optional idx_w =
					detail::any_to_numeric<int64_t>(new_index_res);

				if (not idx_w) {
					return make_bad_evaluation(
						Vec{evaluation_error_e::Conversion_To_Numeric_Failed},
						Vec{evaluation_function_e::
								Sequence_Execution_Environment_Construction},
						Vec{std::format(
							"Could not convert value '{}' to int64_t",
							new_index_res
						)}
					);
				}
				const auto idx = std::any_cast<int64_t>(idx_w);
				INTERPRETER_PRINT(
					"At depth '{}', for variable '{}', set index '{}'.",
					depth,
					name,
					idx
				);
				indices.set_index(depth, name, idx);
			}
		}
		else {
			const int64_t known_index = indices.get_index(depth, name);
			if (not check_equal_anys(new_index_res, known_index)) {
				INTERPRETER_PRINT(
					"Mismatch between known index value '{}' and new "
					"computed index value '{}', for variable '{}'",
					known_index,
					new_index_res,
					name
				);

				return make_bad_evaluation(
					Vec{evaluation_error_e::Sequence_Environment_Index_Mismatch
					},
					Vec{evaluation_function_e::
							Sequence_Execution_Environment_Construction},
					Vec{std::format(
						"Mismatch between known index value '{}' and new "
						"computed index value '{}', for variable '{}'",
						known_index,
						new_index_res,
						name
					)}
				);
			}
		}
	}
	return make_good_evaluation<EvaluationResult>();
}

enum class indices_type_e : int8_t {
	First,
	Last
};

template <indices_type_e indices_type>
[[nodiscard]] static Evaluation add_indices(
	EvaluationContext& ctx,
	const std::unique_ptr<ale::ast::Node>& n,
	const size_t d,
	SequenceIndices& indices
)
{
	static_assert(ale::ast::is_node_binary(ale::ast::node_type_e::Sequence));

	const auto t = n->get_node_type();

	if (t == ale::ast::node_type_e::Sequence) {
		const auto& seq = *static_cast<ale::ast::SequenceNode *>(n.get());
		if constexpr (indices_type == indices_type_e::First) {
			return add_indices<indices_type>(
				ctx, seq.get_left_child(), d + 1, indices
			);
		}
		else {
			return add_indices<indices_type>(
				ctx, seq.get_right_child(), d + 1, indices
			);
		}
	}

	if (t != ale::ast::node_type_e::Subscripted_Variable) {

		if (ale::ast::is_node_unary(t)) {
			const auto& unary = *static_cast<ale::ast::UnaryNode *>(n.get());
			return add_indices<indices_type>(
				ctx, unary.get_child(), d, indices
			);
		}

		if (ale::ast::is_node_binary(t)) {
			const auto& binary = *static_cast<ale::ast::BinaryNode *>(n.get());

			auto res1_eval = add_indices<indices_type>(
				ctx, binary.get_left_child(), d, indices
			);
			if (not res1_eval.has_value()) {
				return res1_eval;
			}
			auto res2_eval = add_indices<indices_type>(
				ctx, binary.get_right_child(), d, indices
			);
			if (not res2_eval.has_value()) {
				return res2_eval;
			}
			return make_good_evaluation<EvaluationResult>();
		}

		if (ale::ast::is_node_ternary(t)) {
			const auto& ternary =
				*static_cast<ale::ast::TernaryNode *>(n.get());
			auto res1_eval = add_indices<indices_type>(
				ctx, ternary.get_first_child(), d, indices
			);
			if (not res1_eval.has_value()) {
				return res1_eval;
			}
			auto res2_eval = add_indices<indices_type>(
				ctx, ternary.get_second_child(), d, indices
			);
			if (not res2_eval.has_value()) {
				return res2_eval;
			}
			auto res3_eval = add_indices<indices_type>(
				ctx, ternary.get_third_child(), d, indices
			);
			if (not res3_eval.has_value()) {
				return res3_eval;
			}
			return make_good_evaluation<EvaluationResult>();
		}

		if (ale::ast::is_node_n_ary(t)) {
			const auto& nary = *static_cast<ale::ast::NAryNode *>(n.get());
			for (const auto& child : nary.get_children()) {
				auto res_eval =
					add_indices<indices_type>(ctx, child, d, indices);
				if (not res_eval.has_value()) {
					return res_eval;
				}
			}
			return make_good_evaluation<EvaluationResult>();
		}

		// Node is zero-ary. Ignore
		return make_good_evaluation<EvaluationResult>();
	}

#if defined ALE_LOGGING_MESSAGES
	INTERPRETER_PRINT("Going to set the indices for the node");
	ale::ast::print_xml_tree(
		n,
		ale::ast::PrintXMLTreeParams{
			.os = ale::logger::get_instance().out(),
			.start = ale::logger::get_instance().tab(),
			.sep = "    ",
			.use_tab = true,
			.include_attributes = true
		}
	);
#endif

	return add_indices_from_subvar(ctx, n, indices);
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

static void fill_node_types(
	const ale::ast::SequenceNode& seq, SequenceExecutionEnvironment& env
)
{
	static_assert(ale::ast::is_node_binary(ale::ast::node_type_e::Sequence));
#if defined DEBUG
	assert(seq.get_operator_type().has_value());
#endif
	env.add_operator_type(*seq.get_operator_type());

	const auto& left = seq.get_left_child();
	if (left->get_node_type() != ale::ast::node_type_e::Sequence) {
		return;
	}

	const auto& seq_left =
		*static_cast<const ale::ast::SequenceNode *>(left.get());
	fill_node_types(seq_left, env);
}

Evaluation make_sequence_execution_environment(
	EvaluationContext& ctx, const ale::ast::SequenceNode& seq
)
{
#if defined DEBUG
	assert(seq.get_depth() == 0);
#endif

	SequenceExecutionEnvironment env;

	INTERPRETER_PRINT("Going to extract first indices.");

	const auto& left = seq.get_left_child();
	auto left_eval = add_indices<indices_type_e::First>(
		ctx, left, 0, env.get_first_indices()
	);
	if (not left_eval.has_value()) {
		return left_eval;
	}

	INTERPRETER_PRINT("Going to extract last indices.");

	const auto& right = seq.get_right_child();
	auto right_eval = add_indices<indices_type_e::Last>(
		ctx, right, 0, env.get_last_indices()
	);
	if (not right_eval.has_value()) {
		return right_eval;
	}

	INTERPRETER_PRINT("Going to check correctness.");

	// check correctness: variables match
	const auto depth_start = env.get_first_indices().get_depth();
	const auto depth_end = env.get_last_indices().get_depth();

	INTERPRETER_PRINT("depth_start= {}.", depth_start);
	INTERPRETER_PRINT("depth_end=   {}.", depth_end);

	if (depth_start != depth_end) {
		return make_bad_evaluation(
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

	// make distances and check correctness
	auto distances_eval = env.make_distances();
	if (not distances_eval) {
		return distances_eval;
	}

	INTERPRETER_PRINT("Going to retrieve the operator types.");

	fill_node_types(seq, env);

#if defined ALE_LOGGING_MESSAGES
	const size_t height = env.get_num_operators();
	INTERPRETER_PRINT("Height (total number of sequence operators): {}.", height);
	for (size_t i = 0; i < height; ++i) {
		INTERPRETER_PRINT(
			"    Node type at depth {}: '{}'.", i, env.get_operator_type(i)
		);
	}
#endif

	const auto& expression = get_expression(seq);
	env.set_expression(&expression);

	return make_good_evaluation<
		EvaluationResult>(std::move(env), detail::type_string_cpp<SequenceExecutionEnvironment>);
}

[[nodiscard]] static std::generator<Evaluation>
enumerate_values_sequence_recursive(
	EvaluationContext& ctx,
	SequenceExecutionEnvironment& env,
	const ale::ast::SequenceNode& seq
)
{
#if defined DEBUG
	assert(ctx.sequence_depth.has_value());
#endif

	const size_t depth = *ctx.sequence_depth;

	INTERPRETER_PRINT("Enumerate value depth: {}/{}.", depth, env.get_depth());
	const bool is_comma =
		depth == env.get_depth()
			? true
			: env.get_operator_type(depth) ==
				  ale::ast::node_type_e::Comma_Separated_Group;

	if (depth == env.get_depth() or not is_comma) {
		INTERPRETER_PRINT("Level {} of the sequence is interpretable.", depth);

		ctx.sequence_execution_environment = &env;
		Evaluation eval;
		if (depth == env.get_depth()) {
			eval = interpret_node(ctx, env.get_expression());
		}
		else {
			eval = evaluate(ctx, seq);
		}
		ctx.sequence_execution_environment.reset();
		co_yield std::move(eval);
		co_return;
	}

#if defined DEBUG
	assert(env.get_distance(depth) > 0);
#endif

	const int64_t distance = env.get_distance(depth);
	for (int64_t i = 0; i < distance; ++i) {
		env.set_working_distance(depth, i);
		ctx.sequence_depth = depth + 1;

		auto gen = enumerate_values_sequence_recursive(ctx, env, seq);
		auto pos = gen.begin();
		const auto end = gen.end();
		while (pos != end) {
			Evaluation eval = *pos;
			if (not eval) {
				co_yield std::move(eval);
				co_return;
			}
			co_yield std::move(eval);
			++pos;
		}
	}

	ctx.sequence_depth.reset();
	co_return;
}

std::generator<Evaluation> enumerate_values_sequence(
	EvaluationContext& ctx,
	SequenceExecutionEnvironment& env,
	const ale::ast::SequenceNode& seq
)
{
	ctx.sequence_depth = {0};
	return enumerate_values_sequence_recursive(ctx, env, seq);
}

[[nodiscard]] static std::generator<Evaluation>
enumerate_names_sequence_recursive(
	EvaluationContext& ctx, SequenceExecutionEnvironment& env
)
{
#if defined DEBUG
	assert(ctx.sequence_depth.has_value());
#endif

	const size_t depth = *ctx.sequence_depth;

	INTERPRETER_PRINT("Enumerate name depth: {}/{}.", depth, env.get_depth());

	if (depth == env.get_depth()) {
		const auto& expr = env.get_expression();
#if defined DEBUG
		assert(
			expr->get_node_type() == ale::ast::node_type_e::Subscripted_Variable
		);
#endif

		const ale::ast::SubscriptedVariableNode& sub =
			*static_cast<const ale::ast::SubscriptedVariableNode *>(expr.get());

		ctx.sequence_execution_environment = &env;
		Evaluation eval = make_subscripted_variable_name(ctx, sub);
		ctx.sequence_execution_environment.reset();

		co_yield std::move(eval);
		co_return;
	}

#if defined DEBUG
	assert(env.get_distance(depth) > 0);
#endif

	const int64_t distance = env.get_distance(depth);
	for (int64_t i = 0; i < distance; ++i) {
		env.set_working_distance(depth, i);
		ctx.sequence_depth = depth + 1;

		auto gen = enumerate_names_sequence_recursive(ctx, env);
		auto pos = gen.begin();
		const auto end = gen.end();
		while (pos != end) {
			Evaluation eval = *pos;
			if (not eval) {
				co_yield std::move(eval);
				co_return;
			}
			co_yield std::move(eval);
			++pos;
		}
	}

	ctx.sequence_depth.reset();
	co_return;
}

std::generator<Evaluation> enumerate_names_sequence(
	EvaluationContext& ctx, SequenceExecutionEnvironment& env
)
{
#if defined DEBUG
	assert(
		env.get_expression()->get_node_type() ==
		ale::ast::node_type_e::Subscripted_Variable
	);
#endif

	INTERPRETER_PRINT("Going to enumerate the variable names of a sequence.");

	ctx.sequence_depth = {0};
	return enumerate_names_sequence_recursive(ctx, env);
}

} // namespace ast
} // namespace intlib
