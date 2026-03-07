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

#pragma once

#include <any>

#include <ale/ast/utils/node_type_enum.hpp>

namespace intlib {
namespace arithmetic {

/// Addition of two std::any values.
[[nodiscard]] std::any
arithmetic_addition(const std::any& a, const std::any& b);

/// Division of two std::any values.
[[nodiscard]] std::any
arithmetic_division(const std::any& a, const std::any& b);

/// Exponentiation of two std::any values.
[[nodiscard]] std::any
arithmetic_exponentiation(const std::any& a, const std::any& b);

/// Modulus of two std::any values.
[[nodiscard]] std::any arithmetic_modulus(const std::any& a, const std::any& b);

/// Multiplication of two std::any values.
[[nodiscard]] std::any
arithmetic_multiplication(const std::any& a, const std::any& b);

/// Subtraction of two std::any values.
[[nodiscard]] std::any
arithmetic_subtraction(const std::any& a, const std::any& b);

/// Computes two std::any according to the type of node
[[nodiscard]] std::any any_arithmetic(
	const ale::ast::node_type_e t, const std::any& a, const std::any& b
);

} // namespace arithmetic
} // namespace intlib
