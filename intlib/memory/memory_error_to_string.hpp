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

#include <intlib/memory/memory_error_enum.hpp>

namespace intlib {
namespace memory {

[[nodiscard]] constexpr std::string_view
memory_error_to_string(const memory_error_e r) noexcept
{
	switch (r) {
	case memory_error_e::Variable_Does_Not_Exist:
		return "Variable_Does_Not_Exist";
	case memory_error_e::Variable_Already_Exists:
		return "Variable_Already_Exists";
	case memory_error_e::Type_Mismatch: return "Type_Mismatch";
	case memory_error_e::Attempt_To_Assign_Value_To_Constant_Variable:
		return "Attempt_To_Assign_Value_To_Constant_Variable";
	default: return "?";
	}
}

} // namespace memory
} // namespace intlib

template <>
struct std::formatter<intlib::memory::memory_error_e>
	: std::formatter<std::string> {
	auto format(
		const intlib::memory::memory_error_e t, std::format_context& ctx
	) const
	{
		return std::format_to(ctx.out(), "{}", memory_error_to_string(t));
	}
};

namespace intlib {
namespace memory {

template <typename stream_t>
stream_t& operator<< (stream_t& os, const memory_error_e t)
{
	os << memory_error_to_string(t);
	return os;
}

} // namespace memory
} // namespace intlib
