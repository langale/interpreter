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

#include <optional>
#include <string>
#include <any>

#include <intlib/logger/macros.hpp>
#include <intlib/detail/any_type.hpp>

namespace intlib {
namespace detail {

template <typename T, typename U>
[[nodiscard]] static std::optional<bool>
any_comparison_less_than(const std::any& left, const std::any& right) noexcept
{
	INTERPRETER_ENTER_COMPARISON_FUNCTION(ale::logger::println);

	if constexpr (std::equality_comparable_with<T, U>) {
		if (detail::is_type<T>(left) and detail::is_type<U>(right)) {
			const T l = std::any_cast<T>(left);
			const U r = std::any_cast<U>(right);
			if constexpr (std::is_floating_point_v<T> or
						  std::is_floating_point_v<U>) {
				typedef std::conditional_t<std::is_floating_point_v<T>, T, U> F;
				const F _l = static_cast<F>(l);
				const F _r = static_cast<F>(r);
				return _l < _r;
			}
			else if (std::is_integral_v<T> and std::is_integral_v<U>) {
				typedef std::conditional_t<std::is_signed_v<T>, T, U> I;
				const I _l = static_cast<I>(l);
				const I _r = static_cast<I>(r);
				return _l < _r;
			}
			else {
				return l < r;
			}
		}
	}
	return {};
}

template <typename Left>
[[nodiscard]] static std::optional<bool> any_comparison_less_than_right_numeric(
	const std::any& left, const std::any& right
) noexcept
{
	INTERPRETER_ENTER_COMPARISON_FUNCTION(ale::logger::println);

	if (auto r = any_comparison_less_than<Left, bool>(left, right);
		r.has_value()) {
		return r;
	}
	if (auto r = any_comparison_less_than<Left, int64_t>(left, right);
		r.has_value()) {
		return r;
	}
	if (auto r = any_comparison_less_than<Left, uint64_t>(left, right);
		r.has_value()) {
		return r;
	}
	if (auto r = any_comparison_less_than<Left, double>(left, right);
		r.has_value()) {
		return r;
	}
	return {};
}

std::optional<bool>
any_comparison_less_than(const std::any& left, const std::any& right) noexcept
{
	INTERPRETER_ENTER_COMPARISON_FUNCTION(ale::logger::println);

	if (auto r = any_comparison_less_than_right_numeric<bool>(left, right);
		r.has_value()) {
		return r;
	}
	if (auto r = any_comparison_less_than_right_numeric<int64_t>(left, right);
		r.has_value()) {
		return r;
	}
	if (auto r = any_comparison_less_than_right_numeric<uint64_t>(left, right);
		r.has_value()) {
		return r;
	}
	if (auto r = any_comparison_less_than_right_numeric<double>(left, right);
		r.has_value()) {
		return r;
	}

	if (auto r =
			any_comparison_less_than<std::string, std::string>(left, right);
		r.has_value()) {
		return r;
	}
	return {};
}

} // namespace detail
} // namespace intlib
