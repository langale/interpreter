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

enum class evaluation_error_e : uint8_t {
	Unhandled_Node_Type,
	Unhandled_Variable_Type,

	Undefined_Variable,
	Valueless_Variable,
	Assignation_Of_Variable,
	Declaration_Of_Variable,

	Evaluation_Of_Node_Failed,
	Evaluation_Of_Node_Is_Void,
	Evaluation_Of_Node_While_Loop_Failed,
	Evaluation_Of_Node_Is_Not_A_Boolean_Value,
	Evaluation_Of_Node_Is_Not_A_Numeric_Value,
	Evaluation_Of_Indices_Failed,
	Evaluation_Of_Node_Is_Forbidden,

	Arithmetic_Operation_Failed,
	Comparison_Operation_Failed,

	Conversion_To_Bool_Failed,
	Conversion_To_Numeric_Failed,
	Conversion_Generic,

	Overfull_Left_Hand_Side_Values,
	Overfull_Right_Hand_Side_Values,

	List_Iteration,

	Sequence_Environment_Index_Mismatch,
	Sequence_Environment_Could_Not_Construct,
	Sequence_Environment_Enumeration_Step_Failed,
	Sequence_Environment_Mismatch_Depth,
	Sequence_Environment_Missing_Right_Variable,
	Sequence_Environment_Mismatch_Distance,

	If_Statement_Condition_Empty,
	If_Statement_First_Branch_Empty,
	If_Statement_Second_Branch_Empty,

	Memory_Variable_Does_Not_Exist,
	Memory_Variable_Already_Exists,
	Memory_Type_Mismatch,
	Memory_Attempt_To_Assign_Value_To_Constant_Variable,

	Node_Is_Malformed,
};

} // namespace ast
} // namespace intlib
