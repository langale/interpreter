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

#include <ale/ast/unary_nodes/PositiveNode.hpp>

#include <intlib/logger/macros.hpp>
#include <intlib/detail/any_type.hpp>
#include <intlib/detail/macros.hpp>
#include <intlib/ast/EvaluationContext.hpp>
#include <intlib/ast/interpretation.hpp>
#include <intlib/memory/utils/wrapped_any_to_string.hpp>

namespace intlib {
namespace ast {

Evaluation evaluate(EvaluationContext& ctx, const ale::ast::PositiveNode& v)
{
	INTERPRETER_ENTER_AST_FUNCTION;

	const auto& child = v.get_child();

	Evaluation int_res_w = interpret_node(ctx, child);
	if (not int_res_w.has_value()) {
		INTERPRETER_PRINT("Node evaluation failed.");
		return append_error(
			std::move(int_res_w.error()),
			evaluation_error_e::Evaluation_Of_Node_Failed,
			evaluation_function_e::Positive,
			"Node evaluation failed"
		);
	}

	const EvaluationResult& res = *int_res_w;
	if (res.type == detail::type_string_cpp<uint64_t>) {
		const auto ri = std::any_cast<uint64_t>(res.value);
		INTERPRETER_PRINT("Evaluation of node is uint64_t: {}.", ri);
		return make_good_evaluation<
			EvaluationResult>(detail::to_uint64(ri), detail::type_string_cpp<uint64_t>);
	}

	if (res.type == detail::type_string_cpp<int64_t>) {
		const auto ri = std::any_cast<int64_t>(res.value);
		INTERPRETER_PRINT("Evaluation of node is int64_t: {}.", ri);
		return make_good_evaluation<
			EvaluationResult>(detail::to_int64(ri), detail::type_string_cpp<int64_t>);
	}

	if (res.type == detail::type_string_cpp<std::float64_t>) {
		const auto ri = std::any_cast<std::float64_t>(res.value);
		INTERPRETER_PRINT("Evaluation of node is std::float64_t: {}.", ri);
		return make_good_evaluation<
			EvaluationResult>(ri, detail::type_string_cpp<std::float64_t>);
	}

	INTERPRETER_PRINT("Unhandled result '{}'.", res);
	return make_bad_evaluation(
		Vec{evaluation_error_e::Unhandled_Variable_Type},
		Vec{evaluation_function_e::Positive},
		Vec{std::format("Unhandled result '{}'", res)}
	);
}

} // namespace ast
} // namespace intlib
