#include "zwlr_screencopy_manager.hpp"

ZwlrScreencopyManager::ZwlrScreencopyManager()
    : QWaylandClientExtensionTemplate<ZwlrScreencopyManager>(
          this->extensionInterface()->version) {
  this->initialize();
}

ZwlrScreencopyManager *ZwlrScreencopyManager::instance() {
  static ZwlrScreencopyManager *instance = nullptr; // NOLINT
  if (instance == nullptr) {
    instance = new ZwlrScreencopyManager();
  }
  return instance;
}

ZwlrScreencopyFrame *
ZwlrScreencopyManager::captureOutput(bool overlayCursor,
                                     QtWaylandClient::QWaylandScreen *screen) {
  auto *frame = this->capture_output(overlayCursor ? 1 : 0, screen->output());
  return new ZwlrScreencopyFrame(frame, screen);
}

ZwlrScreencopyFrame *ZwlrScreencopyManager::captureOutputRegion(
    int32_t overlayCursor, QtWaylandClient::QWaylandScreen *screen, int32_t x,
    int32_t y, int32_t width, int32_t height) {
  auto *frame = this->capture_output_region(overlayCursor, screen->output(), x,
                                            y, width, height);
  return new ZwlrScreencopyFrame(frame, screen);
}
