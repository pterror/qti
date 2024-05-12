#pragma once

#include <QClipboard>
#include <QMimeData>
#include <QPixmap>
#include <QtQml/QQmlEngine>
#include <qclipboard.h>
#include <qtmetamacros.h>

class QtiCore : public QObject { // NOLINT
  Q_OBJECT;
  QML_NAMED_ELEMENT(QtiCore);
  QML_SINGLETON;

  Q_PROPERTY(
      bool quitOnLastWindowClosed READ quitOnLastWindowClosed WRITE
          setQuitOnLastWindowClosed NOTIFY quitOnLastWindowClosedChanged);

public:
  [[nodiscard]] bool quitOnLastWindowClosed();
  void setQuitOnLastWindowClosed(bool quit);

  Q_INVOKABLE [[nodiscard]] static QString env(const QString &variable);
  Q_INVOKABLE [[nodiscard]] static QImage copyImage(const QImage &image,
                                                    const QRect &rect);
  Q_INVOKABLE [[nodiscard]] static QPixmap copyPixmap(const QPixmap &pixmap,
                                                      const QRect &rect);

signals:
  void quitOnLastWindowClosedChanged();
};
