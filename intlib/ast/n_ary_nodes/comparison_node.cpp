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
using namespace std::string_literals;

#include <ale/ast/utils/node_type_enum.hpp>
#include <ale/ast/n_ary_nodes/ComparisonNode.hpp>

#include <intlib/detail/type_string_cpp.hpp>
#include <intlib/ast/EvaluationContext.hpp>
#include <intlib/ast/Evaluation.hpp>
#include <intlib/ast/interpretation.hpp>
#include <intlib/comparison/comparison.hpp>
#include <intlib/logger/macros.hpp>
#include <intlib/memory/utils/unwrap.hpp>
#include <intlib/ast/utils/evaluation_result_to_string.hpp>

namespace intlib {
namespace ast {

[[nodiscard]] static Evaluation
node_interpret(EvaluationContext& ctx, const std::unique_ptr<ale::ast::Node>& c)
{
	Evaluation eval = interpret_node(ctx, c);
	if (not eval) {
		INTERPRETER_PRINT("Evaluation of node within arithmetic node failed.");
		return make_bad_evaluation(
			Vec{evaluation_error_e::Evaluation_Of_Node_Failed},
			Vec{evaluation_function_e::Arithmetic},
			Vec{"Evaluation of node within arithmetic node failed"s}
		);
	}

	EvaluationResult res = std::move(*eval);
	if (res.type == detail::type_string_cpp<void>) {
		INTERPRETER_PRINT("Evaluation of node returned a void value.");
		return make_bad_evaluation(
			Vec{evaluation_error_e::Evaluation_Of_Node_Is_Void},
			Vec{evaluation_function_e::Arithmetic},
			Vec{"Evaluation of node returned a void value"s}
		);
	}

	if (res.type == detail::type_string_cpp<memory::RefConstVar> or
		res.type == detail::type_string_cpp<memory::RefVar>) {
		memory::unwrap_into(res);
	}

	return make_good_evaluation<EvaluationResult>(
		std::move(res.value), res.type
	);
}

Evaluation evaluate(
	EvaluationContext& ctx,
	const ale::ast::ComparisonNode& v,
	const ale::ast::node_type_e t
)
{
	INTERPRETER_ENTER_AST_FUNCTION;

	const auto& children = v.get_children();

#if defined DEBUG
	assert(children.size() > 0);
#endif

	Evaluation eval = node_interpret(ctx, children[0]);
	if (not eval) {
		return eval;
	}

	EvaluationResult previous_res = std::move(*eval);

	for (const std::unique_ptr<ale::ast::Node>& c :
		 children | std::views::drop(1)) {

		eval = node_interpret(ctx, c);
		if (not eval) {
			return eval;
		}

		EvaluationResult current_res = std::move(*eval);

		const std::optional<bool> comparison_result_w =
			comparison::any_comparison(t, previous_res, current_res);

		if (not comparison_result_w.has_value()) {
			INTERPRETER_PRINT(
				"Could not compare two std::any values: '{}' and '{}'.",
				previous_res,
				current_res
			);
			return make_bad_evaluation(
				Vec{evaluation_error_e::Comparison_Operation_Failed},
				Vec{evaluation_function_e::Comparison},
				Vec{std::format(
					"Could not compare two std::any values: '{}' and '{}'",
					previous_res,
					current_res
				)}
			);
		}

		if (not*comparison_result_w) {
			return make_good_evaluation<
				EvaluationResult>(false, detail::type_string_cpp<bool>);
		}
		previous_res = std::move(current_res);
	}

	return make_good_evaluation<
		EvaluationResult>(true, detail::type_string_cpp<bool>);
}

} // namespace ast
} // namespace intlib
