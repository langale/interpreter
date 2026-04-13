/*********************************************************************
 *
 * ALE interpreter -- a command line utility to run programs written in ALE
 * Copyright (C) 2024 Lluís Alemany Puig
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

#include <iostream>
#include <print>

#include <ale/lexer/lexer.hpp>
#include <ale/lexer/utils/token_type_to_string.hpp>
#include <ale/lexer/utils/tokenize_error_to_string.hpp>
#include <ale/parser/utils/ParseResult.hpp>
#include <ale/parser/utils/error_to_string.hpp>
#include <ale/parser/utils/rule_to_string.hpp>
#include <ale/parser/Parser.hpp>
#include <ale/logger/Logger.hpp>

#include <intlib/Program.hpp>

using TokenType = ale::lexer::TokenType;
using Token = ale::lexer::Token;

int main(int argc, char *argv[])
{
	if (argc == 1) {
		std::cout << "./interpreter filename\n";
		return 0;
	}

	std::string filename(argv[1]);
	const ale::lexer::ReadResult read_res = ale::lexer::read_file(filename);

	if (not read_res) {
		const auto& err = read_res.error();
		std::println("File '{}' could not be opened.", filename);
		std::println("    Error: {}.", err.error);
		return 1;
	}

	auto& l = ale::logger::get_instance();
	l.use_terminal_only();

	const ale::lexer::SuccessfulRead& read = *read_res;

	ale::lexer::TokenizeResult tokenize_res = ale::lexer::tokenize(read);
	if (not tokenize_res) {
		const auto& [error, line, character, unmatched_text] =
			tokenize_res.error();

		std::println("Program could not be tokenized.");
		std::println("    Error:          {}.", error);
		std::println("    At line:        {}.", line);
		std::println("    At character:   {}.", character);
		std::println("    Next substring: {}.", unmatched_text);
		return 1;
	}

	ale::lexer::TokenVector& tokvec = *tokenize_res;

	ale::parser::Parser parser(tokvec);
	ale::parser::ParseResult parse_res = parser.parse_program({.i = 0});

	if (ale::parser::result_is_match_error(parse_res)) {
		l.force_flush();

		const auto& [rules, errors, messages, program_token_numbers] =
			std::get<ale::parser::MatchError>(parse_res);

		std::println("Program could not be parsed.");

		size_t num_errors = errors.size();
		for (size_t i = 0; i < num_errors; ++i) {
			std::println("Error #{}.", i);
			std::println("    Type: {}.", errors[i]);
			std::println("    Message: {}.", messages[i]);
			std::println("    At rule: {}.", rules[i]);
			std::println("    At token number: {}.", program_token_numbers[i]);
			std::println("        Which is token:  {}.", tokvec[TokenType{i}]);
			std::println("        Whose string is: {}.", tokvec[Token{i}]);
		}
		return 1;
	}

	auto m = std::move(std::get<ale::parser::MatchedRule>(parse_res));

	intlib::Program p;
	p.set_program_node(std::move(m.node));

	std::cout << "---------------\n";
	std::cout << "Program's evaluation:\n";

	p.run_program();
}
