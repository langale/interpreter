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

#include <optional>

#include <ale/ast/utils/node_type_enum.hpp>

#include <intlib/memory/WrappedAny.hpp>
#include <intlib/arithmetic/definitions.hpp>

namespace intlib {
namespace arithmetic {

/**
 * @brief Addition between two WrappedAny objects.
 * @param left_w Value.
 * @param right_w Value.
 * @returns If any of the two input values is a double, returns a WrappedAny object
 * that contains a double. Otherwise: if the result is positive, a WrappedAny that
 * contains a uint64_t; if the result is negative, a WrappedAny that contains an
 * int64_t.
 */
[[nodiscard]] std::optional<WrappedAny>
arithmetic_addition(const WrappedAny& left_w, const WrappedAny& right_w);

/**
 * @brief Subtraction between two WrappedAny objects.
 * @param left_w Value.
 * @param right_w Value.
 * @returns If any of the two input values is a double, returns a WrappedAny object
 * that contains a double. Otherwise: if the result is positive, a WrappedAny that
 * contains a uint64_t; if the result is negative, a WrappedAny that contains an
 * int64_t.
 */
[[nodiscard]] std::optional<WrappedAny>
arithmetic_subtraction(const WrappedAny& left_w, const WrappedAny& right_w);

/**
 * @brief Multiplication between two WrappedAny objects.
 * @param left_w Value.
 * @param right_w Value.
 * @returns If any of the two input values is a double, returns a WrappedAny object
 * that contains a double. Otherwise: if the result is positive, a WrappedAny that
 * contains a uint64_t; if the result is negative, a WrappedAny that contains an
 * int64_t.
 */
[[nodiscard]] std::optional<WrappedAny>
arithmetic_multiplication(const WrappedAny& left_w, const WrappedAny& right_w);

/**
 * @brief Division between two WrappedAny objects.
 * @param left_w Value.
 * @param right_w Value.
 * @returns If any of the two input values is a double, returns a WrappedAny object
 * that contains a double. Otherwise: if the result is positive, a WrappedAny that
 * contains a uint64_t; if the result is negative, a WrappedAny that contains an
 * int64_t.
 */
[[nodiscard]] std::optional<WrappedAny>
arithmetic_division(const WrappedAny& left_w, const WrappedAny& right_w);

/**
 * @brief Multiplication between two WrappedAny objects.
 * @param left_w Value.
 * @param right_w Value.
 * @returns A WrappedAny object that contains a double, regardless of the two input
 * values.
 */
[[nodiscard]] std::optional<WrappedAny>
arithmetic_exponentiation(const WrappedAny& left_w, const WrappedAny& right_w);

/**
 * @brief Modulus between two WrappedAny objects.
 *
 * Floating-point values are cast-ed to integer values.
 * @param left_w Value.
 * @param right_w Value.
 * @returns If the result is positive, a WrappedAny that contains a uint64_t; if
 * the result is negative, a WrappedAny that contains an int64_t.
 */
[[nodiscard]] std::optional<WrappedAny>
arithmetic_modulus(const WrappedAny& left_w, const WrappedAny& right_w);

/// Arithmetic operation determined by @e t between two WrappedAny objects
[[nodiscard]] std::optional<WrappedAny> any_arithmetic(
	const ale::ast::node_type_e t,
	const WrappedAny& left_w,
	const WrappedAny& right_w
);

} // namespace arithmetic
} // namespace intlib
