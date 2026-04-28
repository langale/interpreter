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
#include <any>

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
struct VariableValue {
	/// The actual value that the variable holds.
	std::any value_w;
	/// The type of this variable. This is an ALE's type.
	const std::string_view type;
	/// Whether the variable is declared with 'const'.
	const bool is_constant;
};

static constinit VariableValue empty_variable{
	.value_w = {}, .type = "", .is_constant = true
};

using RefMemVar = std::reference_wrapper<VariableValue>;
using RefConstMemVar = std::reference_wrapper<const VariableValue>;

} // namespace memory
} // namespace intlib
