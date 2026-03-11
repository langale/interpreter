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

#pragma once

#include <string>
#include <any>

#include <ale/ast/utils/node_type_enum.hpp>
#include <ale/ast/n_ary_nodes/ArithmeticNode.hpp>
#include <ale/ast/n_ary_nodes/comparison/ComparisonGreaterEqualNode.hpp>
#include <ale/ast/n_ary_nodes/logical/LogicalOrNode.hpp>
#include <ale/ast/n_ary_nodes/CommaSeparatedGroupNode.hpp>
#include <ale/ast/n_ary_nodes/SubscriptedVariableNode.hpp>
#include <ale/ast/n_ary_nodes/SubscopeModifierNode.hpp>
#include <ale/ast/n_ary_nodes/ProgramNode.hpp>
#include <ale/ast/ternary_nodes/IfElseNode.hpp>
#include <ale/ast/binary_nodes/AssignationNode.hpp>
#include <ale/ast/binary_nodes/DeclarationNode.hpp>
#include <ale/ast/binary_nodes/SequenceNode.hpp>
#include <ale/ast/binary_nodes/WhileLoopNode.hpp>
#include <ale/ast/unary_nodes/NegationNode.hpp>
#include <ale/ast/unary_nodes/NegativeNode.hpp>
#include <ale/ast/unary_nodes/PositiveNode.hpp>
#include <ale/ast/zero_ary_nodes/TrueNode.hpp>
#include <ale/ast/zero_ary_nodes/FalseNode.hpp>
#include <ale/ast/zero_ary_nodes/literal_nodes/LiteralStringNode.hpp>
#include <ale/ast/zero_ary_nodes/literal_nodes/numeric_nodes/LiteralDecimalNode.hpp>
#include <ale/ast/zero_ary_nodes/literal_nodes/numeric_nodes/LiteralUnsignedIntegerNode.hpp>
#include <ale/ast/zero_ary_nodes/literal_nodes/numeric_nodes/LiteralSignedIntegerNode.hpp>
#include <ale/ast/zero_ary_nodes/VariableNode.hpp>
#include <ale/utils/binary_nodes/sequence_node/SequenceNodeIterator.hpp>

#include <intlib/ast/EvaluationResult.hpp>
#include <intlib/ast/EvaluationContext.hpp>

namespace intlib {
namespace ast {

[[nodiscard]] std::optional<std::any>
get_variable_value(const std::string& var, EvaluationContext& ctx);

/* n-ary nodes */

[[nodiscard]] EvaluationResult evaluate(
	const ale::ast::ArithmeticNode& v,
	EvaluationContext& ctx,
	const ale::ast::node_type_e t
);

[[nodiscard]] EvaluationResult evaluate(
	const ale::ast::ComparisonNode& v,
	EvaluationContext& ctx,
	const ale::ast::node_type_e t
);

[[nodiscard]] EvaluationResult evaluate_logical_node(
	const ale::ast::LogicalNode& v,
	EvaluationContext& ctx,
	const ale::ast::node_type_e t,
	const std::unique_ptr<ale::ast::Node>& c
);

[[nodiscard]] EvaluationResult evaluate(
	const ale::ast::LogicalNode& v,
	EvaluationContext& ctx,
	const ale::ast::node_type_e t
);

[[nodiscard]] EvaluationResult
evaluate(const ale::ast::CommaSeparatedGroupNode& v, EvaluationContext& ctx);

[[nodiscard]] EvaluationResult
evaluate(const ale::ast::SubscopeModifierNode& v, EvaluationContext& ctx);

[[nodiscard]] std::optional<std::vector<int64_t>> get_index_sequence(
	const ale::ast::SubscriptedVariableNode& v, EvaluationContext& ctx
);

[[nodiscard]] std::optional<std::string> make_full_variable_name(
	const ale::ast::SubscriptedVariableNode& v, EvaluationContext& ctx
);

[[nodiscard]] EvaluationResult
evaluate(const ale::ast::SubscriptedVariableNode& v, EvaluationContext& ctx);

[[nodiscard]] EvaluationResult
evaluate(const ale::ast::ProgramNode& v, EvaluationContext& ctx);

/* ternary nodes */

[[nodiscard]] EvaluationResult
evaluate(const ale::ast::IfElseNode& v, EvaluationContext& ctx);

/* binary nodes */

[[nodiscard]] bool retrieve_variable_names_in_assignation(
	const ale::ast::SequenceNode& seq,
	EvaluationContext& ctx,
	std::vector<std::string>& names
);

[[nodiscard]] bool retrieve_variable_names_in_assignation(
	const ale::ast::CommaSeparatedGroupNode& n,
	EvaluationContext& ctx,
	std::vector<std::string>& names
);

[[nodiscard]] EvaluationResult
evaluate(const ale::ast::AssignationNode& v, EvaluationContext& ctx);

[[nodiscard]] bool retrieve_variable_names_in_declaration(
	const ale::ast::SequenceNode& seq,
	EvaluationContext& ctx,
	std::vector<std::string>& names
);

[[nodiscard]] bool retrieve_variable_names_in_declaration(
	const ale::ast::CommaSeparatedGroupNode& seq,
	EvaluationContext& ctx,
	std::vector<std::string>& names
);

[[nodiscard]] EvaluationResult
evaluate(const ale::ast::DeclarationNode& decl, EvaluationContext& ctx);

[[nodiscard]] EvaluationResult first_value(
	const ale::ast::ComparisonNode& v,
	EvaluationContext& ctx,
	const std::unique_ptr<ale::ast::Node>& c
);

[[nodiscard]] EvaluationResult last_value(
	const ale::ast::ComparisonNode& v,
	EvaluationContext& ctx,
	const std::unique_ptr<ale::ast::Node>& c
);

[[nodiscard]] ale::utils::SequenceNodeIterator
make_iterator(const ale::ast::SequenceNode& v, EvaluationContext& ctx);

[[nodiscard]] EvaluationResult
evaluate(const ale::ast::SequenceNode& v, EvaluationContext& ctx);

[[nodiscard]] EvaluationResult
evaluate(const ale::ast::WhileLoopNode& v, EvaluationContext& ctx);

/* unary nodes */

[[nodiscard]] EvaluationResult
evaluate(const ale::ast::NegationNode& v, EvaluationContext& ctx);

[[nodiscard]] EvaluationResult
evaluate(const ale::ast::NegativeNode& v, EvaluationContext& ctx);

[[nodiscard]] EvaluationResult
evaluate(const ale::ast::PositiveNode& v, EvaluationContext& ctx);

/* zero-ary nodes */

[[nodiscard]] EvaluationResult
evaluate(const ale::ast::TrueNode& v, const EvaluationContext& ctx);

[[nodiscard]] EvaluationResult
evaluate(const ale::ast::FalseNode& v, const EvaluationContext& ctx);

[[nodiscard]] EvaluationResult
evaluate(const ale::ast::LiteralStringNode& v, const EvaluationContext& ctx);

[[nodiscard]] EvaluationResult
evaluate(const ale::ast::LiteralDecimalNode& v, const EvaluationContext& ctx);

[[nodiscard]] EvaluationResult
evaluate(const ale::ast::LiteralUnsignedIntegerNode& v, const EvaluationContext& ctx);

[[nodiscard]] EvaluationResult
evaluate(const ale::ast::LiteralSignedIntegerNode& v, const EvaluationContext& ctx);

[[nodiscard]] EvaluationResult
evaluate(const ale::ast::VariableNode& v, const EvaluationContext& ctx);

} // namespace ast
} // namespace intlib
