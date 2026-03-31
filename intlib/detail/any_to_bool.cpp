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

#include <intlib/detail/any_type.hpp>
#include <intlib/detail/macros.hpp>

namespace intlib {
namespace detail {

std::optional<bool> any_to_bool(const std::any& value_w) noexcept
{
	const std::string type = get_type_name(value_w);

	if (detail::is_builtin_type<bool>(type)) {
		return std::any_cast<bool>(value_w);
	}
	if (detail::is_builtin_type<int64_t>(type)) {
		return std::any_cast<int64_t>(value_w);
	}
	if (detail::is_builtin_type<uint64_t>(type)) {
		return std::any_cast<uint64_t>(value_w);
	}
	if (detail::is_builtin_type<double>(type)) {
		return to_int64(std::any_cast<double>(value_w));
	}
	return {};
}

} // namespace detail
} // namespace intlib
