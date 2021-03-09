// libprettyerr: error.h
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

#ifndef _LIBPRETTYERR_ERROR_H
#define _LIBPRETTYERR_ERROR_H

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

struct libprettyerr_str {
    size_t line;
    const char* start;
};

#define PERR_Str(line_, start_) \
    (struct libprettyerr_str){ .line = line_, .start = start_ }
#define PERR_Str_None() \
    PERR_Str(0, NULL)

struct libprettyerr_pos {
    size_t index;
    size_t length;
};

#define PERR_Pos(index_, length_) \
    (struct libprettyerr_pos){ .index = index_, .length = length_ }

enum libprettyerr_errtype {
    PERR_ERROR,   // bold red
    PERR_WARNING, // red
    PERR_INFO,    // bold white
    PERR_NOTE,    // bold yellow
    PERR_OK       // bold green
};

struct libprettyerr_error {
    enum libprettyerr_errtype type;

    struct libprettyerr_str primary;
    struct libprettyerr_pos error_position;
    struct libprettyerr_str secondary;

    const char* main;
    const char* sub;
    const char* fix;

    const char* filename;
};

#define PERR_Error(type_, primary_, error_position_, secondary_, main_, sub_, fix_, filename_) \
    (struct libprettyerr_error){ \
        .type = type_, .primary = primary_, .error_position = error_position_, \
        .secondary = secondary_, .main = main_, .sub = sub_, .fix = fix_, \
        .filename = filename_ \
    }

struct libprettyerr_printer {
    const char* source;
    FILE* stream;
    bool color;
    bool utf8;
    bool basic_style;
};

// Initializes a printer
void perr_printer_init(struct libprettyerr_printer* printer, FILE* stream,
                       const char* source, bool utf8, bool basic_style);

// Uses the printer to display the provided error in
void perr_print_error(const struct libprettyerr_printer* printer,
                      const  struct libprettyerr_error* err);

#endif /* _LIBPRETTYERR_ERROR_H */
