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

// ale includes
#include <ale/detail/any_type.hpp>
#include <ale/detail/macros.hpp>

namespace interpreter {
namespace detail {

[[nodiscard]]
inline std::optional<bool> any_to_bool
(const std::any& a)
noexcept
{
	if (ale::detail::is_type<bool>(a)) { return std::any_cast<bool>(a); }
	if (ale::detail::is_type<int64_t>(a)) { return std::any_cast<int64_t>(a); }
	if (ale::detail::is_type<uint64_t>(a)) { return std::any_cast<uint64_t>(a); }
	if (ale::detail::is_type<double>(a)) { return ale::detail::to_int64(std::any_cast<double>(a)); }
	return {};
}

} // -- namespace detail
} // -- namespace interpreter
