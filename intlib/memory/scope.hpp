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

#pragma once

// C++ includes
#include <vector>

// interpreter includes
#include <intlib/memory/subscope.hpp>

namespace interpreter {
namespace memory {

/**
 * @brief The scope class.
 *
 * A scope is a sequence of subscopes, the last of which is the last subscope
 * created and the first in which variables are looked for and stored in.
 */
class scope {
public:

	/* MODIFIERS */

	/**
	 * @brief Push a new subscope.
	 *
	 * This should be called when entering a sub scope delimited by '{' '}',
	 * such as when entering an if statement, a loop, ...
	 */
	void push_subscope() noexcept {
		m_subscopes.push_back({});
	}
	/**
	 * @brief Pops a subscope.
	 *
	 * This should be called when exiting a sub scope delimited by '{' '}',
	 * such as when leaving an if statement, a loop, ...
	 */
	void pop_subscope() noexcept {
		m_subscopes.pop_back();
	}

	/**
	 * @brief Sets the value of a non-constant variable to the current scope.
	 * @param s Name of the variable.
	 * @param v Value of the variable.
	 */
	void declare_variable(std::string&& s, std::any&& v) noexcept;

	/**
	 * @brief Sets the value of a constant variable to the current scope.
	 * @param s Name of the variable.
	 * @param v Value of the variable.
	 * @pre Variable @e s does not already exist in the current subscope.
	 */
	void declare_constant_variable(std::string&& s, std::any&& v) noexcept;

	/**
	 * @brief Sets the value of a (non-constant) variable in this subscope.
	 * @param s Variable name.
	 * @param a Value of the variable.
	 */
	void set_variable_value(const std::string& s, std::any&& a) noexcept;

	/* GETTERS */

	/**
	 * @brief Get the value of a variable.
	 *
	 * This method looks for @e s in @ref m_subscopes in a right-to-left order.
	 * @param s The name of the variable to look for.
	 * @returns The value of the variable if it exists.
	 */
	std::optional<variable_value> get_variable(const std::string& s) const noexcept;

	/// Does a variable @e s exist?
	bool variable_exists(const std::string& s) const noexcept;
	/// Does a variable @e s exist in the current subscope?
	bool variable_exists_shallow(const std::string& s) const noexcept;

private:
	/// The list of subscopes in this scope.
	std::vector<subscope> m_subscopes;
};

} // -- namespace memory
} // -- namespace interpreter
