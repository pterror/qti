#pragma once

#include <QtQml/QQmlEngine>

class Screenshot : public QObject {
  Q_OBJECT;
  QML_NAMED_ELEMENT(Screenshot);
  QML_SINGLETON;

public:
  Q_INVOKABLE [[nodiscard]] static QUrl captureAllScreens();
  Q_INVOKABLE [[nodiscard]] static QUrl capturePrimaryScreen();
  Q_INVOKABLE [[nodiscard]] static QUrl captureScreen(int index);
  Q_INVOKABLE static void free(const QUrl &url);
};
