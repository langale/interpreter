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

#if defined ALE_LOGGING_MESSAGES
#include <ale/ast/utils/node_type_to_string.hpp>
#endif

#include <intlib/logger/macros.hpp>
#include <intlib/arithmetic/arithmetic.hpp>

namespace intlib {
namespace arithmetic {

std::optional<WrappedAny> any_arithmetic(
	const ale::ast::node_type_e t,
	const WrappedAny& left_w,
	const WrappedAny& right_w
)
{
	INTERPRETER_ENTER_ARITHMETIC_FUNCTION;

	switch (t) {
	case ale::ast::node_type_e::Arithmetic_Addition:
		return arithmetic_addition(left_w, right_w);

	case ale::ast::node_type_e::Arithmetic_Division:
		return arithmetic_division(left_w, right_w);

	case ale::ast::node_type_e::Arithmetic_Exponentiation:
		return arithmetic_exponentiation(left_w, right_w);

	case ale::ast::node_type_e::Arithmetic_Modulus:
		return arithmetic_modulus(left_w, right_w);

	case ale::ast::node_type_e::Arithmetic_Multiplication:
		return arithmetic_multiplication(left_w, right_w);

	case ale::ast::node_type_e::Arithmetic_Subtraction:
		return arithmetic_subtraction(left_w, right_w);

	default:
		INTERPRETER_PRINT("Wrong node type '{}' for arithmetic.", t);
		return {};
	}
}

} // namespace arithmetic
} // namespace intlib
