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

#include <intlib/memory/subscope.hpp>

// C++ includes
#if defined DEBUG
#include <cassert>
#endif

namespace interpreter {
namespace memory {

/* MODIFIERS */

void subscope::declare_variable
(std::string&& s, std::any&& a)
noexcept
{
	Collection::iterator it = find(s);

#if defined DEBUG
	assert(it == m_variables.end());
#endif

	m_variables.insert({
		std::move(s),
		{
			.value = std::move(a),
			.is_constant = false
		}
	});
}

void subscope::declare_constant_variable
(std::string&& s, std::any&& a)
noexcept
{
	Collection::iterator it = find(s);

#if defined DEBUG
	assert(it == m_variables.end());
#endif

	m_variables.insert({
		std::move(s),
		{
			.value = std::move(a),
			.is_constant = true
		}
	});
}

void subscope::set_variable_value
(const std::string& s, std::any&& a)
noexcept
{
	Collection::iterator it = find(s);

#if defined DEBUG
	assert(it != m_variables.end());
	assert(not it->second.is_constant);
#endif

	it->second.value = std::move(a);
}

/* GETTERS */

std::optional<variable_value> subscope::get_variable
(const std::string& s)
const noexcept
{
	const auto it = find(s);
	if (it == m_variables.end()) { return {}; }
	return it->second;
}

} // -- namespace memory
} // -- namespace interpreter
