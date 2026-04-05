/**
 * @file geis_dbus_subscription.c
 * @brief Implementation of the GEIS DBus subscription transport.
 */

/*
 * Copyright 2011 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of version 3 of the GNU Lesser General Public License as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "geis_config.h"
#include "geis_dbus_subscription.h"

#include "geis_dbus.h"
#include "geis_dbus_attr.h"
#include "geis_filter_term.h"
#include "geis_logging.h"
#include "geis_subscription.h"
#include <stdint.h>


/*
 * A filter term is marshalled as a (facility, operation, value) tuple.
 * That would be a (ii(sv)) in DBusspeak.
 */
#define GEIS_DBUS_TYPE_SIGNATURE_TERM \
             DBUS_STRUCT_BEGIN_CHAR_AS_STRING \
             DBUS_TYPE_INT32_AS_STRING \
             DBUS_TYPE_INT32_AS_STRING \
             GEIS_DBUS_TYPE_SIGNATURE_ATTR \
             DBUS_STRUCT_END_CHAR_AS_STRING

/*
 * A term list is an array of terms, as in a(ii(sv)).
 */
#define GEIS_DBUS_TYPE_SIGNATURE_TERM_LIST \
             DBUS_TYPE_ARRAY_AS_STRING \
             GEIS_DBUS_TYPE_SIGNATURE_TERM

/*
 * A filter is a named array of filter terms.
 * That's a {sa(ii(sv))} in the DBus tongue.
 */
#define GEIS_DBUS_TYPE_SIGNATURE_FILTER \
             DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING \
             DBUS_TYPE_STRING_AS_STRING \
             GEIS_DBUS_TYPE_SIGNATURE_TERM_LIST \
             DBUS_DICT_ENTRY_END_CHAR_AS_STRING


/**
 * Adds filter terms to a DBus message.
 *
 * @param[in] filter      The filter for which terms will be marshalled.
 * @param[in] filter_iter A DBus message output iterator.
 */
static void
_geis_dbus_marshall_subscription_filter_terms(GeisFilter       filter,
                                              DBusMessageIter *filter_iter)
{
  DBusMessageIter term_list_iter;
  dbus_message_iter_open_container(filter_iter,
                                   DBUS_TYPE_ARRAY,
                                   GEIS_DBUS_TYPE_SIGNATURE_TERM,
                                   &term_list_iter);
  for (GeisSize i = 0; i < geis_filter_term_count(filter); ++i)
  {
    GeisFilterTerm term = geis_filter_term(filter, i);
    dbus_int32_t facility = geis_filter_term_facility(term);
    dbus_int32_t operation = geis_filter_term_operation(term);
    GeisAttr     attr = geis_filter_term_attr(term);

    DBusMessageIter term_iter;
    dbus_message_iter_open_container(&term_list_iter,
                                     DBUS_TYPE_STRUCT,
                                     NULL,
                                     &term_iter);
    dbus_message_iter_append_basic(&term_iter, DBUS_TYPE_INT32, &facility);
    dbus_message_iter_append_basic(&term_iter, DBUS_TYPE_INT32, &operation);
    geis_dbus_attr_marshall(attr, &term_iter);
    dbus_message_iter_close_container(&term_list_iter, &term_iter);
  }
  dbus_message_iter_close_container(filter_iter, &term_list_iter);
}


/**
 * Squeezes the filters on a subscription into the DBus wire protocol.
 *
 * @param[in] sub      A %GeisSubscription
 * @param[in] msg_iter The open output iterator for a DBus message.
 *
 * The filters are marshalled as an array of DBus dict entries.
 */
static void
_geis_dbus_marshall_subscription_filters(GeisSubscription  subscription,
                                         DBusMessageIter  *msg_iter)
{
  DBusMessageIter filter_list_iter;

  dbus_message_iter_open_container(msg_iter,
                                   DBUS_TYPE_ARRAY,
                                   GEIS_DBUS_TYPE_SIGNATURE_FILTER,
                                   &filter_list_iter);

  for (GeisFilterIterator it = geis_subscription_filter_begin(subscription);
       it != geis_subscription_filter_end(subscription);
       it = geis_subscription_filter_next(subscription, it))
  {
    const char *filter_name = geis_filter_name(*it);

    DBusMessageIter filter_iter;
    dbus_message_iter_open_container(&filter_list_iter,
                                     DBUS_TYPE_DICT_ENTRY,
                                     NULL,
                                     &filter_iter);
    dbus_message_iter_append_basic(&filter_iter, DBUS_TYPE_STRING, &filter_name);
    _geis_dbus_marshall_subscription_filter_terms(*it, &filter_iter);
    dbus_message_iter_close_container(&filter_list_iter, &filter_iter);
  }
  dbus_message_iter_close_container(msg_iter, &filter_list_iter);
}


static void
_geis_dbus_unmarshall_filter_terms(GeisFilter       filter,
                                   DBusMessageIter *filter_iter)
{
  DBusMessageIter term_list_iter;
  dbus_message_iter_recurse(filter_iter, &term_list_iter);
  for (int dtype = dbus_message_iter_get_arg_type(&term_list_iter);
       dtype != DBUS_TYPE_INVALID;
       dbus_message_iter_next(&term_list_iter),
       dtype = dbus_message_iter_get_arg_type(&term_list_iter))
  {
    int ttype = dbus_message_iter_get_arg_type(&term_list_iter);
    if (ttype != DBUS_TYPE_STRUCT)
    {
      geis_error("malformed GeisSubscription term");
      goto final_exit;
    }

    DBusMessageIter term_iter;
    dbus_message_iter_recurse(&term_list_iter, &term_iter);

    dbus_int32_t facility;
    dbus_message_iter_get_basic(&term_iter, &facility);
    dbus_message_iter_next(&term_iter);

    dbus_int32_t operation;
    dbus_message_iter_get_basic(&term_iter, &operation);
    dbus_message_iter_next(&term_iter);

    GeisAttr attr = geis_dbus_attr_unmarshall(&term_iter);
    GeisFilterTerm term = geis_filter_term_new(facility, operation, attr);
    geis_filter_add_term_internal(filter, term);
  }

final_exit:
  return;
}


/**
 * Unmarshalls a filter from a DBus message.
 *
 * @param[in] geis         A GEIS instance.
 * @param[in] filter_iter  A DBus message iterator pointing to the filter.
 */
static GeisFilter
_geis_dbus_unmarshall_filter(Geis geis, DBusMessageIter *filter_iter)
{
  GeisFilter filter = NULL;

  int ftype = dbus_message_iter_get_arg_type(filter_iter);
  if (ftype != DBUS_TYPE_DICT_ENTRY)
  {
    geis_error("malformed GeisSubscription filter");
    goto final_exit;
  }

  DBusMessageIter dict_iter;
  dbus_message_iter_recurse(filter_iter, &dict_iter);

  ftype = dbus_message_iter_get_arg_type(&dict_iter);
  if (ftype != DBUS_TYPE_STRING)
  {
    geis_error("malformed GeisSubscription filter");
    goto final_exit;
  }
  GeisString filter_name;
  dbus_message_iter_get_basic(&dict_iter, &filter_name);
  dbus_message_iter_next(&dict_iter);

  filter = geis_filter_new(geis, filter_name);

  ftype = dbus_message_iter_get_arg_type(&dict_iter);
  if (ftype != DBUS_TYPE_ARRAY)
  {
    geis_error("malformed GeisSubscription filter");
    goto final_exit;
  }
  _geis_dbus_unmarshall_filter_terms(filter, &dict_iter);

final_exit:
  return filter;
}


/**
 * Unmarshalls a list of filters from a DBus message.
 *
 * @param[in] geis               A GEIS instance.
 * @param[in] subscription_iter  A DBus message iterator for the subscription.
 * @param[in] subscription       A GEIS subsccription.
 *
 * This function unmarshalls filters from a GEIS DBus subscription message and
 * adds them to a existing GEIS subscription.
 */
static void
_geis_dbus_unmarshall_subscription_filters(Geis              geis,
                                           DBusMessageIter  *subscription_iter,
                                           GeisSubscription  subscription)
{
  DBusMessageIter filter_list_iter;
  dbus_message_iter_recurse(subscription_iter, &filter_list_iter);
  for (int dtype = dbus_message_iter_get_arg_type(&filter_list_iter);
       dtype != DBUS_TYPE_INVALID;
       dbus_message_iter_next(&filter_list_iter),
       dtype = dbus_message_iter_get_arg_type(&filter_list_iter))
  {
    GeisFilter filter = _geis_dbus_unmarshall_filter(geis, &filter_list_iter);
    if (filter)
    {
      geis_subscription_add_filter(subscription, filter);
    }
  }
}


/*
 * Indicates if a DBus message is a GEIS_DBUS_SUBSCRIPTION_CREATE method call.
 */
GeisBoolean
geis_dbus_message_is_subscription_create_call(DBusMessage *message)
{
  return dbus_message_is_method_call(message,
                                     GEIS_DBUS_SERVICE_INTERFACE,
                                     GEIS_DBUS_SUBSCRIPTION_CREATE);
}


/*
 * Creates a GEIS_DBUS_SUBSCRIPTION_CREATE method call message.
 */
DBusMessage *
geis_dbus_subscription_create_call_message(GeisSubscription subscription)
{
  DBusMessage  *message   = NULL;
  GeisString    sub_name  = "dummy";
  dbus_int32_t  sub_id    = -1;
  dbus_uint32_t sub_flags = 0;
  DBusMessageIter iter;

  message = dbus_message_new_method_call(GEIS_DBUS_SERVICE_INTERFACE,
                                         GEIS_DBUS_SERVICE_PATH,
                                         GEIS_DBUS_SERVICE_INTERFACE,
                                         GEIS_DBUS_SUBSCRIPTION_CREATE);

  if (subscription)
  {
    sub_name  = geis_subscription_name(subscription);
    sub_id    = geis_subscription_id(subscription);
    sub_flags = geis_subscription_flags(subscription);
  }
  dbus_message_iter_init_append(message, &iter);

  dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &sub_name);
  dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32,  &sub_id);
  dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32,  &sub_flags);
  _geis_dbus_marshall_subscription_filters(subscription, &iter);

  return message;
}


/*
 * Creates a %GeisSubscription from a method call message.
 */
GeisSubscription
geis_dbus_subscription_from_create_call_message(Geis geis, DBusMessage *message)
{
  DBusMessageIter  message_iter;
  dbus_message_iter_init(message, &message_iter);

  GeisString       client_sub_name;
  dbus_message_iter_get_basic(&message_iter, &client_sub_name);
  dbus_message_iter_next(&message_iter);

  dbus_int32_t     client_sub_id;
  dbus_message_iter_get_basic(&message_iter, &client_sub_id);
  dbus_message_iter_next(&message_iter);

  dbus_uint32_t    client_sub_flags;
  dbus_message_iter_get_basic(&message_iter, &client_sub_flags);
  dbus_message_iter_next(&message_iter);

  GeisSubscription subscription = NULL;
  subscription = geis_subscription_new(geis, client_sub_name, client_sub_flags);
  if (!subscription)
  {
    geis_error("error creating proxy subscription");
    goto final_exit;
  }
  intptr_t fudge = client_sub_id;
  geis_subscription_set_pdata(subscription, (GeisPointer)fudge);

  int dtype = dbus_message_iter_get_arg_type(&message_iter);
  if (dtype != DBUS_TYPE_ARRAY)
  {
    geis_error("malformed GeisSubscription message"
               " (expected type %c, received type %c)",
               DBUS_TYPE_ARRAY, dtype);
    goto final_exit;
  }

  _geis_dbus_unmarshall_subscription_filters(geis, &message_iter, subscription);

final_exit:
  return subscription;
}


/*
 * Creates a GEIS_DBUS_SUBSCRIPTION_CREATE method return message.
 */
DBusMessage *
geis_dbus_subscription_create_return_message(DBusMessage      *message,
                                             GeisSubscription  subscription)
{
  DBusMessage *reply = dbus_message_new_method_return(message);
  intptr_t fudge = (intptr_t)geis_subscription_pdata(subscription);
  dbus_int32_t client_sub_id = fudge;
  dbus_int32_t server_sub_id = geis_subscription_id(subscription);
  dbus_message_append_args(reply,
                           DBUS_TYPE_INT32, &client_sub_id,
                           DBUS_TYPE_INT32, &server_sub_id,
                           DBUS_TYPE_INVALID);

  return reply;
}


/*
 * Indicates if a DBus message is a GEIS_DBUS_SUBSCRIPTION_ACTIVATE message.
 */
GeisBoolean
geis_dbus_message_is_subscription_activate_call(DBusMessage *message)
{
  return dbus_message_is_method_call(message,
                                     GEIS_DBUS_SERVICE_INTERFACE,
                                     GEIS_DBUS_SUBSCRIPTION_ACTIVATE);
}


/*
 * Creates a GEIS_DBUS_SUBSCRIPTION_ACTIVATE method call message.
 */
DBusMessage *
geis_dbus_subscription_activate_call_message(GeisSubscription subscription)
{
  DBusMessage *message   = NULL;
  DBusMessageIter iter;

  message = dbus_message_new_method_call(GEIS_DBUS_SERVICE_INTERFACE,
                                         GEIS_DBUS_SERVICE_PATH,
                                         GEIS_DBUS_SERVICE_INTERFACE,
                                         GEIS_DBUS_SUBSCRIPTION_ACTIVATE);
  dbus_message_iter_init_append(message, &iter);

  dbus_int32_t subscription_id = geis_subscription_id(subscription);
  dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &subscription_id);
  _geis_dbus_marshall_subscription_filters(subscription, &iter);
  return message;
}


/*
 * Creates a GEIS_DBUS_SUBSCRIPTION_ACTIVATE method return message.
 */
DBusMessage *
geis_dbus_subscription_activate_return_message(DBusMessage      *message,
                                               GeisSubscription  subscription)
{
  DBusMessage *reply   = NULL;
  reply = dbus_message_new_method_return(message);
  dbus_int32_t subscription_id = -1;

  if (subscription)
  {
    subscription_id = geis_subscription_id(subscription);
  }
  dbus_message_append_args(reply,
                           DBUS_TYPE_INT32, &subscription_id,
                           DBUS_TYPE_INVALID);
  return reply;
}


/*
 * Indicates if a DBus message is a GEIS_DBUS_SUBSCRIPTION_DEACTIVATE message.
 */
GeisBoolean
geis_dbus_message_is_subscription_deactivate_call(DBusMessage *message)
{
  return dbus_message_is_method_call(message,
                                     GEIS_DBUS_SERVICE_INTERFACE,
                                     GEIS_DBUS_SUBSCRIPTION_DEACTIVATE);
}


/*
 * Creates a GEIS_DBUS_SUBSCRIPTION_DEACTIVATE method call message.
 */
DBusMessage *
geis_dbus_subscription_deactivate_call_message(GeisSubscription subscription GEIS_UNUSED)
{
  DBusMessage *message   = NULL;
  message = dbus_message_new_method_call(GEIS_DBUS_SERVICE_INTERFACE,
                                         GEIS_DBUS_SERVICE_PATH,
                                         GEIS_DBUS_SERVICE_INTERFACE,
                                         GEIS_DBUS_SUBSCRIPTION_DEACTIVATE);
  return message;
}


/**
 */
DBusMessage *
geis_dbus_subscription_deactivate_return_message(DBusMessage      *message,
                                                 GeisSubscription  subscription)
{
  DBusMessage *reply   = NULL;
  reply = dbus_message_new_method_return(message);
  dbus_int32_t subscription_id = geis_subscription_id(subscription);
  dbus_message_append_args(reply,
                           DBUS_TYPE_INT32, &subscription_id,
                           DBUS_TYPE_INVALID);
  return reply;
}


/*
 * Indicates if a DBus message is a GEIS_DBUS_SUBSCRIPTION_DESTROY message.
 */
GeisBoolean
geis_dbus_message_is_subscription_destroy_call(DBusMessage *message)
{
  return dbus_message_is_method_call(message,
                                     GEIS_DBUS_SERVICE_INTERFACE,
                                     GEIS_DBUS_SUBSCRIPTION_DESTROY);
}


/*
 * Creates a GEIS_DBUS_SUBSCRIPTION_DESTROY method call message.
 */
DBusMessage *
geis_dbus_subscription_destroy_call_message(GeisSubscription subscription)
{
  DBusMessage  *message   = NULL;
  message = dbus_message_new_method_call(GEIS_DBUS_SERVICE_INTERFACE,
                                         GEIS_DBUS_SERVICE_PATH,
                                         GEIS_DBUS_SERVICE_INTERFACE,
                                         GEIS_DBUS_SUBSCRIPTION_DESTROY);

  dbus_int32_t server_sub_id = (intptr_t)geis_subscription_pdata(subscription);
  dbus_message_append_args(message,
                           DBUS_TYPE_INT32, &server_sub_id,
                           DBUS_TYPE_INVALID);

  return message;
}


/*
 * Creates a GEIS_DBUS_SUBSCRIPTION_DESTROY method return message.
 */
DBusMessage *
geis_dbus_subscription_destroy_return_message(DBusMessage *message)
{
 return dbus_message_new_method_return(message);
}


