SUBDIRS = gtest

if HAVE_XINPUT
SUBDIRS += x11_mocks
endif

SUBDIRS += regular

# integration tests are all X11 based
if HAVE_XINPUT
if ENABLE_INTEGRATION_TESTS
SUBDIRS += integration
endif
endif
