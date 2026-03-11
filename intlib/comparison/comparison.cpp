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

#include <intlib/logger/macros.hpp>
#include <intlib/comparison/comparison.hpp>

namespace intlib {
namespace detail {

std::optional<bool> any_comparison(
	const ale::ast::node_type_e t, const std::any& a, const std::any& b
)
{
	switch (t) {
	case ale::ast::node_type_e::Comparison_Equal:
		return any_comparison_equal(a, b);
	case ale::ast::node_type_e::Comparison_Not_Equal:
		return any_comparison_not_equal(a, b);
	case ale::ast::node_type_e::Comparison_Less_Than:
		return any_comparison_less_than(a, b);
	case ale::ast::node_type_e::Comparison_Less_Equal:
		return any_comparison_less_than_equal_to(a, b);
	case ale::ast::node_type_e::Comparison_Greater_Than:
		return any_comparison_greater_than(a, b);
	case ale::ast::node_type_e::Comparison_Greater_Equal:
		return any_comparison_greater_than_equal_to(a, b);
	default:
		INTERPRETER_PRINT_LOC2(
			ale::logger::println, "Wrong node type '{}' for comparison.", t
		);
		return {};
	}
}

} // namespace detail
} // namespace intlib
