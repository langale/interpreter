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
#include <ale/ast/n_ary_nodes/ArithmeticNode.hpp>

#include <intlib/logger/macros.hpp>
#include <intlib/detail/any_type.hpp>
#include <intlib/arithmetic/arithmetic.hpp>
#include <intlib/ast/EvaluationContext.hpp>
#include <intlib/ast/EvaluationResult.hpp>
#include <intlib/ast/interpretation.hpp>

namespace intlib {
namespace ast {

EvaluationResult evaluate(
	EvaluationContext& ctx,
	const ale::ast::ArithmeticNode& v,
	const ale::ast::node_type_e t
)
{
	INTERPRETER_ENTER_AST_FUNCTION(ale::logger::println);

	const auto& children = v.get_children();
#if defined DEBUG
	assert(children.size() >= 2);
#endif

	const auto node_eval =
		[&](const std::unique_ptr<ale::ast::Node>& c) -> EvaluationResult
	{
		EvaluationResult res_w = interpret_node(ctx, c);
		if (not res_w) {
			INTERPRETER_PRINT_LOC(
				ale::logger::println,
				"Evaluation of node within arithmetic node failed."
			);
			return make_bad_evaluation_result(
				std::vector{evaluation_error_e::Evaluation_Of_Node_Failed},
				std::vector<std::string>{
					"Evaluation of node within arithmetic node failed."
				}
			);
		}
		if (detail::is_type<void>(*res_w)) {
			INTERPRETER_PRINT_LOC(
				ale::logger::println,
				"Evaluation of node returned a void value."
			);
			return make_bad_evaluation_result(
				std::vector{evaluation_error_e::Evaluation_Of_Node_Is_Void},
				std::vector<std::string>{
					"Evaluation of node returned a void value."
				}
			);
		}
		return make_good_evaluation_result<std::any>(std::move(*res_w));
	};

	EvaluationResult res_w = node_eval(children[0]);
	if (not res_w) {
		return make_bad_evaluation_result(std::move(res_w.error()));
	}

	std::any expr_res_w = std::move(*res_w);

	for (const std::unique_ptr<ale::ast::Node>& c :
		 children | std::views::drop(1)) {

		res_w = node_eval(c);
		if (not res_w) {
			return make_bad_evaluation_result(std::move(res_w.error()));
		}

		expr_res_w = arithmetic::any_arithmetic(t, expr_res_w, *res_w);

		if (not expr_res_w.has_value()) {
			INTERPRETER_PRINT_LOC2(
				ale::logger::println,
				"Arithmetic operation '{}' did not return a value.",
				v.get_operation_string()
			);
			return make_bad_evaluation_result(
				std::vector{evaluation_error_e::Arithmetic_Operation_Failed},
				std::vector{std::format(
					"Arithmetic operation '{}' did not return a value.",
					v.get_operation_string()
				)}
			);
		}
	}

	return make_good_evaluation_result<std::any>(std::move(expr_res_w));
}

} // namespace ast
} // namespace intlib
