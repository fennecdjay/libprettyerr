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
#include <termcolor.h>

// Used to declare isatty if on Unix
#if defined(__unix__) || defined(__unix) || \
    (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>
#endif

#define _PRINTF(...) tcol_fprintf(printer->stream, __VA_ARGS__)
#define _PUTCHR(...) fputc(__VA_ARGS__, printer->stream)

// attributes
#define CLEAR "\e[0m"
#define BOLD  "\e[1m"

// colors
#define WHITE   "\e[37m"
#define RED     "\e[31m"
#define YELLOW  "\e[33m"
#define GREEN   "\e[32m"
#define MAGENTA "\e[35m"

static const char* _color_errtype_lookup[5] = {
    RED BOLD,
    MAGENTA BOLD,
    WHITE BOLD,
    YELLOW BOLD,
    GREEN BOLD
};

static char const* _bw_errtype_lookup[5] = { "", "", "", "", "" };

static char const* _ascii_box_lookup[4] = { "|", "|", "+", "-" };
static char const* _utf8_box_lookup[4] = { "┃", "╵", "┌", "─" };

static const char* _errtype_lookup[5] = {
    "error",
    "warning",
    "info",
    "note",
    "ok"
};

static void perr_theme_init(perr_printer_t* printer) {
    if(printer->color) {
        printer->theme.color_lookup = _color_errtype_lookup;
    } else {
        printer->theme.color_lookup = _bw_errtype_lookup;
    }
    if(printer->utf8) {
        printer->theme.box_lookup = _utf8_box_lookup;
    } else {
        printer->theme.box_lookup = _ascii_box_lookup;
    }
}

void perr_printer_init(perr_printer_t* printer, FILE* stream,
                       const char* source, bool utf8, bool basic_style) {
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

    printer->utf8 = utf8;
    printer->basic_style = basic_style;
    perr_theme_init(printer);
}

static void perr_print_column(const perr_printer_t* printer, const char *color, const int column) {
    _PRINTF("      %s%s{0} %*s", color, printer->theme.box_lookup[0],
        (int)column, "");
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
    const perr_theme_t theme = printer->theme;
    const char *color = theme.color_lookup[err->type];
//    const char base_color[16];
//    sprintf(base_color, "{+%s}"
//    char color[16];

//int tcol_color_parse(char* dst, size_t dstn, char color[16], size_t k,
//                     size_t* len)
    // Here we print the first row of the error message which provides general
    // information such as filename, line, column, error type and a message.
    _PRINTF("{+W}%s{0}:%zu:%zu: %s%s{0}: ", err->filename, err->primary.line, column,
            color, _errtype_lookup[err->type]);
    _PRINTF(err->main);
    _PUTCHR('\n');

    // Print the line number and a | to denote the start of the line.
    _PRINTF("{-}%5zu{0} %s%s{0} ", err->primary.line, color, theme.box_lookup[0]);

    // Print the line.
    char* nl = strstr(error_line, "\n");
    ptrdiff_t len = nl ? nl - error_line - 1: strlen(error_line);
    _PRINTF("%.*s\n", len, error_line);
//    while (*error_line && *error_line != '\n') {
//        _PUTCHR(*error_line);
//        error_line++;
//    }
//    _PUTCHR('\n');

    // Print a series of '^' showing where the error occurs.
    perr_print_column(printer, color, column);
    _PRINTF("%s{-}%s", color, theme.box_lookup[2]);
    for (size_t i = 1; i < err->error_position.length; i++) {
        _PRINTF(theme.box_lookup[3]);
    }
    _PRINTF("{0}\n");

    // Adds a subsidiary error note, if applicable
    if (err->sub) {
        perr_print_column(printer, color, column);
        _PRINTF("{-}%s%s{0}\n", color, theme.box_lookup[1]);
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

void perr_print_error(const perr_printer_t* printer, const perr_t* err) {
    if (printer->basic_style) {
        perr_print_basic_style(printer, err);
    } else {
        // perr_print_complex(printer, err);
    }
}
