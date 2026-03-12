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
#include <intlib/ast/utils/macros.hpp>

namespace intlib {
namespace ast {

EvaluationResult
evaluate(EvaluationContext& ctx, const ale::ast::PositiveNode& v)
{
	INTERPRETER_ENTER_AST_FUNCTION(ale::logger::println);

	const auto& child = v.get_child();

	EvaluationResult rr = interpret_node(ctx, child);
	if (not rr.has_value()) {
		INTERPRETER_PRINT_LOC(ale::logger::println, "Node evaluation failed.");
		return append_error(
			std::move(rr.error()),
			evaluation_error_e::Evaluation_Of_Node_Failed,
			"Node evaluation failed"
		);
	}

	const std::any& r = *rr;
	if (detail::is_type<uint64_t>(r)) {
		const uint64_t ri = std::any_cast<uint64_t>(r);
		INTERPRETER_PRINT_LOC2(
			ale::logger::println, "Evaluation of node is uint64_t: {}.", ri
		);
		return make_good_evaluation_result(detail::to_uint64(ri));
	}

	if (detail::is_type<int64_t>(r)) {
		const int64_t ri = std::any_cast<int64_t>(r);
		INTERPRETER_PRINT_LOC2(
			ale::logger::println, "Evaluation of node is int64_t: {}.", ri
		);
		return make_good_evaluation_result(ri);
	}

	if (detail::is_type<double>(r)) {
		const double ri = std::any_cast<double>(r);
		INTERPRETER_PRINT_LOC2(
			ale::logger::println, "Evaluation of node is double: {}.", ri
		);
		return make_good_evaluation_result(ri);
	}

	INTERPRETER_PRINT_LOC2(
		ale::logger::println,
		"Unhandled variable type '{}'.",
		detail::get_type_name(*rr)
	);
	return make_bad_evaluation_result(
		std::vector{evaluation_error_e::Unhandled_Variable_Type},
		std::vector{
			std::format("Unhandled type '{}'", detail::get_type_name(*rr))
		}
	);
}

} // namespace ast
} // namespace intlib
