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
#include <any>

#include <ale/ast/utils/node_type_enum.hpp>

namespace intlib {
namespace comparison {

/**
 * @brief Equal comparison between two std::any objects.
 * @param left_w Value.
 * @param right_w Value.
 * @returns If any of the two input values is a double, returns a std::any object
 * that contains a double. Otherwise: if the result is positive, a std::any that
 * contains a uint64_t; if the result is negative, a std::any that contains an
 * int64_t.
 */
[[nodiscard]] std::optional<bool>
any_comparison_equal(const std::any& left_w, const std::any& right_w);

[[nodiscard]] std::optional<bool>
any_comparison_not_equal(const std::any& left_w, const std::any& right_w);

[[nodiscard]] std::optional<bool>
any_comparison_less_than(const std::any& left_w, const std::any& right_w);

[[nodiscard]] std::optional<bool>
any_comparison_less_equal(const std::any& left_w, const std::any& right_w);

[[nodiscard]] std::optional<bool>
any_comparison_greater_than(const std::any& left_w, const std::any& right_w);

[[nodiscard]] std::optional<bool>
any_comparison_greater_equal(const std::any& left_w, const std::any& right_w);

/**
 * @brief Comparison operation determined by @e t between two std::any objects.
 *
 * These two objects need to be of the same type. To ensure correctness, values
 * are cast-ed to the same type and then compared.
 * @param t Type of comparison.
 * @param left_w First value.
 * @param right_w Second value.
 * @returns The result of the comparison.
 */
[[nodiscard]] std::optional<bool> any_comparison(
	const ale::ast::node_type_e t,
	const std::any& left_w,
	const std::any& right_w
);

} // namespace comparison
} // namespace intlib
