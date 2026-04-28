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

#include <ale/ast/utils/node_is_type.hpp>
#include <ale/ast/utils/node_type_to_string.hpp>
#include <ale/ast/zero_ary_nodes/VariableNode.hpp>
#include <ale/ast/n_ary_nodes/SubscriptedVariableNode.hpp>
#include <ale/ast/n_ary_nodes/CommaSeparatedGroupNode.hpp>
#include <ale/ast/binary_nodes/SequenceNode.hpp>
#include <ale/utils/IndexIterator.hpp>

#include <intlib/logger/macros.hpp>
#include <intlib/detail/any_type.hpp>
#include <intlib/detail/any_output.hpp>
#include <intlib/detail/any_to_numeric.hpp>
#include <intlib/ast/EvaluationResult.hpp>
#include <intlib/ast/SequenceExecutionEnvironment.hpp>
#include <intlib/ast/interpretation.hpp>
#include <intlib/ast/utils/variable_names.hpp>
#include <intlib/ast/utils/sequence_execution.hpp>
#include <intlib/ast/evaluation.hpp>

namespace intlib {
namespace ast {

#define aleprln ale::logger::println

[[nodiscard]] static bool is_node_interpretable(const ale::ast::node_type_e t
) noexcept
{

	return ale::ast::is_node_numerical_literal(t) or
		   ale::ast::is_node_arithmetic(t) or ale::ast::is_node_logical(t) or
		   ale::ast::is_node_comparison(t) or
		   t == ale::ast::node_type_e::Negative or
		   t == ale::ast::node_type_e::Positive or
		   t == ale::ast::node_type_e::Negation or
		   t == ale::ast::node_type_e::Variable or
		   t == ale::ast::node_type_e::Subscripted_Variable;
}

template <typename node_t>
[[nodiscard]] static bool is_node_interpretable(
	const node_t *node, const ale::ast::node_type_e t
) noexcept
{

	if (t == ale::ast::node_type_e::Sequence) {
		const ale::ast::SequenceNode *seq =
			static_cast<const ale::ast::SequenceNode *>(node);
		return is_node_interpretable(*seq->get_operator_type());
	}
	return is_node_interpretable(t);
}

std::generator<EvaluationResult>
make_value_iterator(EvaluationContext& ctx, const ale::ast::VariableNode& var)
{
	INTERPRETER_PRINT(
		aleprln, "Make a value iterator for a '{}'.", var.get_node_type()
	);

	const std::string& var_name = var.get_variable_name();
	if (not ctx.memory.variable_exists(var_name)) {
		co_yield make_bad_evaluation_result(
			Vec{evaluation_error_e::Memory_Variable_Does_Not_Exist},
			Vec{evaluation_function_e::Iterator_Value_Subscripted_Variable},
			Vec{std::format("Variable '{}' does not exist", var_name)}
		);
		co_return;
	}

	const memory::VariableValue& variable = ctx.memory.get_variable(var_name);
	co_yield make_good_evaluation_result<std::any>(variable.value_w);
}

std::generator<EvaluationResult> make_value_iterator(
	EvaluationContext& ctx, const ale::ast::SubscriptedVariableNode& var
)
{
	INTERPRETER_PRINT(
		aleprln, "Make a value iterator for a '{}'.", var.get_node_type()
	);

	EvaluationResult res = make_subscripted_variable_name(ctx, var);
	if (not res) {
		INTERPRETER_PRINT(aleprln, "Could not make the name of the variable.");
		co_yield append_error(
			std::move(res.error()),
			evaluation_error_e::Evaluation_Of_Node_Failed,
			evaluation_function_e::Iterator_Value_Subscripted_Variable,
			"Could not make name of the variable"
		);
		co_return;
	}

	const std::string& var_name = var.get_variable_name();
	if (not ctx.memory.variable_exists(var_name)) {
		co_yield make_bad_evaluation_result(
			Vec{evaluation_error_e::Memory_Variable_Does_Not_Exist},
			Vec{evaluation_function_e::Iterator_Value_Subscripted_Variable},
			Vec{std::format("Variable '{}' does not exist", var_name)}
		);
		co_return;
	}

	const memory::VariableValue& variable = ctx.memory.get_variable(var_name);
	co_yield make_good_evaluation_result<std::any>(variable.value_w);
}

[[nodiscard]] static std::generator<EvaluationResult> unidimensional_iterator(
	EvaluationContext& ctx, const ale::ast::SequenceNode& sequence
)
{
	INTERPRETER_PRINT(aleprln, "Interpreting left child.");
	auto first_res = interpret_node(ctx, sequence.get_left_child());
	if (not first_res) {
		co_yield append_error(
			std::move(first_res.error()),
			evaluation_error_e::Evaluation_Of_Node_Failed,
			evaluation_function_e::Iterator_Value_Sequence,
			"Could not evaluate left child of sequence"
		);
		co_return;
	}

	INTERPRETER_PRINT(aleprln, "Interpreting right child.");
	auto second_res = interpret_node(ctx, sequence.get_right_child());
	if (not second_res) {
		co_yield append_error(
			std::move(second_res.error()),
			evaluation_error_e::Evaluation_Of_Node_Failed,
			evaluation_function_e::Iterator_Value_Sequence,
			"Could not evaluate right child of sequence"
		);
		co_return;
	}

	std::any first_value_w = std::move(*first_res);
	std::any second_value_w = std::move(*second_res);

	INTERPRETER_PRINT(
		aleprln, "Converting left value '{}'.", detail::AnyView{first_value_w}
	);
	const std::optional<int64_t> first_val_w =
		detail::any_to_numeric<int64_t>(first_value_w);

	if (not first_val_w) {
		co_yield make_bad_evaluation_result(
			Vec{evaluation_error_e::Conversion_To_Numeric_Failed},
			Vec{evaluation_function_e::Iterator_Value_Sequence},
			Vec{"Evaluation of first value of sequence could not be converted "
				"to a numeric value"s}
		);
		co_return;
	}

	INTERPRETER_PRINT(
		aleprln, "Converting right value '{}'.", detail::AnyView{second_value_w}
	);
	const std::optional<int64_t> second_val_w =
		detail::any_to_numeric<int64_t>(second_value_w);
	if (not second_val_w) {
		co_yield make_bad_evaluation_result(
			Vec{evaluation_error_e::Conversion_To_Numeric_Failed},
			Vec{evaluation_function_e::Iterator_Value_Sequence},
			Vec{"Evaluation of second value of sequence could not be converted "
				"to a numeric value"s}
		);
		co_return;
	}

	INTERPRETER_PRINT(aleprln, "First value: '{}'.", *first_val_w);
	INTERPRETER_PRINT(aleprln, "Second value: '{}'.", *second_val_w);

	for (int64_t i = *first_val_w; i <= *second_val_w; ++i) {
		INTERPRETER_PRINT(aleprln, "Making next value: '{}'.", i);
		co_yield make_good_evaluation_result<int64_t>(i);
	}
}

std::generator<EvaluationResult> make_value_iterator(
	EvaluationContext& ctx, const ale::ast::SequenceNode& sequence
)
{
#if defined DEBUG
	assert(sequence.get_operator_type().has_value());
#endif

	INTERPRETER_PRINT(
		aleprln,
		"Make a value iterator for a '{}' with operator type '{}'.",
		sequence.get_node_type(),
		*sequence.get_operator_type()
	);

	if (is_node_interpretable(*sequence.get_operator_type())) {
		EvaluationResult res = evaluate(ctx, sequence);
		co_yield append_error(
			std::move(res.error()),
			evaluation_error_e::Evaluation_Of_Node_Failed,
			evaluation_function_e::Iterator_Value_Sequence,
			"Could not evaluate sequence"
		);
		co_return;
	}

	INTERPRETER_PRINT(
		aleprln, "Going to make the sequence execution environment."
	);

	auto res = make_sequence_execution_environment(ctx, sequence);

	INTERPRETER_PRINT(
		aleprln, "Successfully constructed the sequence execution environment."
	);

	if (not res.has_value()) {
		co_yield append_error(
			std::move(res.error()),
			evaluation_error_e::Evaluation_Of_Node_Failed,
			evaluation_function_e::Iterator_Value_Sequence,
			"Could not construct sequence execution environment"
		);
		co_return;
	}

	SequenceExecutionEnvironment seq_env =
		std::any_cast<SequenceExecutionEnvironment&&>(std::move(*res));

	const size_t env_depth = seq_env.get_depth();
	INTERPRETER_PRINT(aleprln, "Environment's depth: {}.", env_depth);

	if (env_depth > 0) {
		auto gen = enumerate_values_sequence(ctx, seq_env);
		auto pos = gen.begin();
		auto end = gen.end();
		while (pos != end) {
			EvaluationResult iter_res = *pos;
			if (not iter_res) {
				co_yield make_bad_evaluation_result(
					Vec{evaluation_error_e::
							Sequence_Environment_Enumeration_Step_Failed},
					Vec{evaluation_function_e::Iterator_Value_Sequence},
					Vec{"Evaluation of node failed"s}
				);
				co_return;
			}
			co_yield std::move(iter_res);
			++pos;
		}
		co_return;
	}

	auto gen = unidimensional_iterator(ctx, sequence);
	auto pos = gen.begin();
	auto end = gen.end();
	while (pos != end) {
		EvaluationResult iter_res = *pos;
		if (not iter_res) {
			co_yield make_bad_evaluation_result(
				Vec{evaluation_error_e::
						Sequence_Environment_Enumeration_Step_Failed},
				Vec{evaluation_function_e::Iterator_Value_Sequence},
				Vec{"Evaluation of node failed"s}
			);
			co_return;
		}
		co_yield std::move(iter_res);
		++pos;
	}
}

std::generator<EvaluationResult> make_value_iterator(
	EvaluationContext& ctx, const ale::ast::CommaSeparatedGroupNode& comma
)
{
	INTERPRETER_PRINT(
		aleprln, "Make a value iterator for a '{}'.", comma.get_node_type()
	);

	const auto& children = comma.get_children();
	for (const auto& child : children) {

		const auto t = child->get_node_type();
		if (is_node_interpretable(child.get(), t)) {
			EvaluationResult res = interpret_node(ctx, child);
			if (not res) {
				INTERPRETER_PRINT(aleprln, "Evaluation of node failed.");
				co_yield append_error(
					std::move(res.error()),
					evaluation_error_e::Evaluation_Of_Node_Failed,
					evaluation_function_e::Iterator_Value_Comma_Separated_Group,
					"Evaluation of node failed"s
				);
				co_return;
			}
			co_yield std::move(res);
		}
		else if (t == ale::ast::node_type_e::Sequence) {

			const ale::ast::SequenceNode& seq =
				*static_cast<ale::ast::SequenceNode *>(child.get());

			auto sequence_enumerate = make_value_iterator(ctx, seq);
			auto pos = sequence_enumerate.begin();
			auto end = sequence_enumerate.end();
			while (pos != end) {
				EvaluationResult res = *pos;
				if (not res) {
					co_yield make_bad_evaluation_result(
						Vec{evaluation_error_e::Evaluation_Of_Node_Failed},
						Vec{evaluation_function_e::
								Iterator_Value_Comma_Separated_Group},
						Vec{"Evaluation of node failed"s}
					);
					co_return;
				}
				co_yield std::move(res);
				++pos;
			}
		}
	}
}

std::generator<EvaluationResult> make_value_iterator(
	EvaluationContext& ctx, const std::unique_ptr<ale::ast::Node>& node
)
{
	INTERPRETER_PRINT(
		aleprln,
		"Make a value iterator for a node of type '{}'.",
		node->get_node_type()
	);

	const auto t = node->get_node_type();
	if (t == ale::ast::node_type_e::Variable) {
		const ale::ast::VariableNode& var =
			*static_cast<ale::ast::VariableNode *>(node.get());

		auto gen = make_value_iterator(ctx, var);
		auto pos = gen.begin();
		co_yield std::move(*pos);
		co_return;
	}

	if (t == ale::ast::node_type_e::Subscripted_Variable) {
		const ale::ast::SubscriptedVariableNode& subsvar =
			*static_cast<ale::ast::SubscriptedVariableNode *>(node.get());

		auto gen = make_value_iterator(ctx, subsvar);
		auto pos = gen.begin();
		co_yield std::move(*pos);
		co_return;
	}

	if (t == ale::ast::node_type_e::Sequence) {
		const ale::ast::SequenceNode& seq =
			*static_cast<ale::ast::SequenceNode *>(node.get());

		auto gen = make_value_iterator(ctx, seq);
		auto pos = gen.begin();
		auto end = gen.end();
		while (pos != end) {
			co_yield std::move(*pos);
			++pos;
		}
		co_return;
	}

	if (t == ale::ast::node_type_e::Comma_Separated_Group) {
		const ale::ast::CommaSeparatedGroupNode& group =
			*static_cast<ale::ast::CommaSeparatedGroupNode *>(node.get());

		auto gen = make_value_iterator(ctx, group);
		auto pos = gen.begin();
		auto end = gen.end();
		while (pos != end) {
			co_yield std::move(*pos);
			++pos;
		}
		co_return;
	}

	if (is_node_interpretable(t)) {
		co_yield interpret_node(ctx, node);
		co_return;
	}
}

// -----------------------------------------------------------------------------

std::generator<EvaluationResult>
make_name_iterator(EvaluationContext&, const ale::ast::VariableNode& var)
{
	INTERPRETER_PRINT(
		aleprln,
		"Make a name iterator for a node of type '{}'.",
		var.get_node_type()
	);

	const std::string& var_name = var.get_variable_name();
	co_yield make_good_evaluation_result<const std::string&>(var_name);
}

std::generator<EvaluationResult> make_name_iterator(
	EvaluationContext& ctx, const ale::ast::SubscriptedVariableNode& var
)
{
	INTERPRETER_PRINT(
		aleprln,
		"Make a name iterator for a node of type '{}'.",
		var.get_node_type()
	);

	EvaluationResult res = make_subscripted_variable_name(ctx, var);
	if (not res.has_value()) {
		INTERPRETER_PRINT(aleprln, "Could not make the name of the variable.");
		co_yield append_error(
			std::move(res.error()),
			evaluation_error_e::Evaluation_Of_Node_Failed,
			evaluation_function_e::Iterator_Name_Subscripted_Variable,
			"Could not evaluate right child of sequence"
		);
		co_return;
	}

	auto var_name = std::any_cast<std::string&&>(std::move(*res));
	co_yield make_good_evaluation_result<std::string>(std::move(var_name));
}

std::generator<EvaluationResult> make_name_iterator(
	EvaluationContext& ctx, const ale::ast::SequenceNode& sequence
)
{
	INTERPRETER_PRINT(
		aleprln,
		"Make a name iterator for a '{}' with operator type '{}'.",
		sequence.get_node_type(),
		*sequence.get_operator_type()
	);

	INTERPRETER_PRINT(
		aleprln, "Going to make the sequence execution environment."
	);

	auto res = make_sequence_execution_environment(ctx, sequence);

	INTERPRETER_PRINT(
		aleprln, "Successfully constructed the sequence execution environment."
	);

	if (not res.has_value()) {
		co_yield append_error(
			std::move(res.error()),
			evaluation_error_e::Evaluation_Of_Node_Failed,
			evaluation_function_e::Iterator_Value_Sequence,
			"Could not construct sequence execution environment"
		);
		co_return;
	}

	SequenceExecutionEnvironment seq_env =
		std::any_cast<SequenceExecutionEnvironment&&>(std::move(*res));

	auto gen = enumerate_names_sequence(ctx, seq_env);
	auto pos = gen.begin();
	auto end = gen.end();
	while (pos != end) {
		EvaluationResult iter_res = *pos;
		if (not iter_res) {
			co_yield make_bad_evaluation_result(
				Vec{evaluation_error_e::
						Sequence_Environment_Enumeration_Step_Failed},
				Vec{evaluation_function_e::Iterator_Value_Sequence},
				Vec{"Evaluation of node failed"s}
			);
			co_return;
		}
		co_yield std::move(iter_res);
		++pos;
	}
}

std::generator<EvaluationResult> make_name_iterator(
	EvaluationContext& ctx, const ale::ast::CommaSeparatedGroupNode& comma
)
{
	INTERPRETER_PRINT(
		aleprln,
		"Make a name iterator for a node of type '{}'.",
		comma.get_node_type()
	);

	const auto& children = comma.get_children();
	for (const auto& child : children) {

		const auto t = child->get_node_type();
		if (t == ale::ast::node_type_e::Variable) {
			const ale::ast::VariableNode& var =
				*static_cast<ale::ast::VariableNode *>(child.get());

			auto gen = make_name_iterator(ctx, var);
			auto pos = gen.begin();
			EvaluationResult res = *pos;
			if (not res) {
				co_yield append_error(
					std::move(res.error()),
					evaluation_error_e::Evaluation_Of_Node_Failed,
					evaluation_function_e::Iterator_Name_Sequence,
					"Could not produce the name of a variable node"
				);
				co_return;
			}
			co_yield std::move(res);
		}
		else if (t == ale::ast::node_type_e::Subscripted_Variable) {
			const ale::ast::SubscriptedVariableNode& subvar =
				*static_cast<ale::ast::SubscriptedVariableNode *>(child.get());

			auto gen = make_name_iterator(ctx, subvar);
			auto pos = gen.begin();
			EvaluationResult res = *pos;
			if (not res) {
				co_yield append_error(
					std::move(res.error()),
					evaluation_error_e::Evaluation_Of_Node_Failed,
					evaluation_function_e::Iterator_Name_Sequence,
					"Could not produce the name of a subscripted variable node"
				);
				co_return;
			}
			co_yield std::move(res);
		}
		else if (t == ale::ast::node_type_e::Sequence) {
			const ale::ast::SequenceNode& seq =
				*static_cast<ale::ast::SequenceNode *>(child.get());

			auto gen = make_name_iterator(ctx, seq);
			auto pos = gen.begin();
			auto end = gen.end();
			while (pos != end) {
				EvaluationResult res = *pos;
				if (not res) {
					co_yield append_error(
						std::move(res.error()),
						evaluation_error_e::Evaluation_Of_Node_Failed,
						evaluation_function_e::Iterator_Name_Sequence,
						"Could not produce the name of a node within a sequence"
					);
					co_return;
				}
				co_yield std::move(res);
				++pos;
			}
		}
	}
}

std::generator<EvaluationResult> make_name_iterator(
	EvaluationContext& ctx, const std::unique_ptr<ale::ast::Node>& node
)
{
	INTERPRETER_PRINT(
		aleprln,
		"Make a name iterator for a node of type '{}'.",
		node->get_node_type()
	);

	const auto t = node->get_node_type();
	if (t == ale::ast::node_type_e::Variable) {
		const ale::ast::VariableNode& var =
			*static_cast<ale::ast::VariableNode *>(node.get());

		auto gen = make_name_iterator(ctx, var);
		auto pos = gen.begin();
		co_yield std::move(*pos);
		co_return;
	}

	if (t == ale::ast::node_type_e::Subscripted_Variable) {
		const ale::ast::SubscriptedVariableNode& subsvar =
			*static_cast<ale::ast::SubscriptedVariableNode *>(node.get());

		auto gen = make_name_iterator(ctx, subsvar);
		auto pos = gen.begin();
		co_yield std::move(*pos);
		co_return;
	}

	if (t == ale::ast::node_type_e::Sequence) {
		const ale::ast::SequenceNode& seq =
			*static_cast<ale::ast::SequenceNode *>(node.get());

		auto gen = make_name_iterator(ctx, seq);
		auto pos = gen.begin();
		auto end = gen.end();
		while (pos != end) {
			co_yield std::move(*pos);
			++pos;
		}
		co_return;
	}

	if (t == ale::ast::node_type_e::Comma_Separated_Group) {
		const ale::ast::CommaSeparatedGroupNode& group =
			*static_cast<ale::ast::CommaSeparatedGroupNode *>(node.get());

		auto gen = make_name_iterator(ctx, group);
		auto pos = gen.begin();
		auto end = gen.end();
		while (pos != end) {
			co_yield std::move(*pos);
			++pos;
		}
		co_return;
	}
}

} // namespace ast
} // namespace intlib
