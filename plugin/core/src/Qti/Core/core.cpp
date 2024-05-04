#include "core.hpp"

#include <QGuiApplication>

bool Core::quitOnLastWindowClosed() { // NOLINT
  return QGuiApplication::quitOnLastWindowClosed();
}

void Core::setQuitOnLastWindowClosed(bool quit) { // NOLINT
  QGuiApplication::setQuitOnLastWindowClosed(quit);
  emit this->quitOnLastWindowClosedChanged();
}

void Core::deleteLater(QObject *object) { object->deleteLater(); }

QImage Core::copyImage(const QImage &image, const QRect &rect) {
  return image.copy(rect);
}

QPixmap Core::copyPixmap(const QPixmap &pixmap, const QRect &rect) {
  return pixmap.copy(rect);
}
