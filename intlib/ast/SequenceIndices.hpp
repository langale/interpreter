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

#include <unordered_map>
#include <string_view>
#include <cstdint>
#include <vector>

namespace intlib {
namespace ast {

class SequenceIndices {
public:

	using VariableToIndex = std::unordered_map<std::string_view, int64_t>;

public:

	/* SETTERS */

	void reserve(const size_t n)
	{
		m_indices.reserve(n);
	}

	void set_index(size_t d, std::string_view var, int64_t i);

	/* GETTERS */

	/**
	 * @brief The depth of the sequence in number of indices.
	 *
	 * This value, so-called depth, corresponds to the largest number of indices
	 * among all subscripted variables in the sequence. That is, if there is one
	 * variable with two subscripts, and all the others have at most one subscript
	 * then the depth is 2.
	 */
	[[nodiscard]] size_t get_depth() const noexcept
	{
		return m_indices.size();
	}

	[[nodiscard]] int64_t
	get_index(size_t d, std::string_view var) const noexcept;

	[[nodiscard]] bool
	has_index(const size_t d, const std::string_view var) const noexcept
	{
		return d < m_indices.size() and m_indices[d].contains(var);
	}

	[[nodiscard]] decltype(auto)
	get_variables_depth(this auto& self, const size_t d) noexcept
	{
		return (self.m_indices[d]);
	}

private:

	std::vector<VariableToIndex> m_indices;
};

} // namespace ast
} // namespace intlib
