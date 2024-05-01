#pragma once

#include <QQuickImageProvider>
#include <QtQml/QQmlEngine>
#include <cstdint>
#include <private/qwayland-wayland.h>
#include <qjsvalue.h>
#include <qwayland-wlr-screencopy-unstable-v1.h>
#include <qwayland-xdg-output-unstable-v1.h>

class ScreenshotImageProvider : public QQuickImageProvider {
public:
  ScreenshotImageProvider();

  [[nodiscard]] QPixmap requestPixmap(const QString &id, QSize *size,
                                      const QSize &requestedSize) override;

  [[nodiscard]] QUrl cache(QPixmap pixmap);
  void free(const QUrl &url);

private:
  QMap<QUuid, QPixmap> mCache;
};

class Screenshot : public QObject { // NOLINT
  Q_OBJECT;
  QML_NAMED_ELEMENT(Screenshot);
  QML_SINGLETON;

public:
  Screenshot();
  ~Screenshot() override;

  // TODO: add onSuccess/onFailure to the other two apis too
  // TODO: refactor async frame capture out to a helper function
  Q_INVOKABLE void captureAllScreens(QJSValue onSuccess,
                                     QJSValue onFailure = QJSValue::NullValue,
                                     bool captureCursor = false) const;
  Q_INVOKABLE [[nodiscard]] QUrl
  capturePrimaryScreen(bool captureCursor = false) const;
  Q_INVOKABLE [[nodiscard]] QUrl
  captureScreen(int index, bool captureCursor = false) const;
  Q_INVOKABLE void free(const QUrl &url) const;

signals:
  void ready() const;

private:
  [[nodiscard]] ScreenshotImageProvider *screenshotImageProvider() const;
  static Screenshot *resolve(void *data) {
    return static_cast<Screenshot *>(data);
  }
  static const struct wl_registry_listener REGISTRY_LISTENER;
  static void onWlRegistryGlobal(void *data, struct wl_registry *registry,
                                 uint32_t id, const char *interface,
                                 uint32_t version);
  static void onWlRegistryGlobalRemove(void *data,
                                       struct wl_registry *wlRegistry,
                                       uint32_t id);

  static const wl_output_listener OUTPUT_LISTENER;
  static void onWlOutputGeometry(void *data, wl_output *output, int32_t x,
                                 int32_t y, int32_t width, int32_t height,
                                 int subpixel, const char *make,
                                 const char *model, int32_t transform);
  static void onWlOutputMode(void *data, struct wl_output *output,
                             uint32_t flags, int32_t width, int32_t height,
                             int32_t refresh);
  static void onWlOutputDone(void *data, wl_output *output);
  static void onWlOutputScale(void *data, wl_output *output, int32_t factor);
  static void onWlOutputName(void *data, wl_output *output, const char *name);
  static void onWlOutputDescription(void *data, wl_output *output,
                                    const char *description);

  static const zxdg_output_v1_listener XDG_OUTPUT_LISTENER;
  static void onXdgOutputLogicalPosition(void *data,
                                         struct zxdg_output_v1 *output,
                                         int32_t x, int32_t y);
  static void onXdgOutputLogicalSize(void *data, struct zxdg_output_v1 *output,
                                     int32_t width, int32_t height);
  static void onXdgOutputDone(void *data, struct zxdg_output_v1 *output);
  static void onXdgOutputName(void *data, struct zxdg_output_v1 *output,
                              const char *name);
  static void onXdgOutputDescription(void *data, struct zxdg_output_v1 *output,
                                     const char *description);

  static const zwlr_screencopy_frame_v1_listener FRAME_LISTENER;
  static void onWlScreencopyFrameBuffer(void *data,
                                        struct zwlr_screencopy_frame_v1 *frame,
                                        uint32_t format, uint32_t width,
                                        uint32_t height, uint32_t stride);
  static void onWlScreencopyFrameReady(void *data,
                                       struct zwlr_screencopy_frame_v1 *frame,
                                       uint32_t tvSecHi, uint32_t tvSecLo,
                                       uint32_t tvNsec);
  static void onWlScreencopyFrameFlags(void *data,
                                       struct zwlr_screencopy_frame_v1 *frame,
                                       uint32_t flags);
  static void onWlScreencopyFrameFailed(void *data,
                                        struct zwlr_screencopy_frame_v1 *frame);
  static void onWlScreencopyFrameDamage(void *data,
                                        struct zwlr_screencopy_frame_v1 *frame,
                                        uint32_t x, uint32_t y, uint32_t width,
                                        uint32_t height);
  static void onWlScreencopyFrameLinuxDmabuf(
      void *data, struct zwlr_screencopy_frame_v1 *frame, uint32_t format,
      uint32_t width, uint32_t height);
  static void
  onWlScreencopyFrameBufferDone(void *data,
                                struct zwlr_screencopy_frame_v1 *frame);

  void onWlRegistryGlobal(uint32_t id, const QByteArray &interface);
  void onWlRegistryGlobalRemove(uint32_t id);

  bool mInitialized = false;
  QMap<uint, wl_output *> mWlOutputs;
  QMap<uint, QRect> mWlOutputGeometries;
  wl_display *mWlDisplay = nullptr;
  QtWayland::wl_registry *mWlRegistry = nullptr;
  QtWayland::wl_shm *mWlShm = nullptr;
  QtWayland::zxdg_output_manager_v1 *mXdgOutputManager = nullptr;
  QtWayland::zwlr_screencopy_manager_v1 *mWlrScreencopyManager = nullptr;
};
