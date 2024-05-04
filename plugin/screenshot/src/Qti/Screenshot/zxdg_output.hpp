#pragma once

#include <QtWaylandClient/private/qwayland-xdg-output-unstable-v1.h>

#include <QtQml/QQmlEngine>
#include <QtWaylandClient>

class ZxdgOutput : public QWaylandClientExtensionTemplate<ZxdgOutput>,
                   public QtWayland::zxdg_output_v1 {
  Q_OBJECT;
  QML_NAMED_ELEMENT(ZxdgOutput);

public:
  explicit ZxdgOutput(::zxdg_output_v1 *object);

  QRect bounds;
  QString name;
  QString description;

signals:
  void done();

private:
  void zxdg_output_v1_logical_position(int32_t x, int32_t y) override;
  void zxdg_output_v1_logical_size(int32_t width, int32_t height) override;
  void zxdg_output_v1_done() override;
  void zxdg_output_v1_name(const QString &name) override;
  void zxdg_output_v1_description(const QString &description) override;
};
