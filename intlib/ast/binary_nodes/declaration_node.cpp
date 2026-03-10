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

#include <intlib/logger/macros.hpp>
#include <intlib/detail/any_output.hpp>
#include <intlib/detail/any_type.hpp>
#include <intlib/detail/any_conversion.hpp>
#include <intlib/Program.hpp>

namespace intlib {

bool Program::retrieve_variable_names_in_declaration(
	const ale::ast::SequenceNode& seq, std::vector<std::string>& names
)
{
	INTERPRETER_ENTER_FUNCTION(ale::logger::println);

	/*
	ale::utils::SequenceNodeIterator iter_seq = make_iterator(seq);

	while (not iter_seq.end()) {
		const std::vector<int64_t>& cur_indices =
			iter_seq.get_current_indices();

		std::string var_name = seq.make_variable_name(cur_indices);
		if (m_memory.variable_exists_shallow(var_name)) {
			// ale::error() << ERROR_LOCATION << '\n';
			// ale::error() << "    Redeclaration of variable '" << var_name << "'.\n";
			return false;
		}
		names.push_back(std::move(var_name));
		iter_seq.next_indices();
	}
	*/

	return true;
}

bool Program::retrieve_variable_names_in_declaration(
	const ale::ast::CommaSeparatedGroupNode& group,
	std::vector<std::string>& names
)
{
	INTERPRETER_ENTER_FUNCTION(ale::logger::println);

	/*
	ale::ast::NAryNodeIterator iter(group);

	while (not iter.end()) {
		const std::unique_ptr<ale::ast::Node>& c = iter.next_child();
		if (c->get_node_type() == ale::ast::node_type_e::Variable) {

			std::string var_name =
				static_cast<ale::ast::VariableNode *>(c.get())
					->get_variable_name();
			if (m_memory.variable_exists_shallow(var_name)) {
				// ale::error() << ERROR_LOCATION << '\n';
				// ale::error() << "    Redeclaration of variable '" << var_name << "'.\n";
				return false;
			}
			names.push_back(std::move(var_name));
		}
		else if (c->get_node_type() == ale::ast::node_type_e::Sequence) {

			const bool r = retrieve_variable_names_in_declaration(
				static_cast<const ale::ast::SequenceNode&>(*c.get()), names
			);

			if (not r) {
				return false;
			}
		}
		else {
			// ale::error() << ERROR_LOCATION << '\n';
			// ale::error() << "    Child of comma-separated group is neither a "
			// 				"variable or a variable sequence.\n";
			return false;
		}
	}
	*/
	return true;
}

EvaluationResult Program::evaluate(const ale::ast::DeclarationNode& v)
{
	INTERPRETER_ENTER_FUNCTION(ale::logger::println);

	const auto& left_child = v.get_left_child();
	const auto& right_child = v.get_right_child();

#if defined DEBUG
	assert(left_child != nullptr);
#endif

	if (left_child->get_node_type() == ale::ast::node_type_e::Variable) {

#if defined DEBUG
		assert(
			dynamic_cast<const ale::ast::VariableNode * const>(
				left_child.get()
			) != nullptr
		);
#endif

		std::string var_name =
			static_cast<const ale::ast::VariableNode * const>(left_child.get())
				->get_variable_name();

		if (m_memory.variable_exists_shallow(var_name)) {
			INTERPRETER_PRINT_LOC2(
				ale::logger::println, "Redeclaration of variable {}.", var_name
			);
			return EvaluationError{
				.error = {evaluation_error_e::Memory_Variable_Already_Exists},
				.message = {
					std::format("Redeclaration of variable {}.", var_name)
				}
			};
		}

		std::string var_type = v.get_type();

		// This is a 'declare' node.
		if (right_child == nullptr) {
			[[maybe_unused]] const auto res = m_memory.declare_variable(
				std::move(var_name), {}, std::move(var_type)
			);

#if defined DEBUG
			// None of the following errors can happen:
			// - Variable_Does_Not_Exist, Type_Mismatch, Attempt_To_Assign_Value_To_Constant_Variable:
			//   This cannot be returned when declaring a variable.
			// - Variable_Exists:
			//   This has been ensured will never happen
			assert(res.has_value());
#endif
			return {};
		}

		EvaluationResult res_w = interpret_node(right_child);
		if (not res_w.has_value()) {
			INTERPRETER_PRINT_LOC(
				ale::logger::println, "Evaluation of node failed."
			);
			return EvaluationError{
				.error = {evaluation_error_e::Evaluation_Of_Node_Failed},
				.message = {"Evaluation of node failed."}
			};
		}

		const std::any& value = *res_w;

		INTERPRETER_PRINT_LOC2(
			ale::logger::println,
			"Type returned from node evaluation is: '{}'. Value is: '{}'.",
			detail::get_type_name(value),
			any_view{value}
		);

		std::any value_conv = detail::any_convert_to_type(value, var_type);

		INTERPRETER_PRINT_LOC2(
			ale::logger::println,
			"Value after conversion to '{}' is: '{}'.",
			var_type,
			any_view{value_conv}
		);

		memory::AccessResult res;
		if (v.is_constant()) {
			res = m_memory.declare_constant_variable(
				std::move(var_name), std::move(value_conv), std::move(var_type)
			);
		}
		else {
			res = m_memory.declare_variable(
				std::move(var_name), std::move(value_conv), std::move(var_type)
			);
		}

#if defined DEBUG
		// None of the following errors can happen:
		// - Variable_Does_Not_Exist, Type_Mismatch, Attempt_To_Assign_Value_To_Constant_Variable:
		//   This cannot be returned when declaring a variable.
		// - Variable_Exists:
		//   This has been ensured will never happen
		assert(res.has_value());
#endif

		return {};
	}

	/*
	if (left_child->get_node_type() ==
		ale::ast::node_type_e::Comma_Separated_Group) {
		std::vector<std::string> variable_names;
		const bool r = retrieve_variable_names_in_declaration(
			static_cast<ale::ast::CommaSeparatedGroupNode&>(*left_child.get()),
			variable_names
		);

		if (not r) {
			return {};
		}

		// This is a 'declare' node.
		if (right_child == nullptr) {
			for (std::string& var_name : variable_names) {
				m_memory.declare_variable(std::move(var_name), {});
			}
			return std::any{};
		}

		std::optional<std::any> value = interpret_node(right_child);
		if (not value.has_value()) {
			// ale::error() << ERROR_LOCATION << '\n';
			// ale::error() << "    Evaluation of node failed.\n";
			return {};
		}

		for (std::string& var_name : variable_names) {
			std::any copy = *value;
			if (v.is_constant()) {
				m_memory.declare_constant_variable(
					std::move(var_name), std::move(copy)
				);
			}
			else {
				m_memory.declare_variable(std::move(var_name), std::move(copy));
			}
		}

		return value;
	}

	if (left_child->get_node_type() == ale::ast::node_type_e::Sequence) {
		std::vector<std::string> variable_names;
		const bool r = retrieve_variable_names_in_declaration(
			static_cast<const ale::ast::SequenceNode&>(*left_child.get()),
			variable_names
		);

		if (not r) {
			return {};
		}

		// This is a 'declare' node.
		if (right_child == nullptr) {
			for (std::string& var_name : variable_names) {
				m_memory.declare_variable(std::move(var_name), {});
			}
			return std::any{};
		}

		std::optional<std::any> value = interpret_node(right_child);
		if (not value.has_value()) {
			// ale::error() << ERROR_LOCATION << '\n';
			// ale::error() << "    Evaluation of node failed.\n";
			return {};
		}

		for (std::string& var_name : variable_names) {
			std::any copy = *value;
			if (v.is_constant()) {
				m_memory.declare_constant_variable(
					std::move(var_name), std::move(copy)
				);
			}
			else {
				m_memory.declare_variable(std::move(var_name), std::move(copy));
			}
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

		// This is a 'declare' node.
		if (right_child == nullptr) {
			m_memory.declare_variable(std::move(var_name), {});
			return std::any{};
		}

		std::optional<std::any> value = interpret_node(right_child);
		if (not value.has_value()) {
			// ale::error() << ERROR_LOCATION << '\n';
			// ale::error() << "    Evaluation of node failed.\n";
			return {};
		}

		std::any copy = *value;
		if (v.is_constant()) {
			m_memory.declare_constant_variable(
				std::move(var_name), std::move(copy)
			);
		}
		else {
			m_memory.declare_variable(std::move(var_name), std::move(copy));
		}

		return value;
	}

	// ale::error() << ERROR_LOCATION << '\n';
	// ale::error() << "    Left handside of declaration is not:\n";
	// ale::error() << "        - a variable name\n";
	// ale::error() << "        - a comma-separated group\n";
	// ale::error() << "        - a variable sequence\n";
	// ale::error() << "    Found: '"
	// 			 << node_type_to_string(left_child->get_node_type()) << "'\n";
	*/
	return {};
}

} // namespace intlib
