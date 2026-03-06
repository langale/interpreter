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

#include <ale/ast/n_ary_nodes/CommaSeparatedGroupNode.hpp>

#include <intlib/logger/macros.hpp>
#include <intlib/Program.hpp>

namespace intlib {

EvaluationResult Program::evaluate(const ale::ast::CommaSeparatedGroupNode& v)
{
	INTERPRETER_ENTER_FUNCTION(ale::logger::println);

	INTERPRETER_PRINT_LOC2(
		ale::logger::println,
		"Cannot evaluate nodes of type '{}'.",
		v.get_node_type()
	);
	return EvaluationError{
		.error = {evaluation_error_e::Forbidden_Evaluation_Of_Node},
		.message = {std::format(
			"Cannot evaluate nodes of type '{}'.", v.get_node_type()
		)}
	};
}

} // namespace intlib
