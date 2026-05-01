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

#include <string>

#include <intlib/memory/WrappedAny.hpp>
#include <intlib/detail/type_string_cpp.hpp>

namespace intlib {
namespace memory {

/**
 * @brief Variable model.
 *
 * The contents of a variable are:
 * - its value (@ref value_w),
 * - its type (@ref type),
 * - and whether it is mutable or not (@ref is_constant).
 */
struct Variable {
	/// The actual value that the variable holds.
	WrappedAny wrap;
	/// The type of this variable. This is an ALE's type.
	const std::string_view ale_type;
	/// Whether the variable is declared with 'const'.
	const bool is_constant;
};

static constinit Variable empty_variable{
	.wrap = {}, .ale_type = "", .is_constant = true
};

using RefVar = std::reference_wrapper<Variable>;
using RefConstVar = std::reference_wrapper<const Variable>;

} // namespace memory

namespace detail {

template <>
constexpr inline std::string_view type_string_cpp<memory::RefVar> = "Variable&";

template <>
constexpr inline std::string_view type_string_cpp<memory::RefConstVar> =
	"const Variable&";

} // namespace detail
} // namespace intlib
