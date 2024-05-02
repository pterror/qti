#include "screenshot_wl_registry.hpp"

#include <QAbstractEventDispatcher>
#include <QRect>
#include <QSocketNotifier>
#include <private/qguiapplication_p.h>
#include <qabstracteventdispatcher.h>

namespace {
struct WlOutputPayload {
  ScreenshotWlRegistry *registry;
  uint32_t id = 0;
};
}; // namespace

ScreenshotWlRegistry::ScreenshotWlRegistry(wl_display *display,
                                           struct ::wl_registry *object)
    : QtWayland::wl_registry(object), mDisplay(display) {
  wl_display_roundtrip(this->mDisplay);
  // another roundtrip for wl_output events
  wl_display_roundtrip(this->mDisplay);
  if (this->mXdgOutputManager != nullptr) {
    for (auto kv : this->mOutputs.asKeyValueRange()) {
      auto id = kv.first;
      auto *output = kv.second;
      auto *xdgOutput = zxdg_output_manager_v1_get_xdg_output(
          this->mXdgOutputManager->object(), output);
      auto *data = new WlOutputPayload({.registry = this, .id = id});
      zxdg_output_v1_add_listener(
          xdgOutput, &ScreenshotWlRegistry::XDG_OUTPUT_LISTENER, data);
    }
    wl_display_roundtrip(this->mDisplay);
  }
  auto fd = wl_display_get_fd(this->mDisplay);
  auto *readNotifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
  const auto onActivated = [this]() { wl_display_dispatch(this->mDisplay); };
  QObject::connect(readNotifier, &QSocketNotifier::activated, this,
                   onActivated);
  auto *dispatcher = QGuiApplicationPrivate::eventDispatcher;
  const auto onAwake = [this]() {
    if (wl_display_prepare_read(this->mDisplay) == 0) {
      wl_display_read_events(this->mDisplay);
    }
    wl_display_dispatch_pending(this->mDisplay);
    wl_display_flush(this->mDisplay);
  };
  QObject::connect(dispatcher, &QAbstractEventDispatcher::awake, this, onAwake);
}

QtWayland::wl_shm *ScreenshotWlRegistry::shm() const { return this->mShm; }
QtWayland::zxdg_output_manager_v1 *
ScreenshotWlRegistry::xdgOutputManager() const {
  return this->mXdgOutputManager;
}
QtWayland::zwlr_screencopy_manager_v1 *
ScreenshotWlRegistry::wlrScreencopyManager() const {
  return this->mWlrScreencopyManager;
}
QMap<uint, wl_output *> ScreenshotWlRegistry::outputs() const {
  return this->mOutputs;
}
QMap<uint, QRect> ScreenshotWlRegistry::outputGeometries() const {
  return this->mOutputGeometries;
}

void ScreenshotWlRegistry::registry_global(uint32_t id,
                                           const QString &interface,
                                           uint32_t version) {
  auto versionInt = static_cast<int>(version);
  if (interface == wl_output_interface.name) {
    auto *output = static_cast<wl_output *>(
        wl_registry_bind(this->object(), id, &wl_output_interface, version));
    this->mOutputs.insert(id, output);
    auto *data = new WlOutputPayload({.registry = this, .id = id});
    wl_output_add_listener(output, &ScreenshotWlRegistry::OUTPUT_LISTENER,
                           data);
  } else if (interface == wl_shm_interface.name) {
    this->mShm = new QtWayland::wl_shm(this->object(), id, versionInt);
  } else if (interface == zwlr_screencopy_manager_v1_interface.name) {
    this->mWlrScreencopyManager = new QtWayland::zwlr_screencopy_manager_v1(
        this->object(), id, versionInt);
  } else if (interface == zxdg_output_manager_v1_interface.name) {
    this->mXdgOutputManager =
        new QtWayland::zxdg_output_manager_v1(this->object(), id, versionInt);
  }
}

void ScreenshotWlRegistry::registry_global_remove(uint32_t id) {
  this->mOutputs.remove(id);
  this->mOutputGeometries.remove(id);
}

const wl_output_listener ScreenshotWlRegistry::OUTPUT_LISTENER = {
    .geometry = ScreenshotWlRegistry::onWlOutputGeometry,
    .mode = ScreenshotWlRegistry::onWlOutputMode,
    .done = ScreenshotWlRegistry::onWlOutputDone,
    .scale = ScreenshotWlRegistry::onWlOutputScale,
    .name = ScreenshotWlRegistry::onWlOutputName,
    .description = ScreenshotWlRegistry::onWlOutputDescription};

inline WlOutputPayload *wlOutputPayload(void *data) {
  return static_cast<WlOutputPayload *>(data);
}

void ScreenshotWlRegistry::onWlOutputGeometry(
    void *data, wl_output * /*output*/, int32_t x, int32_t y, int32_t /*width*/,
    int32_t /*height*/, int /*subpixel*/, const char * /*make*/,
    const char * /*model*/, int32_t /*transform*/) {
  const auto *payload = wlOutputPayload(data);
  payload->registry->mOutputGeometries[payload->id] = QRect(x, y, 0, 0);
}

void ScreenshotWlRegistry::onWlOutputMode(void *data,
                                          struct wl_output * /*output*/,
                                          uint32_t flags, int32_t width,
                                          int32_t height, int32_t /*refresh*/) {
  const auto *payload = wlOutputPayload(data);
  if ((flags & WL_OUTPUT_MODE_CURRENT) != 0) {
    auto *rect = &payload->registry->mOutputGeometries[payload->id];
    rect->setWidth(width);
    rect->setHeight(height);
  }
}

void ScreenshotWlRegistry::onWlOutputDone(void * /*data*/,
                                          wl_output * /*output*/) {}

void ScreenshotWlRegistry::onWlOutputScale(void * /*data*/,
                                           wl_output * /*output*/,
                                           int32_t /*factor*/) {}

void ScreenshotWlRegistry::onWlOutputName(void * /*data*/,
                                          wl_output * /*output*/,
                                          const char * /* name */) {}

void ScreenshotWlRegistry::onWlOutputDescription(
    void * /*data*/, wl_output * /*output*/, const char * /* description */) {}

const zxdg_output_v1_listener ScreenshotWlRegistry::XDG_OUTPUT_LISTENER = {
    .logical_position = ScreenshotWlRegistry::onXdgOutputLogicalPosition,
    .logical_size = ScreenshotWlRegistry::onXdgOutputLogicalSize,
    .done = ScreenshotWlRegistry::onXdgOutputDone,
    .name = ScreenshotWlRegistry::onXdgOutputName,
    .description = ScreenshotWlRegistry::onXdgOutputDescription,
};

using XdgOutputPayload = WlOutputPayload;

inline XdgOutputPayload *xdgOutputPayload(void *data) {
  return static_cast<XdgOutputPayload *>(data);
}

void ScreenshotWlRegistry::onXdgOutputLogicalPosition(
    void *data, struct zxdg_output_v1 * /*output*/, int32_t x, int32_t y) {
  const auto *payload = xdgOutputPayload(data);
  auto *rect = &payload->registry->mOutputGeometries[payload->id];
  rect->setX(x);
  rect->setY(y);
}

void ScreenshotWlRegistry::onXdgOutputLogicalSize(
    void *data, struct zxdg_output_v1 * /*output*/, int32_t width,
    int32_t height) {
  const auto *payload = xdgOutputPayload(data);
  auto *rect = &payload->registry->mOutputGeometries[payload->id];
  rect->setWidth(width);
  rect->setHeight(height);
}

void ScreenshotWlRegistry::onXdgOutputDone(void *data,
                                           struct zxdg_output_v1 * /*output*/) {
  const auto *payload = xdgOutputPayload(data);
  delete payload;
}

void ScreenshotWlRegistry::onXdgOutputName(void * /*data*/,
                                           struct zxdg_output_v1 * /*output*/,
                                           const char * /*name*/) {}

void ScreenshotWlRegistry::onXdgOutputDescription(
    void * /*data*/, struct zxdg_output_v1 * /*output*/,
    const char * /*description*/) {}
