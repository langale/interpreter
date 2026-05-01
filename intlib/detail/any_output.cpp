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

#include <stdfloat>
#include <format>
#include <any>

#include <intlib/detail/any_type.hpp>
#include <intlib/detail/any_output.hpp>
#include <intlib/memory/Variable.hpp>
#include <intlib/memory/utils/variable_to_string.hpp>

using namespace intlib::detail;

std::formatter<AnyView>::OutT std::formatter<AnyView>::format(
	const AnyView& view, std::format_context& ctx
) const
{
	const std::any& value_w = view.value;
	const std::string name = get_type_name(value_w);

	if (not value_w.has_value()) {
		return std::format_to(ctx.out(), "<empty>");
	}

	if (holds_cpp_type<intlib::memory::RefVar>(value_w)) {
		const auto& var = std::any_cast<intlib::memory::RefVar>(value_w);
		return std::format_to(ctx.out(), "{}", var.get());
	}
	if (holds_cpp_type<intlib::memory::RefConstVar>(value_w)) {
		const auto& var =
			std::any_cast<intlib::memory::RefConstVar>(value_w);
		return std::format_to(ctx.out(), "{}", var.get());
	}

	if (is_cpp_type<bool>(name)) {
		return std::format_to(
			ctx.out(), "[bool] {}", std::any_cast<bool>(value_w)
		);
	}

	if (is_cpp_type<int8_t>(name)) {
		return std::format_to(
			ctx.out(), "[int8_t] {}", std::any_cast<int8_t>(value_w)
		);
	}
	if (is_cpp_type<uint8_t>(name)) {
		return std::format_to(
			ctx.out(), "[uint8_t] {}", std::any_cast<uint8_t>(value_w)
		);
	}

	if (is_cpp_type<int16_t>(name)) {
		return std::format_to(
			ctx.out(), "[int16_t] {}", std::any_cast<int16_t>(value_w)
		);
	}
	if (is_cpp_type<uint16_t>(name)) {
		return std::format_to(
			ctx.out(), "[uint16_t] {}", std::any_cast<uint16_t>(value_w)
		);
	}

	if (is_cpp_type<int32_t>(name)) {
		return std::format_to(
			ctx.out(), "[int32_t] {}", std::any_cast<int32_t>(value_w)
		);
	}
	if (is_cpp_type<uint32_t>(name)) {
		return std::format_to(
			ctx.out(), "[uint32_t] {}", std::any_cast<uint32_t>(value_w)
		);
	}

	if (is_cpp_type<int64_t>(name)) {
		return std::format_to(
			ctx.out(), "[int64_t] {}", std::any_cast<int64_t>(value_w)
		);
	}
	if (is_cpp_type<uint64_t>(name)) {
		return std::format_to(
			ctx.out(), "[uint64_t] {}", std::any_cast<uint64_t>(value_w)
		);
	}

	if (is_cpp_type<float16_t>(name)) {
		return std::format_to(
			ctx.out(), "[float16_t] {}", std::any_cast<std::float16_t>(value_w)
		);
	}
	if (is_cpp_type<float32_t>(name)) {
		return std::format_to(
			ctx.out(), "[float32_t] {}", std::any_cast<std::float32_t>(value_w)
		);
	}
	if (is_cpp_type<float64_t>(name)) {
		return std::format_to(
			ctx.out(), "[float64_t] {}", std::any_cast<std::float64_t>(value_w)
		);
	}

	if (is_cpp_type<std::string>(name)) {
		return std::format_to(
			ctx.out(), "[string] {}", std::any_cast<std::string>(value_w)
		);
	}

	return std::format_to(
		ctx.out(), "Could not write value of type: '{}'.", name
	);
}
