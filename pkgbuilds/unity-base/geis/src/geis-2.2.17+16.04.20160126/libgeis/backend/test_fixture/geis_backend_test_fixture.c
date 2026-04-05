/**
 * @file geis_backend_test_fixture.c
 * @brief GEIS mock back end test fixture implementation
 *
 * Copyright 2010-2013 Canonical Ltd.
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
#include "geis_backend.h"
#include "geis_backend_protected.h"

#include "geis_attr.h"
#include "geis_class.h"
#include "geis_device.h"
#include "geis_event.h"
#include "geis_filter.h"
#include "geis_filter_term.h"
#include "geis_frame.h"
#include "geis_group.h"
#include "geis_logging.h"
#include "geis_private.h"
#include "geis_subscription.h"
#include "geis_test_api.h"
#include "geis_touch.h"
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>


static inline GeisSize
_min(GeisSize a, GeisSize b)
{
  return (a < b) ? a : b;
}


static inline GeisSize
_max(GeisSize a, GeisSize b)
{
  return (a > b) ? a : b;
}

static GeisSize   g_min_touches = 1;
static GeisSize   g_max_touches = 5;


typedef struct GeisBackendTestFixture
{
  Geis                tf_geis;
} *GeisBackendTestFixture;


typedef struct TestBackendToken
{
  struct GeisBackendToken base;
  GeisBackendTestFixture  be;
  GeisInteger             min_touches;
  GeisInteger             max_touches;
} *TestBackendToken;

static inline TestBackendToken
_tbtoken_from_token(GeisBackendToken bet)
{
  return (TestBackendToken)bet;
}

static GeisBackendToken _token_clone(GeisBackendToken);
static void             _token_finalize(GeisBackendToken);
static void             _token_compose(GeisBackendToken, GeisBackendToken);
static GeisStatus       _token_activate(GeisBackendToken, GeisSubscription);
static GeisStatus       _token_deactivate(GeisBackendToken, GeisSubscription);
static void             _token_free_subscription_pdata(GeisBackendToken, GeisSubscription);

static struct GeisBackendTokenVtable _token_vtbl = {
  _token_clone,
  _token_finalize,
  _token_compose,
  _token_activate,
  _token_deactivate,
  _token_free_subscription_pdata
};

static GeisGestureClass g_poke_class = NULL;

static GeisStatus
_add_device_term(GeisBackendToken     token GEIS_UNUSED,
                 void                *context GEIS_UNUSED,
                 GeisString           name,
                 GeisFilterOperation  op GEIS_UNUSED,
                 void                *value GEIS_UNUSED)
{
  GeisStatus status = GEIS_STATUS_SUCCESS;
  geis_error("called: name=%s", name);
  return status;
}


static void
_create_test_devices(GeisBackendTestFixture tf)
{
  GeisDevice device = geis_device_new("abs-test-device", 0);
  struct GeisFilterableAttribute attrs[] = {
    { GEIS_DEVICE_ATTRIBUTE_NAME,         GEIS_ATTR_TYPE_STRING,  _add_device_term, NULL },
    { GEIS_DEVICE_ATTRIBUTE_ID,           GEIS_ATTR_TYPE_INTEGER, _add_device_term, NULL },
    { GEIS_DEVICE_ATTRIBUTE_TOUCHES,      GEIS_ATTR_TYPE_INTEGER, _add_device_term, NULL },
    { GEIS_DEVICE_ATTRIBUTE_DIRECT_TOUCH, GEIS_ATTR_TYPE_BOOLEAN, _add_device_term, NULL }
  };
  GeisSize attr_count = sizeof(attrs) / sizeof(struct GeisFilterableAttribute);

  geis_register_device(tf->tf_geis, device, attr_count, attrs);
}


static GeisStatus
_add_class_term(GeisBackendToken     gbtoken,
                void                *context GEIS_UNUSED,
                GeisString           name,
                GeisFilterOperation  op,
                void                *value)
{
  GeisStatus status = GEIS_STATUS_SUCCESS;
  TestBackendToken token = _tbtoken_from_token(gbtoken);

  if (0 == strcmp(name, GEIS_CLASS_ATTRIBUTE_NAME)
      && op == GEIS_FILTER_OP_EQ)
  {
    GeisString class_name = (GeisString)value;
    geis_debug("called: attr=%s name=\"%s\"", name, class_name);
  }
  else if (0 == strcmp(name, GEIS_GESTURE_ATTRIBUTE_TOUCHES))
  {
    GeisInteger touches = *(GeisInteger*)value;
    switch (op)
    {
      case GEIS_FILTER_OP_GT:
	token->min_touches = _max(token->min_touches, touches+1);
	break;
      case GEIS_FILTER_OP_GE:
	token->min_touches = _max(token->min_touches, touches);
	break;
      case GEIS_FILTER_OP_LT:
	token->max_touches = _min(touches-1, token->max_touches);
	break;
      case GEIS_FILTER_OP_LE:
	token->max_touches = _min(touches, token->max_touches);
	break;
      case GEIS_FILTER_OP_EQ:
	token->min_touches = _max(token->min_touches, touches);
	token->max_touches = _min(touches, token->max_touches);
	break;
      case GEIS_FILTER_OP_NE:
	break;
    }
    geis_debug("called: attr=%s touches=\"%d\" min=%d max=%d", name, touches, token->min_touches, token->max_touches);
  }
  return status;
}


static void
_create_test_classes(GeisBackendTestFixture tf)
{
  if (!g_poke_class)
  {
    g_poke_class = geis_gesture_class_new("poke", 2100);
    struct GeisFilterableAttribute attrs[] = {
      { GEIS_CLASS_ATTRIBUTE_NAME,      GEIS_ATTR_TYPE_STRING,  _add_class_term, g_poke_class },
      { GEIS_CLASS_ATTRIBUTE_ID,        GEIS_ATTR_TYPE_INTEGER, _add_class_term, g_poke_class },
      { GEIS_GESTURE_ATTRIBUTE_TOUCHES, GEIS_ATTR_TYPE_INTEGER, _add_class_term, g_poke_class }
    };
    GeisSize attr_count = sizeof(attrs) / sizeof(struct GeisFilterableAttribute);

    geis_register_gesture_class(tf->tf_geis, g_poke_class, attr_count, attrs);
  }
}


static void
_construct(void *mem, Geis geis)
{
  GeisBackendTestFixture tf = (GeisBackendTestFixture)mem;
  tf->tf_geis = geis;

  _create_test_devices(tf);
  _create_test_classes(tf);
  geis_post_event(tf->tf_geis, geis_event_new(GEIS_EVENT_INIT_COMPLETE));
}


static void 
_finalize(GeisBackend be)
{
  GeisBackendTestFixture tf GEIS_UNUSED = (GeisBackendTestFixture)be;
}


static GeisBackendToken
_create_token(GeisBackend be, GeisBackendTokenInitState init_state)
{
  TestBackendToken token = NULL;
  token = calloc(1, sizeof(struct TestBackendToken));
  if (token)
  {
    token->base.vtbl = &_token_vtbl;
    token->be = (GeisBackendTestFixture)be;
    if (init_state == GEIS_BACKEND_TOKEN_INIT_ALL)
    {
      token->min_touches = g_min_touches;
      token->max_touches = g_max_touches;
    }
    else
    {
      token->min_touches = g_max_touches;
      token->max_touches = g_min_touches;
    }
  }
  return (GeisBackendToken)token;
}


static GeisStatus
_gmock_accept_gesture(GeisBackend   be GEIS_UNUSED,
                      GeisGroup     group GEIS_UNUSED,
                      GeisGestureId gesture_ID GEIS_UNUSED)
{
  return GEIS_STATUS_SUCCESS;
}


static GeisStatus
_gmock_reject_gesture(GeisBackend   be GEIS_UNUSED,
                      GeisGroup     group GEIS_UNUSED,
                      GeisGestureId gesture_ID GEIS_UNUSED)
{
  return GEIS_STATUS_SUCCESS;
}


static GeisStatus
_gmock_get_configuration(GeisBackend      be GEIS_UNUSED,
                         GeisSubscription subscription GEIS_UNUSED,
                         GeisString       item_name GEIS_UNUSED,
                         GeisPointer      item_value GEIS_UNUSED)
{
  return GEIS_STATUS_NOT_SUPPORTED;
}


static GeisStatus
_gmock_set_configuration(GeisBackend      be GEIS_UNUSED,
                         GeisSubscription subscription GEIS_UNUSED,
                         GeisString       item_name GEIS_UNUSED,
                         GeisPointer      item_value GEIS_UNUSED)
{
  return GEIS_STATUS_NOT_SUPPORTED;
}


static struct GeisBackendVtable tf_vtbl = {
  _construct,
  _finalize,
  _create_token,
  _gmock_accept_gesture,
  _gmock_reject_gesture,
  NULL,
  NULL,
  _gmock_get_configuration,
  _gmock_set_configuration
};


/*
 * Generates a gesture event, the contets of which varies accoring to what's in
 * the token.
 */
static void
_create_gesture_events_for_token(TestBackendToken token)
{
  int i;

  GeisFloat    attr_float;
  GeisInteger  attr_int;
  GeisAttr     attr = NULL;

  GeisEvent    event = geis_event_new(GEIS_EVENT_GESTURE_BEGIN);
  GeisGroupSet groupset = geis_groupset_new();
  GeisGroup    group = geis_group_new(1);
  GeisAttr     group_attr = geis_attr_new(GEIS_EVENT_ATTRIBUTE_GROUPSET,
                                          GEIS_ATTR_TYPE_POINTER,
                                          groupset);
  GeisTouchSet touchset = geis_touchset_new();
  GeisAttr     touch_attr = geis_attr_new(GEIS_EVENT_ATTRIBUTE_TOUCHSET,
                                          GEIS_ATTR_TYPE_POINTER,
                                          touchset);

  GeisFrame frame = geis_frame_new(1);
  geis_frame_set_is_class(frame, g_poke_class);

  attr_int = 13;
  attr = geis_attr_new(GEIS_GESTURE_ATTRIBUTE_DEVICE_ID,
                       GEIS_ATTR_TYPE_INTEGER,
                       &attr_int);
  geis_frame_add_attr(frame, attr);

  attr_int = 1;
  attr = geis_attr_new(GEIS_GESTURE_ATTRIBUTE_TIMESTAMP,
                       GEIS_ATTR_TYPE_INTEGER,
                       &attr_int);
  geis_frame_add_attr(frame, attr);

  attr_int = 2;
  attr = geis_attr_new(GEIS_GESTURE_ATTRIBUTE_ROOT_WINDOW_ID,
                       GEIS_ATTR_TYPE_INTEGER,
                       &attr_int);
  geis_frame_add_attr(frame, attr);

  attr_int = 3;
  attr = geis_attr_new(GEIS_GESTURE_ATTRIBUTE_EVENT_WINDOW_ID,
                       GEIS_ATTR_TYPE_INTEGER,
                       &attr_int);
  geis_frame_add_attr(frame, attr);

  attr_int = 4;
  attr = geis_attr_new(GEIS_GESTURE_ATTRIBUTE_CHILD_WINDOW_ID,
                       GEIS_ATTR_TYPE_INTEGER,
                       &attr_int);
  geis_frame_add_attr(frame, attr);

  attr_float = 123.456;
  attr = geis_attr_new(GEIS_GESTURE_ATTRIBUTE_FOCUS_X,
                       GEIS_ATTR_TYPE_FLOAT,
                       &attr_float);
  geis_frame_add_attr(frame, attr);

  attr_float = 987.654;
  attr = geis_attr_new(GEIS_GESTURE_ATTRIBUTE_FOCUS_Y,
                       GEIS_ATTR_TYPE_FLOAT,
                       &attr_float);
  geis_frame_add_attr(frame, attr);

  attr = geis_attr_new(GEIS_GESTURE_ATTRIBUTE_GESTURE_NAME,
                       GEIS_ATTR_TYPE_STRING,
                       "mock gesture");
  geis_frame_add_attr(frame, attr);

  attr = geis_attr_new(GEIS_GESTURE_ATTRIBUTE_TOUCHES,
                       GEIS_ATTR_TYPE_INTEGER,
                       &token->min_touches);
  geis_frame_add_attr(frame, attr);

  for (i = 0; i < token->min_touches; ++i)
  {
    GeisTouch    touch = geis_touch_new(1);
    geis_touchset_insert(touchset, touch);
    geis_frame_add_touchid(frame, geis_touch_id(touch));
  }

  geis_group_insert_frame(group, frame);

  geis_groupset_insert(groupset, group);

  geis_event_add_attr(event, group_attr);
  geis_event_add_attr(event, touch_attr);

  geis_post_event(token->be->tf_geis, event);
}


static GeisBackendToken
_token_clone(GeisBackendToken gbtoken)
{
  TestBackendToken token = _tbtoken_from_token(gbtoken);
  TestBackendToken new_token = calloc(1, sizeof(struct TestBackendToken));
  if (new_token)
  {
    memcpy(new_token, token, sizeof(struct TestBackendToken));
    return &new_token->base;
  }
  return NULL;
}


void             
_token_finalize(GeisBackendToken gbtoken GEIS_UNUSED)
{
}


void 
_token_compose(GeisBackendToken lhs, GeisBackendToken rhs)
{
  TestBackendToken token1 = _tbtoken_from_token(lhs);
  TestBackendToken token2 = _tbtoken_from_token(rhs);
  token1->min_touches = _min(token1->min_touches, token2->min_touches);
  token1->max_touches = _max(token1->max_touches, token2->max_touches);
}


GeisStatus             
_token_activate(GeisBackendToken gbtoken,
                GeisSubscription subscription GEIS_UNUSED)
{
  GeisStatus status = GEIS_STATUS_SUCCESS;
  TestBackendToken token = _tbtoken_from_token(gbtoken);
  _create_gesture_events_for_token(token);
  return status;
}


GeisStatus             
_token_deactivate(GeisBackendToken gbtoken GEIS_UNUSED,
                  GeisSubscription subscription GEIS_UNUSED)
{
  GeisStatus status = GEIS_STATUS_SUCCESS;
  return status;
}

void
_token_free_subscription_pdata(GeisBackendToken gbtoken GEIS_UNUSED,
                               GeisSubscription subscription GEIS_UNUSED)
{
}

__attribute__((constructor))
static void _register_test_fixture()
{
  geis_register_backend(GEIS_INIT_MOCK_BACKEND,
                        sizeof(struct GeisBackendTestFixture),
                        &tf_vtbl);
}


/* A dummy routine to force linkage of this module without dlopening it */
void
geis_include_backend_test_fixture()
{
}
