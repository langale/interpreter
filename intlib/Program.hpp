/*********************************************************************
 *
 * ALE interpreter library -- the base utilities for a command line utility
 * to run Programs written in ALE
 *
 *     Copyright (C) 2024 - 2026 Lluís Alemany Puig
 *
 * This file is part of the implementation of an interpreter for ALE.
 * The full code is available at:
 *
 *     https://github.com/langale/interpreter
 *
 * This Program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this Program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Contact:
 *
 *     Lluís Alemany Puig
 *     lluis.alemany.puig@gmail.com
 *     https://github.com/lluisalemanypuig
 *
 ********************************************************************/

#pragma once

#if defined DEBUG
#include <cassert>
#endif
#include <any>
#include <memory>
#include <optional>
#include <vector>

#include <ale/logger/Stream.hpp>

#include <ale/ast/utils/node_type_enum.hpp>
#include <ale/ast/n_ary_nodes/arithmetic/ArithmeticAdditionNode.hpp>
#include <ale/ast/n_ary_nodes/arithmetic/ArithmeticDivisionNode.hpp>
#include <ale/ast/n_ary_nodes/comparison/ComparisonGreaterEqualNode.hpp>
#include <ale/ast/n_ary_nodes/logical/LogicalOrNode.hpp>
#include <ale/ast/n_ary_nodes/CommaSeparatedGroupNode.hpp>
#include <ale/ast/n_ary_nodes/SubscriptedVariableNode.hpp>
#include <ale/ast/n_ary_nodes/SubscopeModifierNode.hpp>
#include <ale/ast/n_ary_nodes/ProgramNode.hpp>
#include <ale/ast/ternary_nodes/IfElseNode.hpp>
#include <ale/ast/binary_nodes/AssignationNode.hpp>
#include <ale/ast/binary_nodes/declaration_nodes/DeclarationConstNode.hpp>
#include <ale/ast/binary_nodes/declaration_nodes/DeclarationDeclareNode.hpp>
#include <ale/ast/binary_nodes/declaration_nodes/DeclarationLetNode.hpp>
#include <ale/ast/binary_nodes/SequenceNode.hpp>
#include <ale/ast/binary_nodes/WhileLoopNode.hpp>
#include <ale/ast/unary_nodes/NegationNode.hpp>
#include <ale/ast/unary_nodes/NegativeNode.hpp>
#include <ale/ast/unary_nodes/PositiveNode.hpp>
#include <ale/ast/zero_ary_nodes/FalseNode.hpp>
#include <ale/ast/zero_ary_nodes/literal_nodes/LiteralStringNode.hpp>
#include <ale/ast/zero_ary_nodes/literal_nodes/numeric_nodes/LiteralDecimalNode.hpp>
#include <ale/ast/zero_ary_nodes/literal_nodes/numeric_nodes/LiteralUnsignedIntegerNode.hpp>
#include <ale/ast/zero_ary_nodes/literal_nodes/numeric_nodes/LiteralSignedIntegerNode.hpp>
#include <ale/ast/zero_ary_nodes/TrueNode.hpp>
#include <ale/ast/zero_ary_nodes/VariableNode.hpp>

#include <ale/utils/binary_nodes/sequence_node/SequenceNodeIterator.hpp>

#include <intlib/EvaluationResult.hpp>
#include <intlib/memory/Memory.hpp>

namespace intlib {

/**
 * @brief The Program class.
 *
 * This class stores
 * - all the instructions contained in the Program
 * - the internal state of the Program (variables and their values).
 */
class Program {
public:

	Program() noexcept;
	Program(Program& p) noexcept = delete;
	Program(Program&& p) noexcept = default;
	Program& operator= (Program& p) noexcept = delete;
	Program& operator= (Program&& p) noexcept = default;
	~Program() noexcept = default;

	/* MODIFIERS */

	/**
	 * @brief Adds a list of instructions to the Program.
	 * @param node The program node.
	 * @post Ownership of @e n is taken by this class.
	*/
	void add_instructions(std::unique_ptr<ale::ast::Node>&& node) noexcept;

	/* OTHER */

	/// Execute this Program.
	void run_program(ale::logger::Stream& os);

	/// Output a Program in a tree (AST) format to an output stream.
	void print_tree(ale::logger::Stream& os) const;

private:

	/// Calls the appropriate 'evaluate' function for @e v.
	template <typename node_t, typename... params_t>
	[[nodiscard]] std::optional<std::any>
	call_evaluate(const std::unique_ptr<ale::ast::Node>& v, const params_t&...p)
	{
		return evaluate(*static_cast<const node_t * const>(v.get()), p...);
	}

	/// Interprets the contents of node @e v and, optionally, returns a result.
	[[nodiscard]] EvaluationResult
	interpret_node(const std::unique_ptr<ale::ast::Node>& v)
	{
#if defined DEBUG
		assert(v != nullptr);
#endif
		const auto t = v->get_node_type();
		switch (t) {
			/* n-ary nodes */

		case ale::ast::node_type_e::Arithmetic_Addition:
		case ale::ast::node_type_e::Arithmetic_Subtraction:
		case ale::ast::node_type_e::Arithmetic_Multiplication:
		case ale::ast::node_type_e::Arithmetic_Division:
		case ale::ast::node_type_e::Arithmetic_Exponentiation:
		case ale::ast::node_type_e::Arithmetic_Modulus:
			return call_evaluate<ale::ast::ArithmeticNode>(v, t);

		case ale::ast::node_type_e::Comparison_Equal:
		case ale::ast::node_type_e::Comparison_Not_Equal:
		case ale::ast::node_type_e::Comparison_Less_Equal:
		case ale::ast::node_type_e::Comparison_Less_Than:
		case ale::ast::node_type_e::Comparison_Greater_Equal:
		case ale::ast::node_type_e::Comparison_Greater_Than:
			return call_evaluate<ale::ast::ComparisonNode>(v, t);

		case ale::ast::node_type_e::Logical_And:
		case ale::ast::node_type_e::Logical_Or:
			return call_evaluate<ale::ast::LogicalNode>(v, t);

		case ale::ast::node_type_e::Subscope_Modifier:
			return call_evaluate<ale::ast::SubscopeModifierNode>(v);
		case ale::ast::node_type_e::Subscripted_Variable:
			return call_evaluate<ale::ast::SubscriptedVariableNode>(v);
		case ale::ast::node_type_e::Comma_Separated_Group:
			return call_evaluate<ale::ast::CommaSeparatedGroupNode>(v);

		case ale::ast::node_type_e::Program:
			return call_evaluate<ale::ast::ProgramNode>(v);

			/* ternary nodes */

		case ale::ast::node_type_e::If_Else_Statement:
			return call_evaluate<ale::ast::IfElseNode>(v);

			/* binary nodes */

		case ale::ast::node_type_e::Assignation:
			return call_evaluate<ale::ast::AssignationNode>(v);
		case ale::ast::node_type_e::Declaration_Declare:
			return call_evaluate<ale::ast::DeclarationDeclareNode>(v);
		case ale::ast::node_type_e::Declaration_Const:
			return call_evaluate<ale::ast::DeclarationConstNode>(v);
		case ale::ast::node_type_e::Declaration_Let:
			return call_evaluate<ale::ast::DeclarationLetNode>(v);
		case ale::ast::node_type_e::Sequence:
			return call_evaluate<ale::ast::SequenceNode>(v);
		case ale::ast::node_type_e::While_Loop:
			return call_evaluate<ale::ast::WhileLoopNode>(v);

			/* unary nodes */

		case ale::ast::node_type_e::Negation:
			return call_evaluate<ale::ast::NegationNode>(v);
		case ale::ast::node_type_e::Negative:
			return call_evaluate<ale::ast::NegativeNode>(v);
		case ale::ast::node_type_e::Positive:
			return call_evaluate<ale::ast::PositiveNode>(v);

			/* zero-ary nodes */

		case ale::ast::node_type_e::Literal_True:
			return call_evaluate<ale::ast::TrueNode>(v);
		case ale::ast::node_type_e::Literal_False:
			return call_evaluate<ale::ast::FalseNode>(v);
		case ale::ast::node_type_e::Variable:
			return call_evaluate<ale::ast::VariableNode>(v);
		case ale::ast::node_type_e::Literal_String:
			return call_evaluate<ale::ast::LiteralStringNode>(v);
		case ale::ast::node_type_e::Literal_Decimal:
			return call_evaluate<ale::ast::LiteralDecimalNode>(v);
		case ale::ast::node_type_e::Literal_Unsigned_Integer:
			return call_evaluate<ale::ast::LiteralUnsignedIntegerNode>(v);
		case ale::ast::node_type_e::Literal_Signed_Integer:
			return call_evaluate<ale::ast::LiteralSignedIntegerNode>(v);

			/* invalid values */

		case ale::ast::node_type_e::None:
			// do nothing
			break;
		}

		return std::unexpected{EvaluationError{
			*v,
			evaluation_error_e::Unhandled_Node_Type,
			std::format("Unhandled node type {}.", v->get_node_type())
		}};
	}

private:

	[[nodiscard]] std::optional<std::any>
	get_variable_value(const std::string& var) const;

private:

	/* n-ary nodes */

	[[nodiscard]] std::optional<std::any>
	evaluate(const ale::ast::ArithmeticNode& v, const ale::ast::node_type_e& t);

	[[nodiscard]] std::optional<bool> evaluate_variable_sequence_in_comparison(
		const ale::ast::ComparisonNode& v,
		const ale::ast::node_type_e& t,
		const std::unique_ptr<ale::ast::Node>& c
	);

	[[nodiscard]] std::optional<std::any>
	evaluate(const ale::ast::ComparisonNode& v, const ale::ast::node_type_e& t);

	[[nodiscard]] std::optional<bool> evaluate_logical_node(
		const ale::ast::LogicalNode& v,
		const ale::ast::node_type_e& t,
		const std::unique_ptr<ale::ast::Node>& c
	);

	[[nodiscard]] std::optional<std::any>
	evaluate(const ale::ast::LogicalNode& v, const ale::ast::node_type_e& t);

	[[nodiscard]] std::optional<std::any>
	evaluate(const ale::ast::CommaSeparatedGroupNode& v);

	[[nodiscard]] std::optional<std::any>
	evaluate(const ale::ast::SubscopeModifierNode& v);

	[[nodiscard]] std::vector<int64_t>
	get_index_sequence(const ale::ast::SubscriptedVariableNode& v);

	[[nodiscard]] std::string
	make_full_variable_name(const ale::ast::SubscriptedVariableNode& v);

	[[nodiscard]] std::optional<std::any>
	evaluate(const ale::ast::SubscriptedVariableNode& v);

	[[nodiscard]] std::optional<std::any>
	evaluate(const ale::ast::ProgramNode& v);

	/* ternary nodes */

	[[nodiscard]] EvaluationResult evaluate(const ale::ast::IfElseNode& v);

	/* binary nodes */

	[[nodiscard]] bool retrieve_variable_names_in_assignation(
		const ale::ast::SequenceNode& seq, std::vector<std::string>& names
	);

	[[nodiscard]] bool retrieve_variable_names_in_assignation(
		const ale::ast::CommaSeparatedGroupNode& n,
		std::vector<std::string>& names
	);

	[[nodiscard]] std::optional<std::any>
	evaluate(const ale::ast::AssignationNode& v);

	[[nodiscard]] bool retrieve_variable_names_in_declaration(
		const ale::ast::SequenceNode& seq, std::vector<std::string>& names
	);

	[[nodiscard]] bool retrieve_variable_names_in_declaration(
		const ale::ast::CommaSeparatedGroupNode& seq,
		std::vector<std::string>& names
	);

	[[nodiscard]] std::optional<std::any>
	evaluate(const ale::ast::DeclarationNode& v);

	[[nodiscard]] std::optional<std::any> first_value(
		const ale::ast::ComparisonNode& v,
		const std::unique_ptr<ale::ast::Node>& c
	);

	[[nodiscard]] std::optional<std::any> last_value(
		const ale::ast::ComparisonNode& v,
		const std::unique_ptr<ale::ast::Node>& c
	);

	[[nodiscard]] ale::utils::SequenceNodeIterator
	make_iterator(const ale::ast::SequenceNode& v);

	[[nodiscard]] std::optional<std::any>
	evaluate(const ale::ast::SequenceNode& v);

	[[nodiscard]] std::optional<std::any>
	evaluate(const ale::ast::WhileLoopNode& v);

	/* unary nodes */

	[[nodiscard]] EvaluationResult evaluate(const ale::ast::NegationNode& v);

	[[nodiscard]] EvaluationResult evaluate(const ale::ast::NegativeNode& v);

	[[nodiscard]] EvaluationResult evaluate(const ale::ast::PositiveNode& v);

	/* zero-ary nodes */

	[[nodiscard]] EvaluationResult evaluate(const ale::ast::FalseNode&);

	[[nodiscard]] EvaluationResult
	evaluate(const ale::ast::LiteralStringNode& v);

	[[nodiscard]] EvaluationResult
	evaluate(const ale::ast::LiteralDecimalNode& v);

	[[nodiscard]] EvaluationResult
	evaluate(const ale::ast::LiteralUnsignedIntegerNode& v);

	[[nodiscard]] EvaluationResult
	evaluate(const ale::ast::LiteralSignedIntegerNode& v);

	[[nodiscard]] EvaluationResult evaluate(const ale::ast::TrueNode&);

	[[nodiscard]] EvaluationResult
	evaluate(const ale::ast::VariableNode& v) const;

private:

	/// All the instructions in this Program.
	std::unique_ptr<ale::ast::Node> m_program_node;
	/// This Program's memory.
	memory::Memory m_memory;
};

} // namespace intlib
