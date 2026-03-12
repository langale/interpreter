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

#include <expected>
#include <cstdint>
#include <type_traits>

namespace intlib {
namespace detail {

enum class result_type_e : int8_t {
	Good,
	Bad
};

template <result_type_e res, typename expected_t, typename... params_t>
expected_t make_expected(params_t&&...params)
{
	using Good = expected_t::value_type;
	using Bad = expected_t::error_type;

	if constexpr (res == result_type_e::Good) {
		static_assert(std::is_constructible_v<Good, params_t...>);
		return expected_t(std::in_place_t{}, std::forward<params_t>(params)...);
	}
	else if constexpr (res == result_type_e::Bad) {
		static_assert(std::is_constructible_v<Bad, params_t...>);
		return expected_t(std::unexpect_t{}, std::forward<params_t>(params)...);
	}
	else {
		static_assert(false);
	}

	return {};
}

} // namespace detail
} // namespace intlib
