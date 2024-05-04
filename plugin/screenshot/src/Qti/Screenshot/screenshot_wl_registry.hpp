#pragma once

#include <QtQml/QQmlEngine>
#include <private/qwayland-wayland.h>
#include <qwayland-wlr-screencopy-unstable-v1.h>
#include <qwayland-xdg-output-unstable-v1.h>

class ScreenshotWlRegistry : public QObject, public QtWayland::wl_registry {
  Q_OBJECT;

public:
  explicit ScreenshotWlRegistry(wl_display *display,
                                struct ::wl_registry *object);

  [[nodiscard]] QtWayland::wl_shm *shm() const;
  [[nodiscard]] QMap<uint, wl_output *> outputs() const;
  [[nodiscard]] QMap<uint, QRect> outputGeometries() const;

private:
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

  void registry_global(uint32_t id, const QString &interface,
                       uint32_t version) override;
  void registry_global_remove(uint32_t id) override;

  wl_display *mDisplay = nullptr;
  QtWayland::wl_shm *mShm = nullptr;
  QMap<uint, wl_output *> mOutputs;
  QMap<uint, QRect> mOutputGeometries;
};