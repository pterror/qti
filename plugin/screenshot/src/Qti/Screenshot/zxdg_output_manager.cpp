#include "zxdg_output_manager.hpp"

ZxdgOutputManager::ZxdgOutputManager()
    : QWaylandClientExtensionTemplate<ZxdgOutputManager>(
          zxdg_output_manager_v1_interface.version) {
  this->initialize();
}

ZxdgOutputManager *ZxdgOutputManager::instance() {
  static ZxdgOutputManager *instance = nullptr; // NOLINT
  if (instance == nullptr) {
    instance = new ZxdgOutputManager();
  }
  return instance;
}

QtWayland::zxdg_output_v1 *
ZxdgOutputManager::getXdgOutput(struct ::wl_output *output) {
  auto *xdgOutput = this->get_xdg_output(output);
  return new QtWayland::zxdg_output_v1(xdgOutput);
}
