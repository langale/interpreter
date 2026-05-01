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

#include <ale/logger/Logger.hpp>
#include <ale/ast/n_ary_nodes/SubscriptedVariableNode.hpp>

#include <intlib/logger/macros.hpp>
#include <intlib/detail/any_type.hpp>
#include <intlib/ast/EvaluationContext.hpp>
#include <intlib/ast/utils/variable_names.hpp>

namespace intlib {
namespace ast {

#define aleprln ale::logger::println

Evaluation
evaluate(EvaluationContext& ctx, const ale::ast::SubscriptedVariableNode& v)
{
	INTERPRETER_ENTER_AST_FUNCTION(aleprln);

	Evaluation res_w = make_subscripted_variable_name(ctx, v);
	if (not res_w) {
		INTERPRETER_PRINT(
			aleprln,
			"Full variable name of subscripted variable {} could not be "
			"retrieved.",
			v.get_variable_name()
		);
		return append_error(
			std::move(res_w.error()),
			evaluation_error_e::Valueless_Variable,
			evaluation_function_e::Subscripted_Variable,
			std::format(
				"Full variable name of subscripted variable {} could not be "
				"retrieved",
				v.get_variable_name()
			)
		);
	}

	const memory::WrappedAny& var_name_w = *res_w;
#if defined DEBUG
	assert(var_name_w.type == detail::type_string_cpp<std::string>);
#endif

	const auto& var_name = std::any_cast<const std::string&>(var_name_w.value);

	if (not ctx.memory.variable_exists(var_name)) {
		INTERPRETER_PRINT(
			aleprln, "Variable '{}' is not defined in this scope.", var_name
		);
		return make_bad_evaluation(
			Vec{evaluation_error_e::Undefined_Variable},
			Vec{evaluation_function_e::Subscripted_Variable},
			Vec{std::format(
				"Variable '{}' is not defined in this scope", var_name
			)}
		);
	}

	memory::VariableValue& variable = ctx.memory.get_variable(var_name);

	if (not variable.wrap.value.has_value()) {
		INTERPRETER_PRINT(aleprln, "Variable '{}' has no value.", var_name);
		return make_bad_evaluation(
			Vec{evaluation_error_e::Valueless_Variable},
			Vec{evaluation_function_e::Subscripted_Variable},
			Vec{std::format("Variable '{}' has no value", var_name)}
		);
	}

	if (variable.is_constant) {
		return make_good_evaluation<
			memory::
				WrappedAny>(std::cref(variable), detail::type_string_cpp<memory::RefConstMemVar>);
	}
	return make_good_evaluation<
		memory::
			WrappedAny>(std::ref(variable), detail::type_string_cpp<memory::RefMemVar>);
}

} // namespace ast
} // namespace intlib
