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

#include <ale/logger/Logger.hpp>
#include <ale/logger/macros.hpp>

#include <intlib/detail/any_type.hpp>
#include <intlib/detail/any_output.hpp>
#include <intlib/detail/macros.hpp>

#include <intlib/Program.hpp>

namespace intlib {

std::optional<std::string>
Program::make_full_variable_name(const ale::ast::SubscriptedVariableNode& v)
{
	std::string full_variable_name = v.get_variable_name();
	for (const auto& [i, c] : v.get_children() | std::views::enumerate) {
		const EvaluationResult res = interpret_node(c);
		if (not res) {
			ALE_PRINT_LOC2(
				ale::logger::println,
				"Failed evaluation of child {} of subscripted variable {}.",
				i,
				v.get_variable_name()
			);
			return {};
		}
		const std::any& r = *res;

		const bool is_uint64 = detail::is_type<uint64_t>(r);
		const bool is_int64 = detail::is_type<int64_t>(r);
		if (not is_uint64 and not is_int64) {
			ALE_PRINT_LOC2(
				ale::logger::println,
				"Evaluation of child {} of subscripted variable {} is not an "
				"integral number.",
				i,
				v.get_variable_name()
			);
			return {};
		}

		if (is_uint64) {
			const auto j = std::any_cast<uint64_t>(r);
			full_variable_name += "_" + std::to_string(j);
		}
		else {
			const auto j = std::any_cast<int64_t>(r);
			full_variable_name += "_" + std::to_string(j);
		}
	}
	return full_variable_name;
}

std::optional<std::vector<int64_t>>
Program::get_index_sequence(const ale::ast::SubscriptedVariableNode& v)
{
	std::vector<int64_t> indices(v.get_num_children());
	std::size_t i = 0;
	for (const auto& c : v.get_children()) {
		const std::optional<std::any> res = interpret_node(c);
		if (not res.has_value()) {
			ALE_PRINT_LOC2(
				ale::logger::println,
				"Failed evaluation of child {} of subscripted variable {}.",
				i,
				v.get_variable_name()
			);
			return {};
		}

		const std::any& r = *res;
		const bool is_uint64 = detail::is_type<uint64_t>(r);
		const bool is_int64 = detail::is_type<int64_t>(r);
		if (not is_uint64 and not is_int64) {
			ALE_PRINT_LOC2(
				ale::logger::println,
				"Evaluation of child {} of subscripted variable {} is not an "
				"integral number.",
				i,
				v.get_variable_name()
			);
			return {};
		}

		if (is_uint64) {
			const uint64_t j = std::any_cast<uint64_t>(r);
			indices[i++] = detail::to_int64(j);
		}
		else {
			const int64_t j = std::any_cast<int64_t>(r);
			indices[i++] = j;
		}
	}
	return indices;
}

EvaluationResult Program::evaluate(const ale::ast::SubscriptedVariableNode& v)
{
	const std::optional<std::string> full_variable_name_w =
		make_full_variable_name(v);

	if (not full_variable_name_w) {
		ALE_PRINT_LOC2(
			ale::logger::println,
			"Full variable name of subscripted variable {} could not be "
			"retrieved.",
			v.get_variable_name()
		);
		return EvaluationError{
			.error = {evaluation_error_e::Valueless_Variable},
			.message = {std::format(
				"Full variable name of subscripted variable {} could not be "
				"retrieved.",
				v.get_variable_name()
			)}
		};
	}

	const std::string& full_variable_name = *full_variable_name_w;
	if (not m_memory.variable_exists(full_variable_name)) {
		ALE_PRINT_LOC2(
			ale::logger::println,
			"Variable '{}' is not defined in this scope.",
			full_variable_name
		);
		return EvaluationError{
			.error = {evaluation_error_e::Valueless_Variable},
			.message = {std::format(
				"Variable '{}' is not defined in this scope.",
				full_variable_name
			)}
		};
	}

	std::optional<memory::VariableValue> res =
		m_memory.get_variable(full_variable_name);
#if defined DEBUG
	assert(res.has_value());
#endif

	if (detail::is_type<void>(res->value)) {
		ALE_PRINT_LOC2(
			ale::logger::println, "Variable '{}' has no value.", full_variable_name
		);
		return EvaluationError{
			.error = {evaluation_error_e::Valueless_Variable},
			.message = {
				std::format("Variable '{}' has no value.", full_variable_name)
			}
		};
	}

	return std::move(res->value);
}

} // namespace intlib
