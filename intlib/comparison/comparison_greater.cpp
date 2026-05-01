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
#include <optional>
#include <string>
#include <any>

#include <intlib/logger/macros.hpp>
#include <intlib/memory/WrappedAny.hpp>
#include <intlib/detail/type_string_cpp.hpp>
#include <intlib/comparison/definitions.hpp>

namespace intlib {
namespace comparison {

#define aleprln ale::logger::println

template <typename left_t, typename right_t>
[[nodiscard]] static std::optional<bool>
any_comparison_greater_than(const WrappedAny& left_w, const WrappedAny& right_w)
{
	INTERPRETER_ENTER_COMPARISON_FUNCTION(aleprln);

	if constexpr (std::equality_comparable_with<left_t, right_t>) {
		if (left_w.type == detail::cpp_type_string<left_t> and
			right_w.type == detail::cpp_type_string<right_t>) {

			const auto left = std::any_cast<left_t>(left_w.value);
			const auto right = std::any_cast<right_t>(right_w.value);

			if constexpr (std::is_floating_point_v<left_t> or
						  std::is_floating_point_v<right_t>) {
				using F = std::conditional_t<
					std::is_floating_point_v<left_t>,
					left_t,
					right_t>;

				const auto l = static_cast<F>(left);
				const auto r = static_cast<F>(right);
				return l > r;
			}
			else if (std::is_integral_v<left_t> and
					 std::is_integral_v<right_t>) {

				using I = std::
					conditional_t<std::is_signed_v<left_t>, left_t, right_t>;

				const auto l = static_cast<I>(left);
				const auto r = static_cast<I>(right);
				return l > r;
			}
			else {
				return left > right;
			}
		}
	}
	return {};
}

template <typename left_t>
[[nodiscard]] static std::optional<bool>
any_comparison_greater_than_right_numeric(
	const WrappedAny& left_w, const WrappedAny& right_w
)
{
	INTERPRETER_ENTER_COMPARISON_FUNCTION(aleprln);

	if (const auto r =
			any_comparison_greater_than<left_t, bool>(left_w, right_w);
		r.has_value()) {
		return r;
	}
	if (const auto r =
			any_comparison_greater_than<left_t, int64_t>(left_w, right_w);
		r.has_value()) {
		return r;
	}
	if (const auto r =
			any_comparison_greater_than<left_t, uint64_t>(left_w, right_w);
		r.has_value()) {
		return r;
	}
	if (const auto r = any_comparison_greater_than<left_t, std::float64_t>(
			left_w, right_w
		);
		r.has_value()) {
		return r;
	}
	return {};
}

std::optional<bool>
any_comparison_greater_than(const WrappedAny& left_w, const WrappedAny& right_w)
{
	INTERPRETER_ENTER_COMPARISON_FUNCTION(aleprln);

	if (const auto r =
			any_comparison_greater_than_right_numeric<bool>(left_w, right_w);
		r.has_value()) {
		return r;
	}
	if (const auto r =
			any_comparison_greater_than_right_numeric<int64_t>(left_w, right_w);
		r.has_value()) {
		return r;
	}
	if (const auto r = any_comparison_greater_than_right_numeric<uint64_t>(
			left_w, right_w
		);
		r.has_value()) {
		return r;
	}
	if (const auto r =
			any_comparison_greater_than_right_numeric<std::float64_t>(
				left_w, right_w
			);
		r.has_value()) {
		return r;
	}

	if (const auto r = any_comparison_greater_than<std::string, std::string>(
			left_w, right_w
		);
		r.has_value()) {
		return r;
	}
	return {};
}

} // namespace comparison
} // namespace intlib
