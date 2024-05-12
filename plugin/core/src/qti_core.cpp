#include "qti_core.hpp"

#include <QGuiApplication>
#include <cstdlib>

bool QtiCore::quitOnLastWindowClosed() { // NOLINT
  return QGuiApplication::quitOnLastWindowClosed();
}

void QtiCore::setQuitOnLastWindowClosed(bool quit) { // NOLINT
  QGuiApplication::setQuitOnLastWindowClosed(quit);
  emit this->quitOnLastWindowClosedChanged();
}

QString QtiCore::env(const QString &variable) {
  return QString(std::getenv(variable.toStdString().c_str())); // NOLINT
}

QImage QtiCore::copyImage(const QImage &image, const QRect &rect) {
  return image.copy(rect);
}

QPixmap QtiCore::copyPixmap(const QPixmap &pixmap, const QRect &rect) {
  return pixmap.copy(rect);
}
