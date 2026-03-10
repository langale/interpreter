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

#include <intlib/memory/FunctionScope.hpp>

namespace intlib {
namespace memory {

#define SUCCESSFUL                                                             \
	return AccessResult { }

/* MODIFIERS */

AccessResult FunctionScope::declare_variable(
	std::string&& name, std::any&& value, std::string&& type
) noexcept
{
	const auto res = m_subscopes.back().declare_variable(
		std::move(name), std::move(value), std::move(type)
	);

	if (not res.has_value()) {
		return std::unexpected{res.error()};
	}
	SUCCESSFUL;
}

AccessResult FunctionScope::declare_constant_variable(
	std::string&& name, std::any&& value, std::string&& type
) noexcept
{
	const auto res = m_subscopes.back().declare_constant_variable(
		std::move(name), std::move(value), std::move(type)
	);

	if (not res.has_value()) {
		return std::unexpected{res.error()};
	}
	SUCCESSFUL;
}

AccessResult FunctionScope::set_variable_value(
	const std::string& name, std::any&& value, const std::string& type
) noexcept
{
	for (auto it = m_subscopes.rbegin(); it != m_subscopes.rend(); ++it) {
		if (it->variable_exists(name)) {
			const auto res =
				it->set_variable_value(name, std::move(value), type);

			if (not res.has_value()) {
				return std::unexpected{res.error()};
			}

			break;
		}
	}

	SUCCESSFUL;
}

/* GETTERS */

std::optional<VariableValue>
FunctionScope::get_variable(const std::string& name) const noexcept
{
	for (auto it = m_subscopes.rbegin(); it != m_subscopes.rend(); ++it) {
		std::optional<VariableValue> r = it->get_variable(name);
		if (r.has_value()) {
			return r;
		}
	}
	return {};
}

bool FunctionScope::variable_exists(const std::string& name) const noexcept
{
	for (auto it = m_subscopes.rbegin(); it != m_subscopes.rend(); ++it) {
		if (it->variable_exists(name)) {
			return true;
		}
	}
	return false;
}

bool FunctionScope::variable_exists_shallow(
	const std::string& name
) const noexcept
{
	return m_subscopes.back().variable_exists(name);
}

} // namespace memory
} // namespace intlib
