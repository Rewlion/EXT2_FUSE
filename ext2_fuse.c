#include "ext2_fuse.h"
#include "ext2_helpers.h"
#include "ext2_common.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>

static inline struct ext2_fuse_context* ext2_get_context()
{
	return (struct ext2_fuse_context*)fuse_get_context()->private_data;
}

static inline struct ext2_description* ext2_get_description()
{
	return &ext2_get_context()->d;
}

int ext2_fuse_getattr(const char* path, struct stat* stbuf, struct fuse_file_info* file_info)
{
	printf("getattr, path:%s\n", path);

	const struct ext2_description* d = ext2_get_description();

	const int i = ext2_get_inode(d, path);
	if(i != 0)
	{
		struct ext2_inode inode;
		ext2_read_inode(d, i, &inode);

		stbuf->st_ino = i;
		stbuf->st_size = inode.i_size;
		stbuf->st_mode = inode.i_mode;
		stbuf->st_nlink = inode.i_links_count;
		stbuf->st_uid = inode.i_uid;
		stbuf->st_gid = inode.i_gid;
		stbuf->st_blksize = d->block_size;

		return 0;
	}
	else
		return -ENOENT;
}

int ext2_fuse_readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* file_info, enum fuse_readdir_flags flags)
{
	printf("readdir, path:%s\n", path);

	const struct ext2_description* d = ext2_get_description();

	const int i = ext2_get_inode(d, path);
	if(i!= 0)
	{
		struct ext2_inode inode;
		ext2_read_inode(d, i, &inode);

		if((inode.i_mode & EXT2_FT_MASK) == EXT2_FT_DIR)
		{
			const size_t data_size = ext2_get_data_size(d, &inode);
			uint8_t* blocks = calloc(data_size, 1);

			ext2_read_blocks(d, &inode, blocks);
			struct ext2_dir_entry* de_root = ext2_get_dir_entries(d, blocks);

			for(struct ext2_dir_entry* de = de_root; de != NULL; de = de->next)
				filler(buf, de->name, NULL, 0, 0);

			free(blocks);
			ext2_free_dir_entries(de_root);
		}
		else
			return 0;		
	}
	else
		return -ENOENT;

	return 0;
}

int ext2_fuse_open(const char* path, struct fuse_file_info* file_info)
{
	printf("open, path:%s\n", path);

	return 0;
}

int ext2_fuse_read(const char* path, char* buffer, size_t read_size, off_t offset, struct fuse_file_info* file_info)
{
	printf("read, path:%s\n", path);

	const struct ext2_description* d = ext2_get_description();

	const int i = ext2_get_inode(d, path);
	if(i!= 0)
	{
		struct ext2_inode inode;
		ext2_read_inode(d, i, &inode);

		const size_t data_size = ext2_get_data_size(d, &inode);
		uint8_t* blocks = calloc(data_size, 1);
		ext2_read_blocks(d, &inode, blocks);

		const size_t size_to_read = inode.i_size > offset + read_size ? read_size : inode.i_size - offset;

		memmove(buffer + offset, blocks + offset, size_to_read);

		free(blocks);
		return size_to_read;
	}
	else
		return -ENOENT;
}
