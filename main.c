#include "prettyerr.h"
#include <stdio.h>

#ifdef __attribute__
#undef __attribute__
#endif
#ifndef __GNUC__
#define __attribute__(...) /* nothing */
#endif

#define NUSED __attribute__((unused))

const char src[] = "int main() {\n  retrn 42;\n}";

int main(int argc NUSED, const char* argv[] NUSED) {
    // Initialize the error printer
    perr_printer_t printer;
    perr_printer_init(
        &printer,
        stderr /* our stream is standard error */,
        src /* source code */,
        false /* no utf8 */,
        true /* basic style*/
    );

    // Create a faux error
    const perr_t err =
        PERR_Error(
            PERR_ERROR /* error */,
            PERR_Str(2, src + 15) /* location of error */,
            PERR_Pos(15, 5) /* occurs at src[15] through src[19] */,
            PERR_Str_None() /* no other line referenced */,
            "Unknown identifier 'retrn'" /* main error message */,
            "Error right here" /* example subsidiary error message */,
            "Did you mean 'return'?" /* fix message */,
            "faux.c" /* filename */
        );

    // Display our error
    perr_print_error(&printer, &err);

    return 0;
}
