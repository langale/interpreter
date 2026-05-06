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
#include <string>
using namespace std::string_literals;

#include <ale/ast/zero_ary_nodes/VariableNode.hpp>
#include <ale/ast/n_ary_nodes/SubscriptedVariableNode.hpp>
#include <ale/ast/n_ary_nodes/CommaSeparatedGroupNode.hpp>
#include <ale/ast/binary_nodes/SequenceNode.hpp>
#if defined ALE_LOGGING_MESSAGES
#include <ale/ast/utils/node_type_to_string.hpp>
#endif

#include <intlib/detail/any_to_numeric.hpp>
#include <intlib/ast/Evaluation.hpp>
#include <intlib/ast/SequenceExecutionEnvironment.hpp>
#include <intlib/ast/interpretation.hpp>
#include <intlib/ast/utils/variable_names.hpp>
#include <intlib/ast/utils/sequence_execution.hpp>
#include <intlib/ast/evaluation.hpp>
#include <intlib/logger/macros.hpp>
#if defined ALE_LOGGING_MESSAGES
#include <intlib/ast/utils/evaluation_function_to_string.hpp>
#include <intlib/ast/utils/evaluation_result_to_string.hpp>
#endif

namespace intlib {
namespace ast {

[[nodiscard]] static std::generator<Evaluation>
make_value_iterator_over_interpret(
	EvaluationContext& ctx, const std::unique_ptr<ale::ast::Node>& node
)
{
	auto eval = interpret_node(ctx, node);
	co_yield std::move(eval);
}

[[nodiscard]] static std::generator<Evaluation> iterator_empty()
{
	co_yield make_good_evaluation<EvaluationResult>();
}

std::generator<Evaluation>
make_value_iterator(EvaluationContext& ctx, const ale::ast::VariableNode& var)
{
	INTERPRETER_PRINT("Make a value iterator for a '{}'.", var.get_node_type());

	const std::string& var_name = var.get_variable_name();
	if (not ctx.memory.variable_exists(var_name)) {
		co_yield make_bad_evaluation(
			Vec{evaluation_error_e::Memory_Variable_Does_Not_Exist},
			Vec{evaluation_function_e::Iterator_Value_Subscripted_Variable},
			Vec{std::format("Variable '{}' does not exist", var_name)}
		);
		co_return;
	}

	const memory::Variable& variable = ctx.memory.get_variable(var_name);
	if (variable.is_constant) {
		co_yield make_good_evaluation<
			EvaluationResult>(std::cref(variable), detail::type_string_cpp<memory::RefConstVar>);
		co_return;
	}
	co_yield make_good_evaluation<
		EvaluationResult>(std::ref(variable), detail::type_string_cpp<memory::RefVar>);
}

std::generator<Evaluation> make_value_iterator(
	EvaluationContext& ctx, const ale::ast::SubscriptedVariableNode& var
)
{
	INTERPRETER_PRINT("Make a value iterator for a '{}'.", var.get_node_type());

	Evaluation res_eval = make_subscripted_variable_name(ctx, var);
	if (not res_eval) {
		INTERPRETER_PRINT("Could not make the name of the variable.");
		co_yield append_error(
			std::move(res_eval.error()),
			evaluation_error_e::Evaluation_Of_Node_Failed,
			evaluation_function_e::Iterator_Value_Subscripted_Variable,
			"Could not make name of the variable"
		);
		co_return;
	}

	const std::string& var_name = var.get_variable_name();
	if (not ctx.memory.variable_exists(var_name)) {
		co_yield make_bad_evaluation(
			Vec{evaluation_error_e::Memory_Variable_Does_Not_Exist},
			Vec{evaluation_function_e::Iterator_Value_Subscripted_Variable},
			Vec{std::format("Variable '{}' does not exist", var_name)}
		);
		co_return;
	}

	const memory::Variable& variable = ctx.memory.get_variable(var_name);
	if (variable.is_constant) {
		co_yield make_good_evaluation<
			EvaluationResult>(std::cref(variable), detail::type_string_cpp<memory::RefConstVar>);
		co_return;
	}
	co_yield make_good_evaluation<
		EvaluationResult>(std::ref(variable), detail::type_string_cpp<memory::RefVar>);
}

[[nodiscard]] static std::generator<Evaluation> unidimensional_iterator(
	EvaluationContext& ctx, const ale::ast::SequenceNode& sequence
)
{
	INTERPRETER_PRINT("Interpreting left child.");
	Evaluation first_eval = interpret_node(ctx, sequence.get_left_child());
	if (not first_eval) {
		co_yield append_error(
			std::move(first_eval.error()),
			evaluation_error_e::Evaluation_Of_Node_Failed,
			evaluation_function_e::Iterator_Value_Sequence,
			"Could not evaluate left child of sequence"
		);
		co_return;
	}

	INTERPRETER_PRINT("Interpreting right child.");
	Evaluation second_eval = interpret_node(ctx, sequence.get_right_child());
	if (not second_eval) {
		co_yield append_error(
			std::move(second_eval.error()),
			evaluation_error_e::Evaluation_Of_Node_Failed,
			evaluation_function_e::Iterator_Value_Sequence,
			"Could not evaluate right child of sequence"
		);
		co_return;
	}

	memory::WrappedAny first_w = std::move(*first_eval);
	memory::WrappedAny second_w = std::move(*second_eval);

	INTERPRETER_PRINT("Converting left value '{}'.", first_w);
	const std::optional<int64_t> first_value_w =
		detail::any_to_numeric<int64_t>(first_w);

	if (not first_value_w) {
		co_yield make_bad_evaluation(
			Vec{evaluation_error_e::Conversion_To_Numeric_Failed},
			Vec{evaluation_function_e::Iterator_Value_Sequence},
			Vec{"Evaluation of first value of sequence could not be converted "
				"to a numeric value"s}
		);
		co_return;
	}

	INTERPRETER_PRINT("Converting right value '{}'.", second_w);
	const std::optional<int64_t> second_value_w =
		detail::any_to_numeric<int64_t>(second_w);

	if (not second_value_w) {
		co_yield make_bad_evaluation(
			Vec{evaluation_error_e::Conversion_To_Numeric_Failed},
			Vec{evaluation_function_e::Iterator_Value_Sequence},
			Vec{"Evaluation of second value of sequence could not be converted "
				"to a numeric value"s}
		);
		co_return;
	}

	INTERPRETER_PRINT("First value: '{}'.", *first_value_w);
	INTERPRETER_PRINT("Second value: '{}'.", *second_value_w);

	for (int64_t i = *first_value_w; i <= *second_value_w; ++i) {
		INTERPRETER_PRINT("Next value: '{}'.", i);
		co_yield make_good_evaluation<
			EvaluationResult>(i, detail::type_string_cpp<int64_t>);
	}
}

std::generator<Evaluation> make_value_iterator(
	EvaluationContext& ctx, const ale::ast::SequenceNode& sequence
)
{
#if defined DEBUG
	assert(sequence.get_operator_type().has_value());
#endif

	INTERPRETER_PRINT(
		"Make a value iterator for a '{}' with operator type '{}'.",
		sequence.get_node_type(),
		*sequence.get_operator_type()
	);

	INTERPRETER_PRINT("Going to make the sequence execution environment.");

	Evaluation seq_env_eval =
		make_sequence_execution_environment(ctx, sequence);
	if (not seq_env_eval.has_value()) {
		co_yield append_error(
			std::move(seq_env_eval.error()),
			evaluation_error_e::Evaluation_Of_Node_Failed,
			evaluation_function_e::Iterator_Value_Sequence,
			"Could not construct sequence execution environment"
		);
		co_return;
	}

	INTERPRETER_PRINT(
		"Successfully constructed the sequence execution environment."
	);

	memory::WrappedAny seq_env_w = std::move(*seq_env_eval);
#if defined DEBUG
	INTERPRETER_PRINT("Type of result: '{}'.", seq_env_w.type);
	assert(seq_env_w.type == detail::type_string_cpp<SequenceExecutionEnvironment>);
#endif
	SequenceExecutionEnvironment seq_env =
		std::any_cast<SequenceExecutionEnvironment&&>(std::move(seq_env_w.value)
		);

	if (is_node_interpretable(*sequence.get_operator_type())) {
		INTERPRETER_PRINT("The node is interpretable, so going to interpret.");

		ctx.sequence_execution_environment = &seq_env;
		Evaluation eval = evaluate(ctx, sequence);
		ctx.sequence_execution_environment.reset();

		co_yield std::move(eval);
		co_return;
	}

	const size_t env_depth = seq_env.get_depth();
	INTERPRETER_PRINT("Environment's depth: {}.", env_depth);

	if (env_depth > 0) {
		auto gen = enumerate_values_sequence(ctx, seq_env);
		auto pos = gen.begin();
		auto end = gen.end();
		while (pos != end) {
			Evaluation eval = *pos;
			if (not eval) {
				co_yield make_bad_evaluation(
					Vec{evaluation_error_e::
							Sequence_Environment_Enumeration_Step_Failed},
					Vec{evaluation_function_e::Iterator_Value_Sequence},
					Vec{"Evaluation of node failed"s}
				);
				co_return;
			}
			co_yield std::move(eval);
			++pos;
		}
		co_return;
	}

	auto gen = unidimensional_iterator(ctx, sequence);
	auto pos = gen.begin();
	auto end = gen.end();
	while (pos != end) {
		Evaluation eval = *pos;
		if (not eval) {
			co_yield make_bad_evaluation(
				Vec{evaluation_error_e::
						Sequence_Environment_Enumeration_Step_Failed},
				Vec{evaluation_function_e::Iterator_Value_Sequence},
				Vec{"Evaluation of node failed"s}
			);
			co_return;
		}
		co_yield std::move(eval);
		++pos;
	}
}

std::generator<Evaluation> make_value_iterator(
	EvaluationContext& ctx, const ale::ast::CommaSeparatedGroupNode& comma
)
{
	INTERPRETER_PRINT(
		"Make a value iterator for a '{}'.", comma.get_node_type()
	);

	const auto& children = comma.get_children();
	for (const auto& child : children) {

		const auto t = child->get_node_type();
		if (is_node_interpretable(child.get(), t)) {
			Evaluation eval = interpret_node(ctx, child);
			if (not eval) {
				INTERPRETER_PRINT("Evaluation of node failed.");
				co_yield append_error(
					std::move(eval.error()),
					evaluation_error_e::Evaluation_Of_Node_Failed,
					evaluation_function_e::Iterator_Value_Comma_Separated_Group,
					"Evaluation of node failed"s
				);
				co_return;
			}
			co_yield std::move(eval);
		}
		else if (t == ale::ast::node_type_e::Sequence) {

			const ale::ast::SequenceNode& seq =
				*static_cast<ale::ast::SequenceNode *>(child.get());

			auto sequence_enumerate = make_value_iterator(ctx, seq);
			auto pos = sequence_enumerate.begin();
			auto end = sequence_enumerate.end();
			while (pos != end) {
				Evaluation eval = *pos;
				if (not eval) {
					co_yield make_bad_evaluation(
						Vec{evaluation_error_e::Evaluation_Of_Node_Failed},
						Vec{evaluation_function_e::
								Iterator_Value_Comma_Separated_Group},
						Vec{"Evaluation of node failed"s}
					);
					co_return;
				}
				co_yield std::move(eval);
				++pos;
			}
		}
	}
}

std::generator<Evaluation> make_value_iterator(
	EvaluationContext& ctx, const std::unique_ptr<ale::ast::Node>& node
)
{
	INTERPRETER_PRINT(
		"Make a value iterator for a node of type '{}'.", node->get_node_type()
	);

	const auto t = node->get_node_type();
	if (t == ale::ast::node_type_e::Variable) {
		const ale::ast::VariableNode& var =
			*static_cast<ale::ast::VariableNode *>(node.get());

		return make_value_iterator(ctx, var);
	}

	if (t == ale::ast::node_type_e::Subscripted_Variable) {
		const ale::ast::SubscriptedVariableNode& subsvar =
			*static_cast<ale::ast::SubscriptedVariableNode *>(node.get());

		return make_value_iterator(ctx, subsvar);
	}

	if (t == ale::ast::node_type_e::Sequence) {
		const ale::ast::SequenceNode& seq =
			*static_cast<ale::ast::SequenceNode *>(node.get());

		return make_value_iterator(ctx, seq);
	}

	if (t == ale::ast::node_type_e::Comma_Separated_Group) {
		const ale::ast::CommaSeparatedGroupNode& group =
			*static_cast<ale::ast::CommaSeparatedGroupNode *>(node.get());

		return make_value_iterator(ctx, group);
	}

	if (is_node_interpretable(t)) {
		return make_value_iterator_over_interpret(ctx, node);
	}

	return iterator_empty();
}

// -----------------------------------------------------------------------------

std::generator<Evaluation>
make_name_iterator(EvaluationContext&, const ale::ast::VariableNode& var)
{
	INTERPRETER_PRINT(
		"Make a name iterator for a node of type '{}'.", var.get_node_type()
	);

	const std::string& var_name = var.get_variable_name();
	co_yield make_good_evaluation<
		EvaluationResult>(var_name, detail::type_string_cpp<std::string>);
}

std::generator<Evaluation> make_name_iterator(
	EvaluationContext& ctx, const ale::ast::SubscriptedVariableNode& var
)
{
	INTERPRETER_PRINT(
		"Make a name iterator for a node of type '{}'.", var.get_node_type()
	);

	Evaluation eval = make_subscripted_variable_name(ctx, var);
	if (not eval.has_value()) {
		INTERPRETER_PRINT("Could not make the name of the variable.");
		co_yield append_error(
			std::move(eval.error()),
			evaluation_error_e::Evaluation_Of_Node_Failed,
			evaluation_function_e::Iterator_Name_Subscripted_Variable,
			"Could not evaluate right child of sequence"
		);
		co_return;
	}

	EvaluationResult eval_res = std::move(*eval);
#if defined DEBUG
	assert(eval_res.type == detail::type_string_cpp<std::string>);
#endif

	INTERPRETER_PRINT("Going to cast away from '{}'.", eval_res);
	auto var_name = std::any_cast<std::string&&>(std::move(eval_res.value));
	INTERPRETER_PRINT("variable name '{}'.", var_name);

	co_yield make_good_evaluation<
		EvaluationResult>(std::move(var_name), detail::type_string_cpp<std::string>);
}

std::generator<Evaluation> make_name_iterator(
	EvaluationContext& ctx, const ale::ast::SequenceNode& sequence
)
{
	INTERPRETER_PRINT(
		"Make a name iterator for a '{}' with operator type '{}'.",
		sequence.get_node_type(),
		*sequence.get_operator_type()
	);

	INTERPRETER_PRINT("Going to make the sequence execution environment.");

	Evaluation seq_eval = make_sequence_execution_environment(ctx, sequence);

	if (not seq_eval.has_value()) {
		co_yield append_error(
			std::move(seq_eval.error()),
			evaluation_error_e::Evaluation_Of_Node_Failed,
			evaluation_function_e::Iterator_Value_Sequence,
			"Could not construct sequence execution environment"
		);
		co_return;
	}

	INTERPRETER_PRINT(
		"Successfully constructed the sequence execution environment."
	);

	memory::WrappedAny seq_env_w = std::move(*seq_eval);
	SequenceExecutionEnvironment seq_env =
		std::any_cast<SequenceExecutionEnvironment&&>(std::move(seq_env_w.value)
		);

	INTERPRETER_PRINT(
		"Successfully extracted the sequence execution environment."
	);

	auto gen = enumerate_names_sequence(ctx, seq_env);
	auto pos = gen.begin();
	auto end = gen.end();
	while (pos != end) {
		Evaluation eval = *pos;
		if (not eval) {
			co_yield make_bad_evaluation(
				Vec{evaluation_error_e::
						Sequence_Environment_Enumeration_Step_Failed},
				Vec{evaluation_function_e::Iterator_Value_Sequence},
				Vec{"Evaluation of node failed"s}
			);
			co_return;
		}
		co_yield std::move(eval);
		++pos;
	}
}

std::generator<Evaluation> make_name_iterator(
	EvaluationContext& ctx, const ale::ast::CommaSeparatedGroupNode& comma
)
{
	INTERPRETER_PRINT(
		"Make a name iterator for a node of type '{}'.", comma.get_node_type()
	);

	const auto& children = comma.get_children();
	for (const auto& child : children) {

		const auto t = child->get_node_type();
		if (t == ale::ast::node_type_e::Variable) {
			const ale::ast::VariableNode& var =
				*static_cast<ale::ast::VariableNode *>(child.get());

			auto gen = make_name_iterator(ctx, var);
			auto pos = gen.begin();
			Evaluation eval = *pos;
			if (not eval) {
				co_yield append_error(
					std::move(eval.error()),
					evaluation_error_e::Evaluation_Of_Node_Failed,
					evaluation_function_e::Iterator_Name_Sequence,
					"Could not produce the name of a variable node"
				);
				co_return;
			}
			co_yield std::move(eval);
		}
		else if (t == ale::ast::node_type_e::Subscripted_Variable) {
			const ale::ast::SubscriptedVariableNode& subvar =
				*static_cast<ale::ast::SubscriptedVariableNode *>(child.get());

			auto gen = make_name_iterator(ctx, subvar);
			auto pos = gen.begin();
			Evaluation eval = *pos;
			if (not eval) {
				co_yield append_error(
					std::move(eval.error()),
					evaluation_error_e::Evaluation_Of_Node_Failed,
					evaluation_function_e::Iterator_Name_Sequence,
					"Could not produce the name of a subscripted variable node"
				);
				co_return;
			}
			co_yield std::move(eval);
		}
		else if (t == ale::ast::node_type_e::Sequence) {
			const ale::ast::SequenceNode& seq =
				*static_cast<ale::ast::SequenceNode *>(child.get());

			auto gen = make_name_iterator(ctx, seq);
			auto pos = gen.begin();
			auto end = gen.end();
			while (pos != end) {
				Evaluation eval = *pos;
				if (not eval) {
					co_yield append_error(
						std::move(eval.error()),
						evaluation_error_e::Evaluation_Of_Node_Failed,
						evaluation_function_e::Iterator_Name_Sequence,
						"Could not produce the name of a node within a sequence"
					);
					co_return;
				}
				co_yield std::move(eval);
				++pos;
			}
		}
	}
}

std::generator<Evaluation> make_name_iterator(
	EvaluationContext& ctx, const std::unique_ptr<ale::ast::Node>& node
)
{
	INTERPRETER_PRINT(
		"Make a name iterator for a node of type '{}'.", node->get_node_type()
	);

	const auto t = node->get_node_type();
	if (t == ale::ast::node_type_e::Variable) {
		const ale::ast::VariableNode& var =
			*static_cast<ale::ast::VariableNode *>(node.get());

		return make_name_iterator(ctx, var);
	}

	if (t == ale::ast::node_type_e::Subscripted_Variable) {
		const ale::ast::SubscriptedVariableNode& subsvar =
			*static_cast<ale::ast::SubscriptedVariableNode *>(node.get());

		return make_name_iterator(ctx, subsvar);
	}

	if (t == ale::ast::node_type_e::Sequence) {
		const ale::ast::SequenceNode& seq =
			*static_cast<ale::ast::SequenceNode *>(node.get());

		return make_name_iterator(ctx, seq);
	}

	if (t == ale::ast::node_type_e::Comma_Separated_Group) {
		const ale::ast::CommaSeparatedGroupNode& group =
			*static_cast<ale::ast::CommaSeparatedGroupNode *>(node.get());

		return make_name_iterator(ctx, group);
	}

	return iterator_empty();
}

} // namespace ast
} // namespace intlib
