#pragma once

#include <QDir>
#include <QFileSystemWatcher>
#include <QtQml/QQmlEngine>

#include "file.hpp"
#include "filesystem_entry.hpp"

class Folder : public FilesystemEntry {
  Q_OBJECT;
  QML_NAMED_ELEMENT(Folder);

  Q_PROPERTY(
      bool watching READ watching WRITE setWatching NOTIFY watchingChanged);
  Q_PROPERTY(QList<File *> files READ files NOTIFY filesChanged);
  Q_PROPERTY(QList<Folder *> folders READ folders NOTIFY foldersChanged);

public:
  explicit Folder(QObject *parent = nullptr) : FilesystemEntry(parent) {}

  void setPath(QString path) override;

  [[nodiscard]] QList<File *> files();

  [[nodiscard]] QList<Folder *> folders();

  [[nodiscard]] bool watching() const;
  void setWatching(bool watching);

signals:
  void watchingChanged();
  void filesChanged();
  void foldersChanged();

private:
  void reload();
  void reloadFiles();
  void reloadFolders();

  std::optional<QList<File *>> mFiles;
  std::optional<QList<Folder *>> mFolders;
  bool mWatching = false;
  QDir mDir;
  QFileSystemWatcher mWatcher;
};
