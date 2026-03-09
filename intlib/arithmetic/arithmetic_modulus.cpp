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
		const auto ai = std::any_cast<uint64_t>(a);
		INTERPRETER_PRINT_LOC2(
			ale::logger::println, "First component is uint64_t: {}.", ai
		);

		if (detail::is_type<uint64_t>(b)) {
			const auto bi = std::any_cast<uint64_t>(b);
			INTERPRETER_PRINT_LOC2(
				ale::logger::println, "Second component is uint64_t: {}.", bi
			);

			return ai % bi;
		}

		if (detail::is_type<int64_t>(b)) {
			const auto bi = std::any_cast<int64_t>(b);
			INTERPRETER_PRINT_LOC2(
				ale::logger::println, "Second component is int64_t: {}.", bi
			);

			const auto res = detail::to_int64(ai) % bi;
			return detail::adapt_type(res);
		}

		if (detail::is_type<double>(b)) {
			const double bd = std::any_cast<double>(b);
			INTERPRETER_PRINT_LOC2(
				ale::logger::println, "Second component is double: {}.", bd
			);

			if (bd < 0) {
				const auto res = detail::to_int64(ai) % detail::to_int64(bd);
				return detail::adapt_type(res);
			}

			const auto res = ai % detail::to_uint64(bd);
			return detail::adapt_type(res);
		}

		INTERPRETER_PRINT_LOC2(
			ale::logger::println,
			"Parameter b's type {} is not handled.",
			detail::get_type_name(b)
		);
	}

	if (detail::is_type<int64_t>(a)) {
		const auto ai = std::any_cast<int64_t>(a);
		INTERPRETER_PRINT_LOC2(
			ale::logger::println, "First component is int64_t: {}.", ai
		);

		if (detail::is_type<uint64_t>(b)) {
			const auto bi = std::any_cast<uint64_t>(b);
			INTERPRETER_PRINT_LOC2(
				ale::logger::println, "Second component is uint64_t: {}.", bi
			);

			const auto res = ai % detail::to_int64(bi);
			return detail::adapt_type(res);
		}

		if (detail::is_type<int64_t>(b)) {
			const auto bi = std::any_cast<int64_t>(b);
			INTERPRETER_PRINT_LOC2(
				ale::logger::println, "Second component is int64_t: {}.", bi
			);

			const auto res = ai % bi;
			return detail::adapt_type(res);
		}

		if (detail::is_type<double>(b)) {
			const double bd = std::any_cast<double>(b);
			INTERPRETER_PRINT_LOC2(
				ale::logger::println, "Second component is double: {}.", bd
			);

			if (bd < 0) {
				const auto res = ai % detail::to_int64(bd);
				return detail::adapt_type(res);
			}
			const auto res = detail::to_uint64(ai) % detail::to_uint64(bd);
			return detail::adapt_type(res);
		}

		INTERPRETER_PRINT_LOC2(
			ale::logger::println,
			"Parameter b's type {} is not handled.",
			detail::get_type_name(b)
		);
	}

	if (detail::is_type<double>(a)) {
		const auto ai = std::any_cast<double>(a);
		INTERPRETER_PRINT_LOC2(
			ale::logger::println, "First component is double: {}.", ai
		);

		if (detail::is_type<uint64_t>(b)) {
			const auto bi = std::any_cast<uint64_t>(b);
			INTERPRETER_PRINT_LOC2(
				ale::logger::println, "Second component is uint64_t: {}.", bi
			);

			if (ai < 0) {
				const auto res = detail::to_int64(ai) % detail::to_int64(bi);
				return detail::adapt_type(res);
			}
			const auto res = detail::to_uint64(ai) % bi;
			return detail::adapt_type(res);
		}

		if (detail::is_type<int64_t>(b)) {
			const auto bi = std::any_cast<int64_t>(b);
			INTERPRETER_PRINT_LOC2(
				ale::logger::println, "Second component is int64_t: {}.", bi
			);

			const auto res = detail::to_int64(ai) % bi;
			return detail::adapt_type(res);
		}

		if (detail::is_type<double>(b)) {
			const double bd = std::any_cast<double>(b);
			INTERPRETER_PRINT_LOC2(
				ale::logger::println, "Second component is double: {}.", bd
			);

			if (bd < 0 or ai < 0) {
				const auto res = detail::to_int64(ai) % detail::to_int64(bd);
				return detail::adapt_type(res);
			}
			const auto res = detail::to_uint64(ai) % detail::to_uint64(bd);
			return detail::adapt_type(res);
		}

		INTERPRETER_PRINT_LOC2(
			ale::logger::println,
			"Parameter b's type {} is not handled.",
			detail::get_type_name(b)
		);
	}

	INTERPRETER_PRINT_LOC2(
		ale::logger::println,
		"Parameter a's type {} is not handled.",
		detail::get_type_name(a)
	);

	return {};
}

} // namespace arithmetic
} // namespace intlib
