#pragma once

#define FUSE_USE_VERSION 31

#include <fuse.h>
#include "ext2_common.h"

struct ext2_fuse_context
{
	struct ext2_description d;
};

int ext2_fuse_getattr(const char* path, struct stat* stbuf, struct fuse_file_info* file_info);

int ext2_fuse_readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* file_info, enum fuse_readdir_flags flags);

int ext2_fuse_open(const char* path, struct fuse_file_info* file_info);

int ext2_fuse_read(const char* path, char* buffer, size_t read_size, off_t offset, struct fuse_file_info* file_info);
