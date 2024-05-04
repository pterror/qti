#include "clipboard.hpp"

#include <QGuiApplication>

Clipboard::Clipboard() {
  QObject::connect(QGuiApplication::clipboard(), &QClipboard::changed, this,
                   &Clipboard::changed);
}

void Clipboard::setImage(const QImage &image, QClipboard::Mode mode) {
  QGuiApplication::clipboard()->setImage(image, mode);
}

void Clipboard::setMimeData(QMimeData *src, QClipboard::Mode mode) {
  QGuiApplication::clipboard()->setMimeData(src, mode);
}

void Clipboard::setPixmap(const QPixmap &pixmap, QClipboard::Mode mode) {
  QGuiApplication::clipboard()->setPixmap(pixmap, mode);
}

void Clipboard::setText(const QString &text, QClipboard::Mode mode) {
  QGuiApplication::clipboard()->setText(text, mode);
}
