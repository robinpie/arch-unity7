/*
 * Copyright (C) 2010 Canonical, Ltd.
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Authored by
 *             Mikkel Kamstrup Erlandsen <mikkel.kamstrup@canonical.com>
 */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "zeitgeist-marshal.h"
#include "zeitgeist-monitor.h"
#include "zeitgeist-result-set.h"
#include "zeitgeist-simple-result-set.h"

/**
 * SECTION:zeitgeist-monitor
 * @short_description: Listens for updates to the Zeitgeist event log
 * @include: zeitgeist.h
 *
 * A #ZeitgeistMonitor listens for updates to the Zeitgeist event log
 * matching a given set of templates and with timestamps in some predefined
 * time range.
 *
 * A monitor must be installed into the running Zeitgeist daemon by calling
 * zeitgeist_log_install_monitor(). The monitor will not emit any of the
 * ::events-added or ::events-deleted signals before this.
 */

static gint _monitor_counter = 0;

typedef struct
{
  ZeitgeistTimeRange *time_range;
  GPtrArray          *event_templates;

  /* Client side DBus path the monitor lives under */
  gchar              *monitor_path;
} ZeitgeistMonitorPrivate;

/* Property ids */
enum
{
	PROP_0,

	PROP_TIME_RANGE,
  PROP_EVENT_TEMPLATES,
	
	LAST_PROPERTY
};

/* signal ids */
enum
{
  EVENTS_INSERTED,
  EVENTS_DELETED,
  
  LAST_SIGNAL
};

static guint monitor_signals[LAST_SIGNAL] = { 0 };

G_DEFINE_TYPE (ZeitgeistMonitor, zeitgeist_monitor, G_TYPE_OBJECT);

#define ZEITGEIST_MONITOR_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE(obj, ZEITGEIST_TYPE_MONITOR, ZeitgeistMonitorPrivate))

static void
zeitgeist_monitor_init (ZeitgeistMonitor *object)
{
  ZeitgeistMonitorPrivate *priv = ZEITGEIST_MONITOR_GET_PRIVATE (object);

  priv->monitor_path = g_strdup_printf ("/org/gnome/zeitgeist/monitor/%i",
                                        _monitor_counter++);
}

static void
zeitgeist_monitor_finalize (GObject *object)
{
  ZeitgeistMonitor *monitor = ZEITGEIST_MONITOR (object);
  ZeitgeistMonitorPrivate *priv;
  
  priv = ZEITGEIST_MONITOR_GET_PRIVATE (monitor);

  if (priv->time_range)
    {
      g_object_unref (priv->time_range);
    }
  if (priv->event_templates)
    {
      g_ptr_array_unref (priv->event_templates);
    }
  if (priv->monitor_path)
    {
      g_free (priv->monitor_path);
    }
  
  G_OBJECT_CLASS (zeitgeist_monitor_parent_class)->finalize (object); 
}

static void
zeitgeist_monitor_get_property (GObject    *object,
                                guint       prop_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
  ZeitgeistMonitorPrivate *priv = ZEITGEIST_MONITOR_GET_PRIVATE (object);

  switch (prop_id)
    {
      case PROP_TIME_RANGE:
        g_value_set_object (value, priv->time_range);
        return;
      case PROP_EVENT_TEMPLATES:
        g_value_set_boxed (value, priv->event_templates);
        return;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        return;
        break;
    }
}

static void
zeitgeist_monitor_set_property (GObject      *object,
                                guint         prop_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
  ZeitgeistMonitorPrivate *priv = ZEITGEIST_MONITOR_GET_PRIVATE (object);

  switch (prop_id)
    {
      case PROP_TIME_RANGE:
        priv->time_range = g_value_get_object (value);
        g_object_ref_sink (priv->time_range);
        return;
      case PROP_EVENT_TEMPLATES:
        /* By contract we own the ref to the event_templates
         * passed to the constructor */
        priv->event_templates = (GPtrArray*) g_value_get_boxed (value);
        g_ptr_array_foreach (priv->event_templates,
                             (GFunc) g_object_ref_sink, NULL);
        return;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        return;
        break;
    }
}


static void
zeitgeist_monitor_class_init (ZeitgeistMonitorClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GParamSpec   *pspec;
  
  object_class->finalize = zeitgeist_monitor_finalize;
  object_class->get_property = zeitgeist_monitor_get_property;
  object_class->set_property = zeitgeist_monitor_set_property;
  
  /**
	 * ZeitgeistMonitor:time-range:
	 * 
	 * Events must have timestamps within this timerange in order to trigger
   * the monitor.
	 */
  pspec = g_param_spec_object ("time-range",
                               "Time range",
                               "Events must have timestamps within this time range",
                               ZEITGEIST_TYPE_TIME_RANGE,
                               G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);
	g_object_class_install_property (object_class,
	                                 PROP_TIME_RANGE,
	                                 pspec);

  /**
	 * ZeitgeistMonitor:event-templates:
	 * 
	 * Events must match at least one these templates in order to trigger the
   * monitor
	 */
  pspec = g_param_spec_boxed ("event-templates",
                              "Event templates",
                              "Events must match one of these templates",
                              G_TYPE_PTR_ARRAY,
                              G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);
	g_object_class_install_property (object_class,
	                                 PROP_EVENT_TEMPLATES,
	                                 pspec);

  /**
   * ZeitgeistMonitor::events-inserted:
   * @time_range: A #ZeitgeistTimeRange that specifies the minimum and maximum
   *              of the timestamps in @events
   * @events: A #ZeitgeistResultSet holding the #ZeitgeistEvent<!-- -->s that
   *          have been inserted into the log
   *
   * Emitted when events matching the event templates and with timestamps
   * within the time range of the monitor has been inserted into the log.
   */
  monitor_signals[EVENTS_INSERTED] =
	g_signal_new ("events-inserted",
	              G_TYPE_FROM_CLASS (klass),
	              G_SIGNAL_RUN_LAST,
	              G_STRUCT_OFFSET (ZeitgeistMonitorClass, events_inserted),
	              NULL, NULL,
	              _zeitgeist_cclosure_marshal_VOID__OBJECT_OBJECT,
	              G_TYPE_NONE,
	              2, ZEITGEIST_TYPE_TIME_RANGE, ZEITGEIST_TYPE_RESULT_SET);

  /**
   * ZeitgeistMonitor::events-deleted:
   * @time_range: A #ZeitgeistTimeRange that specifies the minimum and maximum
   *              timestamps of the deleted events
   * @event_ids: A #GArray of #guint32<!-- -->s holding the ids of the deleted
   *             events
   *
   * Emitted when events with timestamps within the time range of this monitor
   * has been deleted from the log. Note that the deleted events may not match
   * the event templates for the monitor.
   */
  monitor_signals[EVENTS_DELETED] =
	g_signal_new ("events-deleted",
	              G_TYPE_FROM_CLASS (klass),
	              G_SIGNAL_RUN_LAST,
	              G_STRUCT_OFFSET (ZeitgeistMonitorClass, events_deleted),
	              NULL, NULL,
	              _zeitgeist_cclosure_marshal_VOID__OBJECT_BOXED,
	              G_TYPE_NONE,
	              2, ZEITGEIST_TYPE_TIME_RANGE, G_TYPE_ARRAY);
  
  g_type_class_add_private (object_class, sizeof (ZeitgeistMonitorPrivate));
}

/**
 * zeitgeist_monitor_new
 * @time_range: The monitor will only listen for events with timestamps within
 *              this time range. Note that it is legal for applications to insert
 *              events that are "in the past".
 * @event_templates: A #GPtrArray of #ZeitgeistEvent<!-- -->s. Only listen for
 *                   events that match any of these templates.
 *                   The monitor will assume ownership of the events and
 *                   the pointer array.
 *                   If you want to keep a reference for yourself you must do a
 *                   g_ptr_array_ref() on @event_templates as well as reffing
 *                   the events held by it before calling this method.
 *
 * Create a new monitor. Before you can receive signals from the monitor you
 * need to install it in the running Zeitgeist daemon by calling
 * zeitgeist_log_install_monitor().
 *
 * Returns: A reference to a newly allocated monitor.
 */
ZeitgeistMonitor*
zeitgeist_monitor_new (ZeitgeistTimeRange *time_range,
                       GPtrArray          *event_templates)
{
  ZeitgeistMonitor        *monitor;

  g_return_val_if_fail (ZEITGEIST_IS_TIME_RANGE (time_range), NULL);
  g_return_val_if_fail (event_templates != NULL, NULL);

  monitor = (ZeitgeistMonitor*) g_object_new (ZEITGEIST_TYPE_MONITOR,
                                              "time-range", time_range,
                                              "event-templates", event_templates,
                                              NULL);

  return monitor;
}

ZeitgeistTimeRange*
zeitgeist_monitor_get_time_range (ZeitgeistMonitor   *self)
{
  ZeitgeistMonitorPrivate *priv;

  g_return_val_if_fail (ZEITGEIST_IS_MONITOR (self), NULL);
  
  priv = ZEITGEIST_MONITOR_GET_PRIVATE (self);
  return priv->time_range;
}

GPtrArray*
zeitgeist_monitor_get_templates  (ZeitgeistMonitor   *self)
{
  ZeitgeistMonitorPrivate *priv;

  g_return_val_if_fail (ZEITGEIST_IS_MONITOR (self), NULL);
  
  priv = ZEITGEIST_MONITOR_GET_PRIVATE (self);
  return priv->event_templates;
}

const gchar*
zeitgeist_monitor_get_path (ZeitgeistMonitor   *self)
{
  ZeitgeistMonitorPrivate *priv;

  g_return_val_if_fail (ZEITGEIST_IS_MONITOR (self), NULL);
  
  priv = ZEITGEIST_MONITOR_GET_PRIVATE (self);
  return priv->monitor_path;
}
