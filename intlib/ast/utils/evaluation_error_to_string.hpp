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

#include <format>

#include <intlib/ast/utils/evaluation_error_enum.hpp>

namespace intlib {
namespace ast {

[[nodiscard]] constexpr std::string_view
evaluation_error_to_string(const evaluation_error_e e) noexcept
{
	switch (e) {
	case evaluation_error_e::Unhandled_Node_Type: return "Unhandled_Node_Type";
	case evaluation_error_e::Unhandled_Variable_Type:
		return "Unhandled_Variable_Type";

	case evaluation_error_e::Undefined_Variable: return "Undefined_Variable";
	case evaluation_error_e::Valueless_Variable: return "Valueless_Variable";

	case evaluation_error_e::Evaluation_Of_Node_Failed:
		return "Evaluation_Of_Node_Failed";
	case evaluation_error_e::Evaluation_Of_Node_Is_Void:
		return "Evaluation_Of_Node_Is_Void";
	case evaluation_error_e::Evaluation_Of_Node_While_Loop_Failed:
		return "Evaluation_Of_Node_While_Loop_Failed";
	case evaluation_error_e::Evaluation_Of_Node_Is_Not_A_Boolean_Value:
		return "Evaluation_Of_Node_Is_Not_A_Boolean_Value";
	case evaluation_error_e::Evaluation_Of_Node_Is_Not_A_Numeric_Value:
		return "Evaluation_Of_Node_Is_Not_A_Numeric_Value";

	case evaluation_error_e::Arithmetic_Operation_Failed:
		return "Arithmetic_Operation_Failed";
	case evaluation_error_e::Comparison_Operation_Failed:
		return "Comparison_Operation_Failed";

	case evaluation_error_e::If_Statement_Condition_Empty:
		return "If_Statement_Condition_Empty";
	case evaluation_error_e::If_Statement_First_Branch_Empty:
		return "If_Statement_First_Branch_Empty";
	case evaluation_error_e::If_Statement_Second_Branch_Empty:
		return "If_Statement_Second_Branch_Empty";

	case evaluation_error_e::Function_Main_Missing:
		return "Function_Main_Missing";
	case evaluation_error_e::Function_Missing: return "Function_Missing";

	case evaluation_error_e::Evaluation_Of_Node_Is_Forbidden:
		return "Forbidden_Evaluation_Of_Node";
	case evaluation_error_e::Evaluation_Of_Indices_Failed:
		return "Evaluation_Of_Indices_Failed";

	case evaluation_error_e::Conversion_To_Bool_Failed:
		return "Conversion_To_Bool_Failed";
	case evaluation_error_e::Conversion_To_Numeric_Failed:
		return "Conversion_To_Numeric_Failed";
	case evaluation_error_e::Conversion_Generic: return "Conversion_Generic";

	case evaluation_error_e::Memory_Variable_Does_Not_Exist:
		return "Memory_Variable_Does_Not_Exist";
	case evaluation_error_e::Memory_Variable_Already_Exists:
		return "Memory_Variable_Already_Exists";
	case evaluation_error_e::Memory_Type_Mismatch:
		return "Memory_Type_Mismatch";
	case evaluation_error_e::
		Memory_Attempt_To_Assign_Value_To_Constant_Variable:
		return "Memory_Attempt_To_Assign_Value_To_Constant_Variable";

	case evaluation_error_e::Assignation_Of_Variable:
		return "Assignation_Of_Variable";
	case evaluation_error_e::Declaration_Of_Variable:
		return "Declaration_Of_Variable";

	case evaluation_error_e::Sequence_Environment_Index_Mismatch:
		return "Sequence_Environment_Index_Mismatch";
	case evaluation_error_e::Sequence_Environment_Could_Not_Construct:
		return "Sequence_Environment_Could_Not_Construct";
	case evaluation_error_e::Sequence_Environment_Enumeration_Step_Failed:
		return "Sequence_Environment_Enumeration_Step_Failed";
	case evaluation_error_e::Sequence_Environment_Mismatch_Depth:
		return "Sequence_Environment_Mismatch_Depth";
	case evaluation_error_e::Sequence_Environment_Missing_Right_Variable:
		return "Sequence_Environment_Missing_Right_Variable";
	case evaluation_error_e::Sequence_Environment_Mismatch_Distance:
		return "Sequence_Environment_Mismatch_Distance";
	case evaluation_error_e::Sequence_Evaluation_Failed:
		return "Sequence_Evaluation_Failed";

	case evaluation_error_e::Overfull_Left_Hand_Side_Values:
		return "Overfull_Left_Hand_Side_Values";
	case evaluation_error_e::Overfull_Right_Hand_Side_Values:
		return "Overfull_Right_Hand_Side_Values";
	case evaluation_error_e::List_Iteration:	return "List_Iteration";
	case evaluation_error_e::Node_Is_Malformed: return "Node_Is_Malformed";
	}

	return "??";
}

} // namespace ast
} // namespace intlib

template <>
struct std::formatter<intlib::ast::evaluation_error_e>
	: std::formatter<std::string> {
	auto format(
		const intlib::ast::evaluation_error_e t, std::format_context& ctx
	) const
	{
		return std::format_to(
			ctx.out(), "{}", intlib::ast::evaluation_error_to_string(t)
		);
	}
};

namespace intlib {
namespace ast {

template <typename stream_t>
stream_t& operator<< (stream_t& os, const evaluation_error_e t)
{
	os << evaluation_error_to_string(t);
	return os;
}

} // namespace ast
} // namespace intlib
