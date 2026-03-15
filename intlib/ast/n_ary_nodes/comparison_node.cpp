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
#include <optional>
#include <ranges>
#include <any>

#include <ale/ast/utils/node_type_enum.hpp>
#include <ale/utils/binary_nodes/sequence_node/SequenceNodeIterator.hpp>
#include <ale/ast/n_ary_nodes/ComparisonNode.hpp>
#include <ale/ast/binary_nodes/SequenceNode.hpp>

#include <intlib/logger/macros.hpp>
#include <intlib/detail/any_type.hpp>
#include <intlib/detail/any_output.hpp>
#include <intlib/ast/EvaluationContext.hpp>
#include <intlib/ast/EvaluationResult.hpp>
#include <intlib/ast/evaluation.hpp>
#include <intlib/ast/interpretation.hpp>
#include <intlib/ast/utils/variable_names.hpp>
#include <intlib/comparison/comparison.hpp>

namespace intlib {
namespace ast {

EvaluationResult get_first_value(
	EvaluationContext& ctx, const std::unique_ptr<ale::ast::Node>& c
)
{
	INTERPRETER_ENTER_AST_FUNCTION(ale::logger::println);

	if (c->get_node_type() == ale::ast::node_type_e::Sequence) {
		const auto& vv = static_cast<const ale::ast::SequenceNode&>(*c.get());
		const ale::utils::SequenceNodeIterator iter = make_iterator(ctx, vv);
		const std::vector<int64_t>& current_idxs = iter.get_first_indices();

		std::string var = get_variable_name(vv);
		append_variable_name(var, current_idxs);

		if (not ctx.memory.variable_exists(var)) {
			INTERPRETER_PRINT_LOC2(
				ale::logger::println,
				"Attempted to access undeclared variable '{}'.",
				var
			);
			return make_bad_evaluation_result(
				std::vector{evaluation_error_e::Memory_Variable_Does_Not_Exist},
				std::vector{std::format(
					"Attempted to access undeclared variable '{}'.", var
				)}
			);
		}

		auto& variable = ctx.memory.get_variable(var);
		return make_good_evaluation_result(variable.value);
	}

	EvaluationResult value_w = interpret_node(ctx, c);
	if (not value_w.has_value()) {
		INTERPRETER_PRINT_LOC(
			ale::logger::println, "Evaluation of node failed."
		);
		return append_error(
			std::move(value_w.error()),
			evaluation_error_e::Evaluation_Of_Node_Failed,
			"Evaluation of node did not produce any value."
		);
	}

	return make_good_evaluation_result(std::move(*value_w));
}

EvaluationResult
get_last_value(EvaluationContext& ctx, const std::unique_ptr<ale::ast::Node>& c)
{
	INTERPRETER_ENTER_AST_FUNCTION(ale::logger::println);

	if (c->get_node_type() == ale::ast::node_type_e::Sequence) {
		const auto& vv = static_cast<const ale::ast::SequenceNode&>(*c.get());
		const ale::utils::SequenceNodeIterator iter = make_iterator(ctx, vv);
		const std::vector<int64_t>& current_idxs = iter.get_last_indices();

		std::string var = get_variable_name(vv);
		append_variable_name(var, current_idxs);

		if (not ctx.memory.variable_exists(var)) {
			INTERPRETER_PRINT_LOC2(
				ale::logger::println,
				"Attempted to access undeclared variable '{}'.",
				var
			);
			return make_bad_evaluation_result(
				std::vector{evaluation_error_e::Memory_Variable_Does_Not_Exist},
				std::vector{std::format(
					"Attempted to access undeclared variable '{}'.", var
				)}
			);
		}

		auto& variable = ctx.memory.get_variable(var);
		return make_good_evaluation_result(variable.value);
	}

	EvaluationResult value_w = interpret_node(ctx, c);
	if (not value_w.has_value()) {
		INTERPRETER_PRINT_LOC(
			ale::logger::println, "Evaluation of node failed."
		);
		return append_error(
			std::move(value_w.error()),
			evaluation_error_e::Evaluation_Of_Node_Failed,
			"Evaluation of node did not produce any value."
		);
	}

	return make_good_evaluation_result(std::move(*value_w));
}

EvaluationResult evaluate(
	EvaluationContext& ctx,
	const ale::ast::ComparisonNode& v,
	const ale::ast::node_type_e t
)
{
	INTERPRETER_ENTER_AST_FUNCTION(ale::logger::println);

	const auto& children = v.get_children();

#if defined DEBUG
	assert(children.size() > 0);
#endif

	if (children[0]->get_node_type() == ale::ast::node_type_e::Sequence) {
		EvaluationResult res_w = interpret_node(ctx, children[0]);
		if (not res_w.has_value()) {
			return make_bad_evaluation_result(std::move(res_w).error());
		}

		const std::any& res = *res_w;
#if defined DEBUG
		assert(detail::is_type<bool>(res));
#endif
		const bool res_bool = std::any_cast<bool>(res);
		if (not res_bool) {
			return false;
		}
	}

	std::any previous;
	{
		EvaluationResult last_w = get_last_value(ctx, children[0]);
		if (not last_w.has_value()) {
			return make_bad_evaluation_result(std::move(last_w.error()));
		}
		previous = std::move(*last_w);
	}

	for (const std::unique_ptr<ale::ast::Node>& c :
		 children | std::views::drop(1)) {

		EvaluationResult current_w = get_first_value(ctx, c);
		if (not current_w.has_value()) {
			return make_bad_evaluation_result(std::move(current_w.error()));
		}
		std::any current = std::move(*current_w);

		const std::optional<bool> comparison_result =
			detail::any_comparison(t, previous, current);

		if (not comparison_result.has_value()) {
			INTERPRETER_PRINT_LOC2(
				ale::logger::println,
				"Could not compare two std::any values: '{}' and '{}'.",
				any_view{previous},
				any_view{current}
			);
			return make_bad_evaluation_result(
				std::vector{evaluation_error_e::Comparison_Operation_Failed},
				std::vector{std::format(
					"Could not compare two std::any values: '{}' and '{}'.",
					any_view{previous},
					any_view{current}
				)}
			);
		}

		if (not *comparison_result) {
			return false;
		}

		if (c->get_node_type() == ale::ast::node_type_e::Sequence) {
			EvaluationResult res_w = interpret_node(ctx, children[0]);
			if (not res_w.has_value()) {
				return make_bad_evaluation_result(std::move(res_w).error());
			}

			const std::any& res = *res_w;
#if defined DEBUG
			assert(detail::is_type<bool>(res));
#endif
			const bool res_bool = std::any_cast<bool>(res);
			if (not res_bool) {
				return false;
			}

			previous = get_last_value(ctx, c);
		}
		else {
			previous = std::move(current);
		}
	}

	return true;
}

} // namespace ast
} // namespace intlib
