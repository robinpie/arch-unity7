#include "gtest_grail_backend.h"
#include "x11_mocks.h"

/*
  Checks trying to delete a null filter doesn't cause problems.

  Regression test for https://bugs.launchpad.net/nux/+bug/1080386
 */

class DeleteNullFilter : public Geis2GrailBackendBase
{
 protected:
  virtual void OnEventInitComplete(GeisEvent event);
};

void DeleteNullFilter::OnEventInitComplete(GeisEvent event)
{
  GeisFilter filter = nullptr;
  geis_filter_delete(filter); // please don't crash
}

TEST_F(DeleteNullFilter, Test)
{
  CreateXMockTouchScreenDevice();

  _geis = geis_new(GEIS_INIT_GRAIL_BACKEND,
                   GEIS_INIT_NO_ATOMIC_GESTURES,
                   nullptr);
  ASSERT_NE(nullptr, _geis);

  Run();

  DestroyXMockDevices();
}
