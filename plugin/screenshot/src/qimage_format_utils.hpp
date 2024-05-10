#pragma once

#include <QImage>

// see the note at the end of https://doc.qt.io/qt-6/qimage.html#Format-enum
// ordered by, in order:
// - invalid vs valid
// - grayscale vs color
// - alpha support (no alpha < 2 bit alpha < 8+ bit alpha)
// - performance (lowest to highest)
static uint8_t qImageFormatScore(QImage::Format format) { // NOLINT
  switch (format) {
  case QImage::NImageFormats:
  case QImage::Format_Invalid: {
    return 0;
  }
  case QImage::Format_Mono:
  case QImage::Format_MonoLSB:
  case QImage::Format_Alpha8:
  case QImage::Format_Grayscale8:
  case QImage::Format_Grayscale16: {
    return 1;
  }
  case QImage::Format_Indexed8:
  case QImage::Format_RGB666:
  case QImage::Format_RGB555:
  case QImage::Format_RGB888:
  case QImage::Format_RGB444:
  case QImage::Format_BGR30:
  case QImage::Format_RGB30:
  case QImage::Format_BGR888:
  case QImage::Format_RGBX16FPx4:
  case QImage::Format_RGBX32FPx4: {
    return 2;
  }
  case QImage::Format_RGB16:
  case QImage::Format_RGBX8888:
  case QImage::Format_RGBX64: {
    return 3;
  }
  case QImage::Format_RGB32: {
    return 4;
  }
  case QImage::Format_A2BGR30_Premultiplied:
  case QImage::Format_A2RGB30_Premultiplied: {
    return 5;
  }
  case QImage::Format_ARGB32:
  case QImage::Format_ARGB8565_Premultiplied:
  case QImage::Format_ARGB6666_Premultiplied:
  case QImage::Format_ARGB8555_Premultiplied:
  case QImage::Format_ARGB4444_Premultiplied:
  case QImage::Format_RGBA8888:
  case QImage::Format_RGBA64:
  case QImage::Format_RGBA16FPx4:
  case QImage::Format_RGBA16FPx4_Premultiplied:
  case QImage::Format_RGBA32FPx4:
  case QImage::Format_RGBA32FPx4_Premultiplied: {
    return 6;
  }
  case QImage::Format_RGBA8888_Premultiplied:
  case QImage::Format_RGBA64_Premultiplied: {
    return 7;
  }
  case QImage::Format_ARGB32_Premultiplied: {
    return 8;
  }
  default: {
    return 0;
  }
  }
}
