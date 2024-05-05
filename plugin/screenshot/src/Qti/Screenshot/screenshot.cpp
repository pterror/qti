#include "screenshot.hpp"

// FIXME: this is not cross platform
#include "screenshot_image_provider.hpp"
#include "zwlr_screencopy_frame.hpp"
#include "zwlr_screencopy_manager.hpp"

#include <limits>

#include <QSignalSpy>
#include <private/qwaylanddisplay_p.h>
#include <qnamespace.h>

void Screenshot::free(const QUrl &url) const {
  auto *provider = ScreenshotImageProvider::instance(qmlEngine(this));
  provider->free(url);
}

QImage Screenshot::grabWindowWaylandInternal(QScreen *screen,
                                             bool captureCursor) const {
  auto *wlScreen =
      dynamic_cast<QtWaylandClient::QWaylandScreen *>(screen->handle());
  auto *screencopyManager = ZwlrScreencopyManager::instance();
  auto *frame = screencopyManager->captureOutput(captureCursor, wlScreen);
  auto image = QImage();
  const auto onReady = [&](QtWaylandClient::QWaylandShmBuffer *buffer) {
    image = std::move(*buffer->image());
    delete buffer;
  };
  QObject::connect(frame, &ZwlrScreencopyFrame::ready, this, onReady);
  QSignalSpy spy(frame, &ZwlrScreencopyFrame::ready);
  spy.wait();
  return image;
}

QUrl Screenshot::capture(const QQmlListReference &screens,
                         bool captureCursor) const {
  auto pixmap = QPixmap();
  if (screens.size() == 1) {
    auto *screen =
        dynamic_cast<QQuickScreenInfo *>(screens.at(0))->wrappedScreen();
    if (!ZwlrScreencopyManager::instance()->isActive()) {
      pixmap = screen->grabWindow();
    } else {
      pixmap = QPixmap::fromImage(
          this->grabWindowWaylandInternal(screen, captureCursor));
    }
  } else {
    const auto minInt = std::numeric_limits<int>::min();
    const auto maxInt = std::numeric_limits<int>::max();
    auto bounds = QRect(QPoint(maxInt, maxInt), QPoint(minInt, minInt));
    for (auto i = 0; i < screens.size(); i += 1) {
      auto *screen =
          dynamic_cast<QQuickScreenInfo *>(screens.at(i))->wrappedScreen();
      const auto geometry = screen->geometry();
      bounds.setLeft(std::min(bounds.left(), geometry.left()));
      bounds.setRight(std::max(bounds.right(), geometry.right()));
      bounds.setTop(std::min(bounds.top(), geometry.top()));
      bounds.setBottom(std::max(bounds.bottom(), geometry.bottom()));
    }
    pixmap = QPixmap(bounds.width(), bounds.height());
    auto painter = QPainter(&pixmap);
    for (auto i = 0; i < screens.size(); i += 1) {
      auto *screen =
          dynamic_cast<QQuickScreenInfo *>(screens.at(i))->wrappedScreen();
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