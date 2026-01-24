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
#if defined DEBUG
#include <cassert>
#endif
#include <optional>
#include <ranges>
#include <any>

// ale includes
#include <ale/logger.hpp>

#include <ale/detail/macros.hpp>
#include <ale/detail/any_type.hpp>
#include <ale/detail/any_output.hpp>

#include <ale/ast/binary_nodes/variable_sequence_node.hpp>

// interpreter includes
#include <intlib/detail/any_comparison.hpp>

namespace interpreter {

using ale::detail::operator<<;

std::optional<std::any> program::first_value
(const ale::ast::comparison_node& v, const std::unique_ptr<ale::ast::node>& c)
noexcept
{
	if (c->get_node_type() == ale::ast::node_type::variable_sequence) {
		const ale::ast::variable_sequence_node& vv =
			static_cast<const ale::ast::variable_sequence_node&>(*c.get());

		const ale::ast::variable_sequence_node_iterator iter = make_iterator(vv);
		const std::vector<int64_t>& current_idx = iter.get_first_indices();
		const std::string var = vv.make_variable_name(current_idx);

		std::optional<std::any> value = get_variable_value(var);
		if (not value.has_value()) {
			return {};
		}

		return value;
	}

	std::optional<std::any> value = interpret_node(c);
	if (not value.has_value()) {
		ale::error() << ERROR_LOCATION << '\n';
		ale::error()
			<< "    Evaluation of node failed in '"
			<< v.get_operation_string()
			<< "' comparison node.\n";
		return {};
	}
	return value;
}

std::optional<std::any> program::last_value
(const ale::ast::comparison_node& v, const std::unique_ptr<ale::ast::node>& c)
noexcept
{
	if (c->get_node_type() == ale::ast::node_type::variable_sequence) {
		const ale::ast::variable_sequence_node& vv =
			static_cast<const ale::ast::variable_sequence_node&>(*c.get());

		const ale::ast::variable_sequence_node_iterator iter = make_iterator(vv);
		const std::vector<int64_t>& current_idx = iter.get_last_indices();
		const std::string var = vv.make_variable_name(current_idx);

		std::optional<std::any> value = get_variable_value(var);
		if (not value.has_value()) {
			return {};
		}

		return value;
	}

	std::optional<std::any> value = interpret_node(c);
	if (not value.has_value()) {
		ale::error() << ERROR_LOCATION << '\n';
		ale::error()
			<< "    Evaluation of node failed in '"
			<< v.get_operation_string()
			<< "' comparison node.\n";
		return {};
	}
	return value;
}

std::optional<bool> program::evaluate_variable_sequence_in_comparison
(
	const ale::ast::comparison_node& v,
	const ale::ast::node_type& t,
	const std::unique_ptr<ale::ast::node>& c
)
noexcept
{
#if defined DEBUG
	assert(c->get_node_type() == ale::ast::node_type::variable_sequence);
#endif

	const ale::ast::variable_sequence_node& vv = static_cast<const ale::ast::variable_sequence_node&>(*c.get());
	ale::ast::variable_sequence_node_iterator iter = make_iterator(vv);

	bool eval = true;
	bool first = true;

	std::optional<std::any> previous;
	while (eval and not iter.end()) {
		const std::vector<int64_t>& current_idx = iter.get_current_indices();
		const std::string var = vv.make_variable_name(current_idx);

		std::optional<std::any> current = get_variable_value(var);
		if (not current.has_value()) {
			return {};
		}

		if (first) [[unlikely]] {
			previous = current;
			first = false;
		}
		else [[likely]] {
			const std::optional<bool> res = detail::any_comparison(t, *previous, *current);
			if (not res.has_value()) {
				ale::error() << ERROR_LOCATION << '\n';
				ale::error()
					<< "    No pair was matched for '"
					<< v.get_operation_string()
					<< "' comparison.\n";
				ale::error() << "    Left:  " << *previous << '\n';
				ale::error() << "    Right: " << *current << '\n';
				return {};
			}

			eval = *res;
			previous = std::move(current);
		}
		iter.next_indices();
	}

	return eval;
}

std::optional<std::any> program::evaluate
(const ale::ast::comparison_node& v, const ale::ast::node_type& t)
noexcept
{
	const auto& children = v.get_children();

#if defined DEBUG
	assert(children.size() > 0);
#endif

	if (children[0]->get_node_type() == ale::ast::node_type::variable_sequence) {
		const std::optional<bool> res = evaluate_variable_sequence_in_comparison(v, t, children[0]);
		if (not res.has_value()) {
			return {};
		}
#if defined DEBUG
		assert(ale::detail::is_type<bool>(*res));
#endif

		if (not *res) {
			return false;
		}
	}

	std::optional<std::any> previous = last_value(v, children[0]);

	if (not previous.has_value()) {
		ale::error() << ERROR_LOCATION << '\n';
		ale::error()
			<< "    Evaluation of node failed in '"
			<< v.get_operation_string()
			<< "' comparison node.\n";
		return {};
	}

	for (const std::unique_ptr<ale::ast::node>& c : children | std::views::drop(1)) {
		std::optional<std::any> current = first_value(v, c);
		if (not current.has_value()) {
			ale::error() << ERROR_LOCATION << '\n';
			ale::error()
				<< "    Evaluation of node failed in '"
				<< v.get_operation_string()
				<< "' comparison node.\n";
			return {};
		}

		{
		const std::optional<bool> comparison_result = detail::any_comparison(t, *previous, *current);
		if (not comparison_result.has_value()) {
			ale::error() << ERROR_LOCATION << '\n';
			ale::error()
				<< "    No pair was matched for '"
				<< v.get_operation_string()
				<< "' comparison.\n";
			ale::error() << "    Left:  " << *previous << '\n';
			ale::error() << "    Right: " << *current << '\n';
			return {};
		}
		if (not *comparison_result) {
			return false;
		}
		}

		if (c->get_node_type() == ale::ast::node_type::variable_sequence) {
			std::optional<bool> res = evaluate_variable_sequence_in_comparison(v, t, c);
			if (not res.has_value()) {
				return {};
			}
			if (not *res) {
				return false;
			}

			previous = last_value(v, c);
		}
		else {
			previous = std::move(current);
		}
	}

	return true;
}

} // -- namespace interpreter
