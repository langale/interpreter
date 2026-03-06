/*********************************************************************
 *
 * ALE language -- an interpreted programming language
 * Copyright (C) 2024 - 2026 Lluís Alemany Puig
 *
 * This file is part of the implementation of ALE. The full code is
 * available at:
 *
 *     https://github.com/langale/ale
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

#if defined DEBUG
#include <cassert>
#endif
#include <ostream>
#include <string>
#include <any>

#include <intlib/detail/any_type.hpp>

namespace intlib {
namespace detail {

/**
 * @brief Operator << for std::any.
 * @param os Output stream.
 * @param a std::any value.
 * @returns A reference to an output stream @e os.
 */
inline std::ostream& operator<< (std::ostream& os, const std::any& a)
{
	const std::string name = demangle_name_type(a.type().name());
	if (is_type<bool>(name)) {
		os << std::boolalpha << std::any_cast<bool>(a);
	}
	else if (is_type<int64_t>(name)) {
		os << std::any_cast<int64_t>(a);
	}
	else if (is_type<uint64_t>(name)) {
		os << std::any_cast<uint64_t>(a);
	}
	else if (is_type<double>(name)) {
		os << std::any_cast<double>(a);
	}
	else if (is_type<std::string>(name)) {
		os << std::any_cast<std::string>(a);
	}
	else {
#if defined DEBUG
		assert(false);
#endif
	}

	return os;
}

} // namespace detail
} // namespace intlib
