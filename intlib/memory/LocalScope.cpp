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
#include <intlib/memory/LocalScope.hpp>

namespace intlib {
namespace memory {

/* MODIFIERS */

void LocalScope::initialize() { }

void LocalScope::declare_variable(
	std::string&& name, std::any&& value_w, std::string&& type
)
{
	INTERPRETER_ENTER_MEMORY_FUNCTION(ale::logger::println);

	const auto it = find(name);

#if defined DEBUG
	assert(it == m_variables.end());
#endif

	m_variables.emplace(
		std::pair<std::string, VariableValue>{
			std::move(name),
			{.value_w = std::move(value_w),
			 .type = std::move(type),
			 .is_constant = false}
		}
	);
}

void LocalScope::declare_constant_variable(
	std::string&& name, std::any&& value_w, std::string&& type
)
{
	INTERPRETER_ENTER_MEMORY_FUNCTION(ale::logger::println);

	const auto it = find(name);

#if defined DEBUG
	assert(it == m_variables.end());
#endif

	m_variables.emplace(
		std::pair<std::string, VariableValue>{
			std::move(name),
			{.value_w = std::move(value_w),
			 .type = std::move(type),
			 .is_constant = true}
		}
	);
}

/* GETTERS */

const VariableValue&
LocalScope::get_variable(const std::string& name) const noexcept
{
	INTERPRETER_ENTER_MEMORY_FUNCTION(ale::logger::println);

	const auto it = find(name);
#if defined DEBUG
	assert(it != m_variables.end());
#endif
	return it->second;
}

VariableValue& LocalScope::get_variable(const std::string& name) noexcept
{
	INTERPRETER_ENTER_MEMORY_FUNCTION(ale::logger::println);

	const auto it = find(name);
#if defined DEBUG
	assert(it != m_variables.end());
#endif
	return it->second;
}

} // namespace memory
} // namespace intlib
