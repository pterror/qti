#include "zwlr_screencopy_frame.hpp"
#include "qimage_format_utils.hpp"

#include <private/qwaylandshm_p.h>

ZwlrScreencopyFrame::ZwlrScreencopyFrame(
    ::zwlr_screencopy_frame_v1 *object, QtWaylandClient::QWaylandScreen *screen)
    : QWaylandClientExtensionTemplate<ZwlrScreencopyFrame>(
          this->extensionInterface()->version),
      QtWayland::zwlr_screencopy_frame_v1(object), mScreen(screen) {
  this->initialize();
}

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

void ZwlrScreencopyFrame::zwlr_screencopy_frame_v1_buffer_done() {
  this->mBuffer = new QtWaylandClient::QWaylandShmBuffer(
      this->mScreen->display(), QSize(this->mWidth, this->mHeight),
      QtWaylandClient::QWaylandShm::formatFrom(this->mFormat));
  zwlr_screencopy_frame_v1_copy(this->object(), this->mBuffer->buffer());
}
