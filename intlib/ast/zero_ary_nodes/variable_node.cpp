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

#include <intlib/logger/macros.hpp>
#include <intlib/detail/any_type.hpp>
#include <intlib/Program.hpp>

namespace intlib {

EvaluationResult Program::evaluate(const ale::ast::VariableNode& v) const
{
	INTERPRETER_ENTER_FUNCTION(ale::logger::println);

	const std::string& name = v.get_variable_name();

	if (not m_memory.variable_exists(name)) {
		INTERPRETER_PRINT_LOC2(
			ale::logger::println,
			"Variable '{}' is not defined in this scope.",
			name
		);
		return EvaluationError{
			.error = {evaluation_error_e::Valueless_Variable},
			.message = {
				std::format("Variable '{}' is not defined in this scope.", name)
			}
		};
	}

	std::optional<memory::VariableValue> res = m_memory.get_variable(name);
#if defined DEBUG
	assert(res.has_value());
#endif

	if (detail::is_type<void>(res->value)) {
		INTERPRETER_PRINT_LOC2(
			ale::logger::println, "Variable '{}' has no value.", name
		);
		return EvaluationError{
			.error = {evaluation_error_e::Valueless_Variable},
			.message = {std::format("Variable '{}' has no value.", name)}
		};
	}

	return std::move(res->value);
}

} // namespace intlib
