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

#include <intlib/ast/SequenceExecutionEnvironment.hpp>

#if defined DEBUG
#include <cassert>
#endif

#include <optional>
#include <ranges>

#include <intlib/ast/Evaluation.hpp>
#include <intlib/logger/macros.hpp>

namespace intlib {
namespace ast {

Evaluation SequenceExecutionEnvironment::make_distances()
{
	const auto depth_levels = m_first_indices.get_depth();

	INTERPRETER_PRINT("depth_levels= {}.", depth_levels);

	m_distances.resize(depth_levels, 0);
	for (size_t depth = 0; depth < depth_levels; ++depth) {
		INTERPRETER_PRINT("At depth= {}.", depth);

		const auto& first = m_first_indices.get_variables_depth(depth);
#if defined ALE_LOGGING_MESSAGES
		INTERPRETER_PRINT("First indices:");
		for (const auto& [var, index] : first) {
			INTERPRETER_PRINT("    {} -> {}.", var, index);
		}
#endif

		const auto& last = m_last_indices.get_variables_depth(depth);
#if defined ALE_LOGGING_MESSAGES
		INTERPRETER_PRINT("Last indices:");
		for (const auto& [var, index] : last) {
			INTERPRETER_PRINT("    {} -> {}.", var, index);
		}
#endif

		std::optional<int64_t> distance;
		for (const auto& [var, first_index] : first) {

			if (not last.contains(var)) {
				return make_bad_evaluation(
					Vec{evaluation_error_e::
							Sequence_Environment_Missing_Right_Variable},
					Vec{evaluation_function_e::
							Sequence_Execution_Environment_Construction},
					Vec{std::format(
						"Variable '{}' is not in the right hand side expression"
						" at depth '{}'",
						var,
						depth
					)}
				);
			}

			const int64_t last_index = last.find(var)->second;
			const int64_t new_distance = last_index - first_index + 1;

			if (not distance) {
				distance = new_distance;
			}
			else if (*distance != new_distance) {
				return make_bad_evaluation(
					Vec{evaluation_error_e::
							Sequence_Environment_Mismatch_Distance},
					Vec{evaluation_function_e::
							Sequence_Execution_Environment_Construction},
					Vec{std::format(
						"Distance '{}' does not coincide for distance '{}' of "
						"variable '{}' at depth '{}'",
						*distance,
						new_distance,
						var,
						depth
					)}
				);
			}
		}

#if defined DEBUG
		assert(distance.has_value());
		assert(*distance > 0);
#endif

		INTERPRETER_PRINT("    calculated distance= {}.", *distance);
		m_distances[depth] = *distance;
	}

	m_working_distances.resize(m_distances.size());
	std::ranges::fill(m_working_distances, 0);

	return make_good_evaluation<EvaluationResult>();
}

} // namespace ast
} // namespace intlib
