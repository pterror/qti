#include "folder.hpp"

void Folder::setPath(QString path) {
  FilesystemEntry::setPath(path);
  this->reload();
}

QList<File *> Folder::files() {
  if (!this->mFiles) {
    this->reloadFiles();
  }
  return *this->mFiles;
}

QList<Folder *> Folder::folders() {
  if (!this->mFolders) {
    this->reloadFolders();
  }
  return *this->mFolders;
}

bool Folder::watching() const { return this->mWatching; }
void Folder::setWatching(bool watching) {
  this->mWatching = watching;
  emit this->watchingChanged();
  this->reload();
}

void Folder::reload() {
  if (this->path() == "") {
    return;
  }
  this->mDir.setPath(this->path());
  this->mDir.refresh();
  if (this->mFiles) {
    this->reloadFiles();
    emit this->filesChanged();
  }
  if (this->mFolders) {
    this->reloadFolders();
    emit this->foldersChanged();
  }
  if (this->mWatching) {
    this->mWatcher.addPath(this->path());
  } else {
    this->mWatcher.removePath(this->path());
  }
  // this->mWatcher.event(QEvent *event);
}

void Folder::reloadFiles() {
  this->mFiles.reset();
  this->mFiles = QList<File *>();
  const auto fileChildrenNames = this->mDir.entryList(QDir::Files);
  const auto basePath = this->path();
  for (const auto &name : fileChildrenNames) {
    auto *const child = this->mFiles->emplace_back(new File(this));
    child->setPath(basePath + "/" + name);
  }
}

void Folder::reloadFolders() {
  this->mFolders.reset();
  this->mFolders = QList<Folder *>();
  const auto directoryChildrenNames =
      this->mDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
  const auto basePath = this->path();
  for (const auto &name : directoryChildrenNames) {
    auto *const child = this->mFolders->emplace_back(new Folder(this));
    child->setPath(basePath + "/" + name);
  }
}
