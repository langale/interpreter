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
#include <ranges>
#include <any>

#include <ale/ast/binary_nodes/SequenceNode.hpp>
#include <ale/utils/binary_nodes/sequence_node/SequenceNodeIterator.hpp>

#include <intlib/logger/macros.hpp>
#include <intlib/ast/EvaluationContext.hpp>
#include <intlib/ast/EvaluationResult.hpp>

namespace intlib {
namespace ast {

ale::utils::SequenceNodeIterator
make_iterator(EvaluationContext& ctx, const ale::ast::SequenceNode& seq)
{
	INTERPRETER_ENTER_AST_FUNCTION(ale::logger::println);

	/*
	const auto& left_child = seq.get_left_child();
	const auto& right_child = seq.get_right_child();
#if defined DEBUG
	assert(left_child != nullptr);
	assert(right_child != nullptr);

	assert(
		left_child->get_node_type() ==
		ale::ast::node_type_e::Subscripted_Variable
	);
	assert(
		right_child->get_node_type() ==
		ale::ast::node_type_e::Subscripted_Variable
	);
#endif

	std::vector<int64_t> first_indices = get_index_sequence(
		static_cast<const ale::ast::SubscriptedVariableNode&>(*left_child.get())
	);
	std::vector<int64_t> last_indices = get_index_sequence(
		static_cast<const ale::ast::SubscriptedVariableNode&>(
			*right_child.get()
		)
	);
	*/

	return ale::utils::SequenceNodeIterator({}, {});
}

std::optional<std::any>
get_variable_value(EvaluationContext& ctx, const std::string& var)
{
	INTERPRETER_ENTER_AST_FUNCTION(ale::logger::println);

	if (not ctx.memory.variable_exists(var)) {
		INTERPRETER_PRINT_LOC2(
			ale::logger::println, "Trying to use undeclared variable {}.", var
		);
		return std::optional<std::any>{};
	}

	memory::VariableValue& variable = ctx.memory.get_variable(var);

	if (not variable.value.has_value()) {
		INTERPRETER_PRINT_LOC2(
			ale::logger::println,
			"Variable {} exists but it does not have a value.",
			var
		);
		return std::optional<std::any>{};
	}

	return std::optional{std::move(variable.value)};
}

EvaluationResult
evaluate(EvaluationContext& ctx, const ale::ast::SequenceNode& v)
{
	INTERPRETER_ENTER_AST_FUNCTION(ale::logger::println);

	return make_good_evaluation_result();
}

} // namespace ast
} // namespace intlib
