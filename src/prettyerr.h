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

#ifndef _LIBPRETTYERR_PRETTYERR_H
#define _LIBPRETTYERR_PRETTYERR_H

#include "internal/error.h"

typedef struct libprettyerr_error   perr_t;
typedef struct libprettyerr_printer perr_printer_t;
typedef libprettyerr_runner_t       perr_runner_t;
extern perr_runner_t perr_runner_basic_style;
extern perr_runner_t perr_runner_secondary_style;
#endif /* _LIBPRETTYERR_PRETTYERR_H */
