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

#include <ale/ast/zero_ary_nodes/VariableNode.hpp>

#include <intlib/logger/macros.hpp>
#include <intlib/ast/Evaluation.hpp>
#include <intlib/ast/EvaluationContext.hpp>

namespace intlib {
namespace ast {

Evaluation
evaluate(const EvaluationContext& ctx, const ale::ast::VariableNode& v)
{
	INTERPRETER_ENTER_AST_FUNCTION;

	const std::string& name = v.get_variable_name();

	if (not ctx.memory.variable_exists(name)) {
		INTERPRETER_PRINT("Variable '{}' is not defined in this scope.", name);
		return make_bad_evaluation(
			Vec{evaluation_error_e::Valueless_Variable},
			Vec{evaluation_function_e::Variable},
			Vec{std::format("Variable '{}' is not defined in this scope.", name)
			}
		);
	}

	memory::Variable& variable = ctx.memory.get_variable(name);

	if (variable.wrap.type == detail::type_string_cpp<void>) {
		INTERPRETER_PRINT("Variable '{}' has no value.", name);
		return make_bad_evaluation(
			Vec{evaluation_error_e::Valueless_Variable},
			Vec{evaluation_function_e::Variable},
			Vec{std::format("Variable '{}' has no value.", name)}
		);
	}

	if (variable.is_constant) {
		return make_good_evaluation<
			memory::
				WrappedAny>(std::cref(variable), detail::type_string_cpp<memory::RefConstVar>);
	}
	return make_good_evaluation<
		memory::
			WrappedAny>(std::ref(variable), detail::type_string_cpp<memory::RefVar>);
}

} // namespace ast
} // namespace intlib
