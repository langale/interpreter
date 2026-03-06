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

#include <ale/ast/n_ary_nodes/comparison/ComparisonEqualNode.hpp>

#include <optional>
#include <any>

#include <ale/logger/Logger.hpp>

#include <intlib/detail/any_arithmetic.hpp>

namespace intlib {
namespace detail {

std::optional<std::any> any_arithmetic(
	const ale::ast::node_type_e t, const std::any& a, const std::any& b
)
{
	switch (t) {
	case ale::ast::node_type_e::Arithmetic_Addition:
		return any_arithmetic_addition(a, b);
	case ale::ast::node_type_e::Arithmetic_Division:
		return any_arithmetic_division(a, b);
	case ale::ast::node_type_e::Arithmetic_Exponentiation:
		return any_arithmetic_exponentiation(a, b);
	case ale::ast::node_type_e::Arithmetic_Modulus:
		return any_arithmetic_modulus(a, b);
	case ale::ast::node_type_e::Arithmetic_Multiplication:
		return any_arithmetic_multiplication(a, b);
	case ale::ast::node_type_e::Arithmetic_Subtraction:
		return any_arithmetic_subtraction(a, b);
	default: return {};
	}
}

} // namespace detail
} // namespace intlib
