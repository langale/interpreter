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

#include <memory>
#include <string>

#include <ale/ast/Node.hpp>

#include <intlib/ast/EvaluationResult.hpp>
#include <intlib/ast/EvaluationContext.hpp>

namespace intlib {
namespace ast {

void append_variable_name(
	std::string& name, const std::vector<int64_t>& indices
);
[[nodiscard]] std::string make_indexed_variable_name(
	const std::string& name, const std::vector<int64_t>& indices
);

[[nodiscard]] std::string
get_variable_name(const ale::ast::SequenceNode& sequence);

[[nodiscard]] EvaluationResult make_subscripted_variable_name(
	EvaluationContext& ctx,
	const std::unique_ptr<ale::ast::Node>& subscripted_variable_w
);

struct ShallowSequenceIndices {
	std::vector<int64_t> left;
	std::vector<int64_t> right;
	std::string base_name;
};

[[nodiscard]] EvaluationResult make_shallow_sequence_indices(
	EvaluationContext& ctx, const std::unique_ptr<ale::ast::Node>& sequence_w
);

} // namespace ast
} // namespace intlib
