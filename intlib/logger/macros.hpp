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

#if defined ALE_LOGGING_MESSAGES

#include <ale/logger/Logger.hpp>
#include <ale/logger/AutomaticTab.hpp>

#define INTERPRETER_ENTER_AST_FUNCTION                                         \
	ale::logger::GreenTab ale_tabulator_variable_do_not_use(                   \
		ale::logger::SourceLocation(                                           \
			std::source_location::current(), INTERPRETER_PROJECT_DIRECTORY     \
		),                                                                     \
		{},                                                                    \
		[]<typename... types_t>(                                               \
			std::format_string<types_t...> fmt, types_t&&...args               \
		)                                                                      \
		{                                                                      \
			ale::logger::println(fmt, std::forward<types_t>(args)...);         \
		}                                                                      \
	)

#define INTERPRETER_ENTER_ARITHMETIC_FUNCTION                                  \
	ale::logger::RedTab ale_tabulator_variable_do_not_use(                     \
		ale::logger::SourceLocation(                                           \
			std::source_location::current(), INTERPRETER_PROJECT_DIRECTORY     \
		),                                                                     \
		{},                                                                    \
		[]<typename... types_t>(                                               \
			std::format_string<types_t...> fmt, types_t&&...args               \
		)                                                                      \
		{                                                                      \
			ale::logger::println(fmt, std::forward<types_t>(args)...);         \
		}                                                                      \
	)

#define INTERPRETER_ENTER_COMPARISON_FUNCTION                                  \
	ale::logger::RedTab ale_tabulator_variable_do_not_use(                     \
		ale::logger::SourceLocation(                                           \
			std::source_location::current(), INTERPRETER_PROJECT_DIRECTORY     \
		),                                                                     \
		{},                                                                    \
		[]<typename... types_t>(                                               \
			std::format_string<types_t...> fmt, types_t&&...args               \
		)                                                                      \
		{                                                                      \
			ale::logger::println(fmt, std::forward<types_t>(args)...);         \
		}                                                                      \
	)

#define INTERPRETER_ENTER_MEMORY_FUNCTION                                      \
	ale::logger::PurpleTab ale_tabulator_variable_do_not_use(                  \
		ale::logger::SourceLocation(                                           \
			std::source_location::current(), INTERPRETER_PROJECT_DIRECTORY     \
		),                                                                     \
		{},                                                                    \
		[]<typename... types_t>(                                               \
			std::format_string<types_t...> fmt, types_t&&...args               \
		)                                                                      \
		{                                                                      \
			ale::logger::println(fmt, std::forward<types_t>(args)...);         \
		}                                                                      \
	)

#define INTERPRETER_ENTER_DETAIL_FUNCTION                                      \
	ale::logger::YellowTab ale_tabulator_variable_do_not_use(                  \
		ale::logger::SourceLocation(                                           \
			std::source_location::current(), INTERPRETER_PROJECT_DIRECTORY     \
		),                                                                     \
		{},                                                                    \
		[]<typename... types_t>(                                               \
			std::format_string<types_t...> fmt, types_t&&...args               \
		)                                                                      \
		{                                                                      \
			ale::logger::println(fmt, std::forward<types_t>(args)...);         \
		}                                                                      \
	)

#define INTERPRETER_PRINT(fmt, ...)                                            \
	ale::logger::printlnloc(                                                   \
		ale::logger::SourceLocation(                                           \
			std::source_location::current(), INTERPRETER_PROJECT_DIRECTORY     \
		),                                                                     \
		fmt __VA_OPT__(, ) __VA_ARGS__                                         \
	)

#else

#define INTERPRETER_ENTER_AST_FUNCTION

#define INTERPRETER_ENTER_ARITHMETIC_FUNCTION

#define INTERPRETER_ENTER_COMPARISON_FUNCTION

#define INTERPRETER_ENTER_MEMORY_FUNCTION

#define INTERPRETER_ENTER_DETAIL_FUNCTION

#define INTERPRETER_PRINT(fmt, ...)

#endif
