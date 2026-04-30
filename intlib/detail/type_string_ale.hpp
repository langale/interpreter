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

#include <string_view>
#include <ranges>
#include <array>

#include <intlib/detail/compile_time_strings.hpp>

namespace intlib {
namespace detail {

static constexpr auto bool_ale = as_c_str("bool"_lr);
static constexpr auto i8_ale = as_c_str("i8"_lr);
static constexpr auto u8_ale = as_c_str("u8"_lr);
static constexpr auto i16_ale = as_c_str("i16"_lr);
static constexpr auto u16_ale = as_c_str("u16"_lr);
static constexpr auto i32_ale = as_c_str("i32"_lr);
static constexpr auto u32_ale = as_c_str("u32"_lr);
static constexpr auto i64_ale = as_c_str("i64"_lr);
static constexpr auto u64_ale = as_c_str("u64"_lr);
static constexpr auto f16_ale = as_c_str("f16"_lr);
static constexpr auto f32_ale = as_c_str("f32"_lr);
static constexpr auto f64_ale = as_c_str("f64"_lr);

static constexpr std::array numeric_types{
	bool_ale,
	u8_ale,
	i8_ale,
	u16_ale,
	i16_ale,
	u32_ale,
	i32_ale,
	u64_ale,
	i64_ale,
	f16_ale,
	f32_ale,
	f64_ale
};

template <auto>
struct AleToCpp {
	using type = void;
};

template <auto identifier>
using AleToCpp_t = AleToCpp<identifier>::type;

#define MAKE_ALE_TO_CPP(ale, cpp)                                              \
	template <>                                                                \
	struct AleToCpp<ale> {                                                     \
		using type = cpp;                                                      \
	}

MAKE_ALE_TO_CPP(bool_ale, int8_t);
MAKE_ALE_TO_CPP(i8_ale, int8_t);
MAKE_ALE_TO_CPP(u8_ale, uint8_t);
MAKE_ALE_TO_CPP(i16_ale, int16_t);
MAKE_ALE_TO_CPP(u16_ale, uint16_t);
MAKE_ALE_TO_CPP(i32_ale, int32_t);
MAKE_ALE_TO_CPP(u32_ale, uint32_t);
MAKE_ALE_TO_CPP(i64_ale, int64_t);
MAKE_ALE_TO_CPP(u64_ale, uint64_t);
MAKE_ALE_TO_CPP(f16_ale, std::float16_t);
MAKE_ALE_TO_CPP(f32_ale, std::float32_t);
MAKE_ALE_TO_CPP(f64_ale, std::float64_t);

[[nodiscard]] static constexpr bool
is_ale_type_numeric(const std::string_view type) noexcept
{
	return std::ranges::find(numeric_types, type) != numeric_types.end();
}

} // namespace detail
} // namespace intlib
