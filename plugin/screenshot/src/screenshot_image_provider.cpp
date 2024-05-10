#include "screenshot_image_provider.hpp"

#include <QUuid>

const auto ID = QString("qti_screenshot");
const auto URL_PREFIX = QString("image://") + ID + "/";

ScreenshotImageProvider *ScreenshotImageProvider::instance(QQmlEngine *engine) {
  auto *imageProviderBase = engine->imageProvider(ID);
  if (imageProviderBase != nullptr) {
    return dynamic_cast<ScreenshotImageProvider *>(imageProviderBase);
  } else {
    auto *imageProvider = new ScreenshotImageProvider();
    engine->addImageProvider(ID, imageProvider);
    return imageProvider;
  }
}

ScreenshotImageProvider::ScreenshotImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap) {}

QPixmap
ScreenshotImageProvider::requestPixmap(const QString &id, QSize *size,
                                       const QSize & /*requestedSize*/) {
  const auto uuid = QUuid::fromString(id);
  auto pixmap = this->mCache[uuid];
  *size = pixmap.size();
  return pixmap;
}

QUrl ScreenshotImageProvider::cache(QPixmap pixmap) {
  const auto uuid = QUuid::createUuid();
  this->mCache[uuid] = std::move(pixmap);
  return QUrl(URL_PREFIX + uuid.toString(QUuid::WithoutBraces));
}

void ScreenshotImageProvider::free(const QUrl &url) {
  const auto urlString = url.toString();
  if (!urlString.startsWith(URL_PREFIX)) {
    qWarning() << "ScreenshotImageProvider::free: invalid url " +
                      url.toDisplayString();
    return;
  }
  auto uuid = QUuid::fromString(urlString.sliced(URL_PREFIX.length()));
  this->mCache.remove(uuid);
}