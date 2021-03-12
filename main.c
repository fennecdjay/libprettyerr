#include "prettyerr.h"
#include <stdio.h>

#ifdef __attribute__
#undef __attribute__
#endif
#ifndef __GNUC__
#define __attribute__(...) /* nothing */
#endif

#define NUSED __attribute__((unused))

const char src[] = "int main() {\n  42 => const int owo;\n 12 => owo;\n}";

int main(int argc NUSED, const char* argv[] NUSED) {
    // Initialize the error printer
    perr_printer_t printer;
    perr_printer_init(
        &printer,
        stderr /* our stream is standard error */,
        src /* source code */,
        true /* use utf8 */,
        perr_runner_basic_style /* basic style*/
    );

    // Create a faux error
    const perr_t err =
        PERR_Error(
            PERR_ERROR /* error */,
            PERR_Str(3, src + 25) /* location of error */,
            PERR_Pos(37, 9) /* occurs at src[37] through src[35] */,
            "Invalid assignement" /* main error message */,
            "`{+/}owo{0}` is immutable" /* example explanatory error message */,
            "Take a reference to `{+/}owo{0}{-}` and mutate that 😄" /* fix message */,
            42, /* error code */
            "faux.c" /* filename */
        );

    const perr_t sec =
        PERR_Secondary(
            PERR_WARNING /* error */,
            PERR_Str(2, src + 15) /* location of error */,
            PERR_Pos(21, 13) /* occurs at src[21] through src[33] */,
            "`{+/}owo{0}` declared `{+/}const{0}` here" /* main error message */,
            "faux.c" /* filename */
        );

    // Display our error
    perr_print_error(&printer, &err);
    printer.runner = perr_runner_secondary_style;
    perr_print_error(&printer, &sec);

    return 0;
}
