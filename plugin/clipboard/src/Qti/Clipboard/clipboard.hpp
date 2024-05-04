#pragma once

#include <QClipboard>
#include <QMimeData>
#include <QPixmap>
#include <QtQml/QQmlEngine>
#include <qclipboard.h>

class Clipboard : public QObject { // NOLINT
  Q_OBJECT;
  QML_NAMED_ELEMENT(Clipboard);
  QML_SINGLETON;

public:
  explicit Clipboard();

  enum Mode {
    Global = QClipboard::Clipboard,
    Selection = QClipboard::Selection,
    FindBuffer = QClipboard::FindBuffer,
  };
  Q_ENUMS(Mode);

  Q_INVOKABLE static void
  setImage(const QImage &image, QClipboard::Mode mode = QClipboard::Clipboard);
  Q_INVOKABLE static void
  setMimeData(QMimeData *src, QClipboard::Mode mode = QClipboard::Clipboard);
  Q_INVOKABLE static void
  setPixmap(const QPixmap &pixmap,
            QClipboard::Mode mode = QClipboard::Clipboard);
  Q_INVOKABLE static void
  setText(const QString &text, QClipboard::Mode mode = QClipboard::Clipboard);

signals:
  void changed(QClipboard::Mode mode);
};
