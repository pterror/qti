#pragma once

#include <QtWaylandClient/private/qwayland-xdg-output-unstable-v1.h>

#include <QtWaylandClient>

class ZxdgOutputManager
    : public QWaylandClientExtensionTemplate<ZxdgOutputManager>,
      public QtWayland::zxdg_output_manager_v1 {
public:
  explicit ZxdgOutputManager();

  [[nodiscard]] static ZxdgOutputManager *instance();

  [[nodiscard]] QtWayland::zxdg_output_v1 *
  getXdgOutput(struct ::wl_output *output);
};
