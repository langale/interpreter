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

// ale includes
#include <ale/logger.hpp>
#include <ale/detail/any_type.hpp>

// program includes
#include <intlib/program.hpp>
#include <detail/any_to_bool.hpp>

namespace interpreter {

std::optional<std::any> program::evaluate
(const ale::ast::negation_node& v)
noexcept
{
	const auto& child = v.get_child();

	std::optional<std::any> rr = interpret_node(child);
	if (not rr.has_value()) {
		ale::error() << ERROR_LOCATION << '\n';
		ale::error() << "    Node evaluation failed.\n";
		return {};
	}

	const std::optional<bool> r = detail::any_to_bool(*rr);
	if (r) {
		return not *r;
	}

	UNHANDLED_ANY(ale::error(), r);

	return {};
}

} // -- namespace interpreter
