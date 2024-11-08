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

// C++ includes
#if defined DEBUG
#include <cassert>
#endif

// ale includes
#include <ale/logger.hpp>
#include <ale/detail/any_type.hpp>

// program includes
#include <intlib/program.hpp>

namespace interpreter {

std::optional<std::any> program::evaluate(const ale::ast::variable_node& v) noexcept
{
	const std::string& name = v.get_name();

	if (not m_memory.variable_exists(name)) {
		ale::error() << ERROR_LOCATION << '\n';
		ale::error() << "    Variable '" << name << "' undefined in the current scope.\n";
		return {};
	}

	std::optional<memory::variable_value> res = m_memory.get_variable(name);
#if defined DEBUG
	assert(res.has_value());
#endif

	if (ale::detail::is_type<void>(res->value)) {
		ale::error() << ERROR_LOCATION << '\n';
		ale::error() << "    Variable '" << name << "' has no value in the current scope.\n";
		return {};
	}

	return std::move(res->value);
}

} // -- namespace interpreter
