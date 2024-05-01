#include "screenshot.hpp"
#include "wayland-xdg-output-unstable-v1-client-protocol.h"

// FIXME: this is not cross platform
#include <cstdint>
#include <private/qwayland-wayland.h>
#include <qabstracteventdispatcher.h>
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
#include <private/qguiapplication_p.h>
#include <wayland-client-core.h>

namespace {
const auto IMAGE_PROVIDER_NAME = "qti_screenshot";
const auto URL_PREFIX = QString("image://") + IMAGE_PROVIDER_NAME + "/";

class ShmBuffer { // NOLINT
public:
  ShmBuffer(const QSize &size, wl_shm *shm, uint32_t format);
  ~ShmBuffer();

  struct wl_buffer *mHandle = nullptr;
  struct wl_shm_pool *mShmPool = nullptr;
  QImage mImage;
};

struct WlOutputPayload {
  Screenshot *screenshot;
  uint32_t id = 0;
};

struct WlrScreencopyFrameMetadata {
  uint32_t format = ~0;
  int width = 0;
  int height = 0;
};

struct ScreenshotFramePayload {
  std::function<void(ShmBuffer *buffer)> onSuccess;
  std::function<void()> onFailure;
  wl_shm *shm;
  std::variant<WlrScreencopyFrameMetadata, ShmBuffer *> buffer;
};

const auto MIN_INT = std::numeric_limits<int>::min();
const auto MAX_INT = std::numeric_limits<int>::max();
QRect initialBounds() {
  return QRect(QPoint(MAX_INT, MAX_INT), QPoint(MIN_INT, MIN_INT));
}

// the following copyright notice applies ONLY to the following function, see
// https://github.com/qt/qtwayland/blob/a1452b95f7758b059ef6994f09b2b9d88a0aa677/src/shared/qwaylandsharedmemoryformathelper_p.h#L19-L39
// Copyright (C) 2020 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR
// GPL-2.0-only OR GPL-3.0-only
inline QImage::Format wlShmFormatToQImageFormat(uint32_t format) {
  switch (format) {
  case WL_SHM_FORMAT_XRGB8888: {
    return QImage::Format_RGB32;
  }
  case WL_SHM_FORMAT_ARGB8888: {
    return QImage::Format_ARGB32_Premultiplied;
  }
  case WL_SHM_FORMAT_RGB565: {
    return QImage::Format_RGB16;
  }
  case WL_SHM_FORMAT_XRGB1555: {
    return QImage::Format_RGB555;
  }
  case WL_SHM_FORMAT_RGB888: {
    return QImage::Format_RGB888;
  }
  case WL_SHM_FORMAT_BGR888: {
    return QImage::Format_BGR888;
  }
  case WL_SHM_FORMAT_XRGB4444: {
    return QImage::Format_RGB444;
  }
  case WL_SHM_FORMAT_ARGB4444: {
    return QImage::Format_ARGB4444_Premultiplied;
  }
  case WL_SHM_FORMAT_XBGR8888: {
    return QImage::Format_RGBX8888;
  }
  case WL_SHM_FORMAT_ABGR8888: {
    return QImage::Format_RGBA8888_Premultiplied;
  }
  case WL_SHM_FORMAT_XBGR2101010: {
    return QImage::Format_BGR30;
  }
  case WL_SHM_FORMAT_ABGR2101010: {
    return QImage::Format_A2BGR30_Premultiplied;
  }
  case WL_SHM_FORMAT_XRGB2101010: {
    return QImage::Format_RGB30;
  }
  case WL_SHM_FORMAT_ARGB2101010: {
    return QImage::Format_A2RGB30_Premultiplied;
  }
  case WL_SHM_FORMAT_C8: {
    return QImage::Format_Alpha8;
  }
  default: {
    return QImage::Format_Invalid;
  }
  }
}

// see the note at the end of https://doc.qt.io/qt-6/qimage.html#Format-enum
// ordered by, in order:
// - invalid vs valid
// - grayscale vs color
// - alpha support (no alpha < 2 bit alpha < 8+ bit alpha)
// - performance (lowest to highest)
uint8_t qImageFormatScore(QImage::Format format) {
  switch (format) {
  case QImage::NImageFormats:
  case QImage::Format_Invalid: {
    return 0;
  }
  case QImage::Format_Mono:
  case QImage::Format_MonoLSB:
  case QImage::Format_Alpha8:
  case QImage::Format_Grayscale8:
  case QImage::Format_Grayscale16: {
    return 1;
  }
  case QImage::Format_Indexed8:
  case QImage::Format_RGB666:
  case QImage::Format_RGB555:
  case QImage::Format_RGB888:
  case QImage::Format_RGB444:
  case QImage::Format_BGR30:
  case QImage::Format_RGB30:
  case QImage::Format_BGR888:
  case QImage::Format_RGBX16FPx4:
  case QImage::Format_RGBX32FPx4: {
    return 2;
  }
  case QImage::Format_RGB16:
  case QImage::Format_RGBX8888:
  case QImage::Format_RGBX64: {
    return 3;
  }
  case QImage::Format_RGB32: {
    return 4;
  }
  case QImage::Format_A2BGR30_Premultiplied:
  case QImage::Format_A2RGB30_Premultiplied: {
    return 5;
  }
  case QImage::Format_ARGB32:
  case QImage::Format_ARGB8565_Premultiplied:
  case QImage::Format_ARGB6666_Premultiplied:
  case QImage::Format_ARGB8555_Premultiplied:
  case QImage::Format_ARGB4444_Premultiplied:
  case QImage::Format_RGBA8888:
  case QImage::Format_RGBA64:
  case QImage::Format_RGBA16FPx4:
  case QImage::Format_RGBA16FPx4_Premultiplied:
  case QImage::Format_RGBA32FPx4:
  case QImage::Format_RGBA32FPx4_Premultiplied: {
    return 6;
  }
  case QImage::Format_RGBA8888_Premultiplied:
  case QImage::Format_RGBA64_Premultiplied: {
    return 7;
  }
  case QImage::Format_ARGB32_Premultiplied: {
    return 8;
  }
  default: {
    return 0;
  }
  }
}
}; // namespace

Screenshot::Screenshot()
    : mWlDisplay(wl_display_connect(nullptr)),
      mWlRegistry(new QtWayland::wl_registry(
          wl_display_get_registry(this->mWlDisplay))) {
  wl_registry_add_listener(this->mWlRegistry->object(),
                           &Screenshot::REGISTRY_LISTENER, this);
  wl_display_roundtrip(this->mWlDisplay);
  // another roundtrip for wl_output events
  wl_display_roundtrip(this->mWlDisplay);
  if (this->mXdgOutputManager != nullptr) {
    for (auto kv : this->mWlOutputs.asKeyValueRange()) {
      auto id = kv.first;
      auto *output = kv.second;
      auto *xdgOutput = zxdg_output_manager_v1_get_xdg_output(
          this->mXdgOutputManager->object(), output);
      auto *data = new WlOutputPayload({.screenshot = this, .id = id});
      zxdg_output_v1_add_listener(xdgOutput, &Screenshot::XDG_OUTPUT_LISTENER,
                                  data);
    }
    wl_display_roundtrip(this->mWlDisplay);
  }
}

Screenshot::~Screenshot() { wl_display_disconnect(this->mWlDisplay); }

ScreenshotImageProvider *Screenshot::screenshotImageProvider() const {
  auto *engine = qmlEngine(this);
  auto *imageProviderBase = engine->imageProvider(IMAGE_PROVIDER_NAME);
  if (imageProviderBase != nullptr) {
    return dynamic_cast<ScreenshotImageProvider *>(imageProviderBase);
  } else {
    auto *imageProvider = new ScreenshotImageProvider();
    engine->addImageProvider(IMAGE_PROVIDER_NAME, imageProvider);
    return imageProvider;
  }
}

// TODO: change API to accept callback
void Screenshot::captureAllScreens(QJSValue onSuccess, QJSValue onFailure,
                                   bool captureCursor) const {
  if (this->mWlrScreencopyManager == nullptr) {
    const auto screens = QGuiApplication::screens();
    auto pixmap = QPixmap();
    if (screens.size() == 1) {
      pixmap = screens[0]->grabWindow();
    } else {
      auto bounds = initialBounds();
      for (auto *const screen : screens) {
        const auto geometry = screen->geometry();
        bounds.setLeft(std::min(bounds.left(), geometry.left()));
        bounds.setRight(std::max(bounds.right(), geometry.right()));
        bounds.setTop(std::min(bounds.top(), geometry.top()));
        bounds.setBottom(std::max(bounds.bottom(), geometry.bottom()));
      }
      pixmap = QPixmap(bounds.width(), bounds.height());
      auto painter = QPainter(&pixmap);
      for (auto *const screen : screens) {
        const auto screenBounds = screen->geometry();
        painter.drawPixmap(screenBounds, screen->grabWindow());
      }
    }
    const auto url = this->screenshotImageProvider()->cache(pixmap);
    if (onSuccess.isCallable()) {
      onSuccess.call(QJSValueList({qmlEngine(this)->toScriptValue(url)}));
    }
  } else {
    // TODO: optimize when there is only one output
    auto failed = false;
    auto bounds = initialBounds();
    for (const auto geometry : this->mWlOutputGeometries) {
      bounds.setLeft(std::min(bounds.left(), geometry.left()));
      bounds.setRight(std::max(bounds.right(), geometry.right()));
      bounds.setTop(std::min(bounds.top(), geometry.top()));
      bounds.setBottom(std::max(bounds.bottom(), geometry.bottom()));
    }
    auto pixmap = QPixmap(bounds.width(), bounds.height());
    auto painter = QPainter(&pixmap);
    auto *screencopyManager = this->mWlrScreencopyManager;
    auto outputsLeft = this->mWlOutputs.size();
    for (auto kv : this->mWlOutputs.asKeyValueRange()) {
      auto id = kv.first;
      auto *output = kv.second;
      auto *frame =
          screencopyManager->capture_output(captureCursor ? 1 : 0, output);
      const auto onFrameSuccess = [&, this](ShmBuffer *buffer) {
        painter.drawImage(this->mWlOutputGeometries[id], buffer->mImage);
        delete buffer;
        outputsLeft -= 1;
        if (outputsLeft == 0) {
          const auto url = this->screenshotImageProvider()->cache(pixmap);
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
                                      .shm = this->mWlShm->object(),
                                      .buffer = WlrScreencopyFrameMetadata()});
      zwlr_screencopy_frame_v1_add_listener(frame, &Screenshot::FRAME_LISTENER,
                                            payload);
    }
    wl_display_roundtrip(this->mWlDisplay);
  }
}

// FIXME: use `captureCursor`
QUrl Screenshot::capturePrimaryScreen(bool /*captureCursor*/) const {
  auto *const screen = QGuiApplication::primaryScreen();
  return this->screenshotImageProvider()->cache(screen->grabWindow());
}

QUrl Screenshot::captureScreen(int index, bool /*captureCursor*/) const {
  auto *const screen = QGuiApplication::screens()[index];
  return this->screenshotImageProvider()->cache(screen->grabWindow());
}

void Screenshot::free(const QUrl &url) const {
  this->screenshotImageProvider()->free(url);
}

ScreenshotImageProvider::ScreenshotImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap) {}

QPixmap
ScreenshotImageProvider::requestPixmap(const QString &id, QSize *size,
                                       const QSize & /*requestedSize*/) {
  const auto uuid = QUuid::fromString(id);
  auto pixmap = this->mCache[uuid];
  *size = pixmap.size();
  return pixmap;
}

QUrl ScreenshotImageProvider::cache(QPixmap pixmap) {
  const auto uuid = QUuid::createUuid();
  this->mCache[uuid] = std::move(pixmap);
  return QUrl(URL_PREFIX + uuid.toString(QUuid::WithoutBraces));
}

void ScreenshotImageProvider::free(const QUrl &url) {
  const auto urlString = url.toString();
  if (!urlString.startsWith(URL_PREFIX)) {
    return;
  }
  auto uuid = QUuid::fromString(urlString.sliced(urlString.length()));
  this->mCache.remove(uuid);
}

const struct wl_registry_listener Screenshot::REGISTRY_LISTENER = {
    .global = Screenshot::onWlRegistryGlobal,
    .global_remove = Screenshot::onWlRegistryGlobalRemove};

void Screenshot::onWlRegistryGlobal(void *data, wl_registry *registry,
                                    uint32_t id, const char *interface,
                                    uint32_t version) {
  Q_UNUSED(registry);
  Q_UNUSED(version);
  Screenshot::resolve(data)->onWlRegistryGlobal(id, QByteArray(interface));
}

void Screenshot::onWlRegistryGlobalRemove(void *data, wl_registry *wlRegistry,
                                          uint32_t id) {
  Q_UNUSED(wlRegistry);
  Screenshot::resolve(data)->onWlRegistryGlobalRemove(id);
}

void Screenshot::onWlRegistryGlobal(uint32_t id, const QByteArray &interface) {
  if (interface == wl_output_interface.name) {
    auto *output = static_cast<wl_output *>(
        wl_registry_bind(this->mWlRegistry->object(), id, &wl_output_interface,
                         wl_output_interface.version));
    this->mWlOutputs.insert(id, output);
    auto *data = new WlOutputPayload({.screenshot = this, .id = id});
    wl_output_add_listener(output, &Screenshot::OUTPUT_LISTENER, data);
  } else if (interface == wl_shm_interface.name) {
    this->mWlShm = new QtWayland::wl_shm(this->mWlRegistry->object(), id,
                                         wl_shm_interface.version);
  } else if (interface == zwlr_screencopy_manager_v1_interface.name) {
    this->mWlrScreencopyManager = new QtWayland::zwlr_screencopy_manager_v1(
        this->mWlRegistry->object(), id,
        zwlr_screencopy_manager_v1_interface.version);
  } else if (interface == zxdg_output_manager_v1_interface.name) {
    this->mXdgOutputManager = new QtWayland::zxdg_output_manager_v1(
        this->mWlRegistry->object(), id,
        zxdg_output_manager_v1_interface.version);
  }
}

void Screenshot::onWlRegistryGlobalRemove(uint32_t id) {
  this->mWlOutputs.remove(id);
  this->mWlOutputGeometries.remove(id);
}

const wl_output_listener Screenshot::OUTPUT_LISTENER = {
    .geometry = Screenshot::onWlOutputGeometry,
    .mode = Screenshot::onWlOutputMode,
    .done = Screenshot::onWlOutputDone,
    .scale = Screenshot::onWlOutputScale,
    .name = Screenshot::onWlOutputName,
    .description = Screenshot::onWlOutputDescription};

inline WlOutputPayload *wlOutputPayload(void *data) {
  return static_cast<WlOutputPayload *>(data);
}

void Screenshot::onWlOutputGeometry(void *data, wl_output * /*output*/,
                                    int32_t x, int32_t y, int32_t /*width*/,
                                    int32_t /*height*/, int /*subpixel*/,
                                    const char * /*make*/,
                                    const char * /*model*/,
                                    int32_t /*transform*/) {
  const auto *payload = wlOutputPayload(data);
  payload->screenshot->mWlOutputGeometries[payload->id] = QRect(x, y, 0, 0);
}

void Screenshot::onWlOutputMode(void *data, struct wl_output * /*output*/,
                                uint32_t flags, int32_t width, int32_t height,
                                int32_t /*refresh*/) {
  const auto *payload = wlOutputPayload(data);
  if ((flags & WL_OUTPUT_MODE_CURRENT) != 0) {
    auto *rect = &payload->screenshot->mWlOutputGeometries[payload->id];
    rect->setWidth(width);
    rect->setHeight(height);
  }
}
void Screenshot::onWlOutputDone(void * /*data*/, wl_output * /*output*/) {}
void Screenshot::onWlOutputScale(void * /*data*/, wl_output * /*output*/,
                                 int32_t /*factor*/) {}
void Screenshot::onWlOutputName(void * /*data*/, wl_output * /*output*/,
                                const char * /* name */) {}
void Screenshot::onWlOutputDescription(void * /*data*/, wl_output * /*output*/,
                                       const char * /* description */) {}

const zxdg_output_v1_listener Screenshot::XDG_OUTPUT_LISTENER = {
    .logical_position = Screenshot::onXdgOutputLogicalPosition,
    .logical_size = Screenshot::onXdgOutputLogicalSize,
    .done = Screenshot::onXdgOutputDone,
    .name = Screenshot::onXdgOutputName,
    .description = Screenshot::onXdgOutputDescription,
};

using XdgOutputPayload = WlOutputPayload;

inline XdgOutputPayload *xdgOutputPayload(void *data) {
  return static_cast<XdgOutputPayload *>(data);
}

void Screenshot::onXdgOutputLogicalPosition(void *data,
                                            struct zxdg_output_v1 * /*output*/,
                                            int32_t x, int32_t y) {
  const auto *payload = xdgOutputPayload(data);
  auto *rect = &payload->screenshot->mWlOutputGeometries[payload->id];
  rect->setX(x);
  rect->setY(y);
}

void Screenshot::onXdgOutputLogicalSize(void *data,
                                        struct zxdg_output_v1 * /*output*/,
                                        int32_t width, int32_t height) {
  const auto *payload = xdgOutputPayload(data);
  auto *rect = &payload->screenshot->mWlOutputGeometries[payload->id];
  rect->setWidth(width);
  rect->setHeight(height);
}

void Screenshot::onXdgOutputDone(void *data,
                                 struct zxdg_output_v1 * /*output*/) {
  const auto *payload = xdgOutputPayload(data);
  delete payload;
}
void Screenshot::onXdgOutputName(void * /*data*/,
                                 struct zxdg_output_v1 * /*output*/,
                                 const char * /*name*/) {}
void Screenshot::onXdgOutputDescription(void * /*data*/,
                                        struct zxdg_output_v1 * /*output*/,
                                        const char * /*description*/) {}

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
      !std::holds_alternative<WlrScreencopyFrameMetadata>(payload->buffer)) {
    return;
  }
  payload->onSuccess(std::get<ShmBuffer *>(payload->buffer));
  delete payload;
  zwlr_screencopy_frame_v1_destroy(frame);
}

void Screenshot::onWlScreencopyFrameFailed(
    void *data, struct zwlr_screencopy_frame_v1 *frame) {
  const auto *payload = wlScreencopyFramePayload(data);
  if (payload == nullptr) {
    return;
  }
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
  if (payload == nullptr) {
    return;
  }
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
  munmap(mImage.bits(), mImage.sizeInBytes());
  wl_buffer_destroy(mHandle);
  wl_shm_pool_destroy(mShmPool);
}
