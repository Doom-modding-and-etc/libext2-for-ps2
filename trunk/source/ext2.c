#include <errno.h>
#include <string.h>
#include "ext2.h"
#include "ext2_fs.h"
#include "ext2fs.h"
#include "ext2_internal.h"
#include "gekko_io.h"
#include "mem_allocate.h"

bool ext2Mount(const char *name, const DISC_INTERFACE *interface, sec_t startSector, u32 cachePageCount, u32 cachePageSize, u32 flags)
{
    errcode_t retval = -1;
    ext2_filsys fs = NULL;
    io_channel io_chan = NULL;
    gekko_fd *fd = NULL;
    ext2_vd * vd = NULL;

    // Sanity check
    if (!name || !interface)
    {
        errno = EINVAL;
        return false;
    }

    // Allocate the device driver descriptor
    fd = (gekko_fd*) mem_alloc(sizeof(gekko_fd));
    if (!fd)
		goto cleanup;

    memset(fd, 0, sizeof(gekko_fd));

    // Setup the device driver descriptor
    fd->interface = interface;
    fd->startSector = startSector;
    fd->sectorSize = 0;
    fd->sectorCount = 0;
    fd->cachePageCount = cachePageCount;
    fd->cachePageSize = cachePageSize;

    fs = mem_alloc(sizeof(struct struct_ext2_filsys));
	if (!fs)
	{
        ext2_log_trace("no memory for fs\n");
        errno = ENOMEM;
        goto cleanup;
    }

	memset(fs, 0, sizeof(struct struct_ext2_filsys));

	io_chan = mem_alloc(sizeof(struct struct_io_channel));
	if (!io_chan)
	{
        ext2_log_trace("no memory for io_chan\n");
        errno = ENOMEM;
        goto cleanup;
    }

	memset(io_chan, 0, sizeof(struct struct_io_channel));

    io_chan->magic = EXT2_ET_MAGIC_IO_CHANNEL;
	io_chan->manager = gekko_io_manager;
	io_chan->name = strdup(name);
	if(!io_chan->name) goto cleanup;
	io_chan->block_size = 1024;
	io_chan->read_error = 0;
	io_chan->write_error = 0;
	io_chan->refcount = 1;
    io_chan->private_data = fd;
	io_chan->flags = EXT2_FLAG_RW | EXT2_FLAG_EXCLUSIVE | EXT2_FLAG_JOURNAL_DEV_OK;

    retval = ext2fs_open2(io_chan->name, 0, io_chan->flags, 0, 0, &io_chan, &fs);
    if(retval)
    {
        ext2_log_trace("error mounting %i\n", (int) retval);
		goto cleanup;
    }

    retval = ext2fs_read_bitmaps(fs);
    if (retval) {
        ext2_log_trace("read bitmats error %i\n", (int) retval);
        goto cleanup;
    }

    vd = mem_alloc(sizeof(ext2_vd));
    if(!vd)
    {
        ext2_log_trace("no memory for vd\n");
		goto cleanup;
    }

    // Initialise the volume descriptor
    ext2InitVolume(vd);
    vd->fs = fs;
    vd->io = io_chan;
    vd->root = EXT2_ROOT_INO;

    // Add the device to the devoptab table
    if (ext2AddDevice(name, vd)) {
        ext2DeinitVolume(vd);
        goto cleanup;
    }

    return true;

cleanup:
	if(fd)
		mem_free(fd);
	if(io_chan)
		mem_free(io_chan);
    if(vd)
        mem_free(vd);
	if(fs)
	{
        ext2fs_close(fs);
		ext2fs_free(fs);
	}

    return false;
}

void ext2Unmount(const char *name)
{
    ext2_vd *vd = NULL;

    // Get the devices volume descriptor
    vd = ext2GetVolume(name);
    if (!vd)
        return;

    // Remove the device from the devoptab table
    ext2RemoveDevice(name);

    // Deinitialise the volume descriptor
    ext2DeinitVolume(vd);

    // Unmount the volume
    ext2fs_close(vd->fs);
    ext2fs_free(vd->fs);

    //Free the io manager
    mem_free(vd->io->private_data);
    mem_free(vd->io);

    // Free the volume descriptor
    mem_free(vd);

    return;
}
