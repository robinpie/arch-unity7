<?xml version="1.0"?>
<api version="1.0">
	<namespace name="Zeitgeist">
		<function name="data_sources_from_variant" symbol="zeitgeist_data_sources_from_variant">
			<return-type type="GPtrArray*"/>
			<parameters>
				<parameter name="sources" type="GVariant*"/>
			</parameters>
		</function>
		<function name="data_sources_to_variant" symbol="zeitgeist_data_sources_to_variant">
			<return-type type="GVariant*"/>
			<parameters>
				<parameter name="sources" type="GPtrArray*"/>
			</parameters>
		</function>
		<function name="events_from_valist" symbol="zeitgeist_events_from_valist">
			<return-type type="GPtrArray*"/>
			<parameters>
				<parameter name="events" type="va_list"/>
			</parameters>
		</function>
		<function name="events_from_variant" symbol="zeitgeist_events_from_variant">
			<return-type type="GPtrArray*"/>
			<parameters>
				<parameter name="events" type="GVariant*"/>
			</parameters>
		</function>
		<function name="events_to_variant" symbol="zeitgeist_events_to_variant">
			<return-type type="GVariant*"/>
			<parameters>
				<parameter name="events" type="GPtrArray*"/>
			</parameters>
		</function>
		<function name="interpretation_for_mimetype" symbol="zeitgeist_interpretation_for_mimetype">
			<return-type type="gchar*"/>
			<parameters>
				<parameter name="mimetype" type="gchar*"/>
			</parameters>
		</function>
		<function name="manifestation_for_uri" symbol="zeitgeist_manifestation_for_uri">
			<return-type type="gchar*"/>
			<parameters>
				<parameter name="uri" type="gchar*"/>
			</parameters>
		</function>
		<function name="register_mimetype" symbol="zeitgeist_register_mimetype">
			<return-type type="void"/>
			<parameters>
				<parameter name="mimetype" type="gchar*"/>
				<parameter name="interpretation_uri" type="gchar*"/>
			</parameters>
		</function>
		<function name="register_mimetype_regex" symbol="zeitgeist_register_mimetype_regex">
			<return-type type="void"/>
			<parameters>
				<parameter name="mimetype_regex" type="gchar*"/>
				<parameter name="interpretation_uri" type="gchar*"/>
			</parameters>
		</function>
		<function name="register_uri_scheme" symbol="zeitgeist_register_uri_scheme">
			<return-type type="void"/>
			<parameters>
				<parameter name="uri_scheme" type="gchar*"/>
				<parameter name="manifestation_type" type="gchar*"/>
			</parameters>
		</function>
		<function name="symbol_get_all_children" symbol="zeitgeist_symbol_get_all_children">
			<return-type type="GList*"/>
			<parameters>
				<parameter name="symbol" type="gchar*"/>
			</parameters>
		</function>
		<function name="symbol_get_children" symbol="zeitgeist_symbol_get_children">
			<return-type type="GList*"/>
			<parameters>
				<parameter name="symbol" type="gchar*"/>
			</parameters>
		</function>
		<function name="symbol_get_parents" symbol="zeitgeist_symbol_get_parents">
			<return-type type="GList*"/>
			<parameters>
				<parameter name="symbol" type="gchar*"/>
			</parameters>
		</function>
		<function name="symbol_is_a" symbol="zeitgeist_symbol_is_a">
			<return-type type="gboolean"/>
			<parameters>
				<parameter name="symbol" type="gchar*"/>
				<parameter name="parent" type="gchar*"/>
			</parameters>
		</function>
		<function name="timestamp_for_now" symbol="zeitgeist_timestamp_for_now">
			<return-type type="gint64"/>
		</function>
		<function name="timestamp_from_date" symbol="zeitgeist_timestamp_from_date">
			<return-type type="gint64"/>
			<parameters>
				<parameter name="date" type="GDate*"/>
			</parameters>
		</function>
		<function name="timestamp_from_dmy" symbol="zeitgeist_timestamp_from_dmy">
			<return-type type="gint64"/>
			<parameters>
				<parameter name="day" type="GDateDay"/>
				<parameter name="month" type="GDateMonth"/>
				<parameter name="year" type="GDateYear"/>
			</parameters>
		</function>
		<function name="timestamp_from_iso8601" symbol="zeitgeist_timestamp_from_iso8601">
			<return-type type="gint64"/>
			<parameters>
				<parameter name="datetime" type="gchar*"/>
			</parameters>
		</function>
		<function name="timestamp_from_timeval" symbol="zeitgeist_timestamp_from_timeval">
			<return-type type="gint64"/>
			<parameters>
				<parameter name="tv" type="GTimeVal*"/>
			</parameters>
		</function>
		<function name="timestamp_next_midnight" symbol="zeitgeist_timestamp_next_midnight">
			<return-type type="gint64"/>
			<parameters>
				<parameter name="timestamp" type="gint64"/>
			</parameters>
		</function>
		<function name="timestamp_prev_midnight" symbol="zeitgeist_timestamp_prev_midnight">
			<return-type type="gint64"/>
			<parameters>
				<parameter name="timestamp" type="gint64"/>
			</parameters>
		</function>
		<function name="timestamp_to_date" symbol="zeitgeist_timestamp_to_date">
			<return-type type="void"/>
			<parameters>
				<parameter name="timestamp" type="gint64"/>
				<parameter name="date" type="GDate*"/>
			</parameters>
		</function>
		<function name="timestamp_to_iso8601" symbol="zeitgeist_timestamp_to_iso8601">
			<return-type type="gchar*"/>
			<parameters>
				<parameter name="timestamp" type="gint64"/>
			</parameters>
		</function>
		<function name="timestamp_to_timeval" symbol="zeitgeist_timestamp_to_timeval">
			<return-type type="void"/>
			<parameters>
				<parameter name="timestamp" type="gint64"/>
				<parameter name="tv" type="GTimeVal*"/>
			</parameters>
		</function>
		<enum name="ZeitgeistResultType" type-name="ZeitgeistResultType" get-type="zeitgeist_result_type_get_type">
			<member name="ZEITGEIST_RESULT_TYPE_MOST_RECENT_EVENTS" value="0"/>
			<member name="ZEITGEIST_RESULT_TYPE_LEAST_RECENT_EVENTS" value="1"/>
			<member name="ZEITGEIST_RESULT_TYPE_MOST_RECENT_SUBJECTS" value="2"/>
			<member name="ZEITGEIST_RESULT_TYPE_LEAST_RECENT_SUBJECTS" value="3"/>
			<member name="ZEITGEIST_RESULT_TYPE_MOST_POPULAR_SUBJECTS" value="4"/>
			<member name="ZEITGEIST_RESULT_TYPE_LEAST_POPULAR_SUBJECTS" value="5"/>
			<member name="ZEITGEIST_RESULT_TYPE_MOST_POPULAR_ACTOR" value="6"/>
			<member name="ZEITGEIST_RESULT_TYPE_LEAST_POPULAR_ACTOR" value="7"/>
			<member name="ZEITGEIST_RESULT_TYPE_MOST_RECENT_ACTOR" value="8"/>
			<member name="ZEITGEIST_RESULT_TYPE_LEAST_RECENT_ACTOR" value="9"/>
			<member name="ZEITGEIST_RESULT_TYPE_MOST_RECENT_ORIGIN" value="10"/>
			<member name="ZEITGEIST_RESULT_TYPE_LEAST_RECENT_ORIGIN" value="11"/>
			<member name="ZEITGEIST_RESULT_TYPE_MOST_POPULAR_ORIGIN" value="12"/>
			<member name="ZEITGEIST_RESULT_TYPE_LEAST_POPULAR_ORIGIN" value="13"/>
			<member name="ZEITGEIST_RESULT_TYPE_OLDEST_ACTOR" value="14"/>
			<member name="ZEITGEIST_RESULT_TYPE_MOST_RECENT_SUBJECT_INTERPRETATION" value="15"/>
			<member name="ZEITGEIST_RESULT_TYPE_LEAST_RECENT_SUBJECT_INTERPRETATION" value="16"/>
			<member name="ZEITGEIST_RESULT_TYPE_MOST_POPULAR_SUBJECT_INTERPRETATION" value="17"/>
			<member name="ZEITGEIST_RESULT_TYPE_LEAST_POPULAR_SUBJECT_INTERPRETATION" value="18"/>
			<member name="ZEITGEIST_RESULT_TYPE_MOST_RECENT_MIMETYPE" value="19"/>
			<member name="ZEITGEIST_RESULT_TYPE_LEAST_RECENT_MIMETYPE" value="20"/>
			<member name="ZEITGEIST_RESULT_TYPE_MOST_POPULAR_MIMETYPE" value="21"/>
			<member name="ZEITGEIST_RESULT_TYPE_LEAST_POPULAR_MIMETYPE" value="22"/>
			<member name="ZEITGEIST_RESULT_TYPE_MOST_RECENT_CURRENT_URI" value="23"/>
			<member name="ZEITGEIST_RESULT_TYPE_LEAST_RECENT_CURRENT_URI" value="24"/>
			<member name="ZEITGEIST_RESULT_TYPE_MOST_POPULAR_CURRENT_URI" value="25"/>
			<member name="ZEITGEIST_RESULT_TYPE_LEAST_POPULAR_CURRENT_URI" value="26"/>
			<member name="ZEITGEIST_RESULT_TYPE_MOST_RECENT_EVENT_ORIGIN" value="27"/>
			<member name="ZEITGEIST_RESULT_TYPE_LEAST_RECENT_EVENT_ORIGIN" value="28"/>
			<member name="ZEITGEIST_RESULT_TYPE_MOST_POPULAR_EVENT_ORIGIN" value="29"/>
			<member name="ZEITGEIST_RESULT_TYPE_LEAST_POPULAR_EVENT_ORIGIN" value="30"/>
			<member name="ZEITGEIST_RESULT_TYPE_RELEVANCY" value="100"/>
		</enum>
		<enum name="ZeitgeistStorageState" type-name="ZeitgeistStorageState" get-type="zeitgeist_storage_state_get_type">
			<member name="ZEITGEIST_STORAGE_STATE_NOT_AVAILABLE" value="0"/>
			<member name="ZEITGEIST_STORAGE_STATE_AVAILABLE" value="1"/>
			<member name="ZEITGEIST_STORAGE_STATE_ANY" value="2"/>
		</enum>
		<object name="ZeitgeistDataSource" parent="GInitiallyUnowned" type-name="ZeitgeistDataSource" get-type="zeitgeist_data_source_get_type">
			<method name="get_description" symbol="zeitgeist_data_source_get_description">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="src" type="ZeitgeistDataSource*"/>
				</parameters>
			</method>
			<method name="get_event_templates" symbol="zeitgeist_data_source_get_event_templates">
				<return-type type="GPtrArray*"/>
				<parameters>
					<parameter name="src" type="ZeitgeistDataSource*"/>
				</parameters>
			</method>
			<method name="get_name" symbol="zeitgeist_data_source_get_name">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="src" type="ZeitgeistDataSource*"/>
				</parameters>
			</method>
			<method name="get_timestamp" symbol="zeitgeist_data_source_get_timestamp">
				<return-type type="gint64"/>
				<parameters>
					<parameter name="src" type="ZeitgeistDataSource*"/>
				</parameters>
			</method>
			<method name="get_unique_id" symbol="zeitgeist_data_source_get_unique_id">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="src" type="ZeitgeistDataSource*"/>
				</parameters>
			</method>
			<method name="is_enabled" symbol="zeitgeist_data_source_is_enabled">
				<return-type type="gboolean"/>
				<parameters>
					<parameter name="src" type="ZeitgeistDataSource*"/>
				</parameters>
			</method>
			<method name="is_running" symbol="zeitgeist_data_source_is_running">
				<return-type type="gboolean"/>
				<parameters>
					<parameter name="src" type="ZeitgeistDataSource*"/>
				</parameters>
			</method>
			<constructor name="new" symbol="zeitgeist_data_source_new">
				<return-type type="ZeitgeistDataSource*"/>
			</constructor>
			<constructor name="new_from_variant" symbol="zeitgeist_data_source_new_from_variant">
				<return-type type="ZeitgeistDataSource*"/>
				<parameters>
					<parameter name="src" type="GVariant*"/>
				</parameters>
			</constructor>
			<constructor name="new_full" symbol="zeitgeist_data_source_new_full">
				<return-type type="ZeitgeistDataSource*"/>
				<parameters>
					<parameter name="id" type="gchar*"/>
					<parameter name="name" type="gchar*"/>
					<parameter name="desc" type="gchar*"/>
					<parameter name="event_templates" type="GPtrArray*"/>
				</parameters>
			</constructor>
			<method name="set_description" symbol="zeitgeist_data_source_set_description">
				<return-type type="void"/>
				<parameters>
					<parameter name="src" type="ZeitgeistDataSource*"/>
					<parameter name="description" type="gchar*"/>
				</parameters>
			</method>
			<method name="set_enabled" symbol="zeitgeist_data_source_set_enabled">
				<return-type type="void"/>
				<parameters>
					<parameter name="src" type="ZeitgeistDataSource*"/>
					<parameter name="enabled" type="gboolean"/>
				</parameters>
			</method>
			<method name="set_event_templates" symbol="zeitgeist_data_source_set_event_templates">
				<return-type type="void"/>
				<parameters>
					<parameter name="src" type="ZeitgeistDataSource*"/>
					<parameter name="event_templates" type="GPtrArray*"/>
				</parameters>
			</method>
			<method name="set_name" symbol="zeitgeist_data_source_set_name">
				<return-type type="void"/>
				<parameters>
					<parameter name="src" type="ZeitgeistDataSource*"/>
					<parameter name="name" type="gchar*"/>
				</parameters>
			</method>
			<method name="set_running" symbol="zeitgeist_data_source_set_running">
				<return-type type="void"/>
				<parameters>
					<parameter name="src" type="ZeitgeistDataSource*"/>
					<parameter name="running" type="gboolean"/>
				</parameters>
			</method>
			<method name="set_timestamp" symbol="zeitgeist_data_source_set_timestamp">
				<return-type type="void"/>
				<parameters>
					<parameter name="src" type="ZeitgeistDataSource*"/>
					<parameter name="timestamp" type="gint64"/>
				</parameters>
			</method>
			<method name="set_unique_id" symbol="zeitgeist_data_source_set_unique_id">
				<return-type type="void"/>
				<parameters>
					<parameter name="src" type="ZeitgeistDataSource*"/>
					<parameter name="unique_id" type="gchar*"/>
				</parameters>
			</method>
			<method name="to_variant" symbol="zeitgeist_data_source_to_variant">
				<return-type type="GVariant*"/>
				<parameters>
					<parameter name="src" type="ZeitgeistDataSource*"/>
				</parameters>
			</method>
			<method name="to_variant_full" symbol="zeitgeist_data_source_to_variant_full">
				<return-type type="GVariant*"/>
				<parameters>
					<parameter name="src" type="ZeitgeistDataSource*"/>
				</parameters>
			</method>
		</object>
		<object name="ZeitgeistDataSourceRegistry" parent="GObject" type-name="ZeitgeistDataSourceRegistry" get-type="zeitgeist_data_source_registry_get_type">
			<method name="get_data_sources" symbol="zeitgeist_data_source_registry_get_data_sources">
				<return-type type="void"/>
				<parameters>
					<parameter name="self" type="ZeitgeistDataSourceRegistry*"/>
					<parameter name="cancellable" type="GCancellable*"/>
					<parameter name="callback" type="GAsyncReadyCallback"/>
					<parameter name="user_data" type="gpointer"/>
				</parameters>
			</method>
			<method name="get_data_sources_finish" symbol="zeitgeist_data_source_registry_get_data_sources_finish">
				<return-type type="GPtrArray*"/>
				<parameters>
					<parameter name="self" type="ZeitgeistDataSourceRegistry*"/>
					<parameter name="res" type="GAsyncResult*"/>
					<parameter name="error" type="GError**"/>
				</parameters>
			</method>
			<constructor name="new" symbol="zeitgeist_data_source_registry_new">
				<return-type type="ZeitgeistDataSourceRegistry*"/>
			</constructor>
			<method name="register_data_source" symbol="zeitgeist_data_source_registry_register_data_source">
				<return-type type="void"/>
				<parameters>
					<parameter name="self" type="ZeitgeistDataSourceRegistry*"/>
					<parameter name="source" type="ZeitgeistDataSource*"/>
					<parameter name="cancellable" type="GCancellable*"/>
					<parameter name="callback" type="GAsyncReadyCallback"/>
					<parameter name="user_data" type="gpointer"/>
				</parameters>
			</method>
			<method name="register_data_source_finish" symbol="zeitgeist_data_source_registry_register_data_source_finish">
				<return-type type="gboolean"/>
				<parameters>
					<parameter name="self" type="ZeitgeistDataSourceRegistry*"/>
					<parameter name="res" type="GAsyncResult*"/>
					<parameter name="error" type="GError**"/>
				</parameters>
			</method>
			<method name="set_data_source_enabled" symbol="zeitgeist_data_source_registry_set_data_source_enabled">
				<return-type type="void"/>
				<parameters>
					<parameter name="self" type="ZeitgeistDataSourceRegistry*"/>
					<parameter name="unique_id" type="gchar*"/>
					<parameter name="enabled" type="gboolean"/>
					<parameter name="cancellable" type="GCancellable*"/>
					<parameter name="callback" type="GAsyncReadyCallback"/>
					<parameter name="user_data" type="gpointer"/>
				</parameters>
			</method>
			<method name="set_data_source_enabled_finish" symbol="zeitgeist_data_source_registry_set_data_source_enabled_finish">
				<return-type type="gboolean"/>
				<parameters>
					<parameter name="self" type="ZeitgeistDataSourceRegistry*"/>
					<parameter name="res" type="GAsyncResult*"/>
					<parameter name="error" type="GError**"/>
				</parameters>
			</method>
			<signal name="source-disconnected" when="FIRST">
				<return-type type="void"/>
				<parameters>
					<parameter name="registry" type="ZeitgeistDataSourceRegistry*"/>
					<parameter name="source" type="ZeitgeistDataSource*"/>
				</parameters>
			</signal>
			<signal name="source-enabled" when="FIRST">
				<return-type type="void"/>
				<parameters>
					<parameter name="registry" type="ZeitgeistDataSourceRegistry*"/>
					<parameter name="unique_id" type="char*"/>
					<parameter name="enabled" type="gboolean"/>
				</parameters>
			</signal>
			<signal name="source-registered" when="FIRST">
				<return-type type="void"/>
				<parameters>
					<parameter name="registry" type="ZeitgeistDataSourceRegistry*"/>
					<parameter name="source" type="ZeitgeistDataSource*"/>
				</parameters>
			</signal>
		</object>
		<object name="ZeitgeistEvent" parent="GInitiallyUnowned" type-name="ZeitgeistEvent" get-type="zeitgeist_event_get_type">
			<method name="add_subject" symbol="zeitgeist_event_add_subject">
				<return-type type="void"/>
				<parameters>
					<parameter name="event" type="ZeitgeistEvent*"/>
					<parameter name="subject" type="ZeitgeistSubject*"/>
				</parameters>
			</method>
			<method name="get_actor" symbol="zeitgeist_event_get_actor">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="event" type="ZeitgeistEvent*"/>
				</parameters>
			</method>
			<method name="get_id" symbol="zeitgeist_event_get_id">
				<return-type type="guint32"/>
				<parameters>
					<parameter name="event" type="ZeitgeistEvent*"/>
				</parameters>
			</method>
			<method name="get_interpretation" symbol="zeitgeist_event_get_interpretation">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="event" type="ZeitgeistEvent*"/>
				</parameters>
			</method>
			<method name="get_manifestation" symbol="zeitgeist_event_get_manifestation">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="event" type="ZeitgeistEvent*"/>
				</parameters>
			</method>
			<method name="get_origin" symbol="zeitgeist_event_get_origin">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="event" type="ZeitgeistEvent*"/>
				</parameters>
			</method>
			<method name="get_payload" symbol="zeitgeist_event_get_payload">
				<return-type type="GByteArray*"/>
				<parameters>
					<parameter name="event" type="ZeitgeistEvent*"/>
				</parameters>
			</method>
			<method name="get_subject" symbol="zeitgeist_event_get_subject">
				<return-type type="ZeitgeistSubject*"/>
				<parameters>
					<parameter name="event" type="ZeitgeistEvent*"/>
					<parameter name="index" type="gint"/>
				</parameters>
			</method>
			<method name="get_timestamp" symbol="zeitgeist_event_get_timestamp">
				<return-type type="gint64"/>
				<parameters>
					<parameter name="event" type="ZeitgeistEvent*"/>
				</parameters>
			</method>
			<constructor name="new" symbol="zeitgeist_event_new">
				<return-type type="ZeitgeistEvent*"/>
			</constructor>
			<constructor name="new_from_variant" symbol="zeitgeist_event_new_from_variant">
				<return-type type="ZeitgeistEvent*"/>
				<parameters>
					<parameter name="event" type="GVariant*"/>
				</parameters>
			</constructor>
			<constructor name="new_full" symbol="zeitgeist_event_new_full">
				<return-type type="ZeitgeistEvent*"/>
				<parameters>
					<parameter name="interpretation" type="gchar*"/>
					<parameter name="manifestation" type="gchar*"/>
					<parameter name="actor" type="gchar*"/>
				</parameters>
			</constructor>
			<constructor name="new_full_valist" symbol="zeitgeist_event_new_full_valist">
				<return-type type="ZeitgeistEvent*"/>
				<parameters>
					<parameter name="interpretation" type="gchar*"/>
					<parameter name="manifestation" type="gchar*"/>
					<parameter name="actor" type="gchar*"/>
					<parameter name="args" type="va_list"/>
				</parameters>
			</constructor>
			<method name="num_subjects" symbol="zeitgeist_event_num_subjects">
				<return-type type="gint"/>
				<parameters>
					<parameter name="event" type="ZeitgeistEvent*"/>
				</parameters>
			</method>
			<method name="set_actor" symbol="zeitgeist_event_set_actor">
				<return-type type="void"/>
				<parameters>
					<parameter name="event" type="ZeitgeistEvent*"/>
					<parameter name="actor" type="gchar*"/>
				</parameters>
			</method>
			<method name="set_actor_from_app_info" symbol="zeitgeist_event_set_actor_from_app_info">
				<return-type type="void"/>
				<parameters>
					<parameter name="event" type="ZeitgeistEvent*"/>
					<parameter name="appinfo" type="GAppInfo*"/>
				</parameters>
			</method>
			<method name="set_id" symbol="zeitgeist_event_set_id">
				<return-type type="void"/>
				<parameters>
					<parameter name="event" type="ZeitgeistEvent*"/>
					<parameter name="id" type="guint32"/>
				</parameters>
			</method>
			<method name="set_interpretation" symbol="zeitgeist_event_set_interpretation">
				<return-type type="void"/>
				<parameters>
					<parameter name="event" type="ZeitgeistEvent*"/>
					<parameter name="interpretation" type="gchar*"/>
				</parameters>
			</method>
			<method name="set_manifestation" symbol="zeitgeist_event_set_manifestation">
				<return-type type="void"/>
				<parameters>
					<parameter name="event" type="ZeitgeistEvent*"/>
					<parameter name="manifestation" type="gchar*"/>
				</parameters>
			</method>
			<method name="set_origin" symbol="zeitgeist_event_set_origin">
				<return-type type="void"/>
				<parameters>
					<parameter name="event" type="ZeitgeistEvent*"/>
					<parameter name="origin" type="gchar*"/>
				</parameters>
			</method>
			<method name="set_payload" symbol="zeitgeist_event_set_payload">
				<return-type type="void"/>
				<parameters>
					<parameter name="event" type="ZeitgeistEvent*"/>
					<parameter name="payload" type="GByteArray*"/>
				</parameters>
			</method>
			<method name="set_timestamp" symbol="zeitgeist_event_set_timestamp">
				<return-type type="void"/>
				<parameters>
					<parameter name="event" type="ZeitgeistEvent*"/>
					<parameter name="timestamp" type="gint64"/>
				</parameters>
			</method>
			<method name="to_variant" symbol="zeitgeist_event_to_variant">
				<return-type type="GVariant*"/>
				<parameters>
					<parameter name="event" type="ZeitgeistEvent*"/>
				</parameters>
			</method>
		</object>
		<object name="ZeitgeistIndex" parent="GObject" type-name="ZeitgeistIndex" get-type="zeitgeist_index_get_type">
			<constructor name="new" symbol="zeitgeist_index_new">
				<return-type type="ZeitgeistIndex*"/>
			</constructor>
			<method name="search" symbol="zeitgeist_index_search">
				<return-type type="void"/>
				<parameters>
					<parameter name="self" type="ZeitgeistIndex*"/>
					<parameter name="query" type="gchar*"/>
					<parameter name="time_range" type="ZeitgeistTimeRange*"/>
					<parameter name="event_templates" type="GPtrArray*"/>
					<parameter name="offset" type="guint32"/>
					<parameter name="num_events" type="guint32"/>
					<parameter name="result_type" type="ZeitgeistResultType"/>
					<parameter name="cancellable" type="GCancellable*"/>
					<parameter name="callback" type="GAsyncReadyCallback"/>
					<parameter name="user_data" type="gpointer"/>
				</parameters>
			</method>
			<method name="search_finish" symbol="zeitgeist_index_search_finish">
				<return-type type="ZeitgeistResultSet*"/>
				<parameters>
					<parameter name="self" type="ZeitgeistIndex*"/>
					<parameter name="res" type="GAsyncResult*"/>
					<parameter name="error" type="GError**"/>
				</parameters>
			</method>
			<method name="search_with_relevancies" symbol="zeitgeist_index_search_with_relevancies">
				<return-type type="void"/>
				<parameters>
					<parameter name="self" type="ZeitgeistIndex*"/>
					<parameter name="query" type="gchar*"/>
					<parameter name="time_range" type="ZeitgeistTimeRange*"/>
					<parameter name="event_templates" type="GPtrArray*"/>
					<parameter name="storage_state" type="ZeitgeistStorageState"/>
					<parameter name="offset" type="guint32"/>
					<parameter name="num_events" type="guint32"/>
					<parameter name="result_type" type="ZeitgeistResultType"/>
					<parameter name="cancellable" type="GCancellable*"/>
					<parameter name="callback" type="GAsyncReadyCallback"/>
					<parameter name="user_data" type="gpointer"/>
				</parameters>
			</method>
			<method name="search_with_relevancies_finish" symbol="zeitgeist_index_search_with_relevancies_finish">
				<return-type type="ZeitgeistResultSet*"/>
				<parameters>
					<parameter name="self" type="ZeitgeistIndex*"/>
					<parameter name="res" type="GAsyncResult*"/>
					<parameter name="relevancies" type="gdouble**"/>
					<parameter name="relevancies_size" type="gint*"/>
					<parameter name="error" type="GError**"/>
				</parameters>
			</method>
		</object>
		<object name="ZeitgeistLog" parent="GObject" type-name="ZeitgeistLog" get-type="zeitgeist_log_get_type">
			<method name="delete_events" symbol="zeitgeist_log_delete_events">
				<return-type type="void"/>
				<parameters>
					<parameter name="self" type="ZeitgeistLog*"/>
					<parameter name="event_ids" type="GArray*"/>
					<parameter name="cancellable" type="GCancellable*"/>
					<parameter name="callback" type="GAsyncReadyCallback"/>
					<parameter name="user_data" type="gpointer"/>
				</parameters>
			</method>
			<method name="delete_events_finish" symbol="zeitgeist_log_delete_events_finish">
				<return-type type="gboolean"/>
				<parameters>
					<parameter name="self" type="ZeitgeistLog*"/>
					<parameter name="res" type="GAsyncResult*"/>
					<parameter name="error" type="GError**"/>
				</parameters>
			</method>
			<method name="delete_log" symbol="zeitgeist_log_delete_log">
				<return-type type="void"/>
				<parameters>
					<parameter name="self" type="ZeitgeistLog*"/>
					<parameter name="cancellable" type="GCancellable*"/>
					<parameter name="callback" type="GAsyncReadyCallback"/>
					<parameter name="user_data" type="gpointer"/>
				</parameters>
			</method>
			<method name="delete_log_finish" symbol="zeitgeist_log_delete_log_finish">
				<return-type type="gboolean"/>
				<parameters>
					<parameter name="self" type="ZeitgeistLog*"/>
					<parameter name="res" type="GAsyncResult*"/>
					<parameter name="error" type="GError**"/>
				</parameters>
			</method>
			<method name="find_event_ids" symbol="zeitgeist_log_find_event_ids">
				<return-type type="void"/>
				<parameters>
					<parameter name="self" type="ZeitgeistLog*"/>
					<parameter name="time_range" type="ZeitgeistTimeRange*"/>
					<parameter name="event_templates" type="GPtrArray*"/>
					<parameter name="storage_state" type="ZeitgeistStorageState"/>
					<parameter name="num_events" type="guint32"/>
					<parameter name="result_type" type="ZeitgeistResultType"/>
					<parameter name="cancellable" type="GCancellable*"/>
					<parameter name="callback" type="GAsyncReadyCallback"/>
					<parameter name="user_data" type="gpointer"/>
				</parameters>
			</method>
			<method name="find_event_ids_finish" symbol="zeitgeist_log_find_event_ids_finish">
				<return-type type="GArray*"/>
				<parameters>
					<parameter name="self" type="ZeitgeistLog*"/>
					<parameter name="res" type="GAsyncResult*"/>
					<parameter name="error" type="GError**"/>
				</parameters>
			</method>
			<method name="find_events" symbol="zeitgeist_log_find_events">
				<return-type type="void"/>
				<parameters>
					<parameter name="self" type="ZeitgeistLog*"/>
					<parameter name="time_range" type="ZeitgeistTimeRange*"/>
					<parameter name="event_templates" type="GPtrArray*"/>
					<parameter name="storage_state" type="ZeitgeistStorageState"/>
					<parameter name="num_events" type="guint32"/>
					<parameter name="result_type" type="ZeitgeistResultType"/>
					<parameter name="cancellable" type="GCancellable*"/>
					<parameter name="callback" type="GAsyncReadyCallback"/>
					<parameter name="user_data" type="gpointer"/>
				</parameters>
			</method>
			<method name="find_events_finish" symbol="zeitgeist_log_find_events_finish">
				<return-type type="ZeitgeistResultSet*"/>
				<parameters>
					<parameter name="self" type="ZeitgeistLog*"/>
					<parameter name="res" type="GAsyncResult*"/>
					<parameter name="error" type="GError**"/>
				</parameters>
			</method>
			<method name="find_related_uris" symbol="zeitgeist_log_find_related_uris">
				<return-type type="void"/>
				<parameters>
					<parameter name="self" type="ZeitgeistLog*"/>
					<parameter name="time_range" type="ZeitgeistTimeRange*"/>
					<parameter name="event_templates" type="GPtrArray*"/>
					<parameter name="result_event_templates" type="GPtrArray*"/>
					<parameter name="storage_state" type="ZeitgeistStorageState"/>
					<parameter name="num_events" type="guint32"/>
					<parameter name="result_type" type="ZeitgeistResultType"/>
					<parameter name="cancellable" type="GCancellable*"/>
					<parameter name="callback" type="GAsyncReadyCallback"/>
					<parameter name="user_data" type="gpointer"/>
				</parameters>
			</method>
			<method name="find_related_uris_finish" symbol="zeitgeist_log_find_related_uris_finish">
				<return-type type="gchar**"/>
				<parameters>
					<parameter name="self" type="ZeitgeistLog*"/>
					<parameter name="res" type="GAsyncResult*"/>
					<parameter name="error" type="GError**"/>
				</parameters>
			</method>
			<method name="get_default" symbol="zeitgeist_log_get_default">
				<return-type type="ZeitgeistLog*"/>
			</method>
			<method name="get_events" symbol="zeitgeist_log_get_events">
				<return-type type="void"/>
				<parameters>
					<parameter name="self" type="ZeitgeistLog*"/>
					<parameter name="event_ids" type="GArray*"/>
					<parameter name="cancellable" type="GCancellable*"/>
					<parameter name="callback" type="GAsyncReadyCallback"/>
					<parameter name="user_data" type="gpointer"/>
				</parameters>
			</method>
			<method name="get_events_finish" symbol="zeitgeist_log_get_events_finish">
				<return-type type="ZeitgeistResultSet*"/>
				<parameters>
					<parameter name="self" type="ZeitgeistLog*"/>
					<parameter name="res" type="GAsyncResult*"/>
					<parameter name="error" type="GError**"/>
				</parameters>
			</method>
			<method name="get_version" symbol="zeitgeist_log_get_version">
				<return-type type="void"/>
				<parameters>
					<parameter name="self" type="ZeitgeistLog*"/>
					<parameter name="major" type="gint*"/>
					<parameter name="minor" type="gint*"/>
					<parameter name="micro" type="gint*"/>
				</parameters>
			</method>
			<method name="insert_events" symbol="zeitgeist_log_insert_events">
				<return-type type="void"/>
				<parameters>
					<parameter name="self" type="ZeitgeistLog*"/>
					<parameter name="cancellable" type="GCancellable*"/>
					<parameter name="callback" type="GAsyncReadyCallback"/>
					<parameter name="user_data" type="gpointer"/>
				</parameters>
			</method>
			<method name="insert_events_finish" symbol="zeitgeist_log_insert_events_finish">
				<return-type type="GArray*"/>
				<parameters>
					<parameter name="self" type="ZeitgeistLog*"/>
					<parameter name="res" type="GAsyncResult*"/>
					<parameter name="error" type="GError**"/>
				</parameters>
			</method>
			<method name="insert_events_from_ptrarray" symbol="zeitgeist_log_insert_events_from_ptrarray">
				<return-type type="void"/>
				<parameters>
					<parameter name="self" type="ZeitgeistLog*"/>
					<parameter name="events" type="GPtrArray*"/>
					<parameter name="cancellable" type="GCancellable*"/>
					<parameter name="callback" type="GAsyncReadyCallback"/>
					<parameter name="user_data" type="gpointer"/>
				</parameters>
			</method>
			<method name="insert_events_no_reply" symbol="zeitgeist_log_insert_events_no_reply">
				<return-type type="void"/>
				<parameters>
					<parameter name="self" type="ZeitgeistLog*"/>
				</parameters>
			</method>
			<method name="insert_events_valist" symbol="zeitgeist_log_insert_events_valist">
				<return-type type="void"/>
				<parameters>
					<parameter name="self" type="ZeitgeistLog*"/>
					<parameter name="cancellable" type="GCancellable*"/>
					<parameter name="callback" type="GAsyncReadyCallback"/>
					<parameter name="user_data" type="gpointer"/>
					<parameter name="events" type="va_list"/>
				</parameters>
			</method>
			<method name="install_monitor" symbol="zeitgeist_log_install_monitor">
				<return-type type="void"/>
				<parameters>
					<parameter name="self" type="ZeitgeistLog*"/>
					<parameter name="monitor" type="ZeitgeistMonitor*"/>
				</parameters>
			</method>
			<method name="is_connected" symbol="zeitgeist_log_is_connected">
				<return-type type="gboolean"/>
				<parameters>
					<parameter name="self" type="ZeitgeistLog*"/>
				</parameters>
			</method>
			<constructor name="new" symbol="zeitgeist_log_new">
				<return-type type="ZeitgeistLog*"/>
			</constructor>
			<method name="quit" symbol="zeitgeist_log_quit">
				<return-type type="void"/>
				<parameters>
					<parameter name="self" type="ZeitgeistLog*"/>
					<parameter name="cancellable" type="GCancellable*"/>
					<parameter name="callback" type="GAsyncReadyCallback"/>
					<parameter name="user_data" type="gpointer"/>
				</parameters>
			</method>
			<method name="quit_finish" symbol="zeitgeist_log_quit_finish">
				<return-type type="gboolean"/>
				<parameters>
					<parameter name="self" type="ZeitgeistLog*"/>
					<parameter name="res" type="GAsyncResult*"/>
					<parameter name="error" type="GError**"/>
				</parameters>
			</method>
			<method name="remove_monitor" symbol="zeitgeist_log_remove_monitor">
				<return-type type="void"/>
				<parameters>
					<parameter name="self" type="ZeitgeistLog*"/>
					<parameter name="monitor" type="ZeitgeistMonitor*"/>
				</parameters>
			</method>
			<property name="connected" type="gboolean" readable="1" writable="0" construct="0" construct-only="0"/>
		</object>
		<object name="ZeitgeistMonitor" parent="GObject" type-name="ZeitgeistMonitor" get-type="zeitgeist_monitor_get_type">
			<method name="get_path" symbol="zeitgeist_monitor_get_path">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="self" type="ZeitgeistMonitor*"/>
				</parameters>
			</method>
			<method name="get_templates" symbol="zeitgeist_monitor_get_templates">
				<return-type type="GPtrArray*"/>
				<parameters>
					<parameter name="self" type="ZeitgeistMonitor*"/>
				</parameters>
			</method>
			<method name="get_time_range" symbol="zeitgeist_monitor_get_time_range">
				<return-type type="ZeitgeistTimeRange*"/>
				<parameters>
					<parameter name="self" type="ZeitgeistMonitor*"/>
				</parameters>
			</method>
			<constructor name="new" symbol="zeitgeist_monitor_new">
				<return-type type="ZeitgeistMonitor*"/>
				<parameters>
					<parameter name="time_range" type="ZeitgeistTimeRange*"/>
					<parameter name="event_templates" type="GPtrArray*"/>
				</parameters>
			</constructor>
			<property name="event-templates" type="GPtrArray*" readable="1" writable="1" construct="0" construct-only="1"/>
			<property name="time-range" type="ZeitgeistTimeRange*" readable="1" writable="1" construct="0" construct-only="1"/>
			<signal name="events-deleted" when="LAST">
				<return-type type="void"/>
				<parameters>
					<parameter name="self" type="ZeitgeistMonitor*"/>
					<parameter name="time_range" type="ZeitgeistTimeRange*"/>
					<parameter name="event_ids" type="GArray*"/>
				</parameters>
			</signal>
			<signal name="events-inserted" when="LAST">
				<return-type type="void"/>
				<parameters>
					<parameter name="self" type="ZeitgeistMonitor*"/>
					<parameter name="time_range" type="ZeitgeistTimeRange*"/>
					<parameter name="events" type="ZeitgeistResultSet*"/>
				</parameters>
			</signal>
		</object>
		<object name="ZeitgeistSubject" parent="GInitiallyUnowned" type-name="ZeitgeistSubject" get-type="zeitgeist_subject_get_type">
			<method name="get_current_uri" symbol="zeitgeist_subject_get_current_uri">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="subject" type="ZeitgeistSubject*"/>
				</parameters>
			</method>
			<method name="get_interpretation" symbol="zeitgeist_subject_get_interpretation">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="subject" type="ZeitgeistSubject*"/>
				</parameters>
			</method>
			<method name="get_manifestation" symbol="zeitgeist_subject_get_manifestation">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="subject" type="ZeitgeistSubject*"/>
				</parameters>
			</method>
			<method name="get_mimetype" symbol="zeitgeist_subject_get_mimetype">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="subject" type="ZeitgeistSubject*"/>
				</parameters>
			</method>
			<method name="get_origin" symbol="zeitgeist_subject_get_origin">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="subject" type="ZeitgeistSubject*"/>
				</parameters>
			</method>
			<method name="get_storage" symbol="zeitgeist_subject_get_storage">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="subject" type="ZeitgeistSubject*"/>
				</parameters>
			</method>
			<method name="get_text" symbol="zeitgeist_subject_get_text">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="subject" type="ZeitgeistSubject*"/>
				</parameters>
			</method>
			<method name="get_uri" symbol="zeitgeist_subject_get_uri">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="subject" type="ZeitgeistSubject*"/>
				</parameters>
			</method>
			<constructor name="new" symbol="zeitgeist_subject_new">
				<return-type type="ZeitgeistSubject*"/>
			</constructor>
			<constructor name="new_full" symbol="zeitgeist_subject_new_full">
				<return-type type="ZeitgeistSubject*"/>
				<parameters>
					<parameter name="uri" type="gchar*"/>
					<parameter name="interpretation" type="gchar*"/>
					<parameter name="manifestation" type="gchar*"/>
					<parameter name="mimetype" type="gchar*"/>
					<parameter name="origin" type="gchar*"/>
					<parameter name="text" type="gchar*"/>
					<parameter name="storage" type="gchar*"/>
				</parameters>
			</constructor>
			<method name="set_current_uri" symbol="zeitgeist_subject_set_current_uri">
				<return-type type="void"/>
				<parameters>
					<parameter name="subject" type="ZeitgeistSubject*"/>
					<parameter name="current_uri" type="gchar*"/>
				</parameters>
			</method>
			<method name="set_interpretation" symbol="zeitgeist_subject_set_interpretation">
				<return-type type="void"/>
				<parameters>
					<parameter name="subject" type="ZeitgeistSubject*"/>
					<parameter name="interpretation" type="gchar*"/>
				</parameters>
			</method>
			<method name="set_manifestation" symbol="zeitgeist_subject_set_manifestation">
				<return-type type="void"/>
				<parameters>
					<parameter name="subject" type="ZeitgeistSubject*"/>
					<parameter name="manifestation" type="gchar*"/>
				</parameters>
			</method>
			<method name="set_mimetype" symbol="zeitgeist_subject_set_mimetype">
				<return-type type="void"/>
				<parameters>
					<parameter name="subject" type="ZeitgeistSubject*"/>
					<parameter name="mimetype" type="gchar*"/>
				</parameters>
			</method>
			<method name="set_origin" symbol="zeitgeist_subject_set_origin">
				<return-type type="void"/>
				<parameters>
					<parameter name="subject" type="ZeitgeistSubject*"/>
					<parameter name="origin" type="gchar*"/>
				</parameters>
			</method>
			<method name="set_storage" symbol="zeitgeist_subject_set_storage">
				<return-type type="void"/>
				<parameters>
					<parameter name="subject" type="ZeitgeistSubject*"/>
					<parameter name="storage" type="gchar*"/>
				</parameters>
			</method>
			<method name="set_text" symbol="zeitgeist_subject_set_text">
				<return-type type="void"/>
				<parameters>
					<parameter name="subject" type="ZeitgeistSubject*"/>
					<parameter name="text" type="gchar*"/>
				</parameters>
			</method>
			<method name="set_uri" symbol="zeitgeist_subject_set_uri">
				<return-type type="void"/>
				<parameters>
					<parameter name="subject" type="ZeitgeistSubject*"/>
					<parameter name="uri" type="gchar*"/>
				</parameters>
			</method>
		</object>
		<object name="ZeitgeistTimeRange" parent="GInitiallyUnowned" type-name="ZeitgeistTimeRange" get-type="zeitgeist_time_range_get_type">
			<method name="get_end" symbol="zeitgeist_time_range_get_end">
				<return-type type="gint64"/>
				<parameters>
					<parameter name="time_range" type="ZeitgeistTimeRange*"/>
				</parameters>
			</method>
			<method name="get_end_iso8601" symbol="zeitgeist_time_range_get_end_iso8601">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="time_range" type="ZeitgeistTimeRange*"/>
				</parameters>
			</method>
			<method name="get_start" symbol="zeitgeist_time_range_get_start">
				<return-type type="gint64"/>
				<parameters>
					<parameter name="time_range" type="ZeitgeistTimeRange*"/>
				</parameters>
			</method>
			<method name="get_start_iso8601" symbol="zeitgeist_time_range_get_start_iso8601">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="time_range" type="ZeitgeistTimeRange*"/>
				</parameters>
			</method>
			<constructor name="new" symbol="zeitgeist_time_range_new">
				<return-type type="ZeitgeistTimeRange*"/>
				<parameters>
					<parameter name="start_msec" type="gint64"/>
					<parameter name="end_msec" type="gint64"/>
				</parameters>
			</constructor>
			<constructor name="new_anytime" symbol="zeitgeist_time_range_new_anytime">
				<return-type type="ZeitgeistTimeRange*"/>
			</constructor>
			<constructor name="new_from_now" symbol="zeitgeist_time_range_new_from_now">
				<return-type type="ZeitgeistTimeRange*"/>
			</constructor>
			<constructor name="new_from_variant" symbol="zeitgeist_time_range_new_from_variant">
				<return-type type="ZeitgeistTimeRange*"/>
				<parameters>
					<parameter name="time_range" type="GVariant*"/>
				</parameters>
			</constructor>
			<constructor name="new_to_now" symbol="zeitgeist_time_range_new_to_now">
				<return-type type="ZeitgeistTimeRange*"/>
			</constructor>
			<method name="to_variant" symbol="zeitgeist_time_range_to_variant">
				<return-type type="GVariant*"/>
				<parameters>
					<parameter name="time_range" type="ZeitgeistTimeRange*"/>
				</parameters>
			</method>
		</object>
		<interface name="ZeitgeistResultSet" type-name="ZeitgeistResultSet" get-type="zeitgeist_result_set_get_type">
			<requires>
				<interface name="GObject"/>
			</requires>
			<method name="estimated_matches" symbol="zeitgeist_result_set_estimated_matches">
				<return-type type="guint"/>
				<parameters>
					<parameter name="self" type="ZeitgeistResultSet*"/>
				</parameters>
			</method>
			<method name="has_next" symbol="zeitgeist_result_set_has_next">
				<return-type type="gboolean"/>
				<parameters>
					<parameter name="self" type="ZeitgeistResultSet*"/>
				</parameters>
			</method>
			<method name="next" symbol="zeitgeist_result_set_next">
				<return-type type="ZeitgeistEvent*"/>
				<parameters>
					<parameter name="self" type="ZeitgeistResultSet*"/>
				</parameters>
			</method>
			<method name="peek" symbol="zeitgeist_result_set_peek">
				<return-type type="ZeitgeistEvent*"/>
				<parameters>
					<parameter name="self" type="ZeitgeistResultSet*"/>
				</parameters>
			</method>
			<method name="seek" symbol="zeitgeist_result_set_seek">
				<return-type type="void"/>
				<parameters>
					<parameter name="self" type="ZeitgeistResultSet*"/>
					<parameter name="pos" type="guint"/>
				</parameters>
			</method>
			<method name="size" symbol="zeitgeist_result_set_size">
				<return-type type="guint"/>
				<parameters>
					<parameter name="self" type="ZeitgeistResultSet*"/>
				</parameters>
			</method>
			<method name="tell" symbol="zeitgeist_result_set_tell">
				<return-type type="guint"/>
				<parameters>
					<parameter name="self" type="ZeitgeistResultSet*"/>
				</parameters>
			</method>
			<vfunc name="estimated_matches">
				<return-type type="guint"/>
				<parameters>
					<parameter name="self" type="ZeitgeistResultSet*"/>
				</parameters>
			</vfunc>
			<vfunc name="has_next">
				<return-type type="gboolean"/>
				<parameters>
					<parameter name="self" type="ZeitgeistResultSet*"/>
				</parameters>
			</vfunc>
			<vfunc name="next">
				<return-type type="ZeitgeistEvent*"/>
				<parameters>
					<parameter name="self" type="ZeitgeistResultSet*"/>
				</parameters>
			</vfunc>
			<vfunc name="peek">
				<return-type type="ZeitgeistEvent*"/>
				<parameters>
					<parameter name="self" type="ZeitgeistResultSet*"/>
				</parameters>
			</vfunc>
			<vfunc name="seek">
				<return-type type="void"/>
				<parameters>
					<parameter name="self" type="ZeitgeistResultSet*"/>
					<parameter name="pos" type="guint"/>
				</parameters>
			</vfunc>
			<vfunc name="size">
				<return-type type="guint"/>
				<parameters>
					<parameter name="self" type="ZeitgeistResultSet*"/>
				</parameters>
			</vfunc>
			<vfunc name="tell">
				<return-type type="guint"/>
				<parameters>
					<parameter name="self" type="ZeitgeistResultSet*"/>
				</parameters>
			</vfunc>
		</interface>
		<constant name="ZEITGEIST_DATA_SOURCE_VARIANT_SIGNATURE" type="char*" value="(sssa(asaasay))"/>
		<constant name="ZEITGEIST_DATA_SOURCE_WITH_INFO_VARIANT_SIGNATURE" type="char*" value="(sssa(asaasay)bxb)"/>
		<constant name="ZEITGEIST_EVENT_VARIANT_SIGNATURE" type="char*" value="(asaasay)"/>
		<constant name="ZEITGEIST_NCAL_ALARM" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Alarm"/>
		<constant name="ZEITGEIST_NCAL_ATTACHMENT" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Attachment"/>
		<constant name="ZEITGEIST_NCAL_CALENDAR" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Calendar"/>
		<constant name="ZEITGEIST_NCAL_CALENDAR_DATA_OBJECT" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#CalendarDataObject"/>
		<constant name="ZEITGEIST_NCAL_EVENT" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Event"/>
		<constant name="ZEITGEIST_NCAL_FREEBUSY" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Freebusy"/>
		<constant name="ZEITGEIST_NCAL_JOURNAL" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Journal"/>
		<constant name="ZEITGEIST_NCAL_TIMEZONE" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Timezone"/>
		<constant name="ZEITGEIST_NCAL_TODO" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#Todo"/>
		<constant name="ZEITGEIST_NCO_CONTACT" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nco#Contact"/>
		<constant name="ZEITGEIST_NCO_CONTACT_GROUP" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nco#ContactGroup"/>
		<constant name="ZEITGEIST_NCO_CONTACT_LIST" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nco#ContactList"/>
		<constant name="ZEITGEIST_NCO_CONTACT_LIST_DATA_OBJECT" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nco#ContactListDataObject"/>
		<constant name="ZEITGEIST_NCO_ORGANIZATION_CONTACT" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nco#OrganizationContact"/>
		<constant name="ZEITGEIST_NCO_PERSON_CONTACT" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nco#PersonContact"/>
		<constant name="ZEITGEIST_NFO_APPLICATION" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Application"/>
		<constant name="ZEITGEIST_NFO_ARCHIVE" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Archive"/>
		<constant name="ZEITGEIST_NFO_ARCHIVE_ITEM" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#ArchiveItem"/>
		<constant name="ZEITGEIST_NFO_ATTACHMENT" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Attachment"/>
		<constant name="ZEITGEIST_NFO_AUDIO" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Audio"/>
		<constant name="ZEITGEIST_NFO_BOOKMARK" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Bookmark"/>
		<constant name="ZEITGEIST_NFO_BOOKMARK_FOLDER" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#BookmarkFolder"/>
		<constant name="ZEITGEIST_NFO_CURSOR" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Cursor"/>
		<constant name="ZEITGEIST_NFO_DATA_CONTAINER" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#DataContainer"/>
		<constant name="ZEITGEIST_NFO_DELETED_RESOURCE" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#DeletedResource"/>
		<constant name="ZEITGEIST_NFO_DOCUMENT" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Document"/>
		<constant name="ZEITGEIST_NFO_EMBEDDED_FILE_DATA_OBJECT" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#EmbeddedFileDataObject"/>
		<constant name="ZEITGEIST_NFO_EXECUTABLE" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Executable"/>
		<constant name="ZEITGEIST_NFO_FILESYSTEM" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Filesystem"/>
		<constant name="ZEITGEIST_NFO_FILESYSTEM_IMAGE" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#FilesystemImage"/>
		<constant name="ZEITGEIST_NFO_FILE_DATA_OBJECT" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#FileDataObject"/>
		<constant name="ZEITGEIST_NFO_FOLDER" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Folder"/>
		<constant name="ZEITGEIST_NFO_FONT" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Font"/>
		<constant name="ZEITGEIST_NFO_HARD_DISK_PARTITION" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#HardDiskPartition"/>
		<constant name="ZEITGEIST_NFO_HTML_DOCUMENT" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#HtmlDocument"/>
		<constant name="ZEITGEIST_NFO_ICON" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Icon"/>
		<constant name="ZEITGEIST_NFO_IMAGE" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Image"/>
		<constant name="ZEITGEIST_NFO_MEDIA" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Media"/>
		<constant name="ZEITGEIST_NFO_MEDIA_LIST" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#MediaList"/>
		<constant name="ZEITGEIST_NFO_MEDIA_STREAM" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#MediaStream"/>
		<constant name="ZEITGEIST_NFO_MIND_MAP" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#MindMap"/>
		<constant name="ZEITGEIST_NFO_OPERATING_SYSTEM" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#OperatingSystem"/>
		<constant name="ZEITGEIST_NFO_PAGINATED_TEXT_DOCUMENT" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#PaginatedTextDocument"/>
		<constant name="ZEITGEIST_NFO_PLAIN_TEXT_DOCUMENT" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#PlainTextDocument"/>
		<constant name="ZEITGEIST_NFO_PRESENTATION" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Presentation"/>
		<constant name="ZEITGEIST_NFO_RASTER_IMAGE" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#RasterImage"/>
		<constant name="ZEITGEIST_NFO_REMOTE_DATA_OBJECT" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#RemoteDataObject"/>
		<constant name="ZEITGEIST_NFO_REMOTE_PORT_ADDRESS" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#RemotePortAddress"/>
		<constant name="ZEITGEIST_NFO_SOFTWARE" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Software"/>
		<constant name="ZEITGEIST_NFO_SOFTWARE_ITEM" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#SoftwareItem"/>
		<constant name="ZEITGEIST_NFO_SOFTWARE_SERVICE" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#SoftwareService"/>
		<constant name="ZEITGEIST_NFO_SOURCE_CODE" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#SourceCode"/>
		<constant name="ZEITGEIST_NFO_SPREADSHEET" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Spreadsheet"/>
		<constant name="ZEITGEIST_NFO_TEXT_DOCUMENT" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#TextDocument"/>
		<constant name="ZEITGEIST_NFO_TRASH" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Trash"/>
		<constant name="ZEITGEIST_NFO_VECTOR_IMAGE" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#VectorImage"/>
		<constant name="ZEITGEIST_NFO_VIDEO" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Video"/>
		<constant name="ZEITGEIST_NFO_VISUAL" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Visual"/>
		<constant name="ZEITGEIST_NFO_WEBSITE" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Website"/>
		<constant name="ZEITGEIST_NFO_WEB_DATA_OBJECT" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#WebDataObject"/>
		<constant name="ZEITGEIST_NMM_MOVIE" type="char*" value="http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#Movie"/>
		<constant name="ZEITGEIST_NMM_MUSIC_ALBUM" type="char*" value="http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#MusicAlbum"/>
		<constant name="ZEITGEIST_NMM_MUSIC_PIECE" type="char*" value="http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#MusicPiece"/>
		<constant name="ZEITGEIST_NMM_TVSERIES" type="char*" value="http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#TVSeries"/>
		<constant name="ZEITGEIST_NMM_TVSHOW" type="char*" value="http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#TVShow"/>
		<constant name="ZEITGEIST_NMO_EMAIL" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#Email"/>
		<constant name="ZEITGEIST_NMO_IMMESSAGE" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#IMMessage"/>
		<constant name="ZEITGEIST_NMO_MAILBOX" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#Mailbox"/>
		<constant name="ZEITGEIST_NMO_MAILBOX_DATA_OBJECT" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#MailboxDataObject"/>
		<constant name="ZEITGEIST_NMO_MESSAGE" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#Message"/>
		<constant name="ZEITGEIST_NMO_MIME_ENTITY" type="char*" value="http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#MimeEntity"/>
		<constant name="ZEITGEIST_TIME_RANGE_VARIANT_SIGNATURE" type="char*" value="(xx)"/>
		<constant name="ZEITGEIST_ZG_ACCEPT_EVENT" type="char*" value="http://www.zeitgeist-project.com/ontologies/2010/01/27/zg#AcceptEvent"/>
		<constant name="ZEITGEIST_ZG_ACCESS_EVENT" type="char*" value="http://www.zeitgeist-project.com/ontologies/2010/01/27/zg#AccessEvent"/>
		<constant name="ZEITGEIST_ZG_CREATE_EVENT" type="char*" value="http://www.zeitgeist-project.com/ontologies/2010/01/27/zg#CreateEvent"/>
		<constant name="ZEITGEIST_ZG_DELETE_EVENT" type="char*" value="http://www.zeitgeist-project.com/ontologies/2010/01/27/zg#DeleteEvent"/>
		<constant name="ZEITGEIST_ZG_DENY_EVENT" type="char*" value="http://www.zeitgeist-project.com/ontologies/2010/01/27/zg#DenyEvent"/>
		<constant name="ZEITGEIST_ZG_EVENT_INTERPRETATION" type="char*" value="http://www.zeitgeist-project.com/ontologies/2010/01/27/zg#EventInterpretation"/>
		<constant name="ZEITGEIST_ZG_EVENT_MANIFESTATION" type="char*" value="http://www.zeitgeist-project.com/ontologies/2010/01/27/zg#EventManifestation"/>
		<constant name="ZEITGEIST_ZG_EXPIRE_EVENT" type="char*" value="http://www.zeitgeist-project.com/ontologies/2010/01/27/zg#ExpireEvent"/>
		<constant name="ZEITGEIST_ZG_HEURISTIC_ACTIVITY" type="char*" value="http://www.zeitgeist-project.com/ontologies/2010/01/27/zg#HeuristicActivity"/>
		<constant name="ZEITGEIST_ZG_LEAVE_EVENT" type="char*" value="http://www.zeitgeist-project.com/ontologies/2010/01/27/zg#LeaveEvent"/>
		<constant name="ZEITGEIST_ZG_MODIFY_EVENT" type="char*" value="http://www.zeitgeist-project.com/ontologies/2010/01/27/zg#ModifyEvent"/>
		<constant name="ZEITGEIST_ZG_MOVE_EVENT" type="char*" value="http://www.zeitgeist-project.com/ontologies/2010/01/27/zg#MoveEvent"/>
		<constant name="ZEITGEIST_ZG_RECEIVE_EVENT" type="char*" value="http://www.zeitgeist-project.com/ontologies/2010/01/27/zg#ReceiveEvent"/>
		<constant name="ZEITGEIST_ZG_SCHEDULED_ACTIVITY" type="char*" value="http://www.zeitgeist-project.com/ontologies/2010/01/27/zg#ScheduledActivity"/>
		<constant name="ZEITGEIST_ZG_SEND_EVENT" type="char*" value="http://www.zeitgeist-project.com/ontologies/2010/01/27/zg#SendEvent"/>
		<constant name="ZEITGEIST_ZG_SYSTEM_NOTIFICATION" type="char*" value="http://www.zeitgeist-project.com/ontologies/2010/01/27/zg#SystemNotification"/>
		<constant name="ZEITGEIST_ZG_USER_ACTIVITY" type="char*" value="http://www.zeitgeist-project.com/ontologies/2010/01/27/zg#UserActivity"/>
		<constant name="ZEITGEIST_ZG_WORLD_ACTIVITY" type="char*" value="http://www.zeitgeist-project.com/ontologies/2010/01/27/zg#WorldActivity"/>
	</namespace>
</api>
