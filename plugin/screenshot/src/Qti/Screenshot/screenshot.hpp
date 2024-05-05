#pragma once

#include <QScreen>
#include <QtQml/QQmlEngine>

class Screenshot : public QObject {
  Q_OBJECT;
  QML_NAMED_ELEMENT(Screenshot);
  QML_SINGLETON;

public:
  Q_INVOKABLE [[nodiscard]] QUrl
  captureScreens(const QList<QScreen *> &screens,
                 bool captureCursor = false) const;
  Q_INVOKABLE void free(const QUrl &url) const;

signals:
  void ready() const;

private:
  QImage grabWindowWaylandInternal(QScreen *screen, bool captureCursor) const;

  bool mInitialized = false;
};
