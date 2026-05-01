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

#include <ale/ast/zero_ary_nodes/LiteralTrueNode.hpp>

#include <intlib/logger/macros.hpp>
#include <intlib/ast/Evaluation.hpp>
#include <intlib/ast/EvaluationContext.hpp>
#include <intlib/detail/type_string_cpp.hpp>

namespace intlib {
namespace ast {

Evaluation evaluate(const EvaluationContext&, const ale::ast::LiteralTrueNode&)
{
	INTERPRETER_ENTER_AST_FUNCTION;

	return make_good_evaluation<
		EvaluationResult>(true, detail::type_string_cpp<bool>);
}

} // namespace ast
} // namespace intlib
