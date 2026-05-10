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
#include <optional>
#include <string>
#include <any>
using namespace std::string_literals;

#include <ale/ast/binary_nodes/DeclarationNode.hpp>
#if defined ALE_LOGGING_MESSAGES
#include <ale/ast/utils/node_type_to_string.hpp>
#endif

#include <intlib/detail/any_conversion.hpp>
#include <intlib/ast/EvaluationContext.hpp>
#include <intlib/ast/Evaluation.hpp>
#include <intlib/ast/interpretation.hpp>
#include <intlib/ast/utils/iterators.hpp>
#include <intlib/memory/utils/unwrap.hpp>
#include <intlib/logger/macros.hpp>
#include <intlib/ast/utils/evaluation_result_to_string.hpp>
#include <intlib/ast/utils/sequence_execution.hpp>
#if defined ALE_LOGGING_MESSAGES
#include <intlib/ast/utils/evaluation_error_to_string.hpp>
#endif

namespace intlib {
namespace ast {

[[nodiscard]] static Evaluation declare_variable(
	EvaluationContext& ctx,
	std::string&& var_name,
	const std::string_view var_type
)
{
	INTERPRETER_ENTER_AST_FUNCTION;

#if defined ALE_LOGGING_MESSAGES
	const std::string variable_name_copy = var_name;
#endif

	INTERPRETER_PRINT("Going to declare variable '{}'.", variable_name_copy);

	if (ctx.memory.variable_exists_shallow(var_name)) {
		INTERPRETER_PRINT("Attempt to redeclare variable {}.", var_name);
		return make_bad_evaluation(
			Vec{evaluation_error_e::Memory_Variable_Already_Exists},
			Vec{evaluation_function_e::Declaration},
			Vec{std::format("Attempted to redeclare variable '{}'", var_name)}
		);
	}

	// This is a 'declare' node.
	ctx.memory.declare_variable(
		std::move(var_name),
		{.value = {}, .type = detail::type_string_cpp<void>},
		var_type
	);
	return make_good_evaluation<EvaluationResult>();
}

[[nodiscard]] static Evaluation declare_variable(
	EvaluationContext& ctx,
	const ale::ast::node_type_e t,
	std::string&& var_name,
	const std::string_view var_type,
	const memory::WrappedAny& value_w
)
{
	INTERPRETER_ENTER_AST_FUNCTION;

#if defined ALE_LOGGING_MESSAGES
	const std::string variable_name_copy = var_name;
#endif

	INTERPRETER_PRINT("Going to declare variable '{}'.", variable_name_copy);

	if (ctx.memory.variable_exists_shallow(var_name)) {
		INTERPRETER_PRINT("Attempt to redeclare variable {}.", var_name);
		return make_bad_evaluation(
			Vec{evaluation_error_e::Memory_Variable_Already_Exists},
			Vec{evaluation_function_e::Declaration},
			Vec{std::format("Attempted to redeclare variable '{}'", var_name)}
		);
	}

	memory::WrappedAny value_conv_w =
		detail::convert_to_ale_type(value_w, var_type);

	if (value_conv_w.type == detail::type_string_cpp<void>) {
		INTERPRETER_PRINT(
			"Could not convert value '{}' to a value of type '{}'.",
			value_w,
			var_type
		);
		return make_bad_evaluation(
			Vec{evaluation_error_e::Conversion_Generic},
			Vec{evaluation_function_e::Declaration},
			Vec{std::format(
				"Could not convert value '{}' to a value of type '{}'",
				value_w,
				var_type
			)}
		);
	}

	INTERPRETER_PRINT(
		"Value after conversion to '{}' is: '{}'.", var_type, value_conv_w
	);

	if (t == ale::ast::node_type_e::Declaration_Const) {
		ctx.memory.declare_constant_variable(
			std::move(var_name), std::move(value_conv_w), var_type
		);
	}
	else {
		ctx.memory.declare_variable(
			std::move(var_name), std::move(value_conv_w), var_type
		);
	}

	INTERPRETER_PRINT(
		"Successfully declared variable '{}'.", variable_name_copy
	);

	return make_good_evaluation<EvaluationResult>();
}

[[nodiscard]] static Evaluation declare_multiple_values_rhs(
	EvaluationContext& ctx,
	const ale::ast::DeclarationNode& decl,
	const ale::ast::node_type_e decl_t,
	const std::unique_ptr<ale::ast::Node>& left_child,
	const std::unique_ptr<ale::ast::Node>& right_child
)
{
	INTERPRETER_ENTER_AST_FUNCTION;

	INTERPRETER_PRINT("Make iterator for the left hand side.");

	EvaluationContext lhs_ctx = new_evaluation_context(ctx.memory);
	auto var_iter = make_name_iterator(lhs_ctx, left_child);
	auto var_iter_pos = var_iter.begin();
	auto var_iter_end = var_iter.end();

	INTERPRETER_PRINT("Make iterator for the left hand side.");

	EvaluationContext rhs_ctx = new_evaluation_context(ctx.memory);
	auto value_iter = make_value_iterator(rhs_ctx, right_child);
	auto value_iter_pos = value_iter.begin();
	auto value_iter_end = value_iter.end();

	while (var_iter_pos != var_iter_end and value_iter_pos != value_iter_end) {
		INTERPRETER_PRINT("Going to declare a variable.");

		Evaluation var_eval = *var_iter_pos;
		if (not var_eval) {
			INTERPRETER_PRINT(
				"Something went wrong when retrieving the next variable."
			);
			INTERPRETER_PRINT("Error: '{}'", var_eval.error().errors.at(0));
			return append_error(
				std::move(var_eval.error()),
				evaluation_error_e::List_Iteration,
				evaluation_function_e::Declaration,
				"Something went wrong when retrieving the next variable"
			);
		}

		Evaluation value_eval = *value_iter_pos;
		if (not value_eval) {
			INTERPRETER_PRINT(
				"Something went wrong when computing the next value."
			);
			INTERPRETER_PRINT("Error: '{}'", value_eval.error().errors.at(0));
			return append_error(
				std::move(value_eval.error()),
				evaluation_error_e::List_Iteration,
				evaluation_function_e::Declaration,
				"Something went wrong when retrieving the next value"
			);
		}

		memory::WrappedAny value_w = std::move(*value_eval);
		const memory::WrappedAny *actual_value_w = memory::unwrap_out(value_w);

		memory::WrappedAny name_w = std::move(*var_eval);
#if defined DEBUG
		INTERPRETER_PRINT("Container of variable name: {}.", name_w);
		assert(name_w.type == detail::type_string_cpp<std::string>);
#endif
		auto name = std::any_cast<std::string&&>(std::move(name_w.value));
		const std::string_view var_type = decl.get_variable_type();

		INTERPRETER_PRINT("Of name:  '{}'.", name);
		INTERPRETER_PRINT("Of type:  '{}'.", var_type);
		INTERPRETER_PRINT("Of value: '{}'.", *actual_value_w);

		Evaluation declaration_eval = declare_variable(
			ctx, decl_t, std::move(name), var_type, *actual_value_w
		);
		if (not declaration_eval) {
			return append_error(
				std::move(declaration_eval.error()),
				evaluation_error_e::Declaration_Of_Variable,
				evaluation_function_e::Declaration,
				"Something went wrong when declaring a variable"
			);
		}

		++value_iter_pos;
		++var_iter_pos;
	}

	if (var_iter_pos != var_iter_end) {
		INTERPRETER_PRINT("Too many values in the right hand side");
		return make_bad_evaluation(
			Vec{evaluation_error_e::Overfull_Right_Hand_Side_Values},
			Vec{evaluation_function_e::Declaration},
			Vec{"Too many values in the right hand side"s}
		);
	}

	if (value_iter_pos != value_iter_end) {
		INTERPRETER_PRINT("Too many values in the left hand side");
		return make_bad_evaluation(
			Vec{evaluation_error_e::Overfull_Left_Hand_Side_Values},
			Vec{evaluation_function_e::Declaration},
			Vec{"Too many values in the left hand side"s}
		);
	}

	return make_good_evaluation<EvaluationResult>();
}

[[nodiscard]] static Evaluation declare_single_values_rhs(
	EvaluationContext& ctx,
	const ale::ast::DeclarationNode& decl,
	const ale::ast::node_type_e decl_t,
	const std::unique_ptr<ale::ast::Node>& left_child,
	const std::unique_ptr<ale::ast::Node>& right_child
)
{
	INTERPRETER_ENTER_AST_FUNCTION;

	INTERPRETER_PRINT("Make iterator for the left hand side.");

	EvaluationContext lhs_ctx = new_evaluation_context(ctx.memory);
	auto var_iter = make_name_iterator(lhs_ctx, left_child);
	auto var_iter_pos = var_iter.begin();
	auto var_iter_end = var_iter.end();

	INTERPRETER_PRINT("Make iterator for the right hand side.");

	EvaluationContext rhs_ctx = new_evaluation_context(ctx.memory);
	Evaluation rhs_eval = interpret_node(rhs_ctx, right_child);
	if (not rhs_eval) {
		return append_error(
			std::move(rhs_eval.error()),
			evaluation_error_e::Evaluation_Of_Node_Failed,
			evaluation_function_e::Declaration,
			"Something went wrong when evaluating a node"
		);
	}

	INTERPRETER_PRINT("Going to retrieve the actual rhs value.");

	const memory::WrappedAny rhs_w = std::move(*rhs_eval);
	const memory::WrappedAny *actual_rhs_w = memory::unwrap_out(rhs_w);

	while (var_iter_pos != var_iter_end) {
		INTERPRETER_PRINT("Going to declare a variable.");

		Evaluation var_eval = *var_iter_pos;
		if (not var_eval) {
			INTERPRETER_PRINT(
				"Something went wrong when retrieving the next variable."
			);
			INTERPRETER_PRINT("Error: '{}'", var_eval.error().errors.at(0));
			return append_error(
				std::move(var_eval.error()),
				evaluation_error_e::List_Iteration,
				evaluation_function_e::Declaration,
				"Something went wrong when retrieving the next variable"
			);
		}

		INTERPRETER_PRINT("Going to retrieve the name of the variable.");

		memory::WrappedAny name_w = std::move(*var_eval);

		auto name = std::any_cast<std::string&&>(std::move(name_w.value));
		const std::string_view var_type = decl.get_variable_type();

		INTERPRETER_PRINT("Of name:  '{}'.", name);
		INTERPRETER_PRINT("Of type:  '{}'.", var_type);
		INTERPRETER_PRINT("Of value: '{}'.", *actual_rhs_w);

		Evaluation declaration_eval = declare_variable(
			ctx, decl_t, std::move(name), var_type, *actual_rhs_w
		);
		if (not declaration_eval) {
			return append_error(
				std::move(declaration_eval.error()),
				evaluation_error_e::Declaration_Of_Variable,
				evaluation_function_e::Declaration,
				"Something went wrong when retrieving the next variable"
			);
		};
		
		++var_iter_pos;
	}

	return make_good_evaluation<EvaluationResult>();
}

Evaluation
evaluate(EvaluationContext& ctx, const ale::ast::DeclarationNode& decl)
{
	INTERPRETER_ENTER_AST_FUNCTION;

	const auto decl_t = decl.get_node_type();
	const auto& lhs = decl.get_left_child();
#if defined DEBUG
	assert(lhs != nullptr);
#endif

	if (decl_t == ale::ast::node_type_e::Declaration_Declare) {
		INTERPRETER_PRINT("Make iterator for the left hand side.");

		auto var_iter = make_name_iterator(ctx, lhs);
		auto var_iter_pos = var_iter.begin();
		auto var_iter_end = var_iter.end();

		while (var_iter_pos != var_iter_end) {
			Evaluation eval = *var_iter_pos;
			if (not eval) {
				return append_error(
					std::move(eval.error()),
					evaluation_error_e::List_Iteration,
					evaluation_function_e::Declaration,
					"Something went wrong when retrieving the next variable"
				);
			}

			memory::WrappedAny name_w = std::move(*eval);
#if defined DEBUG
			assert(name_w.type == detail::type_string_cpp<std::string>);
#endif
			auto name = std::any_cast<std::string&&>(std::move(name_w.value));
			const std::string_view var_type = decl.get_variable_type();

			auto declaration_eval =
				declare_variable(ctx, std::move(name), var_type);

			if (not declaration_eval) {
				return append_error(
					std::move(declaration_eval.error()),
					evaluation_error_e::Declaration_Of_Variable,
					evaluation_function_e::Declaration,
					"Something went wrong when declaring a variable"
				);
			}
			++var_iter_pos;
		}
		return make_good_evaluation<EvaluationResult>();
	}

	const auto& rhs = decl.get_right_child();
#if defined DEBUG
	assert(rhs != nullptr);
#endif
	const auto right_t = rhs->get_node_type();

	bool is_rhs_single_value = true;
	if (right_t == ale::ast::node_type_e::Comma_Separated_Group) {
		is_rhs_single_value = false;
	}
	else if (right_t == ale::ast::node_type_e::Sequence) {
		const auto& seq = *static_cast<ale::ast::SequenceNode *>(rhs.get());
		is_rhs_single_value = *seq.get_operator_type() !=
							  ale::ast::node_type_e::Comma_Separated_Group;
	}

	if (is_rhs_single_value) {
		return declare_single_values_rhs(ctx, decl, decl_t, lhs, rhs);
	}
	
	return declare_multiple_values_rhs(ctx, decl, decl_t, lhs, rhs);
}

} // namespace ast
} // namespace intlib
