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
#include <ale/ast/n_ary_nodes/ComparisonNode.hpp>

#include <intlib/logger/macros.hpp>
#include <intlib/detail/any_type.hpp>
#include <intlib/detail/any_output.hpp>
#include <intlib/ast/EvaluationContext.hpp>
#include <intlib/ast/EvaluationResult.hpp>
#include <intlib/ast/evaluation.hpp>
#include <intlib/ast/interpretation.hpp>
#include <intlib/comparison/comparison.hpp>

namespace intlib {
namespace ast {

EvaluationResult evaluate(
	EvaluationContext& ctx,
	const ale::ast::ComparisonNode& v,
	const ale::ast::node_type_e t
)
{
	INTERPRETER_ENTER_AST_FUNCTION(ale::logger::println);

	const auto& children = v.get_children();

#if defined DEBUG
	assert(children.size() > 0);
#endif

	std::any previous_w;
	{
		EvaluationResult res_w = interpret_node(ctx, children.at(0));
		if (not res_w.has_value()) {
			INTERPRETER_PRINT_LOC(
				ale::logger::println, "Evaluation of node failed."
			);
			return append_error(
				std::move(res_w.error()),
				evaluation_error_e::Evaluation_Of_Node_Failed,
				"Evaluation of node did not produce any value."
			);
		}
		previous_w = std::move(*res_w);
	}

	for (const std::unique_ptr<ale::ast::Node>& c :
		 children | std::views::drop(1)) {

		EvaluationResult res_w = interpret_node(ctx, c);
		if (not res_w.has_value()) {
			INTERPRETER_PRINT_LOC(
				ale::logger::println, "Evaluation of node failed."
			);
			return append_error(
				std::move(res_w.error()),
				evaluation_error_e::Evaluation_Of_Node_Failed,
				"Evaluation of node did not produce any value."
			);
		}
		std::any current_w = std::move(*res_w);

		const std::optional<bool> comparison_result_w =
			detail::any_comparison(t, previous_w, current_w);

		if (not comparison_result_w.has_value()) {
			INTERPRETER_PRINT_LOC2(
				ale::logger::println,
				"Could not compare two std::any values: '{}' and '{}'.",
				any_view{previous_w},
				any_view{current_w}
			);
			return make_bad_evaluation_result(
				std::vector{evaluation_error_e::Comparison_Operation_Failed},
				std::vector{std::format(
					"Could not compare two std::any values: '{}' and '{}'.",
					any_view{previous_w},
					any_view{current_w}
				)}
			);
		}

		if (not *comparison_result_w) {
			return false;
		}
		previous_w = std::move(current_w);
	}

	return true;
}

} // namespace ast
} // namespace intlib
