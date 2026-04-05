/* Simple test to verify that an impossible type passed into a
 * frame_*_get_property() function does not compile. Due to the nature of
 * compile tests, we do not test all combinations of types and functions. This
 * test is merely to check that the improper type mechanism is working at all.
 */

#include <oif/frame.h>

#ifdef __has_extension
#if __has_extension(c_generic_selections)
#define HAS_C_GENERIC_SELECTIONS
#endif // __has_extension(c_generic_selections)
#endif // __has_extension

int main() {

#ifdef HAS_C_GENERIC_SELECTIONS
  UFStatus status;
  UFTouch touch;
  char test;

  /* No touch properties are of type 'char', this should fail to compile */
  status = frame_touch_get_property(touch, UFTouchPropertyId, &test);
#else
#error PASS: C compiler does not support generic selections
#endif

  return 0;
}
