/*
 * A small application that tests that compiling + linking works
 * on a C++ compiler.
 */

#include <geis/geis.h>
#include <stdio.h>

#include "geis_config.h"
#include "geis_test_api.h"


int main(int argc GEIS_UNUSED, char **argv GEIS_UNUSED) {
    Geis g = geis_new(GEIS_INIT_MOCK_BACKEND, NULL);
    if(!g) {
        printf("This really should not be happening.\n");
        return 1;
    }
    return 0;
}
