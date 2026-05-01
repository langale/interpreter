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

#include <vector>

#include <intlib/memory/LocalScope.hpp>
#include <intlib/memory/VariableValue.hpp>
#include <intlib/memory/WrappedAny.hpp>

namespace intlib {
namespace memory {

/**
 * @brief The scope class.
 *
 * A scope is a sequence of subscopes, the last of which is the last subscope
 * created and the first in which variables are looked for and stored in.
 */
class FunctionScope {
public:

	/* MODIFIERS */

	void initialize();

	/**
	 * @brief Push a new subscope.
	 *
	 * This should be called when entering a sub scope delimited by '{' '}',
	 * such as when entering an if statement, a loop, ...
	 */
	void push_local_scope()
	{
		m_local_scopes.emplace_back();
	}

	/**
	 * @brief Pops a subscope.
	 *
	 * This should be called when exiting a sub scope delimited by '{' '}',
	 * such as when leaving an if statement, a loop, ...
	 */
	void pop_local_scope() noexcept
	{
		m_local_scopes.pop_back();
	}

	/**
	 * @brief Sets the value of a non-constant variable to the current scope.
	 * @param name The name of the variable to create.
	 * @param value The value of the variable.
	 * @param type The type of the variable to create.
	 * @pre The variable does not exist.
	 */
	void declare_variable(
		std::string&& name, WrappedAny&& value, const std::string_view type
	);

	/**
	 * @brief Sets the value of a non-constant variable to the current scope.
	 * @param name The name of the variable to create.
	 * @param value The value of the variable.
	 * @param type The type of the variable to create.
	 * @pre The variable does not exist.
	 */
	void declare_constant_variable(
		std::string&& name, WrappedAny&& value, const std::string_view type
	);

	/* GETTERS */

	/**
	 * @brief Get the value of a variable.
	 *
	 * Looks for @e s in @ref m_local_scopes in a right-to-left order.
	 * @param name The name of the variable to look for.
	 * @pre The variable exists.
	 */
	[[nodiscard]] const VariableValue&
	get_variable(const std::string& name) const noexcept;

	/**
	 * @brief Get the value of a variable.
	 *
	 * Looks for @e s in @ref m_local_scopes in a right-to-left order.
	 * @param name The name of the variable to look for.
	 * @pre The variable exists.
	 */
	[[nodiscard]] VariableValue& get_variable(const std::string& name) noexcept;

	/// Does a variable exist?
	[[nodiscard]] bool variable_exists(const std::string& name) const noexcept;

	/// Does a variable exist in the current subscope?
	[[nodiscard]] bool
	variable_exists_shallow(const std::string& name) const noexcept;

private:

	/// The list of subscopes in this scope.
	std::vector<LocalScope> m_local_scopes;
};

} // namespace memory
} // namespace intlib
