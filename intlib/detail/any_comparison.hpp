/*********************************************************************
 *
 * ALE interpreter library -- the base utilities for a command line utility
 * to run programs written in ALE
 *
 *     Copyright (C) 2024 Lluís Alemany Puig
 *
 * This file is part of the implementation of an interpreter for ALE.
 * The full code is available at:
 *
 *     https://github.com/lluisalemanypuig/alelang
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

// C++ includes
#include <optional>
#include <any>

// ale includes
#include <ale/ast/node_type.hpp>

namespace interpreter {
namespace detail {

/// Equal (==) comparison of two std::any.
[[nodiscard]] std::optional<bool> any_comparison_equal
(const std::any& a, const std::any& b)
noexcept;
/// Not-Equal (!=) comparison of two std::any.
[[nodiscard]] std::optional<bool> any_comparison_not_equal
(const std::any& a, const std::any& b)
noexcept;
/// Less than (<) comparison of two std::any.
[[nodiscard]] std::optional<bool> any_comparison_less_than
(const std::any& a, const std::any& b)
noexcept;
/// Less than or equal to (<=) comparison of two std::any.
[[nodiscard]] std::optional<bool> any_comparison_less_than_equal_to
(const std::any& a, const std::any& b)
noexcept;
/// Greater than (>) comparison of two std::any.
[[nodiscard]] std::optional<bool> any_comparison_greater_than
(const std::any& a, const std::any& b)
noexcept;
/// Greater than or equal to (>=) comparison of two std::any.
[[nodiscard]] std::optional<bool> any_comparison_greater_than_equal_to
(const std::any& a, const std::any& b)
noexcept;

/// Compares two std::any according to the type of node
[[nodiscard]] std::optional<bool> any_comparison
(const ale::ast::node_type& t, const std::any& a, const std::any& b)
noexcept;

} // -- namespace detail
} // -- namespace interpreter
