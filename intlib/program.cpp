/*********************************************************************
 *
 * ALE interpreter library -- the base utilities for a command line utility
 * to run programs written in ALE
 *
 *     Copyright (C) 2024 Lluís Alemany Puig
 *
 * This file is part of the implementation of an interpreter for ALE.
 * The full code is available at:
 *
 *     https://github.com/lluisalemanypuig/alelang
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

// ale includes
#include <ale/detail/any_output.hpp>
#include <ale/logger.hpp>

// interpreter includes
#include <intlib/program.hpp>

namespace interpreter {

program::program() noexcept {
	m_memory.initialize();
}

/* MODIFIERS */

void program::clear() noexcept {
	for (auto& i : m_instructions) {
#if defined DEBUG
		assert(i != nullptr);
#endif
		i->clear();
	}
	m_instructions.clear();
}

void program::add_instructions
(std::vector<std::unique_ptr<ale::ast::node>>&& ins)
noexcept
{
	if (m_instructions.size() == 0) {
		m_instructions = std::move(ins);
	}
	else {
		for (auto& p : ins) {
			m_instructions.push_back(std::move(p));
		}
		ins.clear();
	}
}

void program::run_program
(ale::stream& os)
noexcept
{
	using ale::detail::operator<<;

	for (const auto& i : m_instructions) {
		const std::optional<std::any> r = interpret_node(i);

		// the evaluation of this node failed and there is no reason to
		// continue the execution of the program.
		if (not r.has_value()) { break; }
		// (*r) is an object of type 'std::any'
		if ((*r).has_value()) {
			os << *r << '\n';
		}
	}
}

/* OTHERS */

void program::print_tree
(ale::stream& os)
const noexcept
{
	os << "= [fixedsize=false shape=none]\n";
	os << "'PROGRAM'\n";
	for (const auto& i : m_instructions) {
		i->print_tree(os, " ");
	}
}

} // -- namespace interpreter
