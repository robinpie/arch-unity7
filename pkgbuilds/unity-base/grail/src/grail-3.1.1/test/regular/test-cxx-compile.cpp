/*
 * Test that compile+link works with a C++ compiler.
 */

#include <stdio.h>
#include <oif/grail.h>

int main(int argc, char **argv) {
  void *dummy = (void*)grail_new;
  if (!dummy) {
    printf("This really should not be happening.\n");
    return 1;
  }
  return 0;
}
