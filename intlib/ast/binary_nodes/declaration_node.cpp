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
#include <ranges>
#include <any>
using namespace std::string_literals;

#include <ale/ast/binary_nodes/DeclarationNode.hpp>
#include <ale/utils/IndexIterator.hpp>

#include <intlib/logger/macros.hpp>
#include <intlib/detail/any_type.hpp>
#include <intlib/detail/any_output.hpp>
#include <intlib/detail/any_conversion.hpp>
#include <intlib/ast/EvaluationContext.hpp>
#include <intlib/ast/EvaluationResult.hpp>
#include <intlib/ast/interpretation.hpp>
#include <intlib/ast/utils/iterators.hpp>
#include <intlib/ast/utils/evaluation_error_to_string.hpp>

namespace intlib {
namespace ast {

#define aleprln ale::logger::println

[[nodiscard]] static EvaluationResult declare_variable(
	EvaluationContext& ctx,
	std::string&& var_name,
	const std::string_view var_type
)
{
	INTERPRETER_ENTER_AST_FUNCTION(aleprln);

#if defined ALE_LOGGING_MESSAGES
	const std::string variable_name_copy = var_name;
#endif

	INTERPRETER_PRINT(
		aleprln, "Going to declare variable '{}'.", variable_name_copy
	);

	if (ctx.memory.variable_exists_shallow(var_name)) {
		INTERPRETER_PRINT(
			aleprln, "Attempt to redeclare variable {}.", var_name
		);
		return make_bad_evaluation_result(
			Vec{evaluation_error_e::Memory_Variable_Already_Exists},
			Vec{evaluation_function_e::Declaration},
			Vec{std::format("Attempted to redeclare variable '{}'", var_name)}
		);
	}

	// This is a 'declare' node.
	ctx.memory.declare_variable(std::move(var_name), {}, var_type);
	return make_good_evaluation_result<std::any>();
}

[[nodiscard]] static EvaluationResult declare_variable(
	EvaluationContext& ctx,
	const ale::ast::node_type_e t,
	std::string&& var_name,
	const std::string_view var_type,
	const std::any& value_w
)
{
	INTERPRETER_ENTER_AST_FUNCTION(aleprln);

#if defined ALE_LOGGING_MESSAGES
	const std::string variable_name_copy = var_name;
#endif

	INTERPRETER_PRINT(
		aleprln, "Going to declare variable '{}'.", variable_name_copy
	);

	if (ctx.memory.variable_exists_shallow(var_name)) {
		INTERPRETER_PRINT(
			aleprln, "Attempt to redeclare variable {}.", var_name
		);
		return make_bad_evaluation_result(
			Vec{evaluation_error_e::Memory_Variable_Already_Exists},
			Vec{evaluation_function_e::Declaration},
			Vec{std::format("Attempted to redeclare variable '{}'", var_name)}
		);
	}

	std::any value_conv_w = detail::any_to_ale_type(value_w, var_type);

	if (detail::holds_cpp_type<void>(value_conv_w)) {
		INTERPRETER_PRINT(
			aleprln,
			"Could not convert value '{}' to a value of type '{}'.",
			detail::AnyView{value_w},
			var_type
		);
		return make_bad_evaluation_result(
			Vec{evaluation_error_e::Conversion_Generic},
			Vec{evaluation_function_e::Declaration},
			Vec{std::format(
				"Could not convert value '{}' to a value of type '{}'",
				detail::AnyView{value_w},
				var_type
			)}
		);
	}

	INTERPRETER_PRINT(
		aleprln,
		"Value after conversion to '{}' is: '{}'.",
		var_type,
		detail::AnyView{value_conv_w}
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
		aleprln, "Successfully declared variable '{}'.", variable_name_copy
	);

	return make_good_evaluation_result<std::any>();
}

[[nodiscard]] static EvaluationResult declare_multiple_values_rhs(
	EvaluationContext& ctx,
	const ale::ast::DeclarationNode& decl,
	const ale::ast::node_type_e decl_t,
	const std::unique_ptr<ale::ast::Node>& left_child,
	const std::unique_ptr<ale::ast::Node>& right_child
)
{
	auto var_iter = make_name_iterator(ctx, left_child);
	auto var_iter_pos = var_iter.begin();
	auto var_iter_end = var_iter.end();

	auto value_iter = make_value_iterator(ctx, right_child);
	auto value_iter_pos = value_iter.begin();
	auto value_iter_end = value_iter.end();

	while (var_iter_pos != var_iter_end and value_iter_pos != value_iter_end) {
		INTERPRETER_PRINT(aleprln, "Going to declare a variable.");

		EvaluationResult var_res = *var_iter_pos;
		if (not var_res) {
			INTERPRETER_PRINT(
				aleprln,
				"Something went wrong when retrieving the next variable."
			);
			INTERPRETER_PRINT(
				aleprln, "Error: '{}'", var_res.error().errors.at(0)
			);
			return append_error(
				std::move(var_res.error()),
				evaluation_error_e::List_Iteration,
				evaluation_function_e::Declaration,
				"Something went wrong when retrieving the next variable"
			);
		}

		EvaluationResult value_res = *value_iter_pos;
		if (not value_res) {
			INTERPRETER_PRINT(
				aleprln, "Something went wrong when computing the next value."
			);
			INTERPRETER_PRINT(
				aleprln, "Error: '{}'", value_res.error().errors.at(0)
			);
			return append_error(
				std::move(value_res.error()),
				evaluation_error_e::List_Iteration,
				evaluation_function_e::Declaration,
				"Something went wrong when retrieving the next value"
			);
		}
		std::any value_w = std::move(*value_res);
		const std::any *actual_value_w = nullptr;
		if (detail::holds_cpp_type<memory::RefMemVar>(value_w)) {
			actual_value_w =
				&std::any_cast<memory::RefMemVar>(value_w).get().value_w;
		}
		else if (detail::holds_cpp_type<memory::RefConstMemVar>(value_w)) {
			actual_value_w =
				&std::any_cast<memory::RefConstMemVar>(value_w).get().value_w;
		}
		else {
			actual_value_w = &value_w;
		}

		std::any var_name_w = std::move(*var_res);
		auto var_name = std::any_cast<std::string&&>(std::move(var_name_w));
		const std::string_view var_type = decl.get_variable_type();

		INTERPRETER_PRINT(aleprln, "Of name:  '{}'.", var_name);
		INTERPRETER_PRINT(aleprln, "Of type:  '{}'.", var_type);
		INTERPRETER_PRINT(
			aleprln, "Of value: '{}'.", detail::AnyView{*actual_value_w}
		);

		EvaluationResult declaration_res = declare_variable(
			ctx, decl_t, std::move(var_name), var_type, *actual_value_w
		);
		if (not declaration_res) {
			return append_error(
				std::move(declaration_res.error()),
				evaluation_error_e::Declaration_Of_Variable,
				evaluation_function_e::Declaration,
				"Something went wrong when declaring a variable"
			);
		}

		++value_iter_pos;
		++var_iter_pos;
	}

	if (var_iter_pos != var_iter_end) {
		INTERPRETER_PRINT(aleprln, "Too many values in the right hand side");
		return make_bad_evaluation_result(
			Vec{evaluation_error_e::Overfull_Right_Hand_Side_Values},
			Vec{evaluation_function_e::Declaration},
			Vec{"Too many values in the right hand side"s}
		);
	}

	if (value_iter_pos != value_iter_end) {
		INTERPRETER_PRINT(aleprln, "Too many values in the left hand side");
		return make_bad_evaluation_result(
			Vec{evaluation_error_e::Overfull_Left_Hand_Side_Values},
			Vec{evaluation_function_e::Declaration},
			Vec{"Too many values in the left hand side"s}
		);
	}

	return make_good_evaluation_result<std::any>();
}

[[nodiscard]] static EvaluationResult declare_single_values_rhs(
	EvaluationContext& ctx,
	const ale::ast::DeclarationNode& decl,
	const ale::ast::node_type_e decl_t,
	const std::unique_ptr<ale::ast::Node>& left_child,
	const std::unique_ptr<ale::ast::Node>& right_child
)
{
	auto var_iter = make_name_iterator(ctx, left_child);
	auto var_iter_pos = var_iter.begin();
	auto var_iter_end = var_iter.end();

	EvaluationResult compute_res = interpret_node(ctx, right_child);
	if (not compute_res) {
		return append_error(
			std::move(compute_res.error()),
			evaluation_error_e::Evaluation_Of_Node_Failed,
			evaluation_function_e::Declaration,
			"Something went wrong when evaluating a node"
		);
	}

	std::any value_w = std::move(*compute_res);
	const std::any *actual_value_w = nullptr;
	if (detail::holds_cpp_type<memory::RefMemVar>(value_w)) {
		INTERPRETER_PRINT(aleprln, "Value is a variable reference.");
		actual_value_w =
			&std::any_cast<memory::RefMemVar>(value_w).get().value_w;
	}
	else if (detail::holds_cpp_type<memory::RefConstMemVar>(value_w)) {
		INTERPRETER_PRINT(aleprln, "Value is a variable const-reference.");
		actual_value_w =
			&std::any_cast<memory::RefConstMemVar>(value_w).get().value_w;
	}
	else {
		actual_value_w = &value_w;
	}

	while (var_iter_pos != var_iter_end) {
		INTERPRETER_PRINT(aleprln, "Going to declare a variable.");

		EvaluationResult var_res = *var_iter_pos;
		if (not var_res) {
			INTERPRETER_PRINT(
				aleprln,
				"Something went wrong when retrieving the next variable."
			);
			INTERPRETER_PRINT(
				aleprln, "Error: '{}'", var_res.error().errors.at(0)
			);
			return append_error(
				std::move(var_res.error()),
				evaluation_error_e::List_Iteration,
				evaluation_function_e::Declaration,
				"Something went wrong when retrieving the next variable"
			);
		}

		std::any var_name_w = std::move(*var_res);
		auto var_name = std::any_cast<std::string&&>(std::move(var_name_w));
		const std::string_view var_type = decl.get_variable_type();

		INTERPRETER_PRINT(aleprln, "Of name:  '{}'.", var_name);
		INTERPRETER_PRINT(aleprln, "Of type:  '{}'.", var_type);
		INTERPRETER_PRINT(
			aleprln, "Of value: '{}'.", detail::AnyView{*actual_value_w}
		);

		EvaluationResult declaration_res = declare_variable(
			ctx, decl_t, std::move(var_name), var_type, *actual_value_w
		);
		if (not declaration_res) {
			return append_error(
				std::move(declaration_res.error()),
				evaluation_error_e::Declaration_Of_Variable,
				evaluation_function_e::Declaration,
				"Something went wrong when retrieving the next variable"
			);
		};
		++var_iter_pos;
	}

	return make_good_evaluation_result<std::any>();
}

EvaluationResult
evaluate(EvaluationContext& ctx, const ale::ast::DeclarationNode& decl)
{
	INTERPRETER_ENTER_AST_FUNCTION(aleprln);

	const auto decl_t = decl.get_node_type();
	const auto& left_child = decl.get_left_child();
#if defined DEBUG
	assert(left_child != nullptr);
#endif

	if (decl_t == ale::ast::node_type_e::Declaration_Declare) {
		auto var_iter = make_name_iterator(ctx, left_child);
		auto var_iter_pos = var_iter.begin();
		auto var_iter_end = var_iter.end();

		while (var_iter_pos != var_iter_end) {
			EvaluationResult res = *var_iter_pos;
			if (not res) {
				return append_error(
					std::move(res.error()),
					evaluation_error_e::List_Iteration,
					evaluation_function_e::Declaration,
					"Something went wrong when retrieving the next variable"
				);
			}

			std::any var_name_w = std::move(*res);
			auto var_name = std::any_cast<std::string&&>(std::move(var_name_w));
			const std::string_view var_type = decl.get_variable_type();

			auto declaration_res =
				declare_variable(ctx, std::move(var_name), var_type);

			if (not declaration_res) {
				return append_error(
					std::move(declaration_res.error()),
					evaluation_error_e::Declaration_Of_Variable,
					evaluation_function_e::Declaration,
					"Something went wrong when declaring a variable"
				);
			}
			++var_iter_pos;
		}
		return make_good_evaluation_result<std::any>();
	}

	const auto& right_child = decl.get_right_child();
#if defined DEBUG
	assert(right_child != nullptr);
#endif
	const auto right_t = right_child->get_node_type();

	if (right_t == ale::ast::node_type_e::Sequence or
		right_t == ale::ast::node_type_e::Comma_Separated_Group) {

		return declare_multiple_values_rhs(
			ctx, decl, decl_t, left_child, right_child
		);
	}

	return declare_single_values_rhs(
		ctx, decl, decl_t, left_child, right_child
	);
}

} // namespace ast
} // namespace intlib
