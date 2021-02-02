#include "prelude.h"

#include "virtio.h"

void virtio_init(struct virtio_device* virtio);

static struct virtio_device virtio;

int main() {
  virtio_init(&virtio);
}
