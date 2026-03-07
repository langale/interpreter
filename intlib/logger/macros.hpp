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

#include <filesystem>

#include <ale/logger/Logger.hpp>
#include <ale/logger/AutomaticTab.hpp>

#define INTERPRETER_FILENAME                                                   \
	std::filesystem::relative(__FILE__, INTERPRETER_PROJECT_DIRECTORY).string()

#define INTERPRETER_ENTER_FUNCTION(func)                                       \
	ale::logger::YellowTab ale_tabulator_variable_do_not_use(                  \
		INTERPRETER_FILENAME,                                                  \
		__FUNCTION__,                                                          \
		__LINE__,                                                              \
		{},                                                                    \
		[]<typename... types_t>(                                               \
			std::format_string<types_t...> fmt, types_t&&...args               \
		)                                                                      \
		{                                                                      \
			func(fmt, std::forward<types_t>(args)...);                         \
		}                                                                      \
	)

#define INTERPRETER_FUNC_LOCATION_NAME(func) func##loc

#define INTERPRETER_PRINT_LOC(func, fmt)                                       \
	INTERPRETER_FUNC_LOCATION_NAME(func)(INTERPRETER_FILENAME, __LINE__, fmt)

#define INTERPRETER_PRINT_LOC2(func, fmt, ...)                                 \
	INTERPRETER_FUNC_LOCATION_NAME(func)(                                      \
		INTERPRETER_FILENAME, __LINE__, fmt, __VA_ARGS__                       \
	)

#else

#define INTERPRETER_FILENAME

#define INTERPRETER_ENTER_FUNCTION(func)

#define INTERPRETER_PRINT_LOC(func, fmt)

#define INTERPRETER_PRINT_LOC2(func, fmt, ...)

#endif
