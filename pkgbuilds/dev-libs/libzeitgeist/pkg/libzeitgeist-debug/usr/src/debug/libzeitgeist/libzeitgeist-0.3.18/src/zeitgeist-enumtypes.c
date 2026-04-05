
/* Generated data (by glib-mkenums) */

#include "zeitgeist-enums.h"

/* enumerations from "zeitgeist-enums.h" */
GType
zeitgeist_result_type_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if (g_once_init_enter (&g_define_type_id__volatile))
    {
      static const GEnumValue values[] = {
        { ZEITGEIST_RESULT_TYPE_MOST_RECENT_EVENTS, "ZEITGEIST_RESULT_TYPE_MOST_RECENT_EVENTS", "most-recent-events" },
        { ZEITGEIST_RESULT_TYPE_LEAST_RECENT_EVENTS, "ZEITGEIST_RESULT_TYPE_LEAST_RECENT_EVENTS", "least-recent-events" },
        { ZEITGEIST_RESULT_TYPE_MOST_RECENT_SUBJECTS, "ZEITGEIST_RESULT_TYPE_MOST_RECENT_SUBJECTS", "most-recent-subjects" },
        { ZEITGEIST_RESULT_TYPE_LEAST_RECENT_SUBJECTS, "ZEITGEIST_RESULT_TYPE_LEAST_RECENT_SUBJECTS", "least-recent-subjects" },
        { ZEITGEIST_RESULT_TYPE_MOST_POPULAR_SUBJECTS, "ZEITGEIST_RESULT_TYPE_MOST_POPULAR_SUBJECTS", "most-popular-subjects" },
        { ZEITGEIST_RESULT_TYPE_LEAST_POPULAR_SUBJECTS, "ZEITGEIST_RESULT_TYPE_LEAST_POPULAR_SUBJECTS", "least-popular-subjects" },
        { ZEITGEIST_RESULT_TYPE_MOST_POPULAR_ACTOR, "ZEITGEIST_RESULT_TYPE_MOST_POPULAR_ACTOR", "most-popular-actor" },
        { ZEITGEIST_RESULT_TYPE_LEAST_POPULAR_ACTOR, "ZEITGEIST_RESULT_TYPE_LEAST_POPULAR_ACTOR", "least-popular-actor" },
        { ZEITGEIST_RESULT_TYPE_MOST_RECENT_ACTOR, "ZEITGEIST_RESULT_TYPE_MOST_RECENT_ACTOR", "most-recent-actor" },
        { ZEITGEIST_RESULT_TYPE_LEAST_RECENT_ACTOR, "ZEITGEIST_RESULT_TYPE_LEAST_RECENT_ACTOR", "least-recent-actor" },
        { ZEITGEIST_RESULT_TYPE_MOST_RECENT_ORIGIN, "ZEITGEIST_RESULT_TYPE_MOST_RECENT_ORIGIN", "most-recent-origin" },
        { ZEITGEIST_RESULT_TYPE_LEAST_RECENT_ORIGIN, "ZEITGEIST_RESULT_TYPE_LEAST_RECENT_ORIGIN", "least-recent-origin" },
        { ZEITGEIST_RESULT_TYPE_MOST_POPULAR_ORIGIN, "ZEITGEIST_RESULT_TYPE_MOST_POPULAR_ORIGIN", "most-popular-origin" },
        { ZEITGEIST_RESULT_TYPE_LEAST_POPULAR_ORIGIN, "ZEITGEIST_RESULT_TYPE_LEAST_POPULAR_ORIGIN", "least-popular-origin" },
        { ZEITGEIST_RESULT_TYPE_OLDEST_ACTOR, "ZEITGEIST_RESULT_TYPE_OLDEST_ACTOR", "oldest-actor" },
        { ZEITGEIST_RESULT_TYPE_MOST_RECENT_SUBJECT_INTERPRETATION, "ZEITGEIST_RESULT_TYPE_MOST_RECENT_SUBJECT_INTERPRETATION", "most-recent-subject-interpretation" },
        { ZEITGEIST_RESULT_TYPE_LEAST_RECENT_SUBJECT_INTERPRETATION, "ZEITGEIST_RESULT_TYPE_LEAST_RECENT_SUBJECT_INTERPRETATION", "least-recent-subject-interpretation" },
        { ZEITGEIST_RESULT_TYPE_MOST_POPULAR_SUBJECT_INTERPRETATION, "ZEITGEIST_RESULT_TYPE_MOST_POPULAR_SUBJECT_INTERPRETATION", "most-popular-subject-interpretation" },
        { ZEITGEIST_RESULT_TYPE_LEAST_POPULAR_SUBJECT_INTERPRETATION, "ZEITGEIST_RESULT_TYPE_LEAST_POPULAR_SUBJECT_INTERPRETATION", "least-popular-subject-interpretation" },
        { ZEITGEIST_RESULT_TYPE_MOST_RECENT_MIMETYPE, "ZEITGEIST_RESULT_TYPE_MOST_RECENT_MIMETYPE", "most-recent-mimetype" },
        { ZEITGEIST_RESULT_TYPE_LEAST_RECENT_MIMETYPE, "ZEITGEIST_RESULT_TYPE_LEAST_RECENT_MIMETYPE", "least-recent-mimetype" },
        { ZEITGEIST_RESULT_TYPE_MOST_POPULAR_MIMETYPE, "ZEITGEIST_RESULT_TYPE_MOST_POPULAR_MIMETYPE", "most-popular-mimetype" },
        { ZEITGEIST_RESULT_TYPE_LEAST_POPULAR_MIMETYPE, "ZEITGEIST_RESULT_TYPE_LEAST_POPULAR_MIMETYPE", "least-popular-mimetype" },
        { ZEITGEIST_RESULT_TYPE_MOST_RECENT_CURRENT_URI, "ZEITGEIST_RESULT_TYPE_MOST_RECENT_CURRENT_URI", "most-recent-current-uri" },
        { ZEITGEIST_RESULT_TYPE_LEAST_RECENT_CURRENT_URI, "ZEITGEIST_RESULT_TYPE_LEAST_RECENT_CURRENT_URI", "least-recent-current-uri" },
        { ZEITGEIST_RESULT_TYPE_MOST_POPULAR_CURRENT_URI, "ZEITGEIST_RESULT_TYPE_MOST_POPULAR_CURRENT_URI", "most-popular-current-uri" },
        { ZEITGEIST_RESULT_TYPE_LEAST_POPULAR_CURRENT_URI, "ZEITGEIST_RESULT_TYPE_LEAST_POPULAR_CURRENT_URI", "least-popular-current-uri" },
        { ZEITGEIST_RESULT_TYPE_MOST_RECENT_EVENT_ORIGIN, "ZEITGEIST_RESULT_TYPE_MOST_RECENT_EVENT_ORIGIN", "most-recent-event-origin" },
        { ZEITGEIST_RESULT_TYPE_LEAST_RECENT_EVENT_ORIGIN, "ZEITGEIST_RESULT_TYPE_LEAST_RECENT_EVENT_ORIGIN", "least-recent-event-origin" },
        { ZEITGEIST_RESULT_TYPE_MOST_POPULAR_EVENT_ORIGIN, "ZEITGEIST_RESULT_TYPE_MOST_POPULAR_EVENT_ORIGIN", "most-popular-event-origin" },
        { ZEITGEIST_RESULT_TYPE_LEAST_POPULAR_EVENT_ORIGIN, "ZEITGEIST_RESULT_TYPE_LEAST_POPULAR_EVENT_ORIGIN", "least-popular-event-origin" },
        { ZEITGEIST_RESULT_TYPE_RELEVANCY, "ZEITGEIST_RESULT_TYPE_RELEVANCY", "relevancy" },
        { 0, NULL, NULL }
      };
      GType g_define_type_id =
        g_enum_register_static (g_intern_static_string ("ZeitgeistResultType"), values);
      g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);
    }

  return g_define_type_id__volatile;
}

GType
zeitgeist_storage_state_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if (g_once_init_enter (&g_define_type_id__volatile))
    {
      static const GEnumValue values[] = {
        { ZEITGEIST_STORAGE_STATE_NOT_AVAILABLE, "ZEITGEIST_STORAGE_STATE_NOT_AVAILABLE", "not-available" },
        { ZEITGEIST_STORAGE_STATE_AVAILABLE, "ZEITGEIST_STORAGE_STATE_AVAILABLE", "available" },
        { ZEITGEIST_STORAGE_STATE_ANY, "ZEITGEIST_STORAGE_STATE_ANY", "any" },
        { 0, NULL, NULL }
      };
      GType g_define_type_id =
        g_enum_register_static (g_intern_static_string ("ZeitgeistStorageState"), values);
      g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);
    }

  return g_define_type_id__volatile;
}


/* Generated data ends here */

