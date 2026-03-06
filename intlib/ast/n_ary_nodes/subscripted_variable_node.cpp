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
#include <any>

#include <intlib/detail/any_type.hpp>
#include <intlib/detail/any_output.hpp>
#include <intlib/detail/macros.hpp>

#include <intlib/Program.hpp>

namespace intlib {

std::string
Program::make_full_variable_name(const ale::ast::SubscriptedVariableNode& v)
{
	// using ale::detail::operator<<;

	std::string full_variable_name = v.get_variable_name();
	for (const auto& c : v.get_children()) {
		const std::optional<std::any> res = interpret_node(c);
		if (not res.has_value()) {
			// ale::error() << ERROR_LOCATION << '\n';
			// ale::error() << "    Evaluation of subscript node failed.\n";
			return {};
		}
		const std::any& r = *res;

		const bool is_uint64 = detail::is_type<uint64_t>(r);
		const bool is_int64 = detail::is_type<int64_t>(r);
		if (not is_uint64 and not is_int64) {
			// ale::error() << ERROR_LOCATION << '\n';
			// ale::error() << "    Evaluation of subscript is not an unsigned "
			// 				"integer value.\n";
			// ale::error() << "    Result: '" << r << "'.\n";
			return {};
		}

		if (is_uint64) {
			const uint64_t j = std::any_cast<uint64_t>(r);
			full_variable_name += "_" + std::to_string(j);
		}
		else {
			const int64_t j = std::any_cast<int64_t>(r);
			full_variable_name += "_" + std::to_string(j);
		}
	}
	return full_variable_name;
}

std::vector<int64_t>
Program::get_index_sequence(const ale::ast::SubscriptedVariableNode& v)
{
	// using ale::detail::operator<<;

	std::vector<int64_t> indices(v.get_num_children());
	std::size_t i = 0;
	for (const auto& c : v.get_children()) {
		const std::optional<std::any> res = interpret_node(c);
		if (not res.has_value()) {
			// ale::error() << ERROR_LOCATION << '\n';
			// ale::error() << "    Evaluation of subscript node failed.\n";
			return {};
		}

		const std::any& r = *res;
		const bool is_uint64 = detail::is_type<uint64_t>(r);
		const bool is_int64 = detail::is_type<int64_t>(r);
		if (not is_uint64 and not is_int64) {
			// ale::error() << ERROR_LOCATION << '\n';
			// ale::error() << "    Evaluation of subscript is not an unsigned "
			// 				"integer value.\n";
			// ale::error() << "    Result: '" << r << "'.\n";
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

std::optional<std::any>
Program::evaluate(const ale::ast::SubscriptedVariableNode& v)
{
	const std::string full_variable_name = make_full_variable_name(v);
	if (not m_memory.variable_exists(full_variable_name)) {
		// ale::error() << ERROR_LOCATION << '\n';
		// ale::error() << "    Variable '" << full_variable_name
		// 			 << "' does not exist in this scope.\n";
		return {};
	}
	std::optional<memory::VariableValue> res =
		m_memory.get_variable(full_variable_name);
#if defined DEBUG
	assert(res.has_value());
#endif

	if (detail::is_type<void>(res->value)) {
		// ale::error() << ERROR_LOCATION << '\n';
		// ale::error() << "    Variable '" << full_variable_name
		// 			 << "' has no value in the current scope.\n";
		return {};
	}

	return std::move(res->value);
}

} // namespace intlib
