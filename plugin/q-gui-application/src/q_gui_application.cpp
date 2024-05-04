#include "q_gui_application.hpp"

#include <QGuiApplication>

bool QQGuiApplication::quitOnLastWindowClosed() { // NOLINT
  return QGuiApplication::quitOnLastWindowClosed();
}

void QQGuiApplication::setQuitOnLastWindowClosed(bool quit) { // NOLINT
  QGuiApplication::setQuitOnLastWindowClosed(quit);
  emit this->quitOnLastWindowClosedChanged();
}

void QQGuiApplication::deleteLater(QObject *object) { object->deleteLater(); }

QImage QQGuiApplication::copyImage(const QImage &image, const QRect &rect) {
  return image.copy(rect);
}

QPixmap QQGuiApplication::copyPixmap(const QPixmap &pixmap, const QRect &rect) {
  return pixmap.copy(rect);
}
