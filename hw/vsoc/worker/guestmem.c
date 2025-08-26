#include "hw/vsoc/worker/guestmem.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>

typedef struct Region {
    int fd;
    uint64_t gpa_base;
    uint64_t size;
    uint64_t file_offset;
    void *map; // mapped host VA for this region
    struct Region *next;
} Region;

static Region *g_regions = NULL;

static inline uint64_t min_u64(uint64_t a, uint64_t b) { return a < b ? a : b; }

static void region_free(Region *r) {
    if (!r) return;
    if (r->map && r->size) munmap(r->map, (size_t)r->size);
    if (r->fd >= 0) close(r->fd);
    free(r);
}

int guestmem_add_region(const VsocGuestMemRegionInfo *info) {
    if (!info || info->fd < 0 || info->size == 0) return -EINVAL;
    Region *r = (Region*)calloc(1, sizeof(Region));
    if (!r) return -ENOMEM;
    // duplicate fd so caller can close theirs
    int newfd = dup(info->fd);
    if (newfd < 0) { free(r); return -errno; }
    r->fd = newfd;
    r->gpa_base = info->gpa_base;
    r->size = info->size;
    r->file_offset = info->file_offset;
    // Map shared; rely on caller providing correct offset and size
    void *addr = mmap(NULL, (size_t)r->size, PROT_READ | PROT_WRITE, MAP_SHARED, r->fd, (off_t)r->file_offset);
    if (addr == MAP_FAILED) {
        int e = errno; close(r->fd); free(r); return -e;
    }
    r->map = addr;
    // Insert sorted by gpa_base for efficient lookup
    if (!g_regions || r->gpa_base < g_regions->gpa_base) {
        r->next = g_regions; g_regions = r;
    } else {
        Region *cur = g_regions;
        while (cur->next && cur->next->gpa_base < r->gpa_base) cur = cur->next;
        r->next = cur->next; cur->next = r;
    }
    return 0;
}

int guestmem_add_regions(const VsocGuestMemRegionInfo *infos, size_t count) {
    if (!infos && count) return -EINVAL;
    for (size_t i = 0; i < count; ++i) {
        int rc = guestmem_add_region(&infos[i]);
        if (rc != 0) return rc;
    }
    return 0;
}

void guestmem_clear_all(void) {
    Region *r = g_regions; g_regions = NULL;
    while (r) { Region *n = r->next; region_free(r); r = n; }
}

static inline Region *find_region(uint64_t gpa) {
    for (Region *r = g_regions; r; r = r->next) {
        if (gpa >= r->gpa_base && gpa < r->gpa_base + r->size) return r;
    }
    return NULL;
}

void *guestmem_ptr(uint64_t gpa, size_t len, size_t *out_contig) {
    Region *r = find_region(gpa);
    if (!r) return NULL;
    uint64_t offset = gpa - r->gpa_base;
    uint64_t avail = r->size - offset;
    if (len > avail) {
        // spans multiple regions; not guaranteed contiguous
        if (out_contig) *out_contig = (size_t)avail;
    } else {
        if (out_contig) *out_contig = len;
    }
    return (uint8_t*)r->map + offset;
}

bool guestmem_read(uint64_t gpa, void *dst, size_t len) {
    if (len == 0) return true;
    uint8_t *p = (uint8_t*)dst;
    uint64_t cur = gpa;
    size_t remaining = len;
    while (remaining) {
        Region *r = find_region(cur);
        if (!r) return false;
        uint64_t off = cur - r->gpa_base;
        uint64_t avail = r->size - off;
        size_t chunk = (size_t)min_u64((uint64_t)remaining, avail);
        memcpy(p, (uint8_t*)r->map + off, chunk);
        p += chunk; cur += chunk; remaining -= chunk;
    }
    return true;
}

bool guestmem_write(uint64_t gpa, const void *src, size_t len) {
    if (len == 0) return true;
    const uint8_t *p = (const uint8_t*)src;
    uint64_t cur = gpa;
    size_t remaining = len;
    while (remaining) {
        Region *r = find_region(cur);
        if (!r) return false;
        uint64_t off = cur - r->gpa_base;
        uint64_t avail = r->size - off;
        size_t chunk = (size_t)min_u64((uint64_t)remaining, avail);
        memcpy((uint8_t*)r->map + off, p, chunk);
        p += chunk; cur += chunk; remaining -= chunk;
    }
    return true;
}
