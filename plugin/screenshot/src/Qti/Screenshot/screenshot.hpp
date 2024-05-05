#pragma once

#include <QQmlEngine>
#include <QScreen>
#include <QtQuick/private/qquickscreen_p.h>
#include <private/qwaylandshmbackingstore_p.h>

class Screenshot : public QObject {
  Q_OBJECT;
  QML_NAMED_ELEMENT(Screenshot);
  QML_SINGLETON;

public:
  Q_INVOKABLE void capture(const QQmlListReference &screens,
                           const QJSValue &onSuccess,
                           const QJSValue &onFailure = QJSValue::NullValue,
                           bool captureCursor = false) const;
  Q_INVOKABLE void free(const QUrl &url) const;

signals:
  void ready() const;

private:
  [[nodiscard]] QUrl cache(const QPixmap &pixmap) const;
  QImage grabWindowWaylandInternal(
      const QScreen &screen, bool captureCursor,
      const std::function<void(QtWaylandClient::QWaylandShmBuffer *)> &onReady,
      const std::function<void()> &onFailure) const;

  bool mInitialized = false;
};
