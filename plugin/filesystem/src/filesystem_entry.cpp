#include "filesystem_entry.hpp"

#include <utility>

QString FilesystemEntry::path() const { return this->mPath; }
void FilesystemEntry::setPath(QString path) {
  this->mPath = std::move(path);
  emit this->pathChanged();
}
