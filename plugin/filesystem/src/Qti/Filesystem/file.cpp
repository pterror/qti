#include "file.hpp"
#include <QMimeDatabase>

namespace {
const QMimeDatabase MIME_DATABASE = QMimeDatabase();
};

void File::setPath(QString path) {
  FilesystemEntry::setPath(path);
  this->reload();
}

bool File::opened() const { return this->mOpened; }
void File::setOpened(bool opened) {
  this->mOpened = opened;
  emit this->openedChanged();
}

bool File::autoFlush() const { return this->mAutoFlush; }
void File::setAutoFlush(bool autoFlush) {
  this->mAutoFlush = autoFlush;
  emit this->autoFlushChanged();
}

bool File::readable() const { return this->mReadable; }
void File::setReadable(bool readable) {
  this->mReadable = readable;
  emit this->readableChanged();
}

bool File::writable() const { return this->mWritable; }
void File::setWritable(bool writable) {
  this->mWritable = writable;
  emit this->writableChanged();
}

bool File::shouldAppend() const { return this->mShouldAppend; }
void File::setShouldAppend(bool shouldAppend) {
  this->mShouldAppend = shouldAppend;
  emit this->shouldAppendChanged();
}

bool File::shouldTruncate() const { return this->mShouldTruncate; }
void File::setShouldTruncate(bool shouldTruncate) {
  this->mShouldTruncate = shouldTruncate;
  emit this->shouldTruncateChanged();
}

bool File::binary() const { return this->mBinary; }
void File::setBinary(bool binary) {
  this->mBinary = binary;
  emit this->binaryChanged();
}

// TODO: fs needs to be watched in order to do emits for mimetype updates
QString File::mimeType() const {
  return MIME_DATABASE.mimeTypeForFile(this->path()).name();
}

QString File::mimeTypeIconName() const {
  return MIME_DATABASE.mimeTypeForFile(this->path()).iconName();
}

void File::open() {
  this->mOpened = true;
  this->reload();
}

void File::close() {
  this->mOpened = false;
  this->mFile.close();
}

QString File::read() { return this->mFile.readAll(); }

void File::write(const QString &contents) {
  this->mFile.seek(0);
  this->mFile.write(qPrintable(contents));
  this->mFile.resize(this->mFile.pos());
  if (this->mAutoFlush) {
    this->mFile.flush();
  }
}

void File::append(const QString &contents) {
  this->mFile.seek(this->mFile.size());
  this->mFile.write(qPrintable(contents));
  if (this->mAutoFlush) {
    this->mFile.flush();
  }
}

void File::reload() {
  this->mFile.close();
  this->mFile.setFileName(this->path());
  if (!this->mOpened || this->path().isEmpty()) {
    return;
  }
  auto flags = QFlags(QIODevice::NotOpen);
  if (this->mReadable)
    flags |= QIODevice::ReadOnly;
  if (this->mWritable)
    flags |= QIODevice::WriteOnly;
  if (this->mShouldAppend)
    flags |= QIODevice::Append;
  if (this->mShouldTruncate)
    flags |= QIODevice::Truncate;
  if (!this->mBinary)
    flags |= QIODevice::Text;
  if (flags == QIODevice::NotOpen) {
    return;
  }
  // NOTE: `NewOnly` and `ExistingOnly` omitted as they are not needed for now.
  this->mFile.open(flags);
}
