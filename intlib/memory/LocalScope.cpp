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

#include <intlib/memory/LocalScope.hpp>

namespace intlib {
namespace memory {

#define SUCCESSFUL                                                             \
	return AccessResult { }

/* MODIFIERS */

AccessResult LocalScope::declare_variable(
	std::string&& name, std::any&& value, std::string&& type
) noexcept
{
	Collection::iterator it = find(name);

	if (it != m_variables.end()) {
		return std::unexpected{access_error_e::Variable_Already_Exists};
	}

	m_variables.insert(
		{std::move(name),
		 {.value = std::move(value),
		  .type = std::move(type),
		  .is_constant = false}}
	);
	SUCCESSFUL;
}

AccessResult LocalScope::declare_constant_variable(
	std::string&& name, std::any&& value, std::string&& type
) noexcept
{
	Collection::iterator it = find(name);

	if (it != m_variables.end()) {
		return std::unexpected{access_error_e::Variable_Already_Exists};
	}

	m_variables.insert(
		{std::move(name),
		 {.value = std::move(value),
		  .type = std::move(type),
		  .is_constant = true}}
	);
	SUCCESSFUL;
}

AccessResult LocalScope::set_variable_value(
	const std::string& name, std::any&& value
) noexcept
{
	Collection::iterator it = find(name);

	if (it == m_variables.end()) {
		return std::unexpected{access_error_e::Variable_Does_Not_Exist};
	}

	if (it->second.is_constant) {
		return std::unexpected{
			access_error_e::Attempt_To_Assign_Value_To_Constant_Variable
		};
	}

	it->second.value = std::move(value);
	SUCCESSFUL;
}

/* GETTERS */

std::optional<VariableValue>
LocalScope::get_variable(const std::string& name) const noexcept
{
	const auto it = find(name);
	if (it == m_variables.end()) {
		return {};
	}
	return it->second;
}

} // namespace memory
} // namespace intlib
