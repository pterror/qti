#include "screenshot.hpp"

#include "qimage_format_utils.hpp"
#include "screenshot_image_provider.hpp"
#include "zwlr_screencopy_manager.hpp"
#include "zxdg_output.hpp"
#include "zxdg_output_manager.hpp"

// FIXME: this is not cross platform
#include <cstdint>
#include <private/wayland-wayland-client-protocol.h>
#include <qabstracteventdispatcher.h>
#include <qlogging.h>
#include <qtypes.h>
#include <sys/mman.h>

#include <limits>
#include <variant>

#include <QGuiApplication>
#include <QPainter>
#include <QPixmap>
#include <QQmlApplicationEngine>
#include <QScreen>
#include <QSocketNotifier>
#include <QUuid>
#include <QtGui/qpa/qplatformnativeinterface.h>
#include <private/qguiapplication_p.h>
#include <wayland-client-core.h>

namespace {
class ShmBuffer { // NOLINT
public:
  ShmBuffer(const QSize &size, wl_shm *shm, uint32_t format);
  ~ShmBuffer();

  struct wl_buffer *mHandle = nullptr;
  struct wl_shm_pool *mShmPool = nullptr;
  QImage mImage;
};

struct WlrScreencopyFrameMetadata {
  uint32_t format = ~0;
  int width = 0;
  int height = 0;
};

struct ScreenshotFramePayload {
  std::function<void(ShmBuffer *buffer, uint)> onSuccess;
  std::function<void()> onFailure;
  uint id;
  wl_shm *shm;
  std::variant<WlrScreencopyFrameMetadata, ShmBuffer *> buffer;
};

const auto MIN_INT = std::numeric_limits<int>::min();
const auto MAX_INT = std::numeric_limits<int>::max();
QRect initialBounds() {
  return QRect(QPoint(MAX_INT, MAX_INT), QPoint(MIN_INT, MIN_INT));
}

}; // namespace

Screenshot::Screenshot()
    : mWlDisplay(wl_display_connect(nullptr)),
      mWlRegistry(new ScreenshotWlRegistry(
          this->mWlDisplay, wl_display_get_registry(this->mWlDisplay))) {}

Screenshot::~Screenshot() { wl_display_disconnect(this->mWlDisplay); }

QUrl Screenshot::cache(const QPixmap &pixmap) const {
  auto *provider = ScreenshotImageProvider::instance(qmlEngine(this));
  return provider->cache(pixmap);
}

void Screenshot::free(const QUrl &url) const {
  auto *provider = ScreenshotImageProvider::instance(qmlEngine(this));
  provider->free(url);
}

inline QPixmap
compositeAllScreens(const std::function<QPixmap(QScreen *, bool)> &getPixmap,
                    bool captureCursor) {
  const auto screens = QGuiApplication::screens();
  if (screens.size() == 1) {
    return getPixmap(screens[0], captureCursor);
  } else {
    auto bounds = initialBounds();
    for (auto *const screen : screens) {
      const auto geometry = screen->geometry();
      bounds.setLeft(std::min(bounds.left(), geometry.left()));
      bounds.setRight(std::max(bounds.right(), geometry.right()));
      bounds.setTop(std::min(bounds.top(), geometry.top()));
      bounds.setBottom(std::max(bounds.bottom(), geometry.bottom()));
    }
    auto pixmap = QPixmap(bounds.width(), bounds.height());
    auto painter = QPainter(&pixmap);
    for (auto *const screen : screens) {
      const auto screenBounds = screen->geometry();
      painter.drawPixmap(screenBounds, getPixmap(screen, captureCursor));
    }
  }
}

QPixmap Screenshot::captureScreenInternal(QScreen *screen, bool captureCursor) {
  // FIXME: this check probably doesn't work.
  if (ZwlrScreencopyManager::instance() == nullptr) {
    return screen->grabWindow();
  } else {
    return Screenshot::captureScreenInternalWayland(screen, captureCursor);
  }
}

QPixmap Screenshot::captureScreenInternalWayland(QScreen *screen,
                                                 bool captureCursor) {
  auto *outputRaw =
      QGuiApplication::platformNativeInterface()->nativeResourceForScreen(
          "output", screen);
  auto *output = static_cast<::wl_output *>(outputRaw);
  auto *xdgOutputRaw =
      ZxdgOutputManager::instance()->getXdgOutput(output)->object();
  auto *xdgOutput = new ZxdgOutput(xdgOutputRaw);
  QObject::connect(xdgOutput, &ZxdgOutput::done, this, nullptr /* TODO: */);
}

void Screenshot::captureAllScreens(const QJSValue &onSuccess,
                                   const QJSValue &onFailure,
                                   bool captureCursor) const {
  const auto screens = QGuiApplication::screens();
  const auto pixmap =
      compositeAllScreens(&Screenshot::captureScreenInternal, captureCursor);
  const auto url = this->cache(pixmap);
  if (onSuccess.isCallable()) {
    onSuccess.call(QJSValueList({qmlEngine(this)->toScriptValue(url)}));
  }
  // FIXME:
  if (false) {
    auto failed = false;
    auto bounds = initialBounds();
    auto pixmap = QPixmap(bounds.width(), bounds.height());
    auto painter = QPainter(&pixmap);
    auto *screencopyManager = ZwlrScreencopyManager::instance();
    auto outputsLeft = this->mWlRegistry->outputs().size();
    for (auto kv : this->mWlRegistry->outputs().asKeyValueRange()) {
      auto id = kv.first;
      auto *output = kv.second;
      auto *frame = screencopyManager->captureOutput(captureCursor, output);
      const auto onFrameSuccess = [&, this](ShmBuffer *buffer, uint id) {
        painter.drawImage(this->mWlRegistry->outputGeometries()[id],
                          buffer->mImage);
        delete buffer;
        outputsLeft -= 1;
        if (outputsLeft == 0) {
          const auto url = this->cache(pixmap);
          if (onSuccess.isCallable()) {
            onSuccess.call(QJSValueList({qmlEngine(this)->toScriptValue(url)}));
          }
        }
      };
      const auto onFrameFailure = [&]() {
        if (!failed) {
          failed = true;
          if (onFailure.isCallable()) {
            onFailure.call();
          }
        }
      };
      auto *payload =
          new ScreenshotFramePayload({.onSuccess = onFrameSuccess,
                                      .onFailure = onFrameFailure,
                                      .id = id,
                                      .shm = this->mWlRegistry->shm()->object(),
                                      .buffer = WlrScreencopyFrameMetadata()});
      zwlr_screencopy_frame_v1_add_listener(frame, &Screenshot::FRAME_LISTENER,
                                            payload);
    }
    wl_display_roundtrip(this->mWlDisplay);
    wl_display_roundtrip(this->mWlDisplay);
    wl_display_roundtrip(this->mWlDisplay);
  }
}

const zwlr_screencopy_frame_v1_listener Screenshot::FRAME_LISTENER = {
    .buffer = Screenshot::onWlScreencopyFrameBuffer,
    .flags = Screenshot::onWlScreencopyFrameFlags,
    .ready = Screenshot::onWlScreencopyFrameReady,
    .failed = Screenshot::onWlScreencopyFrameFailed,
    .damage = Screenshot::onWlScreencopyFrameDamage,
    .linux_dmabuf = Screenshot::onWlScreencopyFrameLinuxDmabuf,
    .buffer_done = Screenshot::onWlScreencopyFrameBufferDone,
};

inline ScreenshotFramePayload *wlScreencopyFramePayload(void *data) {
  return static_cast<ScreenshotFramePayload *>(data);
}

void Screenshot::onWlScreencopyFrameBuffer(
    void *data, struct zwlr_screencopy_frame_v1 * /*frame*/, uint32_t format,
    uint32_t width, uint32_t height, uint32_t /*stride*/) {
  auto *payload = wlScreencopyFramePayload(data);
  if (payload == nullptr ||
      !std::holds_alternative<WlrScreencopyFrameMetadata>(payload->buffer)) {
    return;
  }
  auto *metadata = &std::get<WlrScreencopyFrameMetadata>(payload->buffer);
  if (qImageFormatScore(wlShmFormatToQImageFormat(format)) >
      qImageFormatScore(wlShmFormatToQImageFormat(metadata->format))) {
    metadata->format = format;
    metadata->width = static_cast<int>(width);
    metadata->height = static_cast<int>(height);
  }
}

void Screenshot::onWlScreencopyFrameFlags(
    void * /*data*/, struct zwlr_screencopy_frame_v1 * /*frame*/,
    uint32_t /*flags*/) {}

void Screenshot::onWlScreencopyFrameReady(
    void *data, struct zwlr_screencopy_frame_v1 *frame, uint32_t /*tvSecHi*/,
    uint32_t /*tvSecLo*/, uint32_t /*tvNsec*/) {
  auto *payload = wlScreencopyFramePayload(data);
  if (payload == nullptr ||
      !std::holds_alternative<ShmBuffer *>(payload->buffer)) {
    return;
  }
  payload->onSuccess(std::get<ShmBuffer *>(payload->buffer), payload->id);
  delete payload;
  zwlr_screencopy_frame_v1_destroy(frame);
}

void Screenshot::onWlScreencopyFrameFailed(
    void *data, struct zwlr_screencopy_frame_v1 *frame) {
  const auto *payload = wlScreencopyFramePayload(data);
  payload->onFailure();
  delete payload;
  zwlr_screencopy_frame_v1_destroy(frame);
}

void Screenshot::onWlScreencopyFrameDamage(
    void * /*data*/, struct zwlr_screencopy_frame_v1 * /*frame*/,
    uint32_t /*x*/, uint32_t /*y*/, uint32_t /*width*/, uint32_t /*height*/) {}

void Screenshot::onWlScreencopyFrameLinuxDmabuf(
    void * /*data*/, struct zwlr_screencopy_frame_v1 * /*frame*/,
    uint32_t /*format*/, uint32_t /*width*/, uint32_t /*height*/) {}

void Screenshot::onWlScreencopyFrameBufferDone(
    void *data, struct zwlr_screencopy_frame_v1 *frame) {
  auto *payload = wlScreencopyFramePayload(data);
  if (!std::holds_alternative<WlrScreencopyFrameMetadata>(payload->buffer)) {
    payload->onFailure();
    delete payload;
  } else {
    auto metadata = std::get<WlrScreencopyFrameMetadata>(payload->buffer);
    auto *buffer = new ShmBuffer(QSize(metadata.width, metadata.height),
                                 payload->shm, metadata.format);
    payload->buffer = buffer;
    zwlr_screencopy_frame_v1_copy(frame, buffer->mHandle);
  }
}

ShmBuffer::ShmBuffer(const QSize &size, wl_shm *shm, uint32_t format) {
  auto stride = size.width() * 4;
  auto alloc = stride * size.height();
  auto *filename = strdup("/tmp/wayland-shm-XXXXXX");
  int fd = mkstemp(filename);
  if (fd < 0) {
    qWarning("ShmBuffer: open %s failed", filename);
    return;
  }
  if (ftruncate(fd, alloc) < 0) {
    qWarning("ShmBuffer: ftruncate failed");
    close(fd);
    return;
  }
  void *data = mmap(nullptr, alloc, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  unlink(filename);
  free(filename); // NOLINT
  if (data == MAP_FAILED) {
    qWarning("ShmBuffer: mmap failed");
    close(fd);
    return;
  }
  this->mImage = QImage(static_cast<uchar *>(data), size.width(), size.height(),
                        stride, wlShmFormatToQImageFormat(format));
  this->mShmPool = wl_shm_create_pool(shm, fd, alloc);
  this->mHandle = wl_shm_pool_create_buffer(mShmPool, 0, size.width(),
                                            size.height(), stride, format);
  close(fd);
}

ShmBuffer::~ShmBuffer() {
  munmap(this->mImage.bits(), this->mImage.sizeInBytes());
  wl_buffer_destroy(this->mHandle);
  wl_shm_pool_destroy(this->mShmPool);
}
