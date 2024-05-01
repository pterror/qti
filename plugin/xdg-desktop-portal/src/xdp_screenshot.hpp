#pragma once

#include <QQuickImageProvider>
#include <QtQml/QQmlEngine>
#include <portal_screenshot.h>
#include <qjsvalue.h>

class XdpScreenshotImageProvider : public QQuickImageProvider {
public:
  XdpScreenshotImageProvider();

  [[nodiscard]] QPixmap requestPixmap(const QString &id, QSize *size,
                                      const QSize &requestedSize) override;

  [[nodiscard]] QUrl cache(QPixmap pixmap);
  void free(const QUrl &url);

private:
  QMap<QUuid, QPixmap> mCache;
};

class XdpScreenshot : public QObject {
  Q_OBJECT;
  QML_NAMED_ELEMENT(XdpScreenshot);
  QML_SINGLETON;

public:
  XdpScreenshot();

  Q_INVOKABLE void capturePrimaryScreen(const QJSValue &onSuccess,
                                        const QJSValue &onError);
  Q_INVOKABLE void free(const QUrl &url) const;

private:
  [[nodiscard]] XdpScreenshotImageProvider *screenshotImageProvider() const;

  org::freedesktop::portal::Screenshot mScreenshotInterface;
};
