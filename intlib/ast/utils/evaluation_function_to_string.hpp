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

#include <format>

#include <intlib/ast/utils/evaluation_function_enum.hpp>

namespace intlib {
namespace ast {

[[nodiscard]] constexpr std::string_view
evaluation_function_to_string(const evaluation_function_e f) noexcept
{
	switch (f) {
	case evaluation_function_e::Assignation: return "Assignation";
	case evaluation_function_e::Declaration: return "Declaration";
	case evaluation_function_e::Sequence:	 return "Sequence";
	case evaluation_function_e::While_Loop:	 return "While_Loop";

	case evaluation_function_e::Arithmetic:	 return "Arithmetic";
	case evaluation_function_e::Comma_Separated_Group:
		return "Comma_Separated_Group";
	case evaluation_function_e::Comparison:		   return "Comparison";
	case evaluation_function_e::Logical:		   return "Logical";
	case evaluation_function_e::Program:		   return "Program";
	case evaluation_function_e::Subscope_Modifier: return "Subscope_Modifier";
	case evaluation_function_e::Subscripted_Variable:
		return "Subscripted_Variable";

	case evaluation_function_e::If_Else:		return "If_Else";

	case evaluation_function_e::Negation:		return "Negation";
	case evaluation_function_e::Negative:		return "Negative";
	case evaluation_function_e::Positive:		return "Positive";

	case evaluation_function_e::False:			return "False";
	case evaluation_function_e::Literal:		return "Literal";
	case evaluation_function_e::True:			return "True";
	case evaluation_function_e::Variable:		return "Variable";

	case evaluation_function_e::Variable_Names: return "Variable_Names";

	case evaluation_function_e::Iterator_Value_Variable:
		return "Iterator_Value_Variable";
	case evaluation_function_e::Iterator_Value_Subscripted_Variable:
		return "Iterator_Value_Subscripted_Variable";
	case evaluation_function_e::Iterator_Value_Sequence:
		return "Iterator_Value_Sequence";
	case evaluation_function_e::Iterator_Value_Comma_Separated_Group:
		return "Iterator_Value_Comma_Separated_Group";
	case evaluation_function_e::Iterator_Name_Variable:
		return "Iterator_Name_Variable";
	case evaluation_function_e::Iterator_Name_Subscripted_Variable:
		return "Iterator_Name_Subscripted_Variable";
	case evaluation_function_e::Iterator_Name_Sequence:
		return "Iterator_Name_Sequence";
	case evaluation_function_e::Iterator_Name_Comma_Separated_Group:
		return "Iterator_Name_Comma_Separated_Group";

	case evaluation_function_e::Sequence_Execution_Environment_Construction:
		return "Sequence_Index_Extraction";

	case evaluation_function_e::Interpretation_Node:
		return "Interpretation_Node";
	}

	return "??";
}

} // namespace ast
} // namespace intlib

template <>
struct std::formatter<intlib::ast::evaluation_function_e>
	: std::formatter<std::string> {
	auto format(
		const intlib::ast::evaluation_function_e t, std::format_context& ctx
	) const
	{
		return std::format_to(
			ctx.out(), "{}", intlib::ast::evaluation_function_to_string(t)
		);
	}
};

namespace intlib {
namespace ast {

template <typename stream_t>
stream_t& operator<< (stream_t& os, const evaluation_function_e t)
{
	os << evaluation_function_to_string(t);
	return os;
}

} // namespace ast
} // namespace intlib
