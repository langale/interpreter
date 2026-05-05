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

#include <memory>

#include <ale/ast/Node.hpp>

#include <intlib/ast/SequenceIndices.hpp>
#include <intlib/ast/Evaluation.hpp>

namespace intlib {
namespace ast {

class SequenceExecutionEnvironment {
public:

	/* MODIFIERS */

	void set_expression(const std::unique_ptr<ale::ast::Node> * const expr
	) noexcept
	{
		m_expression = expr;
	}

	[[nodiscard]] Evaluation make_distances();

	void
	set_working_distance(const uint64_t depth, const int64_t distance) noexcept
	{
#if defined DEBUG
		assert(depth < m_working_distances.size());
#endif
		m_working_distances[depth] = distance;
	}

	void add_node_type(const ale::ast::node_type_e t)
	{
		m_node_types_sequence.push_back(t);
	}

	/* GETTERS */

	[[nodiscard]] int64_t get_working_distance(const uint64_t depth
	) const noexcept
	{
#if defined DEBUG
		assert(depth < m_working_distances.size());
#endif
		return m_working_distances[depth];
	}

	[[nodiscard]] const std::unique_ptr<ale::ast::Node>&
	get_expression() const noexcept
	{
		return *m_expression;
	}

	/// The depth corresponds to the number of indices to be enumerated.
	[[nodiscard]] size_t get_depth() const noexcept
	{
#if defined DEBUG
		assert(m_distances.size() == m_first_indices.get_depth());
		assert(m_distances.size() == m_last_indices.get_depth());
#endif
		return m_distances.size();
	}

	[[nodiscard]] decltype(auto) get_first_indices(this auto& self) noexcept
	{
		return (self.m_first_indices);
	}
	[[nodiscard]] decltype(auto) get_last_indices(this auto& self) noexcept
	{
		return (self.m_last_indices);
	}

	[[nodiscard]] int64_t get_distance(const size_t depth) const noexcept
	{
#if defined DEBUG
		assert(depth < m_distances.size());
#endif
		return m_distances[depth];
	}

	[[nodiscard]] ale::ast::node_type_e get_node_type(const size_t depth
	) const noexcept
	{
#if defined DEBUG
		assert(depth < m_node_types_sequence.size());
#endif
		return m_node_types_sequence[depth];
	}

private:

	const std::unique_ptr<ale::ast::Node> *m_expression = nullptr;
	SequenceIndices m_first_indices;
	SequenceIndices m_last_indices;

	std::vector<int64_t> m_distances;
	std::vector<int64_t> m_working_distances;
	std::vector<ale::ast::node_type_e> m_node_types_sequence;
};

} // namespace ast

namespace detail {

template <>
constexpr inline std::string_view
	type_string_cpp<ast::SequenceExecutionEnvironment> =
		"SequenceExecutionEnvironment";

} // namespace detail
} // namespace intlib
