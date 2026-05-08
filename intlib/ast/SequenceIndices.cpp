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

#include <intlib/ast/SequenceIndices.hpp>

#if defined DEBUG
#include <cassert>
#endif
#include <cstdint>

namespace intlib {
namespace ast {

void SequenceIndices::add_depth(const size_t d)
{
	if (d >= m_indices.size()) {
		m_indices.emplace_back();
	}
}

void SequenceIndices::set_index(
	const size_t d, const std::string_view var, const int64_t i
)
{
	if (d >= m_indices.size()) {
		m_indices.resize(d + 1);
	}
	m_indices[d].insert({var, i});
}

int64_t SequenceIndices::get_index(const size_t d, const std::string_view var)
	const noexcept
{
#if defined DEBUG
	assert(d < m_indices.size());
	assert(m_indices[d].contains(var));
#endif
	return m_indices[d].find(var)->second;
}

} // namespace ast
} // namespace intlib
