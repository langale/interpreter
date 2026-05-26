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
#include <string>
using namespace std::string_literals;

#include <ale/ast/n_ary_nodes/ProgramNode.hpp>
#include <ale/ast/hybrid_nodes/FunctionNode.hpp>

#include <intlib/ast/Evaluation.hpp>
#include <intlib/ast/evaluation.hpp>
#include <intlib/ast/EvaluationContext.hpp>
#include <intlib/ast/utils/evaluation_error_enum.hpp>
#include <intlib/ast/utils/evaluation_function_enum.hpp>
#include <intlib/logger/macros.hpp>
#if defined ALE_LOGGING_MESSAGES
#endif

namespace intlib {
namespace ast {

Evaluation evaluate(EvaluationContext& ctx, const ale::ast::ProgramNode& p)
{
	INTERPRETER_ENTER_AST_FUNCTION;

	// iterate over the functions and execute 'main'
	for (const auto& child : p.get_children()) {
		if (child->get_node_type() == ale::ast::node_type_e::Function) {
			const auto& func =
				*static_cast<ale::ast::FunctionNode *>(child.get());
			if (func.get_name() == "main") {
				return evaluate(ctx, func);
			}
		}
	}

	return make_bad_evaluation(
		Vec{evaluation_error_e::Function_Main_Missing},
		Vec{evaluation_function_e::Program},
		Vec{"Missing 'main' function"s}
	);
}

} // namespace ast
} // namespace intlib
