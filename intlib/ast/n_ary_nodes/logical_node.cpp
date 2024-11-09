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

// C++ includes
#include <optional>
#include <ranges>
#include <any>

// ale includes
#include <ale/detail/any_type.hpp>
#include <ale/detail/any_output.hpp>

// program includes
#include <intlib/program.hpp>

namespace interpreter {

[[nodiscard]] bool compute_logical_expression
(const ale::ast::node_type& t, const bool l, const bool r)
noexcept
{
	if (t == ale::ast::node_type::logical_and) {
		return l and r;
	}
	if (t == ale::ast::node_type::logical_or) {
		return l or r;
	}
	ale::error() << ERROR_LOCATION << '\n';
	ale::error() << "    Wrong type of node type '" << ale::ast::node_type_to_string(t) << "'.\n";
	return false;
}

[[nodiscard]] bool break_when
(const ale::ast::node_type& t)
noexcept
{
	if (t == ale::ast::node_type::logical_and) {
		return false;
	}
	if (t == ale::ast::node_type::logical_or) {
		return true;
	}
	ale::error() << ERROR_LOCATION << '\n';
	ale::error() << "    Wrong type of node type '" << ale::ast::node_type_to_string(t) << "'.\n";
	return false;
}

std::optional<bool> program::evaluate_logical_node(
	const ale::ast::logical_node& v,
	const ale::ast::node_type& t,
	const std::unique_ptr<ale::ast::node>& c
)
noexcept
{
	if (c->get_node_type() == ale::ast::node_type::variable_sequence) {

		const bool when_to_break = break_when(t);
		const ale::ast::variable_sequence_node& seq =
			static_cast<const ale::ast::variable_sequence_node&>(*c.get());

		ale::ast::variable_sequence_node_iterator iter = make_iterator(seq);

		bool eval = not when_to_break;
		bool first = true;

		while ((eval != when_to_break) and not iter.end()) {
			const std::vector<int64_t>& current = iter.get_current_indices();
			const std::string var = seq.make_variable_name(current);

			const std::optional<std::any> opt_value = get_variable_value(var);
			if (not opt_value.has_value()) {
				return {};
			}

			const std::any& value = *opt_value;
			const bool val = std::any_cast<bool>(value);
			if (first) {
				eval = val;
				first = false;
			}
			else {
				eval = compute_logical_expression(t, eval, val);
			}

			iter.next_indices();
		}
		return {eval};
	}

	const std::optional<std::any> res = interpret_node(c);
	if (not res.has_value()) {
		ale::error() << ERROR_LOCATION << '\n';
		ale::error()
			<< "    Evaluation of node failed in logical node '"
			<< v.get_operation_string()
			<< "'.\n";
		return {};
	}
	const std::any& r = *res;
	if (not ale::detail::is_type<bool>(r)) {
		ale::error() << ERROR_LOCATION << '\n';
		ale::error() << "    Evaluation of node is not a Boolean value.\n";
		return {};
	}
	return std::any_cast<bool>(r);
}

std::optional<std::any> program::evaluate
(const ale::ast::logical_node& v, const ale::ast::node_type& t)
noexcept
{
	const auto& children = v.get_children();

	const std::optional<bool> rc = evaluate_logical_node(v, t, children[0]);
	if (not rc.has_value()) {
		return {};
	}

	const bool when_to_break = break_when(t);

	bool r = *rc;
	for (const std::unique_ptr<ale::ast::node>& c : children | std::views::drop(1)) {
		if (r == when_to_break) {
			break;
		}

		const std::optional<bool> rv = evaluate_logical_node(v, t, c);
		if (not rv.has_value()) { return {}; }

		r = compute_logical_expression(t, r, *rv);
	}
	return r;
}

} // -- interpreter
