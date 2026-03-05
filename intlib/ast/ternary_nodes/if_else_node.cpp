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

#include <intlib/detail/any_output.hpp>

#include <intlib/Program.hpp>
#include <intlib/detail/any_to_bool.hpp>

namespace intlib {

std::optional<std::any>
Program::evaluate(const ale::ast::IfElseNode& v) noexcept
{
	// using ale::detail::operator<<;

	const auto& first_child = v.get_first_child();
	const auto& second_child = v.get_second_child();
	const auto& third_child = v.get_third_child();

	if (first_child == nullptr) {
		// ale::error() << ERROR_LOCATION << '\n';
		// ale::error() << "    Condition node of if statement is null.\n";
		return {};
	}

	const std::optional<std::any> cond = interpret_node(first_child);
	if (not cond.has_value()) {
		// ale::error() << ERROR_LOCATION << '\n';
		// ale::error() << "    Evaluation of node failed.\n";
		return {};
	}

	const std::optional<bool> cond_bool = detail::any_to_bool(*cond);
	if (not cond_bool.has_value()) {
		// ale::error() << ERROR_LOCATION << '\n';
		// ale::error() << "    Condition of if statement could not be converted "
		// 				"into a Boolean value.\n";
		// ale::error() << "    Evaluation: " << *cond << '\n';
		return {};
	}

	if (*cond_bool) {
		if (second_child != nullptr) {
			return interpret_node(second_child);
		}
	}
	else if (third_child != nullptr) {
		return interpret_node(third_child);
	}

	return std::any{};
}

} // namespace intlib
