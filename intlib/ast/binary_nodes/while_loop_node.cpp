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

#include <optional>
#include <any>

#include <intlib/detail/any_type.hpp>
#include <intlib/detail/any_output.hpp>

#include <intlib/Program.hpp>
#include <intlib/detail/any_to_bool.hpp>

namespace intlib {

std::optional<std::any> Program::evaluate
(const ale::ast::WhileLoopNode& v)
noexcept
{
	const auto& left_child = v.get_left_child();
	const auto& right_child = v.get_right_child();

	// using ale::detail::operator<<;

	if (left_child == nullptr) {
		// ale::error() << ERROR_LOCATION << '\n';
		// ale::error() << "    Condition node of while loop is null.\n";
		return {};
	}

	bool stop = false;
	while (not stop) {

		const std::optional<std::any> cond = interpret_node(left_child);
		if (not cond.has_value()) {
			// ale::error() << ERROR_LOCATION << '\n';
			// ale::error() << "    Evaluation of while loop condition failed.\n";
			return {};
		}

		const std::optional<bool> cond_bool = detail::any_to_bool(*cond);
		if (not cond_bool.has_value()) {
			// ale::error() << ERROR_LOCATION << '\n';
			// ale::error() << "    Condition of while loop could not be converted into a Boolean value.\n";
			// ale::error() << "    Evaluation: " << *cond << '\n';
			return {};
		}

		stop = not *cond_bool;
		if (*cond_bool) {
			// yes, this may produce infinite loops
			if (right_child == nullptr) { continue; }

			const std::optional<std::any> r = interpret_node(right_child);
			if (not r.has_value()) {
				// ale::error() << ERROR_LOCATION << '\n';
				// ale::error() << "    Evaluation of while loop body.\n";
				return {};
			}
		}
	}

	return std::any{};
}

} // -- interpreter
