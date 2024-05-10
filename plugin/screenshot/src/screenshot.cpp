#include "screenshot.hpp"

// FIXME: this is not cross platform
#include "screenshot_image_provider.hpp"
#include "zwlr_screencopy_frame.hpp"
#include "zwlr_screencopy_manager.hpp"

#include <limits>
#include <memory>

void Screenshot::free(const QUrl &url) const {
  auto *provider = ScreenshotImageProvider::instance(qmlEngine(this));
  provider->free(url);
}

QImage Screenshot::grabWindowWaylandInternal(
    const QScreen &screen, bool captureCursor,
    const std::function<void(QtWaylandClient::QWaylandShmBuffer *)> &onReady,
    const std::function<void()> &onFailure) const {
  auto *wlScreen =
      dynamic_cast<QtWaylandClient::QWaylandScreen *>(screen.handle());
  auto *screencopyManager = ZwlrScreencopyManager::instance();
  auto *frame = screencopyManager->captureOutput(captureCursor, wlScreen);
  auto image = QImage();
  // it's difficult to create a lambda here because it's difficult to properly
  // manage its lifetime
  QObject::connect(frame, &ZwlrScreencopyFrame::ready, this, onReady);
  QObject::connect(frame, &ZwlrScreencopyFrame::failed, this, onFailure);
  return image;
}

QUrl Screenshot::cache(const QPixmap &pixmap) const {
  return ScreenshotImageProvider::instance(qmlEngine(this))->cache(pixmap);
}

void Screenshot::capture(const QQmlListReference &screens,
                         const QJSValue &onSuccess, const QJSValue &onFailure,
                         bool captureCursor) const {
  auto *engine = qmlEngine(this);
  if (screens.size() == 1) {
    auto *screen =
        dynamic_cast<QQuickScreenInfo *>(screens.at(0))->wrappedScreen();
    if (!ZwlrScreencopyManager::instance()->isActive()) {
      onSuccess.call(
          {engine->toScriptValue(this->cache(screen->grabWindow()))});
    } else {
      const auto onSuccessCpp =
          [&](QtWaylandClient::QWaylandShmBuffer *buffer) {
            onSuccess.call({engine->toScriptValue(
                this->cache(QPixmap::fromImage(*buffer->image())))});
          };
      const auto onFailureCpp = [&]() { onFailure.call(); };
      grabWindowWaylandInternal(*screen, captureCursor, onSuccessCpp,
                                onFailureCpp);
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
    if (!ZwlrScreencopyManager::instance()->isActive()) {
      auto pixmap = QPixmap(bounds.width(), bounds.height());
      auto painter = QPainter(&pixmap);
      for (auto i = 0; i < screens.size(); i += 1) {
        auto *screen =
            dynamic_cast<QQuickScreenInfo *>(screens.at(i))->wrappedScreen();
        const auto screenBounds = screen->geometry();
        painter.drawPixmap(screenBounds, screen->grabWindow());
      }
      onSuccess.call({engine->toScriptValue(this->cache(pixmap))});
    } else {
      auto pixmap = std::make_shared<QPixmap>(bounds.width(), bounds.height());
      auto painter = std::make_shared<QPainter>(&*pixmap);
      auto failed = std::make_shared<bool>(false);
      auto screensLeft = std::make_shared<qsizetype>(screens.size());
      for (auto i = 0; i < screens.size(); i += 1) {
        auto *screen =
            dynamic_cast<QQuickScreenInfo *>(screens.at(i))->wrappedScreen();
        const auto onSuccessCpp =
            [screen, painter, screensLeft, onSuccess, engine, pixmap,
             this](QtWaylandClient::QWaylandShmBuffer *buffer) {
              const auto screenBounds = screen->geometry();
              painter->drawImage(screenBounds, *buffer->image());
              delete buffer;
              *screensLeft -= 1;
              if (*screensLeft == 0) {
                onSuccess.call({engine->toScriptValue(this->cache(*pixmap))});
                painter->end();
              }
            };
        const auto onFailureCpp = [=]() {
          if (!*failed) {
            onFailure.call();
          }
          *failed = true;
        };
        grabWindowWaylandInternal(*screen, captureCursor, onSuccessCpp,
                                  onFailureCpp);
      }
    }
  }
}