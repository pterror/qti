#include "xdp_screenshot.hpp"

#include <QUuid>
#include <QtDBus/QDBusConnection>
#include <portal_request.h>
#include <qdbuspendingcall.h>
#include <qjsvalue.h>
#include <qlogging.h>
#include <qobject.h>
#include <quuid.h>

namespace {
const auto FILE_ID = QString("qti_xdp_screenshot");
const auto URL_PREFIX = "image://" + FILE_ID + "/";
QString makeToken() {
  return FILE_ID + "_" + QUuid::createUuid().toString(QUuid::Id128);
}
QString getRequestPath(const QString &token) {
  const auto sender =
      QDBusConnection::sessionBus().baseService().remove(':').replace('.', '_');
  return "/org/freedesktop/portal/desktop/request/" + sender + "/" + token;
}
}; // namespace

XdpScreenshot::XdpScreenshot()
    : mScreenshotInterface("org.freedesktop.portal.Desktop",
                           "/org/freedesktop/portal/desktop",
                           QDBusConnection::sessionBus(), this) {}

XdpScreenshotImageProvider *XdpScreenshot::screenshotImageProvider() const {
  auto *engine = qmlEngine(this);
  auto *imageProviderBase = engine->imageProvider(FILE_ID);
  if (imageProviderBase != nullptr) {
    return dynamic_cast<XdpScreenshotImageProvider *>(imageProviderBase);
  } else {
    auto *imageProvider = new XdpScreenshotImageProvider();
    engine->addImageProvider(FILE_ID, imageProvider);
    return imageProvider;
  }
}

void XdpScreenshot::capturePrimaryScreen(const QJSValue &onSuccess,
                                         const QJSValue &onError) {
  const auto token = makeToken();
  const auto requestPath = getRequestPath(token);
  const auto *request = new org::freedesktop::portal::Request(
      "org.freedesktop.portal.Desktop", requestPath,
      QDBusConnection::sessionBus(), this);
  const auto onResponse = [this, onError, onSuccess](
                              uint response, const QVariantMap &results) {
    if (response != 0) {
      switch (response) {
      case 1: {
        const auto *const message = "org.freedesktop.portal.Screenshot request "
                                    "failed: interaction cancelled by user";
        if (onError.isCallable()) {
          onError.call(QJSValueList({message}));
        } else {
          qWarning() << message;
        }
        break;
      }
      case 2: {
        const auto *const message =
            "org.freedesktop.portal.Screenshot request failed: "
            "interaction cancelled unexpectedly";
        if (onError.isCallable()) {
          onError.call(QJSValueList({message}));
        } else {
          qWarning() << message;
        }
        break;
      }
      default: {
        break;
      }
      }
    }
    return;
    auto uri = results["uri"].value<QString>();
    if (onSuccess.isCallable()) {
      // FIXME: actually this should cache the pixmap
      onSuccess.call(QJSValueList({qmlEngine(this)->toScriptValue(QUrl(uri))}));
    } else {
      qWarning()
          << "missing handler for org.freedesktop.portal.Screenshot response";
    }
  };
  QObject::connect(request, &org::freedesktop::portal::Request::Response, this,
                   onResponse);
  auto reply = this->mScreenshotInterface.Screenshot(
      "", QVariantMap({{"handle_token", token}}));
  // auto reply = this->mScreenshotInterface.Screenshot(
  //     "", QVariantMap({{"handle_token", token}}));
  // TODO: how is this freed?
  const auto *watcher = new QDBusPendingCallWatcher(reply);
  const auto onScreenshot = [onError](QDBusPendingCallWatcher *watcher) {
    const auto reply = QDBusPendingReply<QDBusObjectPath>(*watcher);
    if (reply.isError()) {
      auto error = reply.error();
      const auto message =
          "invalid reply for org.freedesktop.portal.Screenshot call: " +
          error.name() + ": " + error.message();
      if (onError.isCallable()) {
        onError.call(QJSValueList({message}));
      } else {
        qWarning() << message;
      }
      return;
    }
  };
  QObject::connect(watcher, &QDBusPendingCallWatcher::finished, this,
                   onScreenshot);
}

void XdpScreenshot::free(const QUrl &url) const {
  this->screenshotImageProvider()->free(url);
}

XdpScreenshotImageProvider::XdpScreenshotImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap) {}

QPixmap
XdpScreenshotImageProvider::requestPixmap(const QString &id, QSize *size,
                                          const QSize & /*requestedSize*/) {
  const auto uuid = QUuid::fromString(id);
  auto pixmap = this->mCache[uuid];
  *size = pixmap.size();
  qInfo("%d %d", size->width(), size->height());
  return pixmap;
}

QUrl XdpScreenshotImageProvider::cache(QPixmap pixmap) {
  const auto uuid = QUuid::createUuid();
  qInfo("PM %d %d", pixmap.width(), pixmap.height());
  this->mCache[uuid] = std::move(pixmap);
  return QUrl(URL_PREFIX + uuid.toString(QUuid::WithoutBraces));
}

void XdpScreenshotImageProvider::free(const QUrl &url) {
  const auto urlString = url.toString();
  if (!urlString.startsWith(URL_PREFIX)) {
    return;
  }
  auto uuid = QUuid::fromString(urlString.sliced(urlString.length()));
  this->mCache.remove(uuid);
}
