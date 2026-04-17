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

#include <generator>

#include <ale/ast/n_ary_nodes/CommaSeparatedGroupNode.hpp>
#include <ale/ast/binary_nodes/SequenceNode.hpp>

#include <intlib/logger/macros.hpp>
#include <intlib/detail/any_type.hpp>
#include <intlib/detail/any_output.hpp>
#include <intlib/ast/EvaluationResult.hpp>
#include <intlib/ast/interpretation.hpp>

namespace intlib {
namespace ast {

#define aleprln ale::logger::println

std::generator<EvaluationResult> make_iterator(
	EvaluationContext& ctx, const ale::ast::CommaSeparatedGroupNode& comma
)
{
	const auto& children = comma.get_children();
	for (const auto& child : children) {

		EvaluationResult res = interpret_node(ctx, child);
		if (not res.has_value()) {
			INTERPRETER_PRINT_LOC(aleprln, "Evaluation of node failed.");
			co_yield make_bad_evaluation_result(std::move(res.error()));
			co_return;
		}

		std::any value_w = std::move(*res);

		INTERPRETER_PRINT_LOC(
			aleprln,
			"Type returned from node evaluation is: '{}'. Value is: '{}'.",
			detail::get_type_name(value_w),
			any_view{value_w}
		);

		co_yield make_good_evaluation_result<std::any>(std::move(value_w));
	}
}

} // namespace ast
} // namespace intlib
