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

#pragma once

// C++ includes
#if defined DEBUG
#include <cassert>
#endif
#include <optional>
#include <memory>
#include <vector>
#include <any>

// ale includes
#include <ale/logger.hpp>

#include <ale/ast/node_type.hpp>

/* n-ary nodes */

#include <ale/ast/n_ary_nodes/arithmetic/arithmetic_addition_node.hpp>
#include <ale/ast/n_ary_nodes/arithmetic/arithmetic_subtraction_node.hpp>
#include <ale/ast/n_ary_nodes/arithmetic/arithmetic_multiplication_node.hpp>
#include <ale/ast/n_ary_nodes/arithmetic/arithmetic_division_node.hpp>
#include <ale/ast/n_ary_nodes/arithmetic/arithmetic_exponentiation_node.hpp>
#include <ale/ast/n_ary_nodes/arithmetic/arithmetic_modulus_node.hpp>

#include <ale/ast/n_ary_nodes/comparison/comparison_equal_node.hpp>
#include <ale/ast/n_ary_nodes/comparison/comparison_greater_equal_node.hpp>
#include <ale/ast/n_ary_nodes/comparison/comparison_greater_than_node.hpp>
#include <ale/ast/n_ary_nodes/comparison/comparison_less_equal_node.hpp>
#include <ale/ast/n_ary_nodes/comparison/comparison_less_than_node.hpp>
#include <ale/ast/n_ary_nodes/comparison/comparison_not_equal_node.hpp>

#include <ale/ast/n_ary_nodes/logical/logical_and_node.hpp>
#include <ale/ast/n_ary_nodes/logical/logical_or_node.hpp>

#include <ale/ast/n_ary_nodes/comma_separated_group_node.hpp>
#include <ale/ast/n_ary_nodes/subscope_modifier_node.hpp>
#include <ale/ast/n_ary_nodes/subscripted_variable_node.hpp>

/* ternary nodes */

#include <ale/ast/ternary_nodes/if_else_node.hpp>

/* binary nodes */

#include <ale/ast/binary_nodes/assignation_node.hpp>
#include <ale/ast/binary_nodes/declaration_node.hpp>
#include <ale/ast/binary_nodes/variable_sequence_node.hpp>
#include <ale/ast/binary_nodes/while_loop_node.hpp>

/* unary nodes */

#include <ale/ast/unary_nodes/negation_node.hpp>
#include <ale/ast/unary_nodes/negative_node.hpp>
#include <ale/ast/unary_nodes/positive_node.hpp>

/* zero-ary nodes */

#include <ale/ast/zero_ary_nodes/false_node.hpp>
#include <ale/ast/zero_ary_nodes/literal_node.hpp>
#include <ale/ast/zero_ary_nodes/true_node.hpp>
#include <ale/ast/zero_ary_nodes/variable_node.hpp>

// interpreter includes
#include <intlib/memory/memory.hpp>

namespace interpreter {

/**
 * @brief The program class.
 *
 * This class stores
 * - all the instructions contained in the program
 * - the internal state of the program (variables and their values).
 */
class program {
public:
	/// Constructor.
	program() noexcept;
	/// Default destructor.
	~program() noexcept {
		clear();
	}

	/// Deleted copy constructor.
	program(program& p) noexcept = delete;
	/// Default move constructor.
	program(program&& p) noexcept = default;

	/// Deleted copy assignment operator.
	program& operator= (program& p) noexcept = delete;
	/// Default move assignment operator.
	program& operator= (program&& p) noexcept = default;

	/* MODIFIERS */

	/// Clear the memory allocated by program.
	void clear() noexcept;

	/**
	 * @brief Adds a list of instructions to the program.
	 * @param ins New instructions.
	 * @post Ownership of @e n is taken by this class.
	 */
	void add_instructions
	(std::vector<std::unique_ptr<ale::ast::node>>&& ins)
	noexcept;

	/* OTHER */

	/// Execute this program.
	void run_program
	(ale::stream& os)
	noexcept;

	/// Output a program in a tree (AST) format to an output stream.
	void print_tree
	(ale::stream& os)
	const noexcept;

private:
	/// Calls the appropriate 'evaluate' function for @e v.
	template <typename node_t, typename... Params>
	[[nodiscard]] std::optional<std::any> call_evaluate
	(const std::unique_ptr<ale::ast::node>& v, const Params&... p)
	noexcept
	{
		return evaluate(*static_cast<node_t const * const>(v.get()), p...);
	}

	/// Interprets the contents of node @e v and, optionally, returns a result.
	[[nodiscard]] std::optional<std::any> interpret_node
	(const std::unique_ptr<ale::ast::node>& v)
	noexcept
	{
#if defined DEBUG
		assert(v != nullptr);
#endif
		const auto t = v->get_node_type();
		switch (t) {
		/* n-ary nodes */

		case ale::ast::node_type::arithmetic_addition:
		case ale::ast::node_type::arithmetic_subtraction:
		case ale::ast::node_type::arithmetic_multiplication:
		case ale::ast::node_type::arithmetic_division:
		case ale::ast::node_type::arithmetic_exponentiation:
		case ale::ast::node_type::arithmetic_modulus:
			return call_evaluate<ale::ast::arithmetic_node>(v, t);

		case ale::ast::node_type::comparison_equal:
		case ale::ast::node_type::comparison_not_equal:
		case ale::ast::node_type::comparison_less_equal:
		case ale::ast::node_type::comparison_less_than:
		case ale::ast::node_type::comparison_greater_equal:
		case ale::ast::node_type::comparison_greater_than:
			return call_evaluate<ale::ast::comparison_node>(v, t);

		case ale::ast::node_type::logical_and:
		case ale::ast::node_type::logical_or:
			return call_evaluate<ale::ast::logical_node>(v, t);

		case ale::ast::node_type::subscope_modifier:
			return call_evaluate<ale::ast::subscope_modifier_node>(v);
		case ale::ast::node_type::subscripted_variable:
			return call_evaluate<ale::ast::subscripted_variable_node>(v);
		case ale::ast::node_type::comma_separated_group:
			return call_evaluate<ale::ast::comma_separated_group_node>(v);

		/* ternary nodes */

		case ale::ast::node_type::if_else_statement:
			return call_evaluate<ale::ast::if_else_node>(v);

		/* binary nodes */

		case ale::ast::node_type::assignation:
			return call_evaluate<ale::ast::assignation_node>(v);
		case ale::ast::node_type::declaration:
			return call_evaluate<ale::ast::declaration_node>(v);
		case ale::ast::node_type::variable_sequence:
			return call_evaluate<ale::ast::variable_sequence_node>(v);
		case ale::ast::node_type::while_loop:
			return call_evaluate<ale::ast::while_loop_node>(v);

		/* unary nodes */

		case ale::ast::node_type::negation:
			return call_evaluate<ale::ast::negation_node>(v);
		case ale::ast::node_type::negative:
			return call_evaluate<ale::ast::negative_node>(v);
		case ale::ast::node_type::positive:
			return call_evaluate<ale::ast::positive_node>(v);

		/* zero-ary nodes */

		case ale::ast::node_type::literal_true:
			return call_evaluate<ale::ast::true_node>(v);
		case ale::ast::node_type::literal_false:
			return call_evaluate<ale::ast::false_node>(v);
		case ale::ast::node_type::variable:
			return call_evaluate<ale::ast::variable_node>(v);
		case ale::ast::node_type::literal:
			return call_evaluate<ale::ast::literal_node>(v);

		/* invalid values */

		case ale::ast::node_type::None:
			// do nothing
			break;
		}

		ale::error()
			<< "Unhandled node type '"
			<< ale::ast::node_type_to_string(t)
			<< "'\n";
		return {};
	}

private:

	[[nodiscard]] std::optional<std::any> get_variable_value(const std::string& var) const noexcept;

private:
	/* n-ary nodes */

	[[nodiscard]] std::optional<std::any> evaluate
	(const ale::ast::arithmetic_node& v, const ale::ast::node_type& t)
	noexcept;

	[[nodiscard]] std::optional<bool> evaluate_variable_sequence_in_comparison
	(const ale::ast::comparison_node& v, const ale::ast::node_type& t, const std::unique_ptr<ale::ast::node>& c)
	noexcept;
	[[nodiscard]] std::optional<std::any> evaluate
	(const ale::ast::comparison_node& v, const ale::ast::node_type& t)
	noexcept;

	[[nodiscard]] std::optional<bool> evaluate_logical_node
	(const ale::ast::logical_node& v, const ale::ast::node_type& t, const std::unique_ptr<ale::ast::node>& c)
	noexcept;
	[[nodiscard]] std::optional<std::any> evaluate
	(const ale::ast::logical_node& v, const ale::ast::node_type& t)
	noexcept;

	[[nodiscard]] std::optional<std::any> evaluate
	(const ale::ast::comma_separated_group_node& v)
	noexcept;

	[[nodiscard]] std::optional<std::any> evaluate
	(const ale::ast::subscope_modifier_node& v)
	noexcept;

	[[nodiscard]] std::vector<int64_t> get_index_sequence
	(const ale::ast::subscripted_variable_node& v)
	noexcept;
	[[nodiscard]] std::string make_full_variable_name
	(const ale::ast::subscripted_variable_node& v)
	noexcept;
	[[nodiscard]] std::optional<std::any> evaluate
	(const ale::ast::subscripted_variable_node& v)
	noexcept;

	/* ternary nodes */

	[[nodiscard]] std::optional<std::any> evaluate
	(const ale::ast::if_else_node& v)
	noexcept;

	/* binary nodes */

	[[nodiscard]] bool retrieve_variable_names__in_assignation
	(const ale::ast::variable_sequence_node& seq, std::vector<std::string>& names)
	noexcept;
	[[nodiscard]] bool retrieve_variable_names__in_assignation
	(const ale::ast::comma_separated_group_node& n, std::vector<std::string>& names)
	noexcept;
	[[nodiscard]] std::optional<std::any> evaluate
	(const ale::ast::assignation_node& v)
	noexcept;

	[[nodiscard]] bool retrieve_variable_names__in_declaration
	(const ale::ast::variable_sequence_node& seq, std::vector<std::string>& names)
	noexcept;
	[[nodiscard]] bool retrieve_variable_names__in_declaration
	(const ale::ast::comma_separated_group_node& seq, std::vector<std::string>& names)
	noexcept;
	[[nodiscard]] std::optional<std::any> evaluate
	(const ale::ast::declaration_node& v)
	noexcept;

	[[nodiscard]] std::optional<std::any> first_value
	(const ale::ast::comparison_node& v, const std::unique_ptr<ale::ast::node>& c)
	noexcept;
	[[nodiscard]] std::optional<std::any> last_value
	(const ale::ast::comparison_node& v, const std::unique_ptr<ale::ast::node>& c)
	noexcept;
	[[nodiscard]] ale::ast::variable_sequence_node_iterator make_iterator
	(const ale::ast::variable_sequence_node& v)
	noexcept;
	[[nodiscard]] std::optional<std::any> evaluate
	(const ale::ast::variable_sequence_node& v)
	noexcept;

	[[nodiscard]] std::optional<std::any> evaluate
	(const ale::ast::while_loop_node& v)
	noexcept;

	/* unary nodes */

	[[nodiscard]] std::optional<std::any> evaluate
	(const ale::ast::negation_node& v)
	noexcept;
	[[nodiscard]] std::optional<std::any> evaluate
	(const ale::ast::negative_node& v)
	noexcept;
	[[nodiscard]] std::optional<std::any> evaluate
	(const ale::ast::positive_node& v)
	noexcept;

	/* zero-ary nodes */

	[[nodiscard]] std::optional<std::any> evaluate
	(const ale::ast::false_node&)
	noexcept;
	[[nodiscard]] std::optional<std::any> evaluate
	(const ale::ast::literal_node& v)
	noexcept;
	[[nodiscard]] std::optional<std::any> evaluate
	(const ale::ast::true_node&)
	noexcept;
	[[nodiscard]] std::optional<std::any> evaluate
	(const ale::ast::variable_node& v)
	noexcept;

private:
	/// All the instructions in this program.
	std::vector<std::unique_ptr<ale::ast::node>> m_instructions;
	/// This program's memory.
	memory::memory m_memory;
};

} // -- namespace interpreter
