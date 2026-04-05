/**
 * test driver for unit testing of geis internals
 */
#include <check.h>


extern Suite *make_version_macro_suite();
extern Suite *geis2_general_types_suite_new();
extern Suite *geis2_error_codes_suite_new();
extern Suite *geis2_device_suite_new();
extern Suite *geis2_gesture_class_suite_new();
extern Suite *geis2_geis_new_suite_new();
extern Suite *geis2_config_suite_new();
extern Suite *geis2_attr_suite_new();
extern Suite *geis2_region_suite_new();
extern Suite *geis2_filter_suite_new();
extern Suite *geis2_subscription_suite_new();
extern Suite *geis2_event_suite_new();
extern Suite *geis2_gesture_frame_suite_new();

int
main(int argc CK_ATTRIBUTE_UNUSED, char* argv[] CK_ATTRIBUTE_UNUSED)
{
  int num_failed = 0;

  Suite *s = suite_create("\"GEIS v2.0 API\"");

  SRunner *sr = srunner_create(s);
  srunner_add_suite(sr, make_version_macro_suite());
  srunner_add_suite(sr, geis2_general_types_suite_new());
  srunner_add_suite(sr, geis2_error_codes_suite_new());
  srunner_add_suite(sr, geis2_geis_new_suite_new());
  srunner_add_suite(sr, geis2_config_suite_new());
  srunner_add_suite(sr, geis2_attr_suite_new()); 
  srunner_add_suite(sr, geis2_device_suite_new()); 
  srunner_add_suite(sr, geis2_gesture_class_suite_new()); 
  srunner_add_suite(sr, geis2_region_suite_new()); 
  srunner_add_suite(sr, geis2_filter_suite_new()); 
  srunner_add_suite(sr, geis2_subscription_suite_new()); 
  srunner_add_suite(sr, geis2_event_suite_new()); 
  srunner_add_suite(sr, geis2_gesture_frame_suite_new()); 

  srunner_set_log(sr, "geis2_api.log");
  srunner_set_xml(sr, "geis2_api.xml");
  srunner_run_all(sr, CK_NORMAL);
  num_failed = srunner_ntests_failed(sr);

  srunner_free(sr);

  return !(num_failed == 0);
}
