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

#include <stdfloat>

#include <ale/ast/zero_ary_nodes/literal_nodes/LiteralStringNode.hpp>
#include <ale/ast/zero_ary_nodes/literal_nodes/numeric_nodes/LiteralDecimalNode.hpp>
#include <ale/ast/zero_ary_nodes/literal_nodes/numeric_nodes/LiteralIntegerNode.hpp>
#include <ale/ast/zero_ary_nodes/literal_nodes/numeric_nodes/LiteralUnsignedIntegerNode.hpp>
#include <ale/ast/zero_ary_nodes/literal_nodes/numeric_nodes/LiteralSignedIntegerNode.hpp>

#include <intlib/logger/macros.hpp>
#include <intlib/ast/Evaluation.hpp>
#include <intlib/ast/EvaluationContext.hpp>
#include <intlib/detail/type_string_cpp.hpp>

namespace intlib {
namespace ast {

#define aleprln ale::logger::println

Evaluation
evaluate(const EvaluationContext&, const ale::ast::LiteralStringNode& v)
{
	INTERPRETER_ENTER_AST_FUNCTION(aleprln);

	return make_good_evaluation<
		EvaluationResult>(v.get_value(), detail::type_string_cpp<std::string>);
}

Evaluation
evaluate(const EvaluationContext&, const ale::ast::LiteralDecimalNode& v)
{
	INTERPRETER_ENTER_AST_FUNCTION(aleprln);

	return make_good_evaluation<
		EvaluationResult>(static_cast<std::float64_t>(v.get_value()), detail::type_string_cpp<std::float64_t>);
}

Evaluation evaluate(
	const EvaluationContext&, const ale::ast::LiteralUnsignedIntegerNode& v
)
{
	INTERPRETER_ENTER_AST_FUNCTION(aleprln);

	return make_good_evaluation<
		EvaluationResult>(static_cast<uint64_t>(v.get_value()), detail::type_string_cpp<uint64_t>);
}

Evaluation
evaluate(const EvaluationContext&, const ale::ast::LiteralSignedIntegerNode& v)
{
	INTERPRETER_ENTER_AST_FUNCTION(aleprln);

	return make_good_evaluation<
		EvaluationResult>(static_cast<int64_t>(v.get_value()), detail::type_string_cpp<int64_t>);
}

} // namespace ast
} // namespace intlib
