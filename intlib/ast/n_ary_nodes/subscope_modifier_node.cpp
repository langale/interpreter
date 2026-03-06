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

#include <optional>
#include <any>

#include <intlib/detail/any_type.hpp>
#include <intlib/detail/any_output.hpp>

#include <intlib/Program.hpp>

namespace intlib {

std::optional<std::any>
Program::evaluate(const ale::ast::SubscopeModifierNode& v)
{
	// using ale::detail::operator<<;

	m_memory.get_current_scope().push_subscope();
	for (const auto& w : v.get_children()) {
		const std::optional<std::any> r = interpret_node(w);
		if (not r.has_value()) {
			// ale::error() << ERROR_LOCATION << '\n';
			// ale::error() << "    Evaluation of node failed.\n";
			return {};
		}
		const std::any& value = *r;
		if (value.has_value()) {
			// ale::output() << value << '\n';
		}
	}
	m_memory.get_current_scope().pop_subscope();
	return std::any{};
}

} // namespace intlib
