// libprettyerr: error.c
// Copyright (C) 2021 Ethan Uppal
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#include "prettyerr.h"
// #include <termcolor/print.h>

// Used to declare isatty if on Unix
#ifdef __unix__
#include <unistd.h>
#endif

#define _PRINTF(fmt, ...) fprintf(stream, fmt, __VA_ARGS__);

static const char* _errtype_lookup[5] = {
    "error",
    "warning",
    "info",
    "note",
    "ok"
};

void perr_printer_init(struct libprettyerr_printer* printer,
                       const char* source, bool utf8, bool basic_style) {
    printer->source = source;

    // Enables ANSI colors only in Unix terminals. False by default on Windows.
    #ifdef __unix__
    printer->color = isatty(FILENO_STDERR);
    #else
    printer->color = false;
    #endif

    printer->utf8 = utf8;
    printer->basic_style = basic_style;
}

static inline void perr_print_basic_style(FILE* stream, perr_printer_t* printer,
                                          perr_t* err) {
    // Normal errors:
    //
    // $filename:$line:$col: $type: $msg
    //  $line | <long line here>
    //        |       ^^^^

    // Here we scan backwards until we reach the start of the line (or the
    // start of the source code). This allows us to retrieve the line that has
    // the error,
    size_t idx_cpy = err->error_position.index;
    while (idx_cpy > 0 && printer->source[idx_cpy - 1] != '\n') {
        idx_cpy--;
    }
    const char* const error_line = printer->source + idx_cpy;

    // The column is how far the error's index is from the start-of-line's index
    const size_t column = err->error_position.index - idx_cpy;

    // Here we print the first row of the error message which provides general
    // information such as filename, line, column, error type and a message.
    _PRINTF("%s:%zu:%zu: %s: %s\n", err->filename, err->primary.line, column,
            _errtype_lookup[err->type], err->main);
}

static inline void perr_print_complex(FILE* stream, perr_printer_t* printer,
                                      perr_t* err) {
    // IDK COPY BRENDANZAB (DONT)
}

void perr_print_error(FILE* stream, perr_printer_t* printer, perr_t* err) {
    if (printer->basic_style) {
        perr_print_basic_style(stream, printer, err);
    } else {

    }
}
