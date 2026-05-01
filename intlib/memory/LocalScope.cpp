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

#define aleprln ale::logger::println

/* MODIFIERS */

void LocalScope::declare_variable(
	std::string&& name, WrappedAny&& value, const std::string_view type
)
{
	INTERPRETER_ENTER_MEMORY_FUNCTION(aleprln);

	const auto it = find(name);

#if defined DEBUG
	assert(it == m_variables.end());
#endif

	m_variables.emplace(
		std::move(name),
		Variable{
			.wrap = std::move(value), .ale_type = type, .is_constant = false
		}
	);
}

void LocalScope::declare_constant_variable(
	std::string&& name, WrappedAny&& value, const std::string_view type
)
{
	INTERPRETER_ENTER_MEMORY_FUNCTION(aleprln);

	const auto it = find(name);

#if defined DEBUG
	assert(it == m_variables.end());
#endif

	m_variables.emplace(
		std::move(name),
		Variable{
			.wrap = std::move(value), .ale_type = type, .is_constant = true
		}
	);
}

/* GETTERS */

const Variable&
LocalScope::get_variable(const std::string& name) const noexcept
{
	INTERPRETER_ENTER_MEMORY_FUNCTION(aleprln);

	const auto it = find(name);
#if defined DEBUG
	assert(it != m_variables.end());
#endif
	return it->second;
}

Variable& LocalScope::get_variable(const std::string& name) noexcept
{
	INTERPRETER_ENTER_MEMORY_FUNCTION(aleprln);

	const auto it = find(name);
#if defined DEBUG
	assert(it != m_variables.end());
#endif
	return it->second;
}

} // namespace memory
} // namespace intlib
