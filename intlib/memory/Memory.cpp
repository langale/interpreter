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

#include <intlib/memory/Memory.hpp>

namespace intlib {
namespace memory {

/* MODIFIERS */

void Memory::declare_variable
(std::string&& s, std::any&& v)
noexcept
{
	if (is_current_scope_global()) {
		m_global_scope.declare_variable(std::move(s), std::move(v));
	}
	else {
		m_local_scopes.top().declare_variable(std::move(s), std::move(v));
	}
}

void Memory::declare_constant_variable
(std::string&& s, std::any&& v)
noexcept
{
	if (is_current_scope_global()) {
		m_global_scope.declare_constant_variable(std::move(s), std::move(v));
	}
	else {
		m_local_scopes.top().declare_constant_variable(std::move(s), std::move(v));
	}
}

/* GETTERS */

std::optional<VariableValue> Memory::get_variable
(const std::string& s)
const noexcept
{
	if (num_local_scopes() > 0) {
		std::optional<VariableValue> scoped = m_local_scopes.top().get_variable(s);
		if (scoped.has_value()) {
			return scoped;
		}
	}

	std::optional<VariableValue> global = m_global_scope.get_variable(s);
	if (global.has_value()) {
		return global;
	}
	return {};
}

bool Memory::variable_exists
(const std::string& s)
const noexcept
{
	if (is_current_scope_global()) {
		return m_global_scope.variable_exists(s);
	}
	return
		m_local_scopes.top().variable_exists(s)
		or
		m_global_scope.variable_exists(s);
}

bool Memory::variable_exists_shallow
(const std::string& s)
const noexcept
{
	if (is_current_scope_global()) {
		return m_global_scope.variable_exists_shallow(s);
	}
	return m_local_scopes.top().variable_exists_shallow(s);
}

} // -- namespace memory
} // -- namespace intlib
