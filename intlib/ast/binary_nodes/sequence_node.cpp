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

#include <ale/ast/binary_nodes/SequenceNode.hpp>
#include <ale/ast/utils/node_type_to_string.hpp>

#include <intlib/ast/EvaluationContext.hpp>
#include <intlib/ast/Evaluation.hpp>
#include <intlib/ast/evaluation.hpp>
#include <intlib/ast/utils/sequence_execution.hpp>
#include <intlib/ast/interpretation.hpp>
#include <intlib/ast/utils/evaluation_result_to_string.hpp>
#include <intlib/arithmetic/arithmetic.hpp>
#include <intlib/comparison/comparison.hpp>
#include <intlib/detail/any_to_bool.hpp>
#include <intlib/memory/utils/unwrap.hpp>

#include <intlib/logger/macros.hpp>

namespace intlib {
namespace ast {

static constexpr std::string_view evaluation_failed =
	"Evaluation of sequence failed at (depth {}, distance {})";

static constexpr std::string_view could_not_operate =
	"Could not operate values {} and {}";

static constexpr std::string_view could_not_compare =
	"Could not compare values {} and {}";

static constexpr std::string_view could_not_convert_to_bool =
	"Could not convert value {} to Boolean";

[[nodiscard]] static Evaluation evaluate_sequence_arithmetic(
	EvaluationContext& ctx,
	const ale::ast::SequenceNode& seq,
	const ale::ast::node_type_e op_type,
	SequenceExecutionEnvironment& env,
	const size_t depth
)
{
	// first value of the sequence
	env.set_working_distance(depth, 0);
	ctx.sequence_depth = depth + 1;
	Evaluation expression_eval = evaluate(ctx, seq);

	if (not expression_eval) {
		INTERPRETER_PRINT("Expression could not be evaluated.");
		return append_error(
			std::move(expression_eval.error()),
			evaluation_error_e::Sequence_Evaluation_Failed,
			evaluation_function_e::Sequence,
			std::format(evaluation_failed, depth + 1, 0)
		);
	}

	const int64_t distance = env.get_distance(depth);
	int64_t i = 1;

	EvaluationResult seq_res = *expression_eval;
	memory::unwrap_into(seq_res);

	while (i < distance) {
		env.set_working_distance(depth, i);
		ctx.sequence_depth = depth + 1;

		Evaluation current_eval = evaluate(ctx, seq);
		if (not current_eval) {
			INTERPRETER_PRINT("Expression could not be evaluated.");
			return append_error(
				std::move(current_eval.error()),
				evaluation_error_e::Sequence_Evaluation_Failed,
				evaluation_function_e::Sequence,
				std::format(evaluation_failed, depth + 1, i)
			);
		}

		EvaluationResult current_res = *current_eval;
		const memory::WrappedAny *actual_current_w =
			memory::unwrap_out(current_res);

		std::optional res =
			arithmetic::any_arithmetic(op_type, seq_res, *actual_current_w);
		if (not res) {
			INTERPRETER_PRINT("Arithmetic expression failed.");
			return make_bad_evaluation(
				Vec{evaluation_error_e::Arithmetic_Operation_Failed,
					evaluation_error_e::Sequence_Evaluation_Failed},
				Vec{evaluation_function_e::Sequence,
					evaluation_function_e::Sequence},
				Vec{std::format(could_not_operate, seq_res, current_res),
					std::format(evaluation_failed, depth + 1, i)}
			);
		}
		seq_res = std::move(*res);

		++i;
	}

	return make_good_evaluation<EvaluationResult>(std::move(seq_res));
}

[[nodiscard]] static Evaluation evaluate_sequence_comparison(
	EvaluationContext& ctx,
	const ale::ast::SequenceNode& seq,
	const ale::ast::node_type_e op_type,
	SequenceExecutionEnvironment& env,
	const size_t depth
)
{
	// first value of the sequence
	env.set_working_distance(depth, 0);
	ctx.sequence_depth = depth + 1;
	Evaluation expression_eval = evaluate(ctx, seq);

	if (not expression_eval) {
		return append_error(
			std::move(expression_eval.error()),
			evaluation_error_e::Sequence_Evaluation_Failed,
			evaluation_function_e::Sequence,
			std::format(evaluation_failed, depth + 1, 0)
		);
	}

	const int64_t distance = env.get_distance(depth);
	int64_t i = 1;

	EvaluationResult prev_res = *expression_eval;
	memory::unwrap_into(prev_res);

	while (i < distance) {
		env.set_working_distance(depth, i);
		ctx.sequence_depth = depth + 1;

		Evaluation current_eval = evaluate(ctx, seq);
		if (not current_eval) {
			return append_error(
				std::move(current_eval.error()),
				evaluation_error_e::Sequence_Evaluation_Failed,
				evaluation_function_e::Sequence,
				std::format(evaluation_failed, depth + 1, i)
			);
		}

		EvaluationResult current_res = *current_eval;
		const memory::WrappedAny *actual_current_w =
			memory::unwrap_out(current_res);

		std::optional res =
			comparison::any_comparison(op_type, prev_res, *actual_current_w);
		if (not res) {
			return make_bad_evaluation(
				Vec{evaluation_error_e::Comparison_Operation_Failed,
					evaluation_error_e::Sequence_Evaluation_Failed},
				Vec{evaluation_function_e::Sequence,
					evaluation_function_e::Sequence},
				Vec{std::format(could_not_compare, prev_res, current_res),
					std::format(evaluation_failed, depth + 1, i)}
			);
		}

		const bool result_comparison = *res;
		if (not result_comparison) {
			return make_good_evaluation<
				EvaluationResult>(false, detail::type_string_cpp<bool>);
		}

		++i;
	}

	return make_good_evaluation<
		EvaluationResult>(true, detail::type_string_cpp<bool>);
}

[[nodiscard]] static Evaluation evaluate_sequence_logical(
	EvaluationContext& ctx,
	const ale::ast::SequenceNode& seq,
	const ale::ast::node_type_e op_type,
	SequenceExecutionEnvironment& env,
	const size_t depth
)
{
	// first value of the sequence
	env.set_working_distance(depth, 0);
	ctx.sequence_depth = depth + 1;
	Evaluation expression_eval = evaluate(ctx, seq);

	if (not expression_eval) {
		return append_error(
			std::move(expression_eval.error()),
			evaluation_error_e::Sequence_Evaluation_Failed,
			evaluation_function_e::Sequence,
			std::format(evaluation_failed, depth + 1, 0)
		);
	}

	const int64_t distance = env.get_distance(depth);
	int64_t i = 1;

	EvaluationResult seq_res = *expression_eval;
	memory::unwrap_into(seq_res);
	std::optional value_w = detail::any_to_bool(seq_res);
	if (not value_w.has_value()) {
		return make_bad_evaluation(
			Vec{evaluation_error_e::Conversion_To_Bool_Failed,
				evaluation_error_e::Sequence_Evaluation_Failed},
			Vec{evaluation_function_e::Sequence, evaluation_function_e::Sequence
			},
			Vec{std::format(could_not_convert_to_bool, seq_res),
				std::format(evaluation_failed, depth + 1, i)}
		);
	}

	if (op_type == ale::ast::node_type_e::Logical_And and not*value_w) {
		return make_good_evaluation<
			EvaluationResult>(false, detail::type_string_cpp<bool>);
	}
	if (op_type == ale::ast::node_type_e::Logical_Or and *value_w) {
		return make_good_evaluation<
			EvaluationResult>(true, detail::type_string_cpp<bool>);
	}

	while (i < distance) {
		env.set_working_distance(depth, i);
		ctx.sequence_depth = depth + 1;

		Evaluation current_eval = evaluate(ctx, seq);
		if (not current_eval) {
			return append_error(
				std::move(current_eval.error()),
				evaluation_error_e::Sequence_Evaluation_Failed,
				evaluation_function_e::Sequence,
				std::format(evaluation_failed, depth + 1, i)
			);
		}

		EvaluationResult current_res = *current_eval;
		const memory::WrappedAny *actual_current_w =
			memory::unwrap_out(current_res);

		value_w = detail::any_to_bool(*actual_current_w);
		if (not value_w) {
			return make_bad_evaluation(
				Vec{evaluation_error_e::Conversion_To_Bool_Failed,
					evaluation_error_e::Sequence_Evaluation_Failed},
				Vec{evaluation_function_e::Sequence,
					evaluation_function_e::Sequence},
				Vec{std::format(could_not_convert_to_bool, current_res),
					std::format(evaluation_failed, depth + 1, i)}
			);
		}

		if (op_type == ale::ast::node_type_e::Logical_And and not(*value_w)) {
			return make_good_evaluation<
				EvaluationResult>(false, detail::type_string_cpp<bool>);
		}
		if (op_type == ale::ast::node_type_e::Logical_Or and (*value_w)) {
			return make_good_evaluation<
				EvaluationResult>(true, detail::type_string_cpp<bool>);
		}

		++i;
	}

	if (op_type == ale::ast::node_type_e::Logical_And) {
		return make_good_evaluation<
			EvaluationResult>(true, detail::type_string_cpp<bool>);
	}

	return make_good_evaluation<
		EvaluationResult>(false, detail::type_string_cpp<bool>);
}

Evaluation evaluate(EvaluationContext& ctx, const ale::ast::SequenceNode& seq)
{
	INTERPRETER_ENTER_AST_FUNCTION;

	INTERPRETER_PRINT("Trying to evaluate a sequence.");

	if (not ctx.sequence_execution_environment.has_value()) {
		INTERPRETER_PRINT("Context does not have a sequence execution "
						  "environment, make one now.");

		Evaluation eval = make_sequence_execution_environment(ctx, seq);
		if (not eval) {
			return eval;
		}

		EvaluationResult seq_res = std::move(*eval);
#if defined DEBUG
		assert(seq_res.type == detail::type_string_cpp<SequenceExecutionEnvironment>);
#endif
		SequenceExecutionEnvironment seq_env =
			std::any_cast<SequenceExecutionEnvironment&&>(
				std::move(seq_res.value)
			);

		ctx.sequence_execution_environment = &seq_env;
		ctx.sequence_depth = 0;
		Evaluation sequence_evaluation = evaluate(ctx, seq);
		ctx.sequence_execution_environment.reset();
		ctx.sequence_depth.reset();
		return sequence_evaluation;
	}

#if defined DEBUG
	assert(ctx.sequence_execution_environment.has_value());
	assert(ctx.sequence_depth.has_value());
#endif

	SequenceExecutionEnvironment& env = **ctx.sequence_execution_environment;
	const size_t depth = *ctx.sequence_depth;

	if (depth == env.get_depth()) {
		INTERPRETER_PRINT("At depth {}, instantiated all indices.", depth);
		INTERPRETER_PRINT("Going to interpret the expression.");
		return interpret_node(ctx, env.get_expression());
	}

	const auto op_type = env.get_operator_type(depth);

#if defined DEBUG
	assert(is_node_interpretable(op_type));
#endif

	const bool is_arithmetic = ale::ast::is_node_arithmetic(op_type);
	const bool is_comparison = ale::ast::is_node_comparison(op_type);
	const bool is_logical = ale::ast::is_node_logical(op_type);

	INTERPRETER_PRINT("Operator '{}':", op_type);
	INTERPRETER_PRINT("    Is arithmetic? {}", is_arithmetic);
	INTERPRETER_PRINT("    Is comparison? {}", is_comparison);
	INTERPRETER_PRINT("    Is logical?    {}", is_logical);

	if (is_arithmetic) {
		return evaluate_sequence_arithmetic(ctx, seq, op_type, env, depth);
	}
	if (is_comparison) {
		return evaluate_sequence_comparison(ctx, seq, op_type, env, depth);
	}
	if (is_logical) {
		return evaluate_sequence_logical(ctx, seq, op_type, env, depth);
	}

	INTERPRETER_PRINT("    Could not classify operator {}.", op_type);

	return make_bad_evaluation(
		Vec{evaluation_error_e::Sequence_Evaluation_Failed},
		Vec{evaluation_function_e::Sequence},
		Vec{std::format(
			"Invalid operator type '{}' in sequence for evaluation.", op_type
		)}
	);
}

} // namespace ast
} // namespace intlib
