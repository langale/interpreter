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

#include <ale/ast/unary_nodes/NegationNode.hpp>

#include <intlib/detail/any_to_bool.hpp>
#include <intlib/detail/type_string_cpp.hpp>
#include <intlib/ast/EvaluationContext.hpp>
#include <intlib/ast/interpretation.hpp>
#include <intlib/logger/macros.hpp>
#include <intlib/ast/utils/evaluation_result_to_string.hpp>

namespace intlib {
namespace ast {

Evaluation evaluate(EvaluationContext& ctx, const ale::ast::NegationNode& v)
{
	INTERPRETER_ENTER_AST_FUNCTION;

	const auto& child = v.get_child();

	Evaluation eval = interpret_node(ctx, child);
	if (not eval.has_value()) {
		INTERPRETER_PRINT("Node evaluation failed.");
		return append_error(
			std::move(eval.error()),
			evaluation_error_e::Evaluation_Of_Node_Failed,
			evaluation_function_e::Negation,
			"Node evaluation failed"
		);
	}

	const EvaluationResult& res = *eval;
	const std::optional value_w = detail::any_to_bool(res);
	if (value_w) {
		INTERPRETER_PRINT("Evaluation of node: {}.", *value_w);
		return make_good_evaluation<
			EvaluationResult>(not*value_w, detail::type_string_cpp<bool>);
	}

	INTERPRETER_PRINT("Unhandled value of type '{}'.", res.type);
	return make_bad_evaluation(
		Vec{evaluation_error_e::Unhandled_Variable_Type},
		Vec{evaluation_function_e::Negation},
		Vec{std::format("Unhandled value '{}'", res)}
	);
}

} // namespace ast
} // namespace intlib
