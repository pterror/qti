#pragma once

#include <QtQml/QQmlEngine>

class FilesystemEntry : public QObject {
  Q_OBJECT;
  QML_NAMED_ELEMENT(FilesystemEntry);

  Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged);
  // FIXME: move, copy, create directory, mkdir -p.
  // these messages should *ideally* be shared between folders and files
  // (and maybe other things like symlinks, sockets and mount points)
public:
  explicit FilesystemEntry(QObject *parent = nullptr) : QObject(parent) {}

  [[nodiscard]] QString path() const;
  virtual void setPath(QString path);

signals:
  void pathChanged();

private:
  void reload();

  QString mPath;
};
