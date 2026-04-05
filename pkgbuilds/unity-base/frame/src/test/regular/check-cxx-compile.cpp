/*
 * Test compiling and linking with a C++ compiler.
 */

#include <oif/frame_x11.h>

int main(int argc, char **argv) {
  UFWindowId id __attribute__((unused)) = frame_x11_create_window_id(0);
  return 0;
}
