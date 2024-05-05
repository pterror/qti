#include "zwlr_screencopy_frame.hpp"
#include "qimage_format_utils.hpp"

#include <QtGui/qpa/qplatformnativeinterface.h>
#include <QtWaylandClient/private/qwaylanddisplay_p.h>

ZwlrScreencopyFrame::ZwlrScreencopyFrame(::zwlr_screencopy_frame_v1 *object)
    : QWaylandClientExtensionTemplate<ZwlrScreencopyFrame>(
          this->extensionInterface()->version),
      QtWayland::zwlr_screencopy_frame_v1(object) {}

void ZwlrScreencopyFrame::zwlr_screencopy_frame_v1_buffer(uint32_t format,
                                                          uint32_t width,
                                                          uint32_t height,
                                                          uint32_t /*stride*/) {
  auto formatEnum = static_cast<::wl_shm_format>(format);
  if (qImageFormatScore(QtWaylandClient::QWaylandShm::formatFrom(formatEnum)) >
      qImageFormatScore(
          QtWaylandClient::QWaylandShm::formatFrom(this->mFormat))) {
    this->mFormat = formatEnum;
    this->mWidth = static_cast<int>(width);
    this->mHeight = static_cast<int>(height);
  }
}

void ZwlrScreencopyFrame::zwlr_screencopy_frame_v1_flags(uint32_t /*flags*/) {}

void ZwlrScreencopyFrame::zwlr_screencopy_frame_v1_ready(uint32_t /*tvSecHi*/,
                                                         uint32_t /*tvSecLo*/,
                                                         uint32_t /*tvNsec*/) {
  emit this->ready(this->mBuffer);
  delete this;
}

void ZwlrScreencopyFrame::zwlr_screencopy_frame_v1_failed() {
  emit this->failed();
  delete this;
}

void ZwlrScreencopyFrame::zwlr_screencopy_frame_v1_damage(uint32_t /*x*/,
                                                          uint32_t /*y*/,
                                                          uint32_t /*width*/,
                                                          uint32_t /*height*/) {
}

void ZwlrScreencopyFrame::zwlr_screencopy_frame_v1_linux_dmabuf(
    uint32_t /*format*/, uint32_t /*width*/, uint32_t /*height*/) {}

inline QtWaylandClient::QWaylandDisplay *wlDisplay() {
  auto *display =
      QGuiApplication::platformNativeInterface()->nativeResourceForIntegration(
          "display");
  return static_cast<QtWaylandClient::QWaylandDisplay *>(display);
}

void ZwlrScreencopyFrame::zwlr_screencopy_frame_v1_buffer_done() {
  this->mBuffer = new ShmBuffer(QSize(this->mWidth, this->mHeight),
                                wlDisplay()->shm()->object(), this->mFormat);
  zwlr_screencopy_frame_v1_copy(this->object(), this->mBuffer->mHandle);
}
