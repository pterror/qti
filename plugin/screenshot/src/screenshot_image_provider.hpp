#pragma once

#include <QQuickImageProvider>

class ScreenshotImageProvider : public QQuickImageProvider {
  Q_OBJECT;

public:
  ScreenshotImageProvider();

  static ScreenshotImageProvider *instance(QQmlEngine *engine);

  [[nodiscard]] QPixmap requestPixmap(const QString &id, QSize *size,
                                      const QSize &requestedSize) override;

  [[nodiscard]] QUrl cache(QPixmap pixmap);
  void free(const QUrl &url);

private:
  QMap<QUuid, QPixmap> mCache;
};
