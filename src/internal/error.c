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

#include <string.h>
#include <ctype.h>
#include <termcolor.h>
#include "prettyerr.h"

// Used to declare isatty if on Unix
#if defined(__unix__) || defined(__unix) || \
    (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>
#endif

#define _PRINTF(...) tcol_fprintf(printer->stream, __VA_ARGS__)
#define _PUTCHR(...) fputc(__VA_ARGS__, printer->stream)

static char const _tcol_lookup[5] = { 'R', 'M', 'W', 'Y', 'G' };

static char const* _ascii_box_lookup[8] = { "|", "|", "+", "-", "+", "|"};
static char const* _utf8_box_lookup[8] = { "┃", "╵", "┌", "─", "└", "│"};

static const char* _errtype_lookup[5] = {
    "error",
    "warning",
    "info",
    "note",
    "success"
};

void perr_printer_init(perr_printer_t* printer, FILE* stream,
                       const char* source, bool utf8, perr_runner_t style) {
    printer->source = source;
    printer->stream = stream;

    // Enables ANSI colors only in Unix terminals. False by default on Windows.
    #if defined(__unix__) || defined(__unix) || \
        (defined(__APPLE__) && defined(__MACH__))
    const int fd = fileno(stream);
    // if (fd < 0) {
    //     perror("fileno");
    // }
    printer->color = isatty(fd);
    #else
    printer->color = false;
    #endif

    if((printer->utf8 = utf8)) {
        printer->box_lookup = _utf8_box_lookup;
    } else {
        printer->box_lookup = _ascii_box_lookup;
    }
    printer->runner = style;
}

static void perr_print_column(const perr_printer_t* printer, const char *color, const int column) {
    _PRINTF("      %s%s{0} %*s", color, printer->box_lookup[PERR_BOX_THICK_VERT],
        (int)column, "");
}


static inline void _perr_print_filename(const perr_printer_t* printer, const perr_t* err, const int column) {
    _PRINTF("{+W}%s{0}:%zu:%zu: ", err->filename, err->primary.line, column);
}

static void _perr_print_error(const perr_printer_t* printer, const perr_t* err, const char *color) {
    _PRINTF("%s%s", color, _errtype_lookup[err->type]);
    if(err->error_code)
        _PRINTF("[%c%04d]", toupper(_errtype_lookup[err->type][0]), err->error_code);
    _PRINTF("{0}: ");
}

// Print the line number and a | to denote the start of the line.
static void _perr_print_line_number(const perr_printer_t* printer, const perr_t* err, const char *color) {
    _PRINTF("{-}%5zu{0} %s%s{0} ", err->primary.line, color, printer->box_lookup[PERR_BOX_THICK_VERT]);
}

// Print the line.
static void _perr_print_offending_line(const perr_printer_t* printer, const char *error_line) {
    char* nl = strstr(error_line, "\n");
    ptrdiff_t nl_index = nl ? nl - error_line : (ptrdiff_t)strlen(error_line);
    _PRINTF("%.*s\n", nl_index, error_line);
}

// Print a series of '^' showing where the error occurs.
static inline void _perr_print_highlight_error(const perr_printer_t* printer, const perr_t* err, const char *color,
            const int column) {
     perr_print_column(printer, color, column);
     _PRINTF("%s{-}%s", color, printer->box_lookup[PERR_BOX_THIN_UL]);
     for (size_t i = 1; i < err->error_position.length; i++) {
         _PRINTF(printer->box_lookup[PERR_BOX_THIN_HORIZ]);
     }
     _PRINTF("{0}\n");
}

static void lookup_color(char *color, enum libprettyerr_errtype type) {
    char _color[3];
    sprintf(_color, "+%c", _tcol_lookup[type]);
    size_t len;
    int status = tcol_color_parse(color, 16, _color, sizeof(_color) -1, &len);
    if (status != TermColorErrorNone) {
       color[0] = 0;
      // error
    } else
    color[len] = 0;
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

    char color[17];
    lookup_color(color, err->type);

    // Here we print the first row of the error message which provides general
    // information such as filename, line, column, error type and a message.
    _perr_print_filename(printer, err, column);
    _perr_print_error(printer, err, color);
    _PRINTF(err->main);
    _PUTCHR('\n');

    _perr_print_line_number(printer, err, color);
    _perr_print_offending_line(printer, error_line);
    _perr_print_highlight_error(printer, err, color, column);

    // Adds a subsidiary error note, if applicable
    if (err->sub) {
        perr_print_column(printer, color, column);
        _PRINTF("{-}%s%s{0}\n", color,
           printer->box_lookup[PERR_BOX_THIN_HIGH]);
        perr_print_column(printer, color, column);
        _PRINTF(err->sub);
        _PUTCHR('\n');
    }

    // Displays a fix, if applicable.
    if (err->fix) {
        _PRINTF("{-}");
        _PRINTF(err->fix);
        _PRINTF("{0}\n");
    }
}

/*
static inline void perr_print_complex(const perr_printer_t* printer,
                                      const perr_t* err) {
    // IDK COPY BRENDANZAB (DONT)
}
*/

static inline void perr_print_secondary_style(const perr_printer_t* printer,
                                          const perr_t* err) {
    size_t idx_cpy = err->error_position.index;
    while (idx_cpy > 0 && printer->source[idx_cpy - 1] != '\n') {
        idx_cpy--;
    }
    const char* error_line = (printer->source + idx_cpy);
    error_line += (printer->source[idx_cpy] == '\n') ? 1 : 0;

    // The column is how far the error's index is from the start-of-line's
    // index.
    const size_t column = err->error_position.index - idx_cpy;

    char color[17];
    lookup_color(color, err->type);

    _perr_print_filename(printer, err, column);
    _PUTCHR('\n');

    _perr_print_line_number(printer, err, color);
    _perr_print_offending_line(printer, error_line);
    _perr_print_highlight_error(printer, err, color, column);

    perr_print_column(printer, color, column);
    _PRINTF("{-}%s%s{0}\n", color,
        printer->box_lookup[PERR_BOX_THIN_VERT]);
    perr_print_column(printer, color, column);
    _PRINTF("{-}%s%s%.*s{0} ", color, printer->box_lookup[4], 3, printer->box_lookup[3]);
    _PRINTF(err->main);
    _PUTCHR('\n');
}

void perr_print_error(const perr_printer_t* printer, const perr_t* err) {
   printer->runner(printer, err);
}

perr_runner_t perr_runner_basic_style = perr_print_basic_style;
perr_runner_t perr_runner_secondary_style = perr_print_secondary_style;
