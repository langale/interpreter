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

#include <expected>

#include <ale/detail/make_expected.hpp>

#include <intlib/memory/memory_error_enum.hpp>

namespace intlib {
namespace memory {

using AccessResult = std::expected<void, access_error_e>;

[[nodiscard]] inline AccessResult make_good_access_result()
{
	return ale::detail::
		make_expected<ale::detail::expected_type_e::Good, AccessResult>();
}

template <typename... params_t>
[[nodiscard]] AccessResult make_bad_access_result(params_t&&...params)
{
	return ale::detail::make_expected<
		ale::detail::expected_type_e::Bad,
		AccessResult>(std::forward<params_t>(params)...);
}

} // namespace memory
} // namespace intlib
