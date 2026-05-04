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
#include <string>
#include <any>
using namespace std::string_literals;

#include <ale/ast/utils/node_type_enum.hpp>
#include <ale/ast/n_ary_nodes/ArithmeticNode.hpp>

#include <intlib/logger/macros.hpp>
#include <intlib/arithmetic/arithmetic.hpp>
#include <intlib/ast/EvaluationContext.hpp>
#include <intlib/ast/Evaluation.hpp>
#include <intlib/ast/interpretation.hpp>
#include <intlib/ast/utils/evaluation_result_to_string.hpp>

namespace intlib {
namespace ast {

[[nodiscard]] static Evaluation
node_eval(EvaluationContext& ctx, const std::unique_ptr<ale::ast::Node>& c)
{
	Evaluation eval_res = interpret_node(ctx, c);
	if (not eval_res) {
		INTERPRETER_PRINT("Evaluation of node within arithmetic node failed.");
		return make_bad_evaluation(
			Vec{evaluation_error_e::Evaluation_Of_Node_Failed},
			Vec{evaluation_function_e::Arithmetic},
			Vec{"Evaluation of node within arithmetic node failed"s}
		);
	}

	EvaluationResult res = std::move(*eval_res);
	if (res.type == detail::type_string_cpp<void>) {
		INTERPRETER_PRINT("Evaluation of node returned a void value.");
		return make_bad_evaluation(
			Vec{evaluation_error_e::Evaluation_Of_Node_Is_Void},
			Vec{evaluation_function_e::Arithmetic},
			Vec{"Evaluation of node returned a void value"s}
		);
	}
	return make_good_evaluation<EvaluationResult>(
		std::move(res.value), res.type
	);
}

Evaluation evaluate(
	EvaluationContext& ctx,
	const ale::ast::ArithmeticNode& v,
	const ale::ast::node_type_e t
)
{
	INTERPRETER_ENTER_AST_FUNCTION;

	const auto& children = v.get_children();
#if defined DEBUG
	assert(children.size() >= 2);
#endif

	Evaluation eval_res = node_eval(ctx, children[0]);
	if (not eval_res) {
		return eval_res;
	}

	EvaluationResult expr_res_1 = std::move(*eval_res);

	for (const std::unique_ptr<ale::ast::Node>& c :
		 children | std::views::drop(1)) {

		eval_res = node_eval(ctx, c);
		if (not eval_res) {
			return eval_res;
		}

		EvaluationResult expr_res_2 = std::move(*eval_res);
		std::optional<EvaluationResult> operation_res =
			arithmetic::any_arithmetic(t, expr_res_1, expr_res_2);

		if (not operation_res.has_value()) {
			INTERPRETER_PRINT(
				"Arithmetic operation '{}' did not return a value.",
				v.get_operation_string()
			);
			return make_bad_evaluation(
				Vec{evaluation_error_e::Arithmetic_Operation_Failed},
				Vec{evaluation_function_e::Arithmetic},
				Vec{std::format(
					"Could not operate two std::any values: '{}' and '{}'",
					expr_res_1,
					expr_res_2
				)}
			);
		}

		expr_res_1 = std::move(*operation_res);
	}

	return make_good_evaluation<EvaluationResult>(std::move(expr_res_1));
}

} // namespace ast
} // namespace intlib
