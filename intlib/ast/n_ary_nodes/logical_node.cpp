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
#include <ale/ast/utils/node_type_to_string.hpp>
#include <ale/ast/n_ary_nodes/LogicalNode.hpp>

#include <intlib/logger/macros.hpp>
#include <intlib/detail/any_type.hpp>
#include <intlib/detail/any_to_bool.hpp>
#include <intlib/detail/any_output.hpp>
#include <intlib/ast/EvaluationContext.hpp>
#include <intlib/ast/EvaluationResult.hpp>
#include <intlib/ast/interpretation.hpp>

namespace intlib {
namespace ast {

#define aleprln ale::logger::println

[[nodiscard]] static bool compute_logical_expression(
	const ale::ast::node_type_e t, const bool l, const bool r
)
{
	INTERPRETER_ENTER_AST_FUNCTION(aleprln);

	if (t == ale::ast::node_type_e::Logical_And) {
		return l and r;
	}
	if (t == ale::ast::node_type_e::Logical_Or) {
		return l or r;
	}

	INTERPRETER_PRINT_LOC(aleprln, "Wrong node type {}.", t);
#if defined DEBUG
	assert(false);
#endif

	return false;
}

[[nodiscard]] static bool break_when(const ale::ast::node_type_e t)
{
	INTERPRETER_ENTER_AST_FUNCTION(aleprln);

	if (t == ale::ast::node_type_e::Logical_And) {
		return false;
	}
	if (t == ale::ast::node_type_e::Logical_Or) {
		return true;
	}

	INTERPRETER_PRINT_LOC(aleprln, "Wrong node type {}.", t);
#if defined DEBUG
	assert(false);
#endif

	return false;
}

EvaluationResult evaluate_logical_node(
	EvaluationContext& ctx, const std::unique_ptr<ale::ast::Node>& c
)
{
	INTERPRETER_ENTER_AST_FUNCTION(aleprln);

	EvaluationResult res_int = interpret_node(ctx, c);
	if (not res_int) {
		INTERPRETER_PRINT_LOC(aleprln, "Node evaluation failed.");
		return append_error(
			std::move(res_int.error()),
			evaluation_error_e::Evaluation_Of_Node_Failed,
			evaluation_function_e::Logical,
			"Node evaluation failed"
		);
	}

	const std::any& res_w = *res_int;
	INTERPRETER_PRINT_LOC(aleprln, "Evaluation of node '{}'", any_view{res_w});

	if (detail::is_type<void>(res_w)) {
		INTERPRETER_PRINT_LOC(aleprln, "Evaluation of node failed.");
		return append_error(
			std::move(res_int.error()),
			evaluation_error_e::Evaluation_Of_Node_Is_Void,
			evaluation_function_e::Logical,
			"Evaluation of node produced a void value"
		);
	}

	const std::optional r_conv_w = detail::any_to_bool(res_w);
	if (not r_conv_w) {
		INTERPRETER_PRINT_LOC(
			aleprln,
			"Evaluation of node '{}' could not be converted to a Boolean value "
			"'{}'.",
			any_view{res_w},
			any_view{r_conv_w}
		);
		return append_error(
			std::move(res_int.error()),
			evaluation_error_e::Conversion_To_Bool_Failed,
			evaluation_function_e::Logical,
			"Evaluation of node could not be converted to a Boolean value."
		);
	}

	return make_good_evaluation_result<bool>(*r_conv_w);
}

EvaluationResult evaluate(
	EvaluationContext& ctx,
	const ale::ast::LogicalNode& v,
	const ale::ast::node_type_e t
)
{
	INTERPRETER_ENTER_AST_FUNCTION(aleprln);

	const auto& children = v.get_children();

	EvaluationResult res = evaluate_logical_node(ctx, children.at(0));
	if (not res) {
		INTERPRETER_PRINT_LOC(aleprln, "Node evaluation failed.");
		return res.error();
	}

	const bool when_to_break = break_when(t);

	bool rc_value = std::any_cast<bool>(*res);
	for (const std::unique_ptr<ale::ast::Node>& c :
		 children | std::views::drop(1)) {

		if (rc_value == when_to_break) {
			break;
		}

		EvaluationResult rv = evaluate_logical_node(ctx, c);
		if (not rv) {
			INTERPRETER_PRINT_LOC(aleprln, "Node evaluation failed.");
			return res.error();
		}

		const bool rv_value = std::any_cast<bool>(*rv);
		rc_value = compute_logical_expression(t, rc_value, rv_value);
	}

	return make_good_evaluation_result<std::any>(rc_value);
}

} // namespace ast
} // namespace intlib
