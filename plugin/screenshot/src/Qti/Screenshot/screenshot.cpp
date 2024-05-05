#include "screenshot.hpp"

// FIXME: this is not cross platform
#include "screenshot_image_provider.hpp"
#include "shm_buffer.hpp"
#include "zwlr_screencopy_frame.hpp"
#include "zwlr_screencopy_manager.hpp"

#include <limits>

#include <QtGui/qpa/qplatformnativeinterface.h>
#include <QtWaylandClient/private/qwaylanddisplay_p.h>

inline QtWaylandClient::QWaylandDisplay *wlDisplay() {
  auto *display =
      QGuiApplication::platformNativeInterface()->nativeResourceForIntegration(
          "display");
  return static_cast<QtWaylandClient::QWaylandDisplay *>(display);
}

void Screenshot::free(const QUrl &url) const {
  auto *provider = ScreenshotImageProvider::instance(qmlEngine(this));
  provider->free(url);
}

QImage Screenshot::grabWindowWaylandInternal(QScreen *screen,
                                             bool captureCursor) const {
  auto *outputRaw =
      QGuiApplication::platformNativeInterface()->nativeResourceForScreen(
          "output", screen);
  auto *output = static_cast<::wl_output *>(outputRaw);
  auto *screencopyManager = ZwlrScreencopyManager::instance();
  auto *frame = screencopyManager->captureOutput(captureCursor, output);
  auto image = QImage();
  const auto onReady = [&](ShmBuffer *buffer) {
    image = buffer->mImage;
    delete buffer;
  };
  QObject::connect(frame, &ZwlrScreencopyFrame::ready, this, onReady);
  wlDisplay()->forceRoundTrip();
  return image;
}

QUrl Screenshot::captureScreens(const QList<QScreen *> &screens,
                                bool captureCursor) const {
  auto pixmap = QPixmap();
  if (screens.size() == 1) {
    if (!ZwlrScreencopyManager::instance()->isActive()) {
      pixmap = screens[0]->grabWindow();
    } else {
      pixmap = QPixmap::fromImage(
          this->grabWindowWaylandInternal(screens[0], captureCursor));
    }
  } else {
    const auto minInt = std::numeric_limits<int>::min();
    const auto maxInt = std::numeric_limits<int>::max();
    auto bounds = QRect(QPoint(maxInt, maxInt), QPoint(minInt, minInt));
    for (auto *const screen : screens) {
      const auto geometry = screen->geometry();
      bounds.setLeft(std::min(bounds.left(), geometry.left()));
      bounds.setRight(std::max(bounds.right(), geometry.right()));
      bounds.setTop(std::min(bounds.top(), geometry.top()));
      bounds.setBottom(std::max(bounds.bottom(), geometry.bottom()));
    }
    auto pixmap = QPixmap(bounds.width(), bounds.height());
    auto painter = QPainter(&pixmap);
    for (auto *const screen : screens) {
      const auto screenBounds = screen->geometry();
      if (!ZwlrScreencopyManager::instance()->isActive()) {
        painter.drawPixmap(screenBounds, screen->grabWindow());
      } else {
        painter.drawImage(screenBounds, this->grabWindowWaylandInternal(
                                            screen, captureCursor));
      }
    }
  }
  auto *provider = ScreenshotImageProvider::instance(qmlEngine(this));
  return provider->cache(pixmap);
}
