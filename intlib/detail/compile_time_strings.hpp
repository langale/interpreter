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

#include <type_traits>

namespace intlib {
namespace detail {

template <typename type_t>
concept CharLike_c =
	std::is_same_v<type_t, char> or std::is_same_v<type_t, unsigned char> or
	std::is_same_v<type_t, signed char>;

template <CharLike_c char_t, auto... chars>
struct LiteralRange { };

template <typename char_t, char_t... chars>
[[nodiscard]] constexpr auto operator""_lr () noexcept
{
	return LiteralRange<char_t, chars...>{};
}

template <
	CharLike_c char1_t,
	auto... chars1,
	CharLike_c char2_t,
	auto... chars2>
[[nodiscard]] constexpr auto
operator+ (LiteralRange<char1_t, chars1...>, LiteralRange<char2_t, chars2...>)
	noexcept
{
	using CommonType = std::common_type_t<char1_t, char2_t>;
	return LiteralRange<CommonType, chars1..., chars2...>{};
}

template <CharLike_c char_t, auto... chars>
constexpr inline char str[] = {char_t(chars)..., char_t('\0')};

template <CharLike_c char_t, auto... chars>
[[nodiscard]] constexpr const char_t *
as_c_str(LiteralRange<char_t, chars...>) noexcept
{
	return str<char_t, chars...>;
}

} // namespace detail
} // namespace intlib
