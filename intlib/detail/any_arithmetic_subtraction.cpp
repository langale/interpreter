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
#include <cmath>
#include <any>

#include <intlib/logger/macros.hpp>
#include <intlib/detail/macros.hpp>
#include <intlib/detail/any_type.hpp>

namespace intlib {
namespace detail {

std::optional<std::any> any_arithmetic_subtraction
(const std::any& a, const std::any& b)
noexcept
{
	INTERPRETER_ENTER_FUNCTION(ale::logger::println);

	if (is_type<uint64_t>(a)) {
		const uint64_t ai = std::any_cast<uint64_t>(a);

		if (is_type<uint64_t>(b)) {
			const uint64_t bi = std::any_cast<uint64_t>(b);
			if (ai < bi) {
				return to_int64(ai) - to_int64(bi);
			}
			return ai - bi;
		}

		if (is_type<int64_t>(b)) {
			const int64_t bi = std::any_cast<int64_t>(b);
			const int64_t r = to_int64(ai) - bi;
			if (r >= 0) {
				return static_cast<uint64_t>(r);
			}
			return r;
		}

		if (is_type<double>(b)) {
			const double bd = std::any_cast<double>(b);
			return to_double(ai) - bd;
		}

		// UNHANDLED_ANY(ale::error(), b);
	}

	if (is_type<int64_t>(a)) {
		const int64_t ai = std::any_cast<int64_t>(a);

		if (is_type<uint64_t>(b)) {
			const uint64_t bi = std::any_cast<uint64_t>(b);
			const int64_t r = ai - to_int64(bi);
			if (r >= 0) {
				return static_cast<uint64_t>(r);
			}
			return r;
		}

		if (is_type<int64_t>(b)) {
			const int64_t bi = std::any_cast<int64_t>(b);
			const int64_t r = ai - bi;
			if (r >= 0) {
				return static_cast<uint64_t>(r);
			}
			return r;
		}

		if (is_type<double>(b)) {
			return to_double(ai) - std::any_cast<double>(b);
		}

		// UNHANDLED_ANY(ale::error(), b);
	}

	if (is_type<double>(a)) {
		const double ai = std::any_cast<double>(a);

		if (is_type<uint64_t>(b)) {
			const uint64_t bi = std::any_cast<uint64_t>(b);
			return ai - to_double(bi);
		}

		if (is_type<int64_t>(b)) {
			const int64_t bi = std::any_cast<int64_t>(b);
			return ai - to_double(bi);
		}

		if (is_type<double>(b)) {
			const double bd = std::any_cast<double>(b);
			return ai - bd;
		}

		// UNHANDLED_ANY(ale::error(), b);
	}

	// UNHANDLED_ANY(ale::error(), a);

	return {};
}

} // -- namespace detail
} // -- namespace intlib
