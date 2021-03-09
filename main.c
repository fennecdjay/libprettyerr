#include "prettyerr.h"
#include <stdio.h>

const char src[] = "int main() {\n    retrn 42;\n}";

int main(int argc, const char* argv[])
    // Initialize the error printer
    perr_printer_t printer;
    perr_printer_init(
        &printer,
        src /* source code */,
        false /* no utf8 */,
        true /* basic style*/
    );

    // Create a faux error
    const perr_t err =
        PERR_Error(
            PERR_ERROR /* error */,
            PERR_Str(2, src + 17) /* location of error */,
            PERR_Pos(17, 5) /* occurs at src[17] through src[21] */,
            PERR_Str_None() /* no other line referenced */,
            "Unknown identifier 'retrn'" /* main error message */,
            NULL /* no secondary error message */,
            "Did you mean 'return'?" /* fix message */,
            "faux.c" /* filename */
        );

    // Display our error
    perr_print_error(stderr, &printer, err);

    return 0;
}
