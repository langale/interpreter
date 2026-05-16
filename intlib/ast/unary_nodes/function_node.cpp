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

#include <ale/ast/unary_nodes/FunctionNode.hpp>

#include <intlib/ast/EvaluationContext.hpp>
#include <intlib/ast/interpretation.hpp>
#include <intlib/logger/macros.hpp>

namespace intlib {
namespace ast {

Evaluation evaluate(EvaluationContext& ctx, const ale::ast::FunctionNode& v)
{
	INTERPRETER_ENTER_AST_FUNCTION;
	INTERPRETER_PRINT("Executing function '{}'.", v.get_name());

	/// TODO: push parameters into the scope

	const auto& child = v.get_child();
	return interpret_node(ctx, child);
}

} // namespace ast
} // namespace intlib
