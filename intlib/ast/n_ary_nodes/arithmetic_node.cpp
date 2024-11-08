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

#include <intlib/program.hpp>

// C++ includes
#include <optional>
#include <ranges>
#include <any>

// ale includes
#include <ale/detail/any_type.hpp>
#include <ale/detail/any_output.hpp>

// interpreter includes
#include <detail/any_arithmetic.hpp>

namespace interpreter {

std::optional<std::any> program::evaluate
(const ale::ast::arithmetic_node& v, const ale::ast::node_type& t)
noexcept
{
	const auto& m_children = v.get_children();
#if defined DEBUG
	assert(m_children.size() >= 2);
#endif

	const auto node_eval =
	[&](const std::unique_ptr<ale::ast::node>& c) -> std::optional<std::any>
	{
		const std::optional<std::any> res = interpret_node(c);
		if (not res.has_value()) {
			ale::error() << ERROR_LOCATION << '\n';
			ale::error()
				<< "    Evaluation of node failed in arithmetic node '"
				<< v.get_operation_string()
				<< "'.\n";
			return {};
		}
		if (ale::detail::is_type<void>(*res)) {
			ale::error() << ERROR_LOCATION << '\n';
			ale::error() << "    Evaluation of node returned a void value.\n";
			return {};
		}
		return res;
	};

	std::optional<std::any> r = node_eval(m_children[0]);
	if (not r.has_value()) {
		return {};
	}

	for (const std::unique_ptr<ale::ast::node>& c : m_children | std::views::drop(1)) {
		const std::optional<std::any> rv = node_eval(c);
		if (not rv.has_value()) {
			return {};
		}

		r = detail::any_arithmetic(t, *r, *rv);

		if (not r.has_value()) {
			ale::error() << ERROR_LOCATION << '\n';
			ale::error()
				<< "    Operation in arithmetic node '"
				<< v.get_operation_string()
				<< "' failed.\n";
			return {};
		}
	}
	return r;
}

} // -- interpreter
