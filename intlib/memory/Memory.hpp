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

#include <string>
#include <stack>

#include <intlib/memory/FunctionScope.hpp>
#include <intlib/memory/VariableValue.hpp>

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

	/// Initializes the memory.
	void initialize() noexcept;

	/**
	 * @brief Sets the value of a non-constant variable to the current scope.
	 * @param name The name of the variable to create.
	 * @param value The value of the variable.
	 * @param type The type of the variable to create.
	 * @pre The variable does not exist.
	 */
	void
	declare_variable(std::string&& name, std::any&& value, std::string&& type);

	/**
	 * @brief Sets the value of a constant variable to the current scope.
	 * @param name The name of the variable to create.
	 * @param value The value of the variable.
	 * @param type The type of the variable to create.
	 * @pre The variable does not exist.
	*/
	void declare_constant_variable(
		std::string&& name, std::any&& value, std::string&& type
	);

	/* GETTERS */

	/**
	 * @brief Returns the value of variable @e s.
	 * @pre The variable exists.
	 */
	[[nodiscard]] const VariableValue&
	get_variable(const std::string& name) const noexcept;

	/**
	 * @brief Returns the value of variable @e s.
	 * @pre The variable exists.
	 */
	[[nodiscard]] VariableValue& get_variable(const std::string& name) noexcept;

	/// Returns the number of scopes.
	[[nodiscard]] std::size_t num_local_scopes() const noexcept
	{
		return m_function_scopes.size();
	}

	[[nodiscard]] LocalScope& get_global_scope() noexcept
	{
		return m_global_scope;
	}
	[[nodiscard]] const LocalScope& get_global_scope() const noexcept
	{
		return m_global_scope;
	}

	[[nodiscard]] FunctionScope& get_current_scope() noexcept;
	[[nodiscard]] const FunctionScope& get_current_scope() const noexcept;

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

	LocalScope m_global_scope;

	/// The stack of scopes in available in this memory.
	std::stack<FunctionScope> m_function_scopes;
};

} // namespace memory
} // namespace intlib
