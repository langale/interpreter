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

template <typename left_t, typename right_t>
[[nodiscard]] static std::optional<bool>
any_comparison_not_equal(const std::any& left_w, const std::any& right_w)
{
	INTERPRETER_ENTER_COMPARISON_FUNCTION(ale::logger::println);

	if constexpr (std::equality_comparable_with<left_t, right_t>) {
		if (detail::is_type<left_t>(left_w) and
			detail::is_type<right_t>(right_w)) {
			const auto left = std::any_cast<left_t>(left_w);
			const auto right = std::any_cast<right_t>(right_w);

			if constexpr (std::is_floating_point_v<left_t> or
						  std::is_floating_point_v<right_t>) {

				using F = std::conditional_t<
					std::is_floating_point_v<left_t>,
					left_t,
					right_t>;

				const auto l = static_cast<F>(left);
				const auto r = static_cast<F>(right);
				return l != r;
			}
			else if (std::is_integral_v<left_t> and
					 std::is_integral_v<right_t>) {

				using I = std::
					conditional_t<std::is_signed_v<left_t>, left_t, right_t>;

				const auto l = static_cast<I>(left);
				const auto r = static_cast<I>(right);
				return l != r;
			}
			else {
				return left != right;
			}
		}
	}
	return {};
}

template <typename left_t>
[[nodiscard]] static std::optional<bool> any_comparison_not_equal_right_numeric(
	const std::any& left_w, const std::any& right_w
)
{
	INTERPRETER_ENTER_COMPARISON_FUNCTION(ale::logger::println);

	if (const auto r = any_comparison_not_equal<left_t, bool>(left_w, right_w);
		r.has_value()) {
		return r;
	}
	if (const auto r =
			any_comparison_not_equal<left_t, int64_t>(left_w, right_w);
		r.has_value()) {
		return r;
	}
	if (const auto r =
			any_comparison_not_equal<left_t, uint64_t>(left_w, right_w);
		r.has_value()) {
		return r;
	}
	if (const auto r =
			any_comparison_not_equal<left_t, double>(left_w, right_w);
		r.has_value()) {
		return r;
	}
	return {};
}

std::optional<bool>
any_comparison_not_equal(const std::any& left_w, const std::any& right_w)
{
	INTERPRETER_ENTER_COMPARISON_FUNCTION(ale::logger::println);

	if (const auto r =
			any_comparison_not_equal_right_numeric<bool>(left_w, right_w);
		r.has_value()) {
		return r;
	}
	if (const auto r =
			any_comparison_not_equal_right_numeric<int64_t>(left_w, right_w);
		r.has_value()) {
		return r;
	}
	if (const auto r =
			any_comparison_not_equal_right_numeric<uint64_t>(left_w, right_w);
		r.has_value()) {
		return r;
	}
	if (const auto r =
			any_comparison_not_equal_right_numeric<double>(left_w, right_w);
		r.has_value()) {
		return r;
	}

	if (const auto r =
			any_comparison_not_equal<std::string, std::string>(left_w, right_w);
		r.has_value()) {
		return r;
	}
	return {};
}

} // namespace detail
} // namespace intlib
