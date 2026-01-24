/*********************************************************************
 *
 * ALE interpreter -- a command line utility to run programs written in ALE
 * Copyright (C) 2024 Lluís Alemany Puig
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

// C++ includes
#include <iostream>
#include <optional>

// ale includes
#include <ale/lexer/lexer.hpp>
#include <ale/parser/parser.hpp>

// interpreter includes
#include <intlib/program.hpp>

int main(int argc, char *argv[]) {
	if (argc == 1) {
		std::cout << "./interpreter filename\n";
		return 0;
	}

	std::optional<ale::lexer::token_vector> f =
		ale::lexer::read_file(argv[1]);

	if (not f.has_value()) {
		std::cerr << "File could not be opened.\n";
		return 1;
	}

	auto& l = ale::logger::get_instance();
	l.use_terminal_stream();

	std::cout << "File contents: '" << f->get_all_chars() << "'\n";
	const bool res = ale::lexer::tokenize(*f);
	if (not res) {
		std::cerr << "Tokenization failed.\n";
		return 1;
	}

	std::cout << "Parsing program...\n";
	std::optional<std::vector<std::unique_ptr<ale::ast::node>>> instructions =
		ale::parser::parse_program(*f);

	if (not instructions) {
		std::cerr << "Program could not be parsed.\n";
		l.force_flush();
		return 1;
	}

	interpreter::program p;
	p.add_instructions(std::move(*instructions));

	std::cout << "---------------\n";
	std::cout << "Program's evaluation:\n";
	p.run_program(ale::output());

	p.clear();
}
