#include "zwlr_screencopy_manager.hpp"

ZwlrScreencopyManager::ZwlrScreencopyManager()
    : QWaylandClientExtensionTemplate<ZwlrScreencopyManager>(
          zwlr_screencopy_manager_v1_interface.version) {
  this->initialize();
}

ZwlrScreencopyManager *ZwlrScreencopyManager::instance() {
  static ZwlrScreencopyManager *instance = nullptr; // NOLINT
  if (instance == nullptr) {
    instance = new ZwlrScreencopyManager();
  }
  return instance;
}

QtWayland::zwlr_screencopy_frame_v1 *
ZwlrScreencopyManager::captureOutput(bool overlayCursor, ::wl_output *output) {
  auto *frame = this->capture_output(overlayCursor ? 1 : 0, output);
  return new QtWayland::zwlr_screencopy_frame_v1(frame);
}

QtWayland::zwlr_screencopy_frame_v1 *ZwlrScreencopyManager::captureOutputRegion(
    int32_t overlayCursor, ::wl_output *output, int32_t x, int32_t y,
    int32_t width, int32_t height) {
  auto *frame =
      this->capture_output_region(overlayCursor, output, x, y, width, height);
  return new QtWayland::zwlr_screencopy_frame_v1(frame);
}
