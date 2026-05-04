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

#include <intlib/memory/utils/wrapped_any_to_string.hpp>

#include <stdfloat>
#include <format>

#include <intlib/memory/WrappedAny.hpp>
#include <intlib/detail/any_output.hpp>
#include <intlib/detail/type_string_cpp.hpp>
#include <intlib/memory/utils/variable_to_string.hpp>

#define print(cpp_type, value)                                                 \
	if (type == intlib::detail::type_string_cpp<cpp_type>) {                   \
		return std::format_to(                                                 \
			ctx.out(), "[C++: {}] {}", type, std::any_cast<cpp_type>(value)    \
		);                                                                     \
	}

std::formatter<intlib::memory::WrappedAny>::OutT
std::formatter<intlib::memory::WrappedAny>::format(
	const intlib::memory::WrappedAny& res, std::format_context& ctx
) const
{
	const std::any& value = res.value;
	const std::string_view type = res.type;

	if (not value.has_value()) {
		return std::format_to(ctx.out(), "[C++: {}] <empty>", type);
	}

	if (type == intlib::detail::type_string_cpp<intlib::memory::RefVar>) {
		const auto& var = std::any_cast<intlib::memory::RefVar>(value);
		return std::format_to(ctx.out(), "{}", var.get());
	}
	if (type == intlib::detail::type_string_cpp<intlib::memory::RefConstVar>) {
		const auto& var = std::any_cast<intlib::memory::RefConstVar>(value);
		return std::format_to(ctx.out(), "{}", var.get());
	}

	print(bool, value);

	print(int8_t, value);
	print(uint8_t, value);

	print(int16_t, value);
	print(uint16_t, value);

	print(int32_t, value);
	print(uint32_t, value);

	print(int64_t, value);
	print(uint64_t, value);

	print(std::float16_t, value);
	print(std::float32_t, value);
	print(std::float64_t, value);

	print(std::string, value);

	return std::format_to(
		ctx.out(), "[c++: {}]: {}", type, intlib::detail::AnyView{value}
	);
}
