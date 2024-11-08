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
#include <optional>
#include <string>
#include <stack>
#include <any>

// interpreter includes
#include <intlib/memory/scope.hpp>

namespace interpreter {
namespace memory {

/**
 * @brief The memory class
 *
 * This class holds all the variables declared in the program, organized in
 * scopes (@ref interpreter::memory::scope) and subscopes (@ref interpreter::memory::subscope).
 */
class memory {
public:
	/* MODIFIERS */

	/// Initializes this scope.
	void initialize() noexcept {
		m_global_scope.push_subscope();
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
	 */
	void declare_constant_variable(std::string&& s, std::any&& v) noexcept;

	/**
	 * @brief Sets the value of a (non-constant) variable in this subscope.
	 * @param s Variable name.
	 * @param a Value of the variable.
	 */
	void set_variable_value(const std::string& s, std::any&& a) noexcept;

	/* GETTERS */

	/// Returns the value of variable @e s if it exists.
	std::optional<variable_value> get_variable(const std::string& s) const noexcept;

	/// Returns the number of scopes.
	std::size_t num_local_scopes() const noexcept { return m_local_scopes.size(); }

	/// Returns a reference to the current scope.
	const scope& get_current_scope() const noexcept {
		return is_current_scope_global() ? m_global_scope : m_local_scopes.top();
	}
	/// Returns a reference to the current scope.
	scope& get_current_scope() noexcept {
		return is_current_scope_global() ? m_global_scope : m_local_scopes.top();
	}

	/// Returns a reference to the global scope.
	const scope& get_global_scope() const noexcept { return m_global_scope; }
	/// Returns a reference to the global scope.
	scope& get_global_scope() noexcept { return m_global_scope; }

	/// Does a variable @e s exist?
	bool variable_exists(const std::string& s) const noexcept;
	/// Does a variable @e s exist in the current subscope?
	bool variable_exists_shallow(const std::string& s) const noexcept;

	/// Is the current scope the global scope?
	bool is_current_scope_global() const noexcept {
		return num_local_scopes() == 0;
	}

private:
	/// The global scope of this program.
	scope m_global_scope;

	/// The stack of scopes in available in this memory.
	std::stack<scope> m_local_scopes;
};

} // -- namespace memory
} // -- namespace interpreter
