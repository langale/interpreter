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

#include <intlib/memory/FunctionScope.hpp>

#if defined DEBUG
#include <cassert>
#endif

namespace intlib {
namespace memory {

/* MODIFIERS */

void FunctionScope::declare_variable(std::string&& s, std::any&& v) noexcept
{
#if defined DEBUG
	assert(not m_subscopes.back().variable_exists(s));
#endif
	m_subscopes.back().declare_variable(std::move(s), std::move(v));
}

void FunctionScope::declare_constant_variable(std::string&& s, std::any&& v) noexcept
{
#if defined DEBUG
	assert(not m_subscopes.back().variable_exists(s));
#endif
	m_subscopes.back().declare_constant_variable(std::move(s), std::move(v));
}

void FunctionScope::set_variable_value(const std::string& s, std::any&& a) noexcept
{
	for (auto it = m_subscopes.rbegin(); it != m_subscopes.rend(); ++it) {
		if (it->variable_exists(s)) {
			it->set_variable_value(s, std::move(a));
			break;
		}
	}
}

/* GETTERS */

std::optional<VariableValue>
FunctionScope::get_variable(const std::string& s) const noexcept
{
	for (auto it = m_subscopes.rbegin(); it != m_subscopes.rend(); ++it) {
		std::optional<VariableValue> r = it->get_variable(s);
		if (r.has_value()) {
			return r;
		}
	}
	return {};
}

bool FunctionScope::variable_exists(const std::string& s) const noexcept
{
	for (auto it = m_subscopes.rbegin(); it != m_subscopes.rend(); ++it) {
		if (it->variable_exists(s)) {
			return true;
		}
	}
	return false;
}

bool FunctionScope::variable_exists_shallow(const std::string& s) const noexcept
{
	return m_subscopes.back().variable_exists(s);
}

} // namespace memory
} // namespace intlib
