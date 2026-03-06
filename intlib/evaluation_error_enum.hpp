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

namespace intlib {

enum class evaluation_error_e : uint8_t {
	Unhandled_Node_Type,
	Unhandled_Variable_Type,

	Undefined_Variable,
	Valueless_Variable,

	Evaluation_Of_Node_Failed,

	If_Statement_Condition_Empty
};

[[nodiscard]] constexpr std::string_view
evaluation_error_to_string(const evaluation_error_e r) noexcept
{
	switch (r) {
	case evaluation_error_e::Unhandled_Node_Type: return "Unhandled_Node_Type";
	case evaluation_error_e::Unhandled_Variable_Type:
		return "Unhandled_Variable_Type";
	case evaluation_error_e::Undefined_Variable: return "Undefined_Variable";
	case evaluation_error_e::Valueless_Variable: return "Valueless_Variable";
	case evaluation_error_e::Evaluation_Of_Node_Failed:
		return "Evaluation_Of_Node_Failed";
	case evaluation_error_e::If_Statement_Condition_Empty:
		return "If_Statement_Condition_Empty";
	default: return "?";
	}
}

} // namespace intlib

template <>
struct std::formatter<intlib::evaluation_error_e>
	: std::formatter<std::string> {
	auto
	format(const intlib::evaluation_error_e t, std::format_context& ctx) const
	{
		return std::format_to(
			ctx.out(), "{}", intlib::evaluation_error_to_string(t)
		);
	}
};

namespace intlib {

template <typename stream_t>
stream_t& operator<< (stream_t& os, const evaluation_error_e t)
{
	os << evaluation_error_to_string(t);
	return os;
}

} // namespace intlib
