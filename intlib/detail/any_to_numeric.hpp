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

#if defined DEBUG
#include <cassert>
#endif
#include <optional>
#include <stdfloat>

#include <intlib/detail/any_type.hpp>
#include <intlib/detail/concepts.hpp>
#include <intlib/detail/type_string_ale.hpp>
#include <intlib/memory/VariableValue.hpp>

namespace intlib {
namespace detail {

template <typename cpp_t, auto compare_ale_type>
[[nodiscard]] std::optional<cpp_t> convert(
	const std::string_view ale_type,
	const std::string& any_name,
	const std::any& value_w
)
{
	if (ale_type == compare_ale_type) {
		using CppType = AleToCpp_t<compare_ale_type>;
#if defined DEBUG
		assert(is_cpp_basic_type<CppType>(any_name));
#endif
		return static_cast<cpp_t>(std::any_cast<CppType>(value_w));
	}
	return {};
}

template <Numeric_c cpp_t>
[[nodiscard]] std::optional<cpp_t>
variable_to_numeric(const memory::VariableValue& var) noexcept
{
	const std::string_view ale_type = var.type;
	const std::any& value_w = var.value_w;
	const std::string name = get_type_name(value_w);

	if (const auto r = convert<cpp_t, bool_ale>(ale_type, name, value_w);
		r.has_value()) {
		return r;
	}

	if (const auto r = convert<cpp_t, i8_ale>(ale_type, name, value_w);
		r.has_value()) {
		return r;
	}
	if (const auto r = convert<cpp_t, u8_ale>(ale_type, name, value_w);
		r.has_value()) {
		return r;
	}

	if (const auto r = convert<cpp_t, i16_ale>(ale_type, name, value_w);
		r.has_value()) {
		return r;
	}
	if (const auto r = convert<cpp_t, u16_ale>(ale_type, name, value_w);
		r.has_value()) {
		return r;
	}

	if (const auto r = convert<cpp_t, i32_ale>(ale_type, name, value_w);
		r.has_value()) {
		return r;
	}
	if (const auto r = convert<cpp_t, u32_ale>(ale_type, name, value_w);
		r.has_value()) {
		return r;
	}

	if (const auto r = convert<cpp_t, i64_ale>(ale_type, name, value_w);
		r.has_value()) {
		return r;
	}
	if (const auto r = convert<cpp_t, u64_ale>(ale_type, name, value_w);
		r.has_value()) {
		return r;
	}

	if (const auto r = convert<cpp_t, f16_ale>(ale_type, name, value_w);
		r.has_value()) {
		return r;
	}
	if (const auto r = convert<cpp_t, f32_ale>(ale_type, name, value_w);
		r.has_value()) {
		return r;
	}
	if (const auto r = convert<cpp_t, f64_ale>(ale_type, name, value_w);
		r.has_value()) {
		return r;
	}

	return {};
}

template <Numeric_c cpp_t>
[[nodiscard]] std::optional<cpp_t> any_to_numeric(const std::any& value_w
) noexcept
{
	if (detail::holds_cpp_type<memory::RefMemVar>(value_w)) {
		return variable_to_numeric<cpp_t>(
			std::any_cast<memory::RefMemVar>(value_w).get()
		);
	}
	if (detail::holds_cpp_type<memory::RefConstMemVar>(value_w)) {
		return variable_to_numeric<cpp_t>(
			std::any_cast<memory::RefConstMemVar>(value_w).get()
		);
	}

	const std::string name = get_type_name(value_w);

	if (is_cpp_basic_type<bool>(name)) {
		return static_cast<cpp_t>(std::any_cast<bool>(value_w));
	}

	if (is_cpp_basic_type<char>(name)) {
		return static_cast<cpp_t>(std::any_cast<char>(value_w));
	}
	if (is_cpp_basic_type<unsigned char>(name)) {
		return static_cast<cpp_t>(std::any_cast<unsigned char>(value_w));
	}
	if (is_cpp_basic_type<signed char>(name)) {
		return static_cast<cpp_t>(std::any_cast<signed char>(value_w));
	}

	if (is_cpp_basic_type<uint8_t>(name)) {
		return static_cast<cpp_t>(std::any_cast<uint8_t>(value_w));
	}
	if (is_cpp_basic_type<int8_t>(name)) {
		return static_cast<cpp_t>(std::any_cast<int8_t>(value_w));
	}

	if (is_cpp_basic_type<uint16_t>(name)) {
		return static_cast<cpp_t>(std::any_cast<uint16_t>(value_w));
	}
	if (is_cpp_basic_type<int16_t>(name)) {
		return static_cast<cpp_t>(std::any_cast<int16_t>(value_w));
	}

	if (is_cpp_basic_type<uint32_t>(name)) {
		return static_cast<cpp_t>(std::any_cast<uint32_t>(value_w));
	}
	if (is_cpp_basic_type<int32_t>(name)) {
		return static_cast<cpp_t>(std::any_cast<int32_t>(value_w));
	}

	if (is_cpp_basic_type<uint64_t>(name)) {
		return static_cast<cpp_t>(std::any_cast<uint64_t>(value_w));
	}
	if (is_cpp_basic_type<int64_t>(name)) {
		return static_cast<cpp_t>(std::any_cast<int64_t>(value_w));
	}

	if (is_cpp_basic_type<std::float16_t>(name)) {
		return static_cast<cpp_t>(std::any_cast<std::float16_t>(value_w));
	}
	if (is_cpp_basic_type<std::float32_t>(name)) {
		return static_cast<cpp_t>(std::any_cast<std::float32_t>(value_w));
	}
	if (is_cpp_basic_type<std::float64_t>(name)) {
		return static_cast<cpp_t>(std::any_cast<std::float64_t>(value_w));
	}

	return {};
}

} // namespace detail
} // namespace intlib
