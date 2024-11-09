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
#if defined DEBUG
#include <cassert>
#endif

// ale includes
#include <ale/logger.hpp>

// program includes
#include <intlib/program.hpp>

namespace interpreter {

bool program::retrieve_variable_names__in_assignation
(
	const ale::ast::variable_sequence_node& seq,
	std::vector<std::string>& names
)
noexcept
{
	ale::ast::variable_sequence_node_iterator iter_seq = make_iterator(seq);

	while (not iter_seq.end()) {
		const std::vector<int64_t>& cur_indices = iter_seq.get_current_indices();

		std::string var_name = seq.make_variable_name(cur_indices);
		if (not m_memory.variable_exists(var_name)) {
			ale::error() << ERROR_LOCATION << '\n';
			ale::error()
				<< "    Trying to set a value to an undeclared variable '"
				<< var_name << "'.\n";
			return false;
		}

		const std::optional<memory::variable_value> var = m_memory.get_variable(var_name);
		if (var->is_constant) {
			ale::error() << ERROR_LOCATION << '\n';
			ale::error()
				<< "    Trying to assign a value to constant variable '"
				<< var_name << "'.\n";
			return false;
		}

		names.push_back(std::move(var_name));
		iter_seq.next_indices();
	}

	return true;
}

bool program::retrieve_variable_names__in_assignation
(
	const ale::ast::comma_separated_group_node& group,
	std::vector<std::string>& names
)
noexcept
{
	ale::ast::n_ary_node_iterator iter(group);

	while (not iter.end()) {
		const std::unique_ptr<ale::ast::node>& c = iter.next_child();
		const auto c_type = c->get_node_type();

		if (c_type == ale::ast::node_type::variable) {

			std::string var_name = static_cast<ale::ast::variable_node *>(c.get())->get_name();
			if (not m_memory.variable_exists(var_name)) {
				ale::error() << ERROR_LOCATION << '\n';
				ale::error()
					<< "    Trying to set a value to an undeclared variable '"
					<< var_name << "'.\n";
				return false;
			}

			const std::optional<memory::variable_value> var = m_memory.get_variable(var_name);
			if (var->is_constant) {
				ale::error() << ERROR_LOCATION << '\n';
				ale::error()
					<< "    Trying to assign a value to constant variable '"
					<< var_name << "'.\n";
				return false;
			}

			names.push_back(std::move(var_name));
		}
		else if (c_type == ale::ast::node_type::variable_sequence) {
			const ale::ast::variable_sequence_node& s = static_cast<ale::ast::variable_sequence_node&>(*c.get());
			const bool r = retrieve_variable_names__in_assignation(s, names);
			if (not r) {
				return false;
			}

		}
		else if (c_type == ale::ast::node_type::subscripted_variable) {
			const ale::ast::subscripted_variable_node& s = static_cast<ale::ast::subscripted_variable_node&>(*c.get());
			names.emplace_back(make_full_variable_name(s));
		}
		else {
			ale::error() << ERROR_LOCATION << '\n';
			ale::error() << "    Child of comma-separated group is not:\n";
			ale::error() << "        - a variable\n";
			ale::error() << "        - a variable sequence\n";
			ale::error() << "        - a subscripted variable\n";
			ale::error() << "    Found: '" << node_type_to_string(c_type) << "'\n";
			return false;
		}
	}
	return true;
}

std::optional<std::any> program::evaluate
(const ale::ast::assignation_node& v)
noexcept
{
	const auto& left_child = v.get_left_child();
	const auto& right_child = v.get_right_child();

#if defined DEBUG
	assert(left_child != nullptr);
	assert(right_child != nullptr);
#endif

	const std::optional<std::any> value = interpret_node(right_child);
	if (not value.has_value()) {
		ale::error() << ERROR_LOCATION << '\n';
		ale::error() << "    Evaluation of node failed.\n";
		return {};
	}

	if (left_child->get_node_type() == ale::ast::node_type::variable) {
		const std::string var_name = static_cast<ale::ast::variable_node *>(left_child.get())->get_name();

		if (not m_memory.variable_exists(var_name)) {
			ale::error() << ERROR_LOCATION << '\n';
			ale::error()
				<< "    Trying to set a value to an undeclared variable '"
				<< var_name << "'.\n";
			return {};
		}

		const std::optional<memory::variable_value> var = m_memory.get_variable(var_name);
		if (var->is_constant) {
			ale::error() << ERROR_LOCATION << '\n';
			ale::error()
				<< "    Trying to assign a value to constant variable '"
				<< var_name << "'.\n";
			return {};
		}

		std::any copy = *value;
		m_memory.get_current_scope().set_variable_value(var_name, std::move(copy));
		return value;
	}

	if (left_child->get_node_type() == ale::ast::node_type::comma_separated_group) {
		std::vector<std::string> variable_names;
		const bool r =
			retrieve_variable_names__in_assignation(
				static_cast<const ale::ast::comma_separated_group_node&>(*left_child.get()),
				variable_names
			);

		if (not r) {
			return {};
		}

		for (std::string& var_name : variable_names) {
			std::any copy = *value;
			m_memory.get_current_scope().set_variable_value(std::move(var_name), std::move(copy));
		}

		return value;
	}

	if (left_child->get_node_type() == ale::ast::node_type::variable_sequence) {
		std::vector<std::string> variable_names;
		const bool r =
			retrieve_variable_names__in_assignation(
				static_cast<const ale::ast::variable_sequence_node&>(*left_child.get()),
				variable_names
			);

		if (not r) {
			return {};
		}

		for (std::string& var_name : variable_names) {
			std::any copy = *value;
			m_memory.get_current_scope().set_variable_value(std::move(var_name), std::move(copy));
		}

		return value;
	}

	if (left_child->get_node_type() == ale::ast::node_type::subscripted_variable) {
		std::string var_name = make_full_variable_name
			(static_cast<const ale::ast::subscripted_variable_node&>(*left_child.get()));

		if (not m_memory.variable_exists(var_name)) {
			ale::error() << ERROR_LOCATION << '\n';
			ale::error()
				<< "    Trying to set a value to an undeclared variable '"
				<< var_name << "'.\n";
			return {};
		}

		const std::optional<memory::variable_value> var = m_memory.get_variable(var_name);
		if (var->is_constant) {
			ale::error() << ERROR_LOCATION << '\n';
			ale::error()
				<< "    Trying to assign a value to constant variable '"
				<< var_name << "'.\n";
			return {};
		}

		std::any copy = *value;
		m_memory.get_current_scope().set_variable_value(std::move(var_name), std::move(copy));

		return value;
	}

	ale::error() << ERROR_LOCATION << '\n';
	ale::error() << "    Left handside of assignation is not:\n";
	ale::error() << "        - a variable name\n";
	ale::error() << "        - a comma-separated group\n";
	ale::error() << "        - a variable sequence\n";
	ale::error() << "        - a subscripted variable\n";
	ale::error() << "    Found: '" << node_type_to_string(left_child->get_node_type()) << "'\n";
	return {};
}

} // -- namespace interpreter
