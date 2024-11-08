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

#include <intlib/memory/memory.hpp>

namespace interpreter {
namespace memory {

/* MODIFIERS */

void memory::declare_variable(std::string&& s, std::any&& v) noexcept {
	if (is_current_scope_global()) {
		m_global_scope.declare_variable(std::move(s), std::move(v));
	}
	else {
		m_local_scopes.top().declare_variable(std::move(s), std::move(v));
	}
}

void memory::declare_constant_variable(std::string&& s, std::any&& v) noexcept {
	if (is_current_scope_global()) {
		m_global_scope.declare_constant_variable(std::move(s), std::move(v));
	}
	else {
		m_local_scopes.top().declare_constant_variable(std::move(s), std::move(v));
	}
}

/* GETTERS */

std::optional<variable_value> memory::get_variable(const std::string& s)
const noexcept
{
	if (num_local_scopes() > 0) {
		std::optional<variable_value> scoped = m_local_scopes.top().get_variable(s);
		if (scoped.has_value()) {
			return scoped;
		}
	}

	std::optional<variable_value> global = m_global_scope.get_variable(s);
	if (global.has_value()) {
		return global;
	}
	return {};
}

bool memory::variable_exists(const std::string& s) const noexcept {
	if (is_current_scope_global()) {
		return m_global_scope.variable_exists(s);
	}
	return
		m_local_scopes.top().variable_exists(s)
		or
		m_global_scope.variable_exists(s);
}

bool memory::variable_exists_shallow(const std::string& s) const noexcept {
	if (is_current_scope_global()) {
		return m_global_scope.variable_exists_shallow(s);
	}
	return m_local_scopes.top().variable_exists_shallow(s);
}

} // -- namespace memory
} // -- namespace interpreter
