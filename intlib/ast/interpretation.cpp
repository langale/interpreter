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

#include <string>

#include <ale/ast/binary_nodes/declaration_nodes/DeclarationConstNode.hpp>
#include <ale/ast/binary_nodes/declaration_nodes/DeclarationLetNode.hpp>
#include <ale/ast/binary_nodes/declaration_nodes/DeclarationDeclareNode.hpp>
#include <ale/ast/utils/node_type_to_string.hpp>

#include <intlib/ast/EvaluationResult.hpp>
#include <intlib/ast/evaluation.hpp>
#include <intlib/ast/EvaluationContext.hpp>

namespace intlib {
namespace ast {

template <typename node_t, typename... params_t>
[[nodiscard]] EvaluationResult call_evaluate(
	EvaluationContext& ctx,
	const std::unique_ptr<ale::ast::Node>& v,
	params_t&&...p
)
{
	return evaluate(
		ctx,
		*static_cast<const node_t * const>(v.get()),
		std::forward<params_t>(p)...
	);
}

[[nodiscard]] EvaluationResult
interpret_node(EvaluationContext& ctx, const std::unique_ptr<ale::ast::Node>& v)
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
		return call_evaluate<ale::ast::ArithmeticNode>(ctx, v, t);

	case ale::ast::node_type_e::Comparison_Equal:
	case ale::ast::node_type_e::Comparison_Not_Equal:
	case ale::ast::node_type_e::Comparison_Less_Equal:
	case ale::ast::node_type_e::Comparison_Less_Than:
	case ale::ast::node_type_e::Comparison_Greater_Equal:
	case ale::ast::node_type_e::Comparison_Greater_Than:
		return call_evaluate<ale::ast::ComparisonNode>(ctx, v, t);

	case ale::ast::node_type_e::Logical_And:
	case ale::ast::node_type_e::Logical_Or:
		return call_evaluate<ale::ast::LogicalNode>(ctx, v, t);

	case ale::ast::node_type_e::Subscope_Modifier:
		return call_evaluate<ale::ast::SubscopeModifierNode>(ctx, v);
	case ale::ast::node_type_e::Subscripted_Variable:
		return call_evaluate<ale::ast::SubscriptedVariableNode>(ctx, v);
	case ale::ast::node_type_e::Comma_Separated_Group:
		return call_evaluate<ale::ast::CommaSeparatedGroupNode>(ctx, v);

	case ale::ast::node_type_e::Program:
		return call_evaluate<ale::ast::ProgramNode>(ctx, v);

		/* ternary nodes */

	case ale::ast::node_type_e::If_Else_Statement:
		return call_evaluate<ale::ast::IfElseNode>(ctx, v);

		/* binary nodes */

	case ale::ast::node_type_e::Assignation:
		return call_evaluate<ale::ast::AssignationNode>(ctx, v);
	case ale::ast::node_type_e::Declaration_Declare:
		return call_evaluate<ale::ast::DeclarationDeclareNode>(ctx, v);
	case ale::ast::node_type_e::Declaration_Const:
		return call_evaluate<ale::ast::DeclarationConstNode>(ctx, v);
	case ale::ast::node_type_e::Declaration_Let:
		return call_evaluate<ale::ast::DeclarationLetNode>(ctx, v);
	case ale::ast::node_type_e::Sequence:
		return call_evaluate<ale::ast::SequenceNode>(ctx, v);
	case ale::ast::node_type_e::While_Loop:
		return call_evaluate<ale::ast::WhileLoopNode>(ctx, v);

		/* unary nodes */

	case ale::ast::node_type_e::Negation:
		return call_evaluate<ale::ast::NegationNode>(ctx, v);
	case ale::ast::node_type_e::Negative:
		return call_evaluate<ale::ast::NegativeNode>(ctx, v);
	case ale::ast::node_type_e::Positive:
		return call_evaluate<ale::ast::PositiveNode>(ctx, v);

		/* zero-ary nodes */

	case ale::ast::node_type_e::Literal_True:
		return call_evaluate<ale::ast::LiteralTrueNode>(ctx, v);
	case ale::ast::node_type_e::Literal_False:
		return call_evaluate<ale::ast::LiteralFalseNode>(ctx, v);
	case ale::ast::node_type_e::Variable:
		return call_evaluate<ale::ast::VariableNode>(ctx, v);
	case ale::ast::node_type_e::Literal_String:
		return call_evaluate<ale::ast::LiteralStringNode>(ctx, v);
	case ale::ast::node_type_e::Literal_Decimal:
		return call_evaluate<ale::ast::LiteralDecimalNode>(ctx, v);
	case ale::ast::node_type_e::Literal_Unsigned_Integer:
		return call_evaluate<ale::ast::LiteralUnsignedIntegerNode>(ctx, v);
	case ale::ast::node_type_e::Literal_Signed_Integer:
		return call_evaluate<ale::ast::LiteralSignedIntegerNode>(ctx, v);

		/* invalid values */

	case ale::ast::node_type_e::None:
		// do nothing
		break;
	}

	return make_bad_evaluation_result(
		Vec{evaluation_error_e::Unhandled_Node_Type},
		Vec{evaluation_function_e::Interpretation_Node},
		Vec{std::format("Unhandled node type {}.", v->get_node_type())}
	);
}

} // namespace ast
} // namespace intlib
