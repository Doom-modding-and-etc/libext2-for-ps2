#ifndef __EXT2_H_
#define __EXT2_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gccore.h>

bool ext2Mount(const char *name, const DISC_INTERFACE *interface, sec_t startSector, u32 cachePageCount, u32 cachePageSize, u32 flags);
void ext2Unmount(const char *name);

#ifdef __cplusplus
}
#endif

#endif
