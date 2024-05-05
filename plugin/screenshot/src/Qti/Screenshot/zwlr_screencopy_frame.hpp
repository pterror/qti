#pragma once

#include <QtQml/QQmlEngine>
#include <QtWaylandClient>
#include <private/qwaylandscreen_p.h>
#include <private/qwaylandshmbackingstore_p.h>
#include <private/wayland-wayland-client-protocol.h>
#include <qqmlintegration.h>
#include <qwayland-wlr-screencopy-unstable-v1.h>

class ZwlrScreencopyFrame
    : public QWaylandClientExtensionTemplate<ZwlrScreencopyFrame>,
      public QtWayland::zwlr_screencopy_frame_v1 {
  Q_OBJECT;
  QML_NAMED_ELEMENT(ZwlrScreencopyFrame);
  QML_UNCREATABLE("Wrapper around Wayland type");

public:
  explicit ZwlrScreencopyFrame(struct ::zwlr_screencopy_frame_v1 *object,
                               QtWaylandClient::QWaylandScreen *screen);

  void copy(struct ::wl_buffer *buffer);
  void destroy();
  void copyWithDamage(struct ::wl_buffer *buffer);

signals:
  void ready(QtWaylandClient::QWaylandShmBuffer *buffer);
  void failed();

private:
  void zwlr_screencopy_frame_v1_buffer(uint32_t format, uint32_t width,
                                       uint32_t height,
                                       uint32_t stride) override;
  void zwlr_screencopy_frame_v1_flags(uint32_t flags) override;
  void zwlr_screencopy_frame_v1_ready(uint32_t tvSecHi, uint32_t tvSecLo,
                                      uint32_t tvNsec) override;
  void zwlr_screencopy_frame_v1_failed() override;
  void zwlr_screencopy_frame_v1_damage(uint32_t x, uint32_t y, uint32_t width,
                                       uint32_t height) override;
  void zwlr_screencopy_frame_v1_linux_dmabuf(uint32_t format, uint32_t width,
                                             uint32_t height) override;
  void zwlr_screencopy_frame_v1_buffer_done() override;

  QtWaylandClient::QWaylandShmBuffer *mBuffer = nullptr;
  QtWaylandClient::QWaylandScreen *mScreen;
  // default to a format with a low score
  ::wl_shm_format mFormat = WL_SHM_FORMAT_C8;
  int mWidth = 0;
  int mHeight = 0;
};
