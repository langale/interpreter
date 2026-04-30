/*********************************************************************
 *
 * ALE interpreter -- a command line utility to run programs written in ALE
 * Copyright (C) 2024 - 2026 Lluís Alemany Puig
 *
 * This file is part of the implementation of an interpreter for ALE.
 * The full code is available at:
 *
 *     https://github.com/langale/ale
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

#include <concepts>
#include <stdfloat>
#include <cstdint>
#include <any>

namespace intlib {
namespace detail {

template <typename type_t>
[[nodiscard]] double to_double(const type_t& x) noexcept
{
	return static_cast<double>(x);
}

template <typename type_t>
[[nodiscard]] std::float64_t to_float64(const type_t& x) noexcept
{
	return static_cast<std::float64_t>(x);
}

template <typename type_t>
[[nodiscard]] uint64_t to_uint64(const type_t& x) noexcept
{
	return static_cast<uint64_t>(x);
}

template <typename type_t>
[[nodiscard]] int64_t to_int64(const type_t& x) noexcept
{
	return static_cast<int64_t>(x);
}

template <std::integral type_t>
[[nodiscard]] constexpr std::any adapt_type(type_t v) noexcept
{
	if (v >= 0) {
		return detail::to_uint64(v);
	}
	return detail::to_int64(v);
}

} // namespace detail
} // namespace intlib
