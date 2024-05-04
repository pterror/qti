#pragma once

#include "screenshot_wl_registry.hpp"

#include <QQuickImageProvider>
#include <QtQml/QQmlEngine>

class ScreenshotImageProvider : public QQuickImageProvider {
public:
  ScreenshotImageProvider();

  [[nodiscard]] QPixmap requestPixmap(const QString &id, QSize *size,
                                      const QSize &requestedSize) override;

  [[nodiscard]] QUrl cache(QPixmap pixmap);
  void free(const QUrl &url);

private:
  QMap<QUuid, QPixmap> mCache;
};

class Screenshot : public QObject { // NOLINT
  Q_OBJECT;
  QML_NAMED_ELEMENT(Screenshot);
  QML_SINGLETON;

public:
  explicit Screenshot();
  ~Screenshot() override;

  // TODO: add onSuccess/onFailure to the other two apis too
  // TODO: refactor async frame capture out to a helper function
  Q_INVOKABLE void
  captureAllScreens(const QJSValue &onSuccess,
                    const QJSValue &onFailure = QJSValue::NullValue,
                    bool captureCursor = false) const;
  Q_INVOKABLE [[nodiscard]] QUrl
  capturePrimaryScreen(bool captureCursor = false) const;
  Q_INVOKABLE [[nodiscard]] QUrl
  captureScreen(int index, bool captureCursor = false) const;
  Q_INVOKABLE void free(const QUrl &url) const;

signals:
  void ready() const;

private:
  [[nodiscard]] ScreenshotImageProvider *screenshotImageProvider() const;
  static Screenshot *resolve(void *data) {
    return static_cast<Screenshot *>(data);
  }

  static const zwlr_screencopy_frame_v1_listener FRAME_LISTENER;
  static void onWlScreencopyFrameBuffer(void *data,
                                        struct zwlr_screencopy_frame_v1 *frame,
                                        uint32_t format, uint32_t width,
                                        uint32_t height, uint32_t stride);
  static void onWlScreencopyFrameReady(void *data,
                                       struct zwlr_screencopy_frame_v1 *frame,
                                       uint32_t tvSecHi, uint32_t tvSecLo,
                                       uint32_t tvNsec);
  static void onWlScreencopyFrameFlags(void *data,
                                       struct zwlr_screencopy_frame_v1 *frame,
                                       uint32_t flags);
  static void onWlScreencopyFrameFailed(void *data,
                                        struct zwlr_screencopy_frame_v1 *frame);
  static void onWlScreencopyFrameDamage(void *data,
                                        struct zwlr_screencopy_frame_v1 *frame,
                                        uint32_t x, uint32_t y, uint32_t width,
                                        uint32_t height);
  static void onWlScreencopyFrameLinuxDmabuf(
      void *data, struct zwlr_screencopy_frame_v1 *frame, uint32_t format,
      uint32_t width, uint32_t height);
  static void
  onWlScreencopyFrameBufferDone(void *data,
                                struct zwlr_screencopy_frame_v1 *frame);

  void onWlRegistryGlobal(uint32_t id, const QByteArray &interface);
  void onWlRegistryGlobalRemove(uint32_t id);

  bool mInitialized = false;
  wl_display *mWlDisplay = nullptr;
  ScreenshotWlRegistry *mWlRegistry = nullptr;
};