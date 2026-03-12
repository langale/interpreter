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

#include <optional>
#include <string>
#include <stack>
#include <any>

#include <intlib/memory/FunctionScope.hpp>
#include <intlib/memory/AccessResult.hpp>

namespace intlib {
namespace memory {

/**
 * @brief The memory class
 *
 * This class holds all the variables declared in the program, organized in
 * function scopes (@ref interpreter::memory::scope) and local scopes
 * (@ref interpreter::memory::subscope).
 */
class Memory {
public:

	/* MODIFIERS */

	/// Initializes this scope.
	void initialize() noexcept
	{
		m_global_scope.push_subscope();
	}

	/**
	 * @brief Sets the value of a non-constant variable to the current scope.
	 * @param name Name of the variable.
	 * @param value Value of the variable.
	 */
	[[nodiscard]] AccessResult
	declare_variable(std::string&& name, std::any&& value, std::string&& type);

	/**
	 * @brief Sets the value of a constant variable to the current scope.
	 * @param name Name of the variable.
	 * @param value Value of the variable.
	 */
	[[nodiscard]] AccessResult declare_constant_variable(
		std::string&& name, std::any&& value, std::string&& type
	);

	/**
	 * @brief Sets the value of a (non-constant) variable in this subscope.
	 * @param name Variable name.
	 * @param value Value of the variable.
	 */
	[[nodiscard]] AccessResult
	set_variable_value(const std::string& name, std::any&& value) noexcept;

	/* GETTERS */

	/// Returns the value of variable @e s if it exists.
	[[nodiscard]] std::optional<VariableValue>
	get_variable(const std::string& name) const noexcept;

	/// Returns the number of scopes.
	[[nodiscard]] std::size_t num_local_scopes() const noexcept
	{
		return m_local_scopes.size();
	}

	/// Returns a reference to the current scope.
	[[nodiscard]] const FunctionScope& get_current_scope() const noexcept
	{
		return is_current_scope_global() ? m_global_scope
										 : m_local_scopes.top();
	}
	/// Returns a reference to the current scope.
	[[nodiscard]] FunctionScope& get_current_scope() noexcept
	{
		return is_current_scope_global() ? m_global_scope
										 : m_local_scopes.top();
	}

	/// Returns a reference to the global scope.
	[[nodiscard]] const FunctionScope& get_global_scope() const noexcept
	{
		return m_global_scope;
	}
	/// Returns a reference to the global scope.
	[[nodiscard]] FunctionScope& get_global_scope() noexcept
	{
		return m_global_scope;
	}

	/// Does a variable @e s exist?
	[[nodiscard]] bool variable_exists(const std::string& name) const noexcept;
	/// Does a variable @e s exist in the current subscope?
	[[nodiscard]] bool
	variable_exists_shallow(const std::string& name) const noexcept;

	/// Is the current scope the global scope?
	[[nodiscard]] bool is_current_scope_global() const noexcept
	{
		return num_local_scopes() == 0;
	}

private:

	/// The global scope of this program.
	FunctionScope m_global_scope;

	/// The stack of scopes in available in this memory.
	std::stack<FunctionScope> m_local_scopes;
};

} // namespace memory
} // namespace intlib
