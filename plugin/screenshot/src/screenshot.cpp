#include "screenshot.hpp"

#include <QGuiApplication>
#include <QPainter>
#include <QPixmap>
#include <QScreen>
#include <QUuid>
#include <qurl.h>
#include <utility>

namespace {
const auto URL_PREFIX = QString("image://qti_screenshot/");
// NOLINTNEXTLINE i genuinely have no idea how to avoid a global cache
auto pixmapCache = QMap<QUuid, QPixmap>();

QUrl cache(QPixmap pixmap) {
  const auto uuid = QUuid();
  pixmapCache[uuid] = std::move(pixmap);
  return QUrl(URL_PREFIX + uuid.toString());
}
}; // namespace

QUrl Screenshot::captureAllScreens() {
  const auto screens = QGuiApplication::screens();
  auto pixmap = QPixmap();
  if (screens.size() == 1) {
    pixmap = screens[0]->grabWindow(0);
  } else {
    auto left = 0;
    auto right = 0;
    auto top = 0;
    auto bottom = 0;
    for (auto *const screen : screens) {
      const auto geometry = screen->geometry();
      left = std::min(left, geometry.left());
      right = std::max(right, geometry.right());
      top = std::min(top, geometry.top());
      bottom = std::max(bottom, geometry.bottom());
    }
    pixmap = QPixmap(right - left, bottom - top);
    pixmap.fill(Qt::transparent);
    auto painter = QPainter(&pixmap);
    for (auto *const screen : screens) {
      const auto geometry = screen->geometry();
      const auto coordinate =
          QPoint(geometry.left() - left, geometry.top() - top);
      painter.drawPixmap(coordinate, screen->grabWindow(0));
    }
  }
  return cache(pixmap);
}

QUrl Screenshot::capturePrimaryScreen() {
  auto *const screen = QGuiApplication::primaryScreen();
  return cache(screen->grabWindow(0));
}

QUrl Screenshot::captureScreen(int index) {
  auto *const screen = QGuiApplication::screens()[index];
  return cache(screen->grabWindow(0));
}

void Screenshot::free(const QUrl &url) {
  const auto urlString = url.toString();
  if (!urlString.startsWith(URL_PREFIX)) {
    return;
  }
  auto uuid = QUuid::fromString(urlString.sliced(urlString.length()));
  pixmapCache.remove(uuid);
}
