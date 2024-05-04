#pragma once

#include <QFile>
#include <QtQml/QQmlEngine>

#include "filesystem_entry.hpp"

class File : public FilesystemEntry {
  Q_OBJECT;
  QML_NAMED_ELEMENT(File);
  Q_PROPERTY(bool opened READ opened WRITE setOpened NOTIFY openedChanged);
  Q_PROPERTY(
      bool autoFlush READ autoFlush WRITE setAutoFlush NOTIFY autoFlushChanged);
  Q_PROPERTY(
      bool readable READ readable WRITE setReadable NOTIFY readableChanged);
  Q_PROPERTY(
      bool writable READ writable WRITE setWritable NOTIFY writableChanged);
  Q_PROPERTY(bool shouldAppend READ shouldAppend WRITE setShouldAppend NOTIFY
                 shouldAppendChanged);
  Q_PROPERTY(bool shouldTruncate READ shouldTruncate WRITE setShouldTruncate
                 NOTIFY shouldTruncateChanged);
  Q_PROPERTY(bool binary READ binary WRITE setBinary NOTIFY binaryChanged);
  Q_PROPERTY(QString mimeType READ mimeType NOTIFY mimeTypeChanged);
  Q_PROPERTY(QString mimeTypeIconName READ mimeTypeIconName NOTIFY
                 mimeTypeIconNameChanged);

public:
  explicit File(QObject *parent = nullptr) : FilesystemEntry(parent) {}

  void setPath(QString path) override;

  [[nodiscard]] bool opened() const;
  void setOpened(bool opened);

  [[nodiscard]] bool autoFlush() const;
  void setAutoFlush(bool autoFlush);

  [[nodiscard]] bool readable() const;
  void setReadable(bool readable);

  [[nodiscard]] bool shouldAppend() const;
  void setShouldAppend(bool shouldAppend);

  [[nodiscard]] bool shouldTruncate() const;
  void setShouldTruncate(bool shouldTruncate);

  [[nodiscard]] bool writable() const;
  void setWritable(bool writable);

  [[nodiscard]] bool binary() const;
  void setBinary(bool binary);

  [[nodiscard]] QString mimeType() const;

  [[nodiscard]] QString mimeTypeIconName() const;

  Q_INVOKABLE void open();
  Q_INVOKABLE void close();
  Q_INVOKABLE [[nodiscard]] QString read();
  Q_INVOKABLE void write(const QString &contents);
  Q_INVOKABLE void append(const QString &contents);

signals:
  void openedChanged();
  void autoFlushChanged();
  void readableChanged();
  void writableChanged();
  void shouldAppendChanged();
  void shouldTruncateChanged();
  void binaryChanged();
  void mimeTypeChanged();
  void mimeTypeIconNameChanged();

private:
  void reload();

  QFile mFile;
  bool mAutoFlush = true;
  bool mOpened = false;
  bool mReadable = false;
  bool mWritable = false;
  bool mShouldAppend = false;
  bool mShouldTruncate = false;
  bool mBinary = false;
};
