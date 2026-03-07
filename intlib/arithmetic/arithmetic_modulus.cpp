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

#include <any>

#include <intlib/logger/macros.hpp>
#include <intlib/detail/macros.hpp>
#include <intlib/detail/any_type.hpp>

namespace intlib {
namespace arithmetic {

std::any arithmetic_modulus(const std::any& a, const std::any& b)
{
	INTERPRETER_ENTER_FUNCTION(ale::logger::println);

	if (detail::is_type<uint64_t>(a)) {
		const uint64_t ai = std::any_cast<uint64_t>(a);

		if (detail::is_type<uint64_t>(b)) {
			const uint64_t bi = std::any_cast<uint64_t>(b);
			return ai % bi;
		}

		if (detail::is_type<int64_t>(b)) {
			const int64_t bi = std::any_cast<int64_t>(b);
			return detail::to_int64(ai) % bi;
		}

		// UNHANDLED_ANY(ale::error(), b);
	}

	if (detail::is_type<int64_t>(a)) {
		const int64_t ai = std::any_cast<int64_t>(a);

		if (detail::is_type<uint64_t>(b)) {
			const uint64_t bi = std::any_cast<uint64_t>(b);
			return ai % detail::to_int64(bi);
		}

		if (detail::is_type<int64_t>(b)) {
			const int64_t bi = std::any_cast<int64_t>(b);
			return ai % bi;
		}

		// UNHANDLED_ANY(ale::error(), b);
	}

	// UNHANDLED_ANY(ale::error(), a);

	return {};
}

} // namespace arithmetic
} // namespace intlib
