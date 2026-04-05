/**
 * test driver for unit testing of geis v2.0 internals
 */
#include <check.h>

#define LOGFILE_PREFIX "geis2_internals"

extern Suite *make_attr_suite();
extern Suite *make_backend_event_posting_suite();
extern Suite *make_backend_multiplexor_suite();
extern Suite *make_backend_token_suite();
extern Suite *make_device_suite();
extern Suite *make_error_reporting_suite();
extern Suite *make_event_queue_suite();
extern Suite *make_filter_suite();
extern Suite *make_filter_term_suite();
extern Suite *make_gesture_class_suite();
extern Suite *make_region_suite();
extern Suite *make_select_devices_suite();
extern Suite *make_subscription_suite();
extern Suite *make_timer_suite();


int
main(int argc CK_ATTRIBUTE_UNUSED, char* argv[] CK_ATTRIBUTE_UNUSED)
{
  int num_failed = 0;

  Suite *s = suite_create("\"GEIS v2.0 internals\"");

  SRunner *sr = srunner_create(s);
  srunner_add_suite(sr, make_error_reporting_suite());
  srunner_add_suite(sr, make_attr_suite());
  srunner_add_suite(sr, make_event_queue_suite());
  srunner_add_suite(sr, make_backend_multiplexor_suite());
  srunner_add_suite(sr, make_backend_token_suite());
  srunner_add_suite(sr, make_backend_event_posting_suite());
  srunner_add_suite(sr, make_device_suite());
  srunner_add_suite(sr, make_gesture_class_suite());
  srunner_add_suite(sr, make_region_suite());
  srunner_add_suite(sr, make_filter_suite());
  srunner_add_suite(sr, make_filter_term_suite());
  srunner_add_suite(sr, make_select_devices_suite());
  srunner_add_suite(sr, make_subscription_suite());
  srunner_add_suite(sr, make_timer_suite());

  srunner_set_log(sr, LOGFILE_PREFIX".log");
  srunner_set_xml(sr, LOGFILE_PREFIX".xml");
  srunner_run_all(sr, CK_NORMAL);
  num_failed = srunner_ntests_failed(sr);

  srunner_free(sr);

  return !(num_failed == 0);
}
