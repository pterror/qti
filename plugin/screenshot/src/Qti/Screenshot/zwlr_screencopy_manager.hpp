#pragma once

#include <qwayland-wlr-screencopy-unstable-v1.h>

#include <QtQml/QQmlEngine>
#include <QtWaylandClient>

class ZwlrScreencopyManager
    : public QWaylandClientExtensionTemplate<ZwlrScreencopyManager>,
      public QtWayland::zwlr_screencopy_manager_v1 {
  Q_OBJECT;
  QML_NAMED_ELEMENT(ZwlrScreencopyManager);

public:
  explicit ZwlrScreencopyManager();

  [[nodiscard]] static ZwlrScreencopyManager *instance();

  [[nodiscard]] QtWayland::zwlr_screencopy_frame_v1 *
  captureOutput(bool overlayCursor, ::wl_output *output);
  [[nodiscard]] QtWayland::zwlr_screencopy_frame_v1 *
  captureOutputRegion(int32_t overlayCursor, ::wl_output *output, int32_t x,
                      int32_t y, int32_t width, int32_t height);
};
