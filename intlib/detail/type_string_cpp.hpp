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
#include <stdfloat>

namespace intlib {
namespace detail {

template <typename type_t>
constexpr inline std::string_view cpp_type_string = "invalid!";

template <>
constexpr inline std::string_view cpp_type_string<bool> = "bool";
template <>
constexpr inline std::string_view cpp_type_string<char> = "char";

template <>
constexpr inline std::string_view cpp_type_string<int8_t> = "signed char";
template <>
constexpr inline std::string_view cpp_type_string<uint8_t> = "unsigned char";

template <>
constexpr inline std::string_view cpp_type_string<int16_t> = "short";
template <>
constexpr inline std::string_view cpp_type_string<uint16_t> = "unsigned short";

template <>
constexpr inline std::string_view cpp_type_string<int32_t> = "int";
template <>
constexpr inline std::string_view cpp_type_string<uint32_t> = "unsigned int";

template <>
constexpr inline std::string_view cpp_type_string<int64_t> = "long";
template <>
constexpr inline std::string_view cpp_type_string<uint64_t> = "unsigned long";

template <>
inline constexpr std::string_view cpp_type_string<std::float16_t> = "_Float16";
template <>
inline constexpr std::string_view cpp_type_string<std::float32_t> = "_Float32";
template <>
inline constexpr std::string_view cpp_type_string<std::float64_t> = "_Float64";

template <>
constexpr inline std::string_view cpp_type_string<void> = "void";

} // namespace detail
} // namespace intlib
