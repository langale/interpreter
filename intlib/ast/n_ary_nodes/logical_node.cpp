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
#include <ranges>
#include <any>

#include <ale/ast/utils/node_type_enum.hpp>
#include <ale/ast/n_ary_nodes/LogicalNode.hpp>
#if defined ALE_LOGGING_MESSAGES
#include <ale/ast/utils/node_type_to_string.hpp>
#endif

#include <intlib/detail/any_to_bool.hpp>
#include <intlib/ast/EvaluationContext.hpp>
#include <intlib/ast/Evaluation.hpp>
#include <intlib/ast/interpretation.hpp>
#include <intlib/logger/macros.hpp>
#if defined ALE_LOGGING_MESSAGES
#include <intlib/ast/utils/evaluation_result_to_string.hpp>
#endif

namespace intlib {
namespace ast {

[[nodiscard]] static bool compute_logical_expression(
	const ale::ast::node_type_e t, const bool l, const bool r
)
{
	INTERPRETER_ENTER_AST_FUNCTION;

	if (t == ale::ast::node_type_e::Logical_And) {
		return l and r;
	}
	if (t == ale::ast::node_type_e::Logical_Or) {
		return l or r;
	}

	INTERPRETER_PRINT("Wrong node type {}.", t);
#if defined DEBUG
	assert(false);
#endif

	return false;
}

[[nodiscard]] static bool break_when(const ale::ast::node_type_e t)
{
	INTERPRETER_ENTER_AST_FUNCTION;

	if (t == ale::ast::node_type_e::Logical_And) {
		return false;
	}
	if (t == ale::ast::node_type_e::Logical_Or) {
		return true;
	}

	INTERPRETER_PRINT("Wrong node type {}.", t);
#if defined DEBUG
	assert(false);
#endif

	return false;
}

[[nodiscard]] static Evaluation evaluate_logical_node(
	EvaluationContext& ctx, const std::unique_ptr<ale::ast::Node>& c
)
{
	INTERPRETER_ENTER_AST_FUNCTION;

	Evaluation res_w = interpret_node(ctx, c);
	if (not res_w) {
		INTERPRETER_PRINT("Node evaluation failed.");
		return append_error(
			std::move(res_w.error()),
			evaluation_error_e::Evaluation_Of_Node_Failed,
			evaluation_function_e::Logical,
			"Node evaluation failed"
		);
	}

	const EvaluationResult& res = *res_w;
	INTERPRETER_PRINT("Evaluation of node '{}'", res);

	if (res.type == detail::type_string_cpp<void>) {
		INTERPRETER_PRINT("Evaluation of node failed.");
		return append_error(
			std::move(res_w.error()),
			evaluation_error_e::Evaluation_Of_Node_Is_Void,
			evaluation_function_e::Logical,
			"Evaluation of node produced a void value"
		);
	}

	const std::optional r_conv_w = detail::any_to_bool(res);
	if (not r_conv_w) {
		INTERPRETER_PRINT(
			"Value '{}' could not be converted to a Boolean value.", res
		);
		return append_error(
			std::move(res_w.error()),
			evaluation_error_e::Conversion_To_Bool_Failed,
			evaluation_function_e::Logical,
			"Evaluation of node could not be converted to a Boolean value"
		);
	}

	return make_good_evaluation<
		memory::WrappedAny>(*r_conv_w, detail::type_string_cpp<bool>);
}

Evaluation evaluate(
	EvaluationContext& ctx,
	const ale::ast::LogicalNode& v,
	const ale::ast::node_type_e t
)
{
	INTERPRETER_ENTER_AST_FUNCTION;

	const auto& children = v.get_children();

	Evaluation eval_w = evaluate_logical_node(ctx, children.at(0));
	if (not eval_w) {
		INTERPRETER_PRINT("Node evaluation failed.");
		return eval_w;
	}

	const bool when_to_break = break_when(t);

	const EvaluationResult& res = *eval_w;
#if defined DEBUG
	assert(res.type == detail::type_string_cpp<bool>);
#endif

	bool acc_value = std::any_cast<bool>(res.value);

	for (const std::unique_ptr<ale::ast::Node>& c :
		 children | std::views::drop(1)) {

		if (acc_value == when_to_break) {
			break;
		}

		eval_w = evaluate_logical_node(ctx, c);
		if (not eval_w) {
			INTERPRETER_PRINT("Node evaluation failed.");
			return eval_w;
		}

		const EvaluationResult& eval = *eval_w;
#if defined DEBUG
		assert(eval.type == detail::type_string_cpp<bool>);
#endif
		const bool new_value = std::any_cast<bool>(eval.value);

		acc_value = compute_logical_expression(t, acc_value, new_value);
	}

	return make_good_evaluation<
		EvaluationResult>(acc_value, detail::type_string_cpp<bool>);
}

} // namespace ast
} // namespace intlib
