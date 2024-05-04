#include "zxdg_output.hpp"

ZxdgOutput::ZxdgOutput(::zxdg_output_v1 *object)
    : QWaylandClientExtensionTemplate<ZxdgOutput>(
          zxdg_output_v1_interface.version),
      QtWayland::zxdg_output_v1(object) {}

void ZxdgOutput::zxdg_output_v1_logical_position(int32_t x, int32_t y) {
  this->bounds.setX(x);
  this->bounds.setY(y);
}

void ZxdgOutput::zxdg_output_v1_logical_size(int32_t width, int32_t height) {
  this->bounds.setWidth(width);
  this->bounds.setHeight(height);
}

void ZxdgOutput::zxdg_output_v1_done() { emit this->done(); }

void ZxdgOutput::zxdg_output_v1_name(const QString &name) { this->name = name; }

void ZxdgOutput::zxdg_output_v1_description(const QString &description) {
  this->description = description;
}
