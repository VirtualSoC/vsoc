// Worker-side direct guest memory mapping helpers (memfd-backed RAM)
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct VsocGuestMemRegionInfo {
    int fd;                // file descriptor (will be dup()'d internally)
    uint64_t gpa_base;     // GPA base covered by this region
    uint64_t size;         // size in bytes of this region
    uint64_t file_offset;  // file offset to map (page-aligned)
} VsocGuestMemRegionInfo;

void guestmem_init(void);
int guestmem_add_region(const VsocGuestMemRegionInfo *info);
int guestmem_add_regions(const VsocGuestMemRegionInfo *infos, size_t count);
void guestmem_clear_all(void);
int guestmem_replace_all(const VsocGuestMemRegionInfo *infos, size_t count);
bool guestmem_read(uint64_t gpa, void *dst, size_t len);
bool guestmem_write(uint64_t gpa, const void *src, size_t len);
void *guestmem_ptr(uint64_t gpa, size_t len, size_t *out_contig);

#ifdef __cplusplus
}
#endif
