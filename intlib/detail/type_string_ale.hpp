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
#include <cstdint>
#include <array>

namespace intlib {
namespace detail {

static constexpr inline char ale_bool[] = "bool";
static constexpr inline char ale_i8[] = "i8";
static constexpr inline char ale_u8[] = "u8";
static constexpr inline char ale_i16[] = "i16";
static constexpr inline char ale_u16[] = "u16";
static constexpr inline char ale_i32[] = "i32";
static constexpr inline char ale_u32[] = "u32";
static constexpr inline char ale_i64[] = "i64";
static constexpr inline char ale_u64[] = "u64";
static constexpr inline char ale_f16[] = "f16";
static constexpr inline char ale_f32[] = "f32";
static constexpr inline char ale_f64[] = "f64";

static constexpr std::array numeric_types{
	ale_bool,
	ale_u8,
	ale_i8,
	ale_u16,
	ale_i16,
	ale_u32,
	ale_i32,
	ale_u64,
	ale_i64,
	ale_f16,
	ale_f32,
	ale_f64
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

MAKE_ALE_TO_CPP(ale_bool, int8_t);
MAKE_ALE_TO_CPP(ale_i8, int8_t);
MAKE_ALE_TO_CPP(ale_u8, uint8_t);
MAKE_ALE_TO_CPP(ale_i16, int16_t);
MAKE_ALE_TO_CPP(ale_u16, uint16_t);
MAKE_ALE_TO_CPP(ale_i32, int32_t);
MAKE_ALE_TO_CPP(ale_u32, uint32_t);
MAKE_ALE_TO_CPP(ale_i64, int64_t);
MAKE_ALE_TO_CPP(ale_u64, uint64_t);
MAKE_ALE_TO_CPP(ale_f16, std::float16_t);
MAKE_ALE_TO_CPP(ale_f32, std::float32_t);
MAKE_ALE_TO_CPP(ale_f64, std::float64_t);

[[nodiscard]] static constexpr bool
is_ale_type_numeric(const std::string_view type) noexcept
{
	return std::ranges::find(numeric_types, type) != numeric_types.end();
}

} // namespace detail
} // namespace intlib
