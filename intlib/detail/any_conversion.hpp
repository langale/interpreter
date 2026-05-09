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

#include <intlib/memory/WrappedAny.hpp>
#include <intlib/detail/type_string_cpp.hpp>

namespace intlib {
namespace detail {

[[nodiscard]] memory::WrappedAny convert_to_ale_type(
	const memory::WrappedAny& value, const std::string_view ale_type
);

template <typename type_t, typename output_t>
[[nodiscard]] bool adapt_to_type(memory::WrappedAny& w)
{
	if (w.type == detail::type_string_cpp<type_t>) {
		w.type = detail::type_string_cpp<output_t>;
		w.value = static_cast<output_t>(std::any_cast<type_t>(w.value));
		return true;
	}
	return false;
}

void upcast_numeric(memory::WrappedAny& w);

} // namespace detail
} // namespace intlib
