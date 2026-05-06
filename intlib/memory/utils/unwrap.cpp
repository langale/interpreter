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

#include <intlib/memory/WrappedAny.hpp>
#include <intlib/memory/Variable.hpp>

namespace intlib {
namespace memory {

const WrappedAny *unwrap_out(const WrappedAny& w)
{
	if (w.type == detail::type_string_cpp<RefVar>) {
		return &std::any_cast<RefVar>(w.value).get().wrap;
	}
	if (w.type == detail::type_string_cpp<RefConstVar>) {
		return &std::any_cast<const RefConstVar>(w.value).get().wrap;
	}
	return &w;
}

void unwrap_into(WrappedAny& w)
{
	if (w.type == detail::type_string_cpp<RefVar>) {
		w = std::any_cast<RefVar>(w.value).get().wrap;
	}
	else if (w.type == detail::type_string_cpp<RefConstVar>) {
		w = std::any_cast<const RefVar>(w.value).get().wrap;
	}
}

} // namespace memory
} // namespace intlib
