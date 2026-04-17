/*********************************************************************
 *
 * ALE interpreter library -- the base utilities for a command line utility
 * to run Programs written in ALE
 *
 *     Copyright (C) 2024 - 2026 Lluís Alemany Puig
 *
 * This file is part of the implementation of an interpreter for ALE.
 * The full code is available at:
 *
 *     https://github.com/langale/interpreter
 *
 * This Program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this Program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Contact:
 *
 *     Lluís Alemany Puig
 *     lluis.alemany.puig@gmail.com
 *     https://github.com/lluisalemanypuig
 *
 ********************************************************************/

#pragma once

#include <cstdint>

namespace intlib {
namespace ast {

enum class evaluation_function_e : uint8_t {
	Assignation,
	Declaration,
	Sequence,
	While_Loop,

	Arithmetic,
	Comma_Separated_Group,
	Comparison,
	Logical,
	Program,
	Subscope_Modifier,
	Subscripted_Variable,

	If_Else,

	Negation,
	Negative,
	Positive,

	False,
	Literal,
	True,
	Variable,

	Variable_Names,

	Iterator_Value_Variable,
	Iterator_Value_Subscripted_Variable,
	Iterator_Value_Sequence,
	Iterator_Value_Comma_Separated_Group,

	Iterator_Name_Variable,
	Iterator_Name_Subscripted_Variable,
	Iterator_Name_Sequence,
	Iterator_Name_Comma_Separated_Group,

	Interpretation_Node
};

} // namespace ast
} // namespace intlib
