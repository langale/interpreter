/*********************************************************************
 *
 * ALE interpreter library -- the base utilities for a command line utility
 * to run Programs written in ALE
 *
 *     Copyright (C) 2024 - 2026 Lluís Alemany Puig
 *
 * This file is part of the implementation of an interpreter for ALE.
 * The full code is available at:
 *
 *     https://github.com/langale/interpreter
 *
 * This Program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this Program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Contact:
 *
 *     Lluís Alemany Puig
 *     lluis.alemany.puig@gmail.com
 *     https://github.com/lluisalemanypuig
 *
 ********************************************************************/

#pragma once

#include <memory>

#include <ale/logger/Stream.hpp>
#include <intlib/ast/evaluation.hpp>
#include <intlib/memory/Memory.hpp>

namespace intlib {

/**
 * @brief The Program class.
 *
 * This class stores
 * - all the instructions contained in the Program
 * - the internal state of the Program (variables and their values).
 */
class Program {
public:

	Program() noexcept;
	Program(Program& p) noexcept = delete;
	Program(Program&& p) noexcept = default;
	Program& operator= (Program& p) noexcept = delete;
	Program& operator= (Program&& p) noexcept = default;
	~Program() noexcept = default;

	/* MODIFIERS */

	/**
	 * @brief Adds a list of instructions to the Program.
	 * @param node The program node.
	 * @post Ownership of @e n is taken by this class.
	*/
	void set_program_node(std::unique_ptr<ale::ast::Node>&& node) noexcept
	{
		m_program_node = std::move(node);
	}

	/* GETTERS */

	/**
	 * @brief Adds a list of instructions to the Program.
	 * @returns A constant reference to the main node of this program.
	*/
	[[nodiscard]] const std::unique_ptr<ale::ast::Node>&
	get_program_node() noexcept
	{
		return m_program_node;
	}

	[[nodiscard]] const memory::Memory& get_memory() const noexcept
	{
		return m_memory;
	}

	/* OTHER */

	/// Execute this Program.
	void run_program(ale::logger::Stream& os);

	/// Output a Program in a tree (AST) format to an output stream.
	void print_tree(ale::logger::Stream& os) const;

private:

	/// All the instructions in this Program.
	std::unique_ptr<ale::ast::Node> m_program_node;
	/// This Program's memory.
	memory::Memory m_memory;
};

} // namespace intlib
