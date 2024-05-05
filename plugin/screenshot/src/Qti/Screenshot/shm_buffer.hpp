#pragma once

#include <QImage>
#include <QSize>
#include <QtWaylandClient/private/wayland-wayland-client-protocol.h>

class ShmBuffer { // NOLINT
public:
  ShmBuffer(const QSize &size, wl_shm *shm, ::wl_shm_format format);
  ~ShmBuffer();

  struct wl_buffer *mHandle = nullptr;
  struct wl_shm_pool *mShmPool = nullptr;
  QImage mImage;
};
