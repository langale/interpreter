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

#include <ale/logger/Stream.hpp>
#include <ale/ast/utils/xml_tree.hpp>

#include <intlib/Program.hpp>
#include <intlib/ast/interpretation.hpp>
#include <intlib/ast/EvaluationResult.hpp>

namespace intlib {

Program::Program() noexcept
{
	m_memory.initialize();
}

/* OTHERS */

ast::EvaluationResult Program::run_program()
{
	ast::EvaluationContext ctx{.memory = m_memory};
	return ast::interpret_node(ctx, m_program_node);
}

/* OTHERS */

void Program::print_tree(ale::logger::Stream& os) const
{
	os.out() << "= [fixedsize=false shape=none]\n";
	os.out() << "'PROGRAM'\n";
	ale::ast::print_xml_tree(
		m_program_node,
		{.os = os.out(),
		 .start = " ",
		 .sep = " ",
		 .use_tab = true,
		 .include_attributes = true}
	);
}

} // namespace intlib
