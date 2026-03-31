/*********************************************************************
 *
 * ALE language -- an interpreted programming language
 * Copyright (C) 2024 - 2026 Lluís Alemany Puig
 *
 * This file is part of the implementation of ALE. The full code is
 * available at:
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

#include <stdfloat>
#include <format>
#include <any>

#include <intlib/detail/any_output.hpp>
#include <intlib/detail/any_type.hpp>

std::formatter<any_view>::OutT std::formatter<any_view>::format(
	const any_view view, std::format_context& ctx
) const
{
	const std::any& value_w = view.value;
	const std::string name = intlib::detail::get_type_name(value_w);

	if (not value_w.has_value()) {
		return std::format_to(ctx.out(), "<empty>");
	}

	if (intlib::detail::is_type<bool>(name)) {
		return std::format_to(ctx.out(), "{}", std::any_cast<bool>(value_w));
	}

	if (intlib::detail::is_type<int8_t>(name)) {
		return std::format_to(ctx.out(), "{}", std::any_cast<int8_t>(value_w));
	}
	if (intlib::detail::is_type<uint8_t>(name)) {
		return std::format_to(ctx.out(), "{}", std::any_cast<uint8_t>(value_w));
	}

	if (intlib::detail::is_type<int16_t>(name)) {
		return std::format_to(ctx.out(), "{}", std::any_cast<int16_t>(value_w));
	}
	if (intlib::detail::is_type<uint16_t>(name)) {
		return std::format_to(
			ctx.out(), "{}", std::any_cast<uint16_t>(value_w)
		);
	}

	if (intlib::detail::is_type<int32_t>(name)) {
		return std::format_to(ctx.out(), "{}", std::any_cast<int32_t>(value_w));
	}
	if (intlib::detail::is_type<uint32_t>(name)) {
		return std::format_to(
			ctx.out(), "{}", std::any_cast<uint32_t>(value_w)
		);
	}

	if (intlib::detail::is_type<int64_t>(name)) {
		return std::format_to(ctx.out(), "{}", std::any_cast<int64_t>(value_w));
	}
	if (intlib::detail::is_type<uint64_t>(name)) {
		return std::format_to(
			ctx.out(), "{}", std::any_cast<uint64_t>(value_w)
		);
	}

	if (intlib::detail::is_type<float16_t>(name)) {
		return std::format_to(
			ctx.out(), "{}", std::any_cast<float16_t>(value_w)
		);
	}
	if (intlib::detail::is_type<float32_t>(name)) {
		return std::format_to(
			ctx.out(), "{}", std::any_cast<float32_t>(value_w)
		);
	}
	if (intlib::detail::is_type<float64_t>(name)) {
		return std::format_to(
			ctx.out(), "{}", std::any_cast<float64_t>(value_w)
		);
	}

	if (intlib::detail::is_type<float>(name)) {
		return std::format_to(ctx.out(), "{}", std::any_cast<float>(value_w));
	}
	if (intlib::detail::is_type<double>(name)) {
		return std::format_to(ctx.out(), "{}", std::any_cast<double>(value_w));
	}

	if (intlib::detail::is_type<std::string>(name)) {
		return std::format_to(
			ctx.out(), "{}", std::any_cast<std::string>(value_w)
		);
	}

	return std::format_to(
		ctx.out(), "Could not write value of type: '{}'.", name
	);
}
