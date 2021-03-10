#include "prettyerr.h"
#include <stdio.h>

#ifdef __attribute__
#undef __attribute__
#endif
#ifndef __GNUC__
#define __attribute__(...) /* nothing */
#endif

#define NUSED __attribute__((unused))

#define BOLD(a) "\033[1m" a "\033[0m"
const char src[] = "int main() {\n  retrn 42;\n}";

int main(int argc NUSED, const char* argv[] NUSED) {
    // Initialize the error printer
    perr_printer_t printer;
    perr_printer_init(
        &printer,
        stderr /* our stream is standard error */,
        src /* source code */,
        true /* use utf8 */,
        true /* basic style*/
    );

    // Create a faux error
    /*const */perr_t err =
        PERR_Error(
            PERR_ERROR /* error */,
            PERR_Str(2, src + 15) /* location of error */,
            PERR_Pos(15, 5) /* occurs at src[15] through src[19] */,
            PERR_Str_None() /* no other line referenced */,
            "Unknown identifier `{+/}retrn{0}`" /* main error message */,
            "{_R}Error{0} right here" /* example subsidiary error message */,
            "Did you mean `{+/}return{0}`?" /* fix message */,
            "faux.c" /* filename */
        );

    // Display our error
    perr_print_error(&printer, &err);
    err.type = PERR_WARNING;
    perr_print_error(&printer, &err);
    err.type = PERR_INFO;
    perr_print_error(&printer, &err);
    err.type = PERR_NOTE;
    perr_print_error(&printer, &err);
    err.type = PERR_OK;
    perr_print_error(&printer, &err);

    return 0;
}
