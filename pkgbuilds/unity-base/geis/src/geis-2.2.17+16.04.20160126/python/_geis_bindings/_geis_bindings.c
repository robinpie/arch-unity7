#include <Python.h>

#include <geis/geis.h>
#include "geis_test_api.h"


PyDoc_STRVAR(module_doc,
	"Low-level python bindings for libgeis.\n"
	"Do not use this module directly "
	"-- the public API is provided by the 'geis' module."
);

static PyMethodDef exported_functions[] = {
  {NULL, NULL, 0, NULL}
};
static struct PyModuleDef moduledef = {
  PyModuleDef_HEAD_INIT,
  "_geis_bindings",
  module_doc,
 sizeof(NULL),
  exported_functions,
  NULL,
  NULL,
  NULL,
  NULL
};

PyObject *
PyInit__geis_bindings(void)
{
  PyObject *this_module;

  this_module = PyModule_Create(&moduledef);
  if (this_module == NULL)
    return NULL;

  /* Global status values. */
  PyModule_AddIntConstant(this_module, "GEIS_STATUS_SUCCESS",
  			  GEIS_STATUS_SUCCESS);
  PyModule_AddIntConstant(this_module, "GEIS_STATUS_CONTINUE",
  			  GEIS_STATUS_CONTINUE);
  PyModule_AddIntConstant(this_module, "GEIS_STATUS_EMPTY",
  			  GEIS_STATUS_EMPTY);
  PyModule_AddIntConstant(this_module, "GEIS_STATUS_NOT_SUPPORTED",
  			  GEIS_STATUS_NOT_SUPPORTED);
  PyModule_AddIntConstant(this_module, "GEIS_STATUS_BAD_ARGUMENT",
  			  GEIS_STATUS_BAD_ARGUMENT);
  PyModule_AddIntConstant(this_module, "GEIS_STATUS_UNKNOWN_ERROR",
  			  GEIS_STATUS_UNKNOWN_ERROR);

  /* Attribute types */
  PyModule_AddIntConstant(this_module, "GEIS_ATTR_TYPE_BOOLEAN",
  			  GEIS_ATTR_TYPE_BOOLEAN);
  PyModule_AddIntConstant(this_module, "GEIS_ATTR_TYPE_FLOAT",
  			  GEIS_ATTR_TYPE_FLOAT);
  PyModule_AddIntConstant(this_module, "GEIS_ATTR_TYPE_INTEGER",
  			  GEIS_ATTR_TYPE_INTEGER);
  PyModule_AddIntConstant(this_module, "GEIS_ATTR_TYPE_POINTER",
  			  GEIS_ATTR_TYPE_POINTER);
  PyModule_AddIntConstant(this_module, "GEIS_ATTR_TYPE_STRING",
  			  GEIS_ATTR_TYPE_STRING);

  /* Initialization Options */
  PyModule_AddStringMacro(this_module, GEIS_INIT_SERVICE_PROVIDER);
  PyModule_AddStringMacro(this_module, GEIS_INIT_TRACK_DEVICES);
  PyModule_AddStringMacro(this_module, GEIS_INIT_TRACK_GESTURE_CLASSES);
  PyModule_AddStringMacro(this_module, GEIS_INIT_SYNCHRONOUS_START);
  PyModule_AddStringMacro(this_module, GEIS_INIT_MOCK_BACKEND);
  PyModule_AddStringMacro(this_module, GEIS_INIT_DBUS_BACKEND);
  PyModule_AddStringMacro(this_module, GEIS_INIT_GRAIL_BACKEND);
  PyModule_AddStringMacro(this_module, GEIS_INIT_XCB_BACKEND);

  /* Configuration Items */
  PyModule_AddStringMacro(this_module, GEIS_CONFIGURATION_FD);
  PyModule_AddStringMacro(this_module, GEIS_CONFIG_MAX_EVENTS);

  /* Device Constants */
  PyModule_AddStringMacro(this_module, GEIS_EVENT_ATTRIBUTE_DEVICE);
  PyModule_AddStringMacro(this_module, GEIS_DEVICE_ATTRIBUTE_NAME);
  PyModule_AddStringMacro(this_module, GEIS_DEVICE_ATTRIBUTE_ID);
  PyModule_AddStringMacro(this_module, GEIS_DEVICE_ATTRIBUTE_TOUCHES);
  PyModule_AddStringMacro(this_module, GEIS_DEVICE_ATTRIBUTE_DIRECT_TOUCH);
  PyModule_AddStringMacro(this_module, GEIS_DEVICE_ATTRIBUTE_INDEPENDENT_TOUCH);

  /* Gesture Class Constants */
  PyModule_AddStringMacro(this_module, GEIS_EVENT_ATTRIBUTE_CLASS);
  PyModule_AddStringMacro(this_module, GEIS_CLASS_ATTRIBUTE_NAME);
  PyModule_AddStringMacro(this_module, GEIS_CLASS_ATTRIBUTE_ID);

  /* Region Constants */
  PyModule_AddStringMacro(this_module, GEIS_REGION_ATTRIBUTE_WINDOWID);
  PyModule_AddStringMacro(this_module, GEIS_REGION_X11_ROOT);
  PyModule_AddStringMacro(this_module, GEIS_REGION_X11_WINDOWID);

  /* Gesture Constants */
  PyModule_AddStringMacro(this_module, GEIS_EVENT_ATTRIBUTE_GROUPSET);
  PyModule_AddStringMacro(this_module, GEIS_EVENT_ATTRIBUTE_TOUCHSET);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_ANGLE);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_ANGLE_DELTA);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_ANGULAR_VELOCITY);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_BOUNDINGBOX_X1);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_BOUNDINGBOX_X2);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_BOUNDINGBOX_Y1);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_BOUNDINGBOX_Y2);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_CHILD_WINDOW_ID);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_DELTA_X);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_DELTA_Y);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_DEVICE_ID);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_EVENT_WINDOW_ID);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_FOCUS_X);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_FOCUS_Y);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_GESTURE_NAME);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_POSITION_X);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_POSITION_Y);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_RADIAL_VELOCITY);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_RADIUS);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_RADIUS_DELTA);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_ROOT_WINDOW_ID);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_TAP_TIME);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_TIMESTAMP);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_TOUCH_0_ID);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_TOUCH_0_X);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_TOUCH_0_Y);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_TOUCH_1_ID);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_TOUCH_1_X);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_TOUCH_1_Y);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_TOUCH_2_ID);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_TOUCH_2_X);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_TOUCH_2_Y);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_TOUCH_3_ID);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_TOUCH_3_X);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_TOUCH_3_Y);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_TOUCH_4_ID);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_TOUCH_4_X);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_TOUCH_4_Y);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_TOUCHES);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_VELOCITY_X);
  PyModule_AddStringMacro(this_module, GEIS_GESTURE_ATTRIBUTE_VELOCITY_Y);
  PyModule_AddStringMacro(this_module, GEIS_TOUCH_ATTRIBUTE_ID);
  PyModule_AddStringMacro(this_module, GEIS_TOUCH_ATTRIBUTE_X);
  PyModule_AddStringMacro(this_module, GEIS_TOUCH_ATTRIBUTE_Y);

  /* Event Types */
  PyModule_AddIntConstant(this_module, "GEIS_EVENT_DEVICE_AVAILABLE",
  			  GEIS_EVENT_DEVICE_AVAILABLE);
  PyModule_AddIntConstant(this_module, "GEIS_EVENT_DEVICE_UNAVAILABLE",
  			  GEIS_EVENT_DEVICE_UNAVAILABLE);
  PyModule_AddIntConstant(this_module, "GEIS_EVENT_CLASS_AVAILABLE",
  			  GEIS_EVENT_CLASS_AVAILABLE);
  PyModule_AddIntConstant(this_module, "GEIS_EVENT_CLASS_CHANGED",
  			  GEIS_EVENT_CLASS_CHANGED);
  PyModule_AddIntConstant(this_module, "GEIS_EVENT_CLASS_UNAVAILABLE",
  			  GEIS_EVENT_CLASS_UNAVAILABLE);
  PyModule_AddIntConstant(this_module, "GEIS_EVENT_GESTURE_BEGIN",
  			  GEIS_EVENT_GESTURE_BEGIN);
  PyModule_AddIntConstant(this_module, "GEIS_EVENT_GESTURE_UPDATE",
  			  GEIS_EVENT_GESTURE_UPDATE);
  PyModule_AddIntConstant(this_module, "GEIS_EVENT_GESTURE_END",
  			  GEIS_EVENT_GESTURE_END);
  PyModule_AddIntConstant(this_module, "GEIS_EVENT_INIT_COMPLETE",
  			  GEIS_EVENT_INIT_COMPLETE);
  PyModule_AddIntConstant(this_module, "GEIS_EVENT_USER_DEFINED",
  			  GEIS_EVENT_USER_DEFINED);
  PyModule_AddIntConstant(this_module, "GEIS_EVENT_ERROR",
  			  GEIS_EVENT_ERROR);

  /* Filtery Bits */
  PyModule_AddIntConstant(this_module, "GEIS_FILTER_DEVICE", GEIS_FILTER_DEVICE);
  PyModule_AddIntConstant(this_module, "GEIS_FILTER_CLASS", GEIS_FILTER_CLASS);
  PyModule_AddIntConstant(this_module, "GEIS_FILTER_REGION", GEIS_FILTER_REGION);

  PyModule_AddIntConstant(this_module, "GEIS_FILTER_OP_EQ", GEIS_FILTER_OP_EQ);
  PyModule_AddIntConstant(this_module, "GEIS_FILTER_OP_NE", GEIS_FILTER_OP_NE);
  PyModule_AddIntConstant(this_module, "GEIS_FILTER_OP_GT", GEIS_FILTER_OP_GT);
  PyModule_AddIntConstant(this_module, "GEIS_FILTER_OP_GE", GEIS_FILTER_OP_GE);
  PyModule_AddIntConstant(this_module, "GEIS_FILTER_OP_LT", GEIS_FILTER_OP_LT);
  PyModule_AddIntConstant(this_module, "GEIS_FILTER_OP_LE", GEIS_FILTER_OP_LE);

  return this_module;
}

