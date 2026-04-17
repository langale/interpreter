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

#include <any>

#include <ale/ast/n_ary_nodes/SubscopeModifierNode.hpp>

#include <intlib/logger/macros.hpp>
#include <intlib/detail/any_type.hpp>
#include <intlib/ast/EvaluationContext.hpp>
#include <intlib/ast/interpretation.hpp>

namespace intlib {
namespace ast {

#define aleprln ale::logger::println

EvaluationResult
evaluate(EvaluationContext& ctx, const ale::ast::SubscopeModifierNode& v)
{
	INTERPRETER_ENTER_AST_FUNCTION(aleprln);

	ctx.memory.get_current_scope().push_local_scope();
	for (const auto& w : v.get_children()) {
		EvaluationResult res_w = interpret_node(ctx, w);
		if (not res_w) {
			INTERPRETER_PRINT_LOC(
				aleprln, "Evaluation of node failed."
			);
			return append_error(
				std::move(res_w.error()),
				evaluation_error_e::Evaluation_Of_Node_Failed,
				evaluation_function_e::Subscope_Modifier,
				"Node evaluation failed"
			);
		}

		const std::any& value = *res_w;
		if (value.has_value()) {
			INTERPRETER_PRINT_LOC(
				aleprln,
				"Potentially-ignored return value or expression."
			);
		}
	}
	ctx.memory.get_current_scope().pop_local_scope();
	return make_good_evaluation_result<std::any>();
}

} // namespace ast
} // namespace intlib
