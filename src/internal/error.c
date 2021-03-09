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

#define _PRINTF(...) fprintf(printer->stream, __VA_ARGS__)
#define _PUTCHR(...) fputc(__VA_ARGS__, printer->stream)

static const char* _errtype_lookup[5] = {
    "error",
    "warning",
    "info",
    "note",
    "ok"
};

void perr_printer_init(perr_printer_t* printer, FILE* stream,
                       const char* source, bool utf8, bool basic_style) {
    printer->source = source;
    printer->stream = stream;

    // Enables ANSI colors only in Unix terminals. False by default on Windows.
    #ifdef __unix__
    const int fd = fileno(stream);
    // if (fd < 0) {
    //     perror("fileno");
    // }
    printer->color = isatty(fd);
    #else
    printer->color = false;
    #endif

    printer->utf8 = utf8;
    printer->basic_style = basic_style;
}

static inline void perr_print_basic_style(const perr_printer_t* printer,
                                          const perr_t* err) {
    // Normal errors:
    //
    // $filename:$line:$col: $type: $msg
    //  $line | <long line here>
    //        |       ^^^^

    // Here we scan backwards until we reach the start of the line (or the
    // start of the source code). This allows us to retrieve the line that has
    // the error.
    size_t idx_cpy = err->error_position.index;
    while (idx_cpy > 0 && printer->source[idx_cpy - 1] != '\n') {
        idx_cpy--;
    }
    const char* error_line = (printer->source + idx_cpy);
    error_line += (printer->source[idx_cpy] == '\n') ? 1 : 0;

    // The column is how far the error's index is from the start-of-line's
    // index.
    const size_t column = err->error_position.index - idx_cpy;

    // Here we print the first row of the error message which provides general
    // information such as filename, line, column, error type and a message.
    _PRINTF("%s:%zu:%zu: %s: %s\n", err->filename, err->primary.line, column,
            _errtype_lookup[err->type], err->main);

    // Print the line number and a | to denote the start of the line. When
    // colors are added, this segment should be dimmed.
    _PRINTF("%5zu | ", err->primary.line);

    // Print the line.
    while (*error_line && *error_line != '\n') {
        _PUTCHR(*error_line);
        error_line++;
    }
    _PUTCHR('\n');

    // Print a series of '^' showing where the error occurs.
    _PRINTF("      | %*s", (int)column, "");
    for (size_t i = 0; i < err->error_position.length; i++) {
        // Should be red when color is added.
        _PUTCHR('^');
    }
    _PUTCHR('\n');

    // Adds a subsidiary error note, if applicable
    if (err->sub) {
        _PRINTF("      | %*s|\n", (int)column, "");
        _PRINTF("      | %*s%s", (int)column, "", err->sub);
        _PUTCHR('\n');
    }

    // Displays a fix, if applicable.
    if (err->fix) {
        _PRINTF("%s\n", err->fix);
    }
}

/*
static inline void perr_print_complex(const perr_printer_t* printer,
                                      const perr_t* err) {
    // IDK COPY BRENDANZAB (DONT)
}
*/

void perr_print_error(const perr_printer_t* printer, const perr_t* err) {
    if (printer->basic_style) {
        perr_print_basic_style(printer, err);
    } else {
        // perr_print_complex(printer, err);
    }
}
