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

#if defined DEBUG
#include <cassert>
#endif

#include <ale/ast/n_ary_nodes/CommaSeparatedGroupNode.hpp>
#include <ale/ast/binary_nodes/AssignationNode.hpp>
#include <ale/ast/binary_nodes/SequenceNode.hpp>

#include <intlib/logger/macros.hpp>
#include <intlib/ast/EvaluationResult.hpp>
#include <intlib/ast/EvaluationContext.hpp>

namespace intlib {
namespace ast {

bool retrieve_variable_names_in_assignation(
	const ale::ast::SequenceNode& seq,
	EvaluationContext& ctx,
	std::vector<std::string>& names
)
{
	INTERPRETER_ENTER_FUNCTION(ale::logger::println);

	/*
	ale::utils::SequenceNodeIterator iter_seq = make_iterator(seq);

	while (not iter_seq.end()) {
		const std::vector<int64_t>& cur_indices =
			iter_seq.get_current_indices();

		std::string var_name = seq.make_variable_name(cur_indices);
		if (not ctx.memory.variable_exists(var_name)) {
			// ale::error() << ERROR_LOCATION << '\n';
			// ale::error()
			// 	<< "    Trying to set a value to an undeclared variable '"
			// 	<< var_name << "'.\n";
			return false;
		}

		const std::optional<memory::VariableValue> var =
			ctx.memory.get_variable(var_name);
		if (var->is_constant) {
			// ale::error() << ERROR_LOCATION << '\n';
			// ale::error()
			// 	<< "    Trying to assign a value to constant variable '"
			// 	<< var_name << "'.\n";
			return false;
		}

		names.push_back(std::move(var_name));
		iter_seq.next_indices();
	}
	*/

	return true;
}

bool retrieve_variable_names_in_assignation(
	const ale::ast::CommaSeparatedGroupNode& group,
	EvaluationContext& ctx,
	std::vector<std::string>& names
)
{
	INTERPRETER_ENTER_FUNCTION(ale::logger::println);

	/*
	ale::ast::NAryNodeIterator iter(group);

	while (not iter.end()) {
		const std::unique_ptr<ale::ast::Node>& c = iter.next_child();
		const auto c_type = c->get_node_type();

		if (c_type == ale::ast::node_type_e::Variable) {

			std::string var_name =
				static_cast<ale::ast::VariableNode *>(c.get())
					->get_variable_name();

			if (not ctx.memory.variable_exists(var_name)) {
				// ale::error() << ERROR_LOCATION << '\n';
				// ale::error()
				// 	<< "    Trying to set a value to an undeclared variable '"
				// 	<< var_name << "'.\n";
				return false;
			}

			const std::optional<memory::VariableValue> var =
				ctx.memory.get_variable(var_name);
			if (var->is_constant) {
				// ale::error() << ERROR_LOCATION << '\n';
				// ale::error()
				// 	<< "    Trying to assign a value to constant variable '"
				// 	<< var_name << "'.\n";
				return false;
			}

			names.push_back(std::move(var_name));
		}
		else if (c_type == ale::ast::node_type_e::Sequence) {
			const ale::ast::SequenceNode& s =
				static_cast<ale::ast::SequenceNode&>(*c.get());
			const bool r = retrieve_variable_names_in_assignation(s, names);
			if (not r) {
				return false;
			}
		}
		else if (c_type == ale::ast::node_type_e::Subscripted_Variable) {
			const ale::ast::SubscriptedVariableNode& s =
				static_cast<ale::ast::SubscriptedVariableNode&>(*c.get());
			names.emplace_back(make_full_variable_name(s));
		}
		else {
			// ale::error() << ERROR_LOCATION << '\n';
			// ale::error() << "    Child of comma-separated group is not:\n";
			// ale::error() << "        - a variable\n";
			// ale::error() << "        - a variable sequence\n";
			// ale::error() << "        - a subscripted variable\n";
			// ale::error() << "    Found: '" << c_type << "'\n";
			return false;
		}
	}
	*/
	return true;
}

EvaluationResult
evaluate(const ale::ast::AssignationNode& v, EvaluationContext& ctx)
{
	INTERPRETER_ENTER_FUNCTION(ale::logger::println);

	const auto& left_child = v.get_left_child();
	const auto& right_child = v.get_right_child();

#if defined DEBUG
	assert(left_child != nullptr);
	assert(right_child != nullptr);
#endif

	/*
	std::optional<std::any> value = interpret_node(right_child);
	if (not value.has_value()) {
		// ale::error() << ERROR_LOCATION << '\n';
		// ale::error() << "    Evaluation of node failed.\n";
		return {};
	}

	if (left_child->get_node_type() == ale::ast::node_type_e::Variable) {
		std::string var_name =
			static_cast<ale::ast::VariableNode *>(left_child.get())
				->get_variable_name();

		if (not ctx.memory.variable_exists(var_name)) {
			// ale::error() << ERROR_LOCATION << '\n';
			// ale::error()
			// 	<< "    Trying to set a value to an undeclared variable '"
			// 	<< var_name << "'.\n";
			return {};
		}

		const std::optional<memory::VariableValue> var =
			ctx.memory.get_variable(var_name);
		if (var->is_constant) {
			// ale::error() << ERROR_LOCATION << '\n';
			// ale::error()
			// 	<< "    Trying to assign a value to constant variable '"
			// 	<< var_name << "'.\n";
			return {};
		}

		std::any copy = *value;
		ctx.memory.get_current_scope().set_variable_value(
			var_name, std::move(copy)
		);
		return value;
	}

	if (left_child->get_node_type() ==
		ale::ast::node_type_e::Comma_Separated_Group) {
		std::vector<std::string> variable_names;
		const bool r = retrieve_variable_names_in_assignation(
			static_cast<const ale::ast::CommaSeparatedGroupNode&>(
				*left_child.get()
			),
			variable_names
		);

		if (not r) {
			return {};
		}

		for (std::string& var_name : variable_names) {
			std::any copy = *value;
			ctx.memory.get_current_scope().set_variable_value(
				std::move(var_name), std::move(copy)
			);
		}

		return value;
	}

	if (left_child->get_node_type() == ale::ast::node_type_e::Sequence) {
		std::vector<std::string> variable_names;
		const bool r = retrieve_variable_names_in_assignation(
			static_cast<const ale::ast::SequenceNode&>(*left_child.get()),
			variable_names
		);

		if (not r) {
			return {};
		}

		for (std::string& var_name : variable_names) {
			std::any copy = *value;
			ctx.memory.get_current_scope().set_variable_value(
				std::move(var_name), std::move(copy)
			);
		}

		return value;
	}

	if (left_child->get_node_type() ==
		ale::ast::node_type_e::Subscripted_Variable) {
		std::string var_name = make_full_variable_name(
			static_cast<const ale::ast::SubscriptedVariableNode&>(
				*left_child.get()
			)
		);

		if (not ctx.memory.variable_exists(var_name)) {
			// ale::error() << ERROR_LOCATION << '\n';
			// ale::error()
			// 	<< "    Trying to set a value to an undeclared variable '"
			// 	<< var_name << "'.\n";
			return {};
		}

		const std::optional<memory::VariableValue> var =
			ctx.memory.get_variable(var_name);
		if (var->is_constant) {
			// ale::error() << ERROR_LOCATION << '\n';
			// ale::error()
			// 	<< "    Trying to assign a value to constant variable '"
			// 	<< var_name << "'.\n";
			return {};
		}

		std::any copy = *value;
		ctx.memory.get_current_scope().set_variable_value(
			std::move(var_name), std::move(copy)
		);

		return value;
	}

	// ale::error() << ERROR_LOCATION << '\n';
	// ale::error() << "    Left handside of assignation is not:\n";
	// ale::error() << "        - a variable name\n";
	// ale::error() << "        - a comma-separated group\n";
	// ale::error() << "        - a variable sequence\n";
	// ale::error() << "        - a subscripted variable\n";
	// ale::error() << "    Found: '" << left_child->get_node_type() << "'\n";
	*/

	return {};
}

} // namespace ast
} // namespace intlib
