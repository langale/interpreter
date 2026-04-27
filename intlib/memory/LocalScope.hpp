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
#include <map>
#include <any>

#include <intlib/memory/memory_error_enum.hpp>
#include <intlib/memory/VariableValue.hpp>

namespace intlib {
namespace memory {

/**
 * @brief Subscope class.
 *
 * This implements a simple association of variable names (std::string) to their
 * value (@ref interpreter::memory::variable_value). Variables can be constant or
 * mutable.
 */
class LocalScope {
public:

	/* MODIFIERS */

	void initialize();

	/**
	 * @brief Adds a new mutable variable to this subscope.
	 * @param name The name of the variable to create.
	 * @param value_w The value of the variable.
	 * @param type The type of the variable to create.
	 * @pre The variable does not exist.
	 */
	void declare_variable(
		std::string&& name, std::any&& value_w, std::string&& type
	);

	/**
	 * @brief Adds a new constant variable to this subscope.
	 * @param name The name of the variable to create.
	 * @param value_w The value of the variable.
	 * @param type The type of the variable to create.
	 * @pre The variable does not exist.
	 */
	void declare_constant_variable(
		std::string&& name, std::any&& value_w, std::string&& type
	);

	/* GETTERS */

	/**
	 * @brief Returns the value of variable @e s.
	 * @param name The name of the variable to create.
	 * @pre The variable exists.
	 */
	[[nodiscard]] const VariableValue& get_variable(const std::string& name
	) const noexcept;

	/**
	 * @brief Returns the value of variable @e s.
	 * @param name The name of the variable to create.
	 * @pre The variable exists.
	 */
	[[nodiscard]] VariableValue& get_variable(const std::string& name) noexcept;

	/// Does a variable exist?
	[[nodiscard]] bool variable_exists(const std::string& name) const noexcept
	{
		return find(name) != m_variables.end();
	}

private:

	/// Useful typedef.
	using Collection = std::map<std::string, VariableValue>;

	/// Find a variable.
	[[nodiscard]] Collection::const_iterator find(const std::string& name
	) const noexcept
	{
		return m_variables.find(name);
	}
	/// Find a variable.
	[[nodiscard]] Collection::iterator find(const std::string& name) noexcept
	{
		return m_variables.find(name);
	}

private:

	/// The collection of non-constant variables in this subscope.
	Collection m_variables;
};

} // namespace memory
} // namespace intlib
