#include "shm_buffer.hpp"

#include <private/qwaylandshm_p.h>
#include <sys/mman.h>

ShmBuffer::ShmBuffer(const QSize &size, wl_shm *shm, ::wl_shm_format format) {
  auto stride = size.width() * 4;
  auto alloc = stride * size.height();
  auto *filename = strdup("/tmp/wayland-shm-XXXXXX");
  int fd = mkstemp(filename);
  if (fd < 0) {
    qWarning("ShmBuffer: open %s failed", filename);
    return;
  }
  if (ftruncate(fd, alloc) < 0) {
    qWarning("ShmBuffer: ftruncate failed");
    close(fd);
    return;
  }
  void *data = mmap(nullptr, alloc, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  unlink(filename);
  free(filename); // NOLINT
  if (data == MAP_FAILED) {
    qWarning("ShmBuffer: mmap failed");
    close(fd);
    return;
  }
  this->mImage =
      QImage(static_cast<uchar *>(data), size.width(), size.height(), stride,
             QtWaylandClient::QWaylandShm::formatFrom(format));
  this->mShmPool = wl_shm_create_pool(shm, fd, alloc);
  this->mHandle = wl_shm_pool_create_buffer(mShmPool, 0, size.width(),
                                            size.height(), stride, format);
  close(fd);
}

ShmBuffer::~ShmBuffer() {
  munmap(this->mImage.bits(), this->mImage.sizeInBytes());
  wl_buffer_destroy(this->mHandle);
  wl_shm_pool_destroy(this->mShmPool);
}
