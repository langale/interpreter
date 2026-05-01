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

#if defined DEBUG
#include <cassert>
#endif

#include <intlib/logger/macros.hpp>
#include <intlib/memory/FunctionScope.hpp>

namespace intlib {
namespace memory {

/* MODIFIERS */

void FunctionScope::initialize()
{
	INTERPRETER_ENTER_MEMORY_FUNCTION;

	m_local_scopes.emplace_back();
}

void FunctionScope::declare_variable(
	std::string&& name, WrappedAny&& value, const std::string_view type
)
{
	INTERPRETER_ENTER_MEMORY_FUNCTION;

	m_local_scopes.back().declare_variable(
		std::move(name), std::move(value), type
	);
}

void FunctionScope::declare_constant_variable(
	std::string&& name, WrappedAny&& value, const std::string_view type
)
{
	INTERPRETER_ENTER_MEMORY_FUNCTION;

	m_local_scopes.back().declare_constant_variable(
		std::move(name), std::move(value), type
	);
}

/* GETTERS */

const Variable& FunctionScope::get_variable(const std::string& name
) const noexcept
{
	INTERPRETER_ENTER_MEMORY_FUNCTION;

	for (auto it = m_local_scopes.rbegin(); it != m_local_scopes.rend(); ++it) {
		if (it->variable_exists(name)) {
			return it->get_variable(name);
		}
	}

#if defined DEBUG
	assert(false);
#endif

	// so the compiler does not cry in release compilations
	return empty_variable;
}

Variable& FunctionScope::get_variable(const std::string& name) noexcept
{
	INTERPRETER_ENTER_MEMORY_FUNCTION;

	for (auto it = m_local_scopes.rbegin(); it != m_local_scopes.rend(); ++it) {
		if (it->variable_exists(name)) {
			return it->get_variable(name);
		}
	}

#if defined DEBUG
	assert(false);
#endif

	// so the compiler does not cry in release compilations
	return empty_variable;
}

bool FunctionScope::variable_exists(const std::string& name) const noexcept
{
	INTERPRETER_ENTER_MEMORY_FUNCTION;

	for (auto it = m_local_scopes.rbegin(); it != m_local_scopes.rend(); ++it) {
		if (it->variable_exists(name)) {
			return true;
		}
	}
	return false;
}

bool FunctionScope::variable_exists_shallow(const std::string& name
) const noexcept
{
	INTERPRETER_ENTER_MEMORY_FUNCTION;

	return m_local_scopes.back().variable_exists(name);
}

} // namespace memory
} // namespace intlib
