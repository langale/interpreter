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
#include <intlib/memory/Memory.hpp>

namespace intlib {
namespace memory {

#define aleprln ale::logger::println

/* MODIFIERS */

void Memory::initialize() noexcept
{
	m_global_scope.initialize();
}

void Memory::declare_variable(
	std::string&& name, std::any&& value_w, const std::string_view type
)
{
	INTERPRETER_ENTER_MEMORY_FUNCTION(aleprln);

	if (not is_current_scope_global()) {
		m_function_scopes.top().declare_variable(
			std::move(name), std::move(value_w), type
		);
	}
	else {
		m_global_scope.declare_variable(
			std::move(name), std::move(value_w), type
		);
	}
}

void Memory::declare_constant_variable(
	std::string&& name, std::any&& value_w, const std::string_view type
)
{
	INTERPRETER_ENTER_MEMORY_FUNCTION(aleprln);

	if (not is_current_scope_global()) {
		m_function_scopes.top().declare_constant_variable(
			std::move(name), std::move(value_w), type
		);
	}
	else {
		m_global_scope.declare_constant_variable(
			std::move(name), std::move(value_w), type
		);
	}
}

/* GETTERS */

const VariableValue&
Memory::get_variable(const std::string& name) const noexcept
{
	INTERPRETER_ENTER_MEMORY_FUNCTION(aleprln);

	if (not is_current_scope_global()) {
		return m_function_scopes.top().get_variable(name);
	}
	return m_global_scope.get_variable(name);
}

VariableValue& Memory::get_variable(const std::string& name) noexcept
{
	INTERPRETER_ENTER_MEMORY_FUNCTION(aleprln);

	if (not is_current_scope_global()) {
		return m_function_scopes.top().get_variable(name);
	}
	return m_global_scope.get_variable(name);
}

FunctionScope& Memory::get_current_scope() noexcept
{
	INTERPRETER_ENTER_MEMORY_FUNCTION(aleprln);

#if defined DEBUG
	assert(not is_current_scope_global());
#endif
	return m_function_scopes.top();
}

const FunctionScope& Memory::get_current_scope() const noexcept
{
	INTERPRETER_ENTER_MEMORY_FUNCTION(aleprln);

#if defined DEBUG
	assert(not is_current_scope_global());
#endif
	return m_function_scopes.top();
}

bool Memory::variable_exists(const std::string& name) const noexcept
{
	INTERPRETER_ENTER_MEMORY_FUNCTION(aleprln);

	if (not is_current_scope_global()) {
		return m_function_scopes.top().variable_exists(name)
				   ? true
				   : m_global_scope.variable_exists(name);
	}
	return m_global_scope.variable_exists(name);
}

bool Memory::variable_exists_shallow(const std::string& name) const noexcept
{
	INTERPRETER_ENTER_MEMORY_FUNCTION(aleprln);

	if (not is_current_scope_global()) {
		return m_function_scopes.top().variable_exists_shallow(name);
	}
	return m_global_scope.variable_exists(name);
}

} // namespace memory
} // namespace intlib
