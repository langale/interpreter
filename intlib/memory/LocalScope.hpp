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
#include <expected>
#include <string>
#include <map>
#include <any>

#include <intlib/memory/memory_error_enum.hpp>
#include <intlib/memory/AccessResult.hpp>

namespace intlib {
namespace memory {

/// Data associated to each variable name.
struct VariableValue {
	/// The actual value that the variable holds.
	std::any value;
	/// The type of this variable.
	const std::string type;
	/// Whether or not the variable is declared with 'const'.
	const bool is_constant;
};

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

	/**
	 * @brief Adds a new non-constant variable to this subscope.
	 * @param name Variable name.
	 * @param value Value of the variable.
	 */
	[[nodiscard]] AccessResult declare_variable(
		std::string&& name, std::any&& value, std::string&& type
	) noexcept;

	/**
	 * @brief Adds a new constant variable to this subscope.
	 * @param name Variable name.
	 * @param value Value of the variable.
	 */
	[[nodiscard]] AccessResult declare_constant_variable(
		std::string&& name, std::any&& value, std::string&& type
	) noexcept;

	/**
	 * @brief Sets the value of a (non-constant) variable in this subscope.
	 * @param name Variable name.
	 * @param value Value of the variable.
	 */
	[[nodiscard]] AccessResult set_variable_value(
		const std::string& name, std::any&& value, const std::string& type
	) noexcept;

	/* GETTERS */

	/**
	 * @brief Gets the value of a non-constant variable.
	 * @param name The name of the variable to look for.
	 * @returns The value of the variable if it exists.
	 */
	[[nodiscard]] std::optional<VariableValue>
	get_variable(const std::string& name) const noexcept;

	/// Does variable @e s exist?
	[[nodiscard]] bool variable_exists(const std::string& name) const noexcept
	{
		return find(name) != m_variables.end();
	}

private:

	/// Useful typedef.
	using Collection = std::map<std::string, VariableValue>;

	/// Find a variable @e s.
	[[nodiscard]] Collection::const_iterator
	find(const std::string& s) const noexcept
	{
		return m_variables.find(s);
	}
	/// Find a variable @e s.
	[[nodiscard]] Collection::iterator find(const std::string& s) noexcept
	{
		return m_variables.find(s);
	}

private:

	/// The collection of non-constant variables in this subscope.
	Collection m_variables;
};

} // namespace memory
} // namespace intlib
