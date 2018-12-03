#include "ext2_helpers.h"
#include "endian_helpers.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>

#ifdef __GNUC__
#define SWAP_ENDIAN_32(x) (x = __builtin_bswap32(x))
#define SWAP_ENDIAN_16(x) (x =__builtin_bswap16(x))
#else
#error only gcc is supported.
#endif

void ext2_dump_group_desc(struct ext2_group_desc* gdh)
{
    printf("block_bitmap:%d\ninode_bitmap:%d\ninode_table:%d\nfree_blocks_count:%d\nfree_inodes_count:%d\nused_dirs_count:%d\n",
        gdh->bg_block_bitmap,
        gdh->bg_inode_bitmap,
        gdh->bg_inode_table,
        gdh->bg_free_blocks_count,
        gdh->bg_free_inodes_count,
        gdh->bg_used_dirs_count);
}

static void ext2_swap_endian_in_super_block(struct ext2_super_block* sb)
{
	SWAP_ENDIAN_32(sb->s_inodes_count);
	SWAP_ENDIAN_32(sb->s_blocks_count);
	SWAP_ENDIAN_32(sb->s_r_blocks_count);
	SWAP_ENDIAN_32(sb->s_free_blocks_count);
	SWAP_ENDIAN_32(sb->s_free_inodes_count);
	SWAP_ENDIAN_32(sb->s_first_data_block);
	SWAP_ENDIAN_32(sb->s_log_block_size);
	SWAP_ENDIAN_32(sb->s_log_frag_size);
	SWAP_ENDIAN_32(sb->s_blocks_per_group);
	SWAP_ENDIAN_32(sb->s_frags_per_group);
	SWAP_ENDIAN_32(sb->s_inodes_per_group);
	SWAP_ENDIAN_32(sb->s_mtime);
	SWAP_ENDIAN_32(sb->s_wtime);
	SWAP_ENDIAN_16(sb->s_mnt_count);
	SWAP_ENDIAN_16(sb->s_max_mnt_count);
	SWAP_ENDIAN_16(sb->s_magic);
	SWAP_ENDIAN_16(sb->s_state);
	SWAP_ENDIAN_16(sb->s_errors);
	SWAP_ENDIAN_16(sb->s_minor_rev_level);
	SWAP_ENDIAN_32(sb->s_lastcheck);
	SWAP_ENDIAN_32(sb->s_checkinterval);
	SWAP_ENDIAN_32(sb->s_creator_os);
	SWAP_ENDIAN_32(sb->s_rev_level);
	SWAP_ENDIAN_32(sb->s_def_resuid);
	SWAP_ENDIAN_32(sb->s_def_resgid);
}

static void ext2_swap_endian_in_group_desriptor(struct ext2_group_desc* gd)
{
	SWAP_ENDIAN_32(gd->bg_block_bitmap);
	SWAP_ENDIAN_32(gd->bg_inode_bitmap);
	SWAP_ENDIAN_32(gd->bg_inode_table);
	SWAP_ENDIAN_16(gd->bg_free_blocks_count);
	SWAP_ENDIAN_16(gd->bg_free_inodes_count);
	SWAP_ENDIAN_16(gd->bg_used_dirs_count);
	SWAP_ENDIAN_16(gd->bg_pad);
}

static void ext2_swap_endian_in_description(struct ext2_description* d)
{
	ext2_swap_endian_in_super_block(&d->sb);

	for(int i = 0; i < d->groups_count; ++i)
		ext2_swap_endian_in_group_desriptor(&d->gdt[i]);
}

static void read_helper(const int fd, void* buffer, size_t size, const char* err_msg)
{
	int n = 0;
	size_t offset = 0;
	while((n = read(fd, buffer + offset, size)))
	{
		if(n == -1)
		{
			printf(err_msg, strerror(errno));
			exit(-1);		
		}
		size -= n;
		offset += n;
	}
}

static inline void ext2_read_super_block(const int fd, struct ext2_super_block* sb)
{
	lseek(fd, OFFSET_SUPERBLOCK, SEEK_SET);

	read_helper(fd, sb, sizeof(*sb), "can't read a super block. :%s\n");
}

static inline void insure_device_is_ext2(const struct ext2_super_block* sb)
{
	if(sb->s_magic != EXT2_MAGIC)
	{
		printf("device is not ext2. wrong magic:%#x, expected:%#x.", sb->s_magic, EXT2_MAGIC);
		exit(-1);		
	}
}

static inline void ext2_read_group_descriptor_table(const int fd, const struct ext2_description* d, struct ext2_group_desc* gdt)
{
	const size_t gdt_block_number = d->block_size == 1024 ? 2 : 1;
	const size_t offset = gdt_block_number * d->block_size;

	lseek(fd, offset, SEEK_SET);

	read_helper(fd, gdt, sizeof(*gdt) * d->groups_count, "can't read a group descriptor table. :%s\n");
}

void parse_ext2_device(const char* dev, struct ext2_description* d)
{
	const int fd = open(dev, O_RDONLY);

	if(fd == -1)
	{
		printf("can't open a device. :%s\n", strerror(errno));
		exit(-1);
	}

	ext2_read_super_block(fd, &d->sb);
	insure_device_is_ext2(&d->sb);

	d->block_size = 1024 << d->sb.s_log_block_size;
	d->group_size = d->sb.s_blocks_per_group * d->block_size;
	d->groups_count = d->sb.s_blocks_count / d->sb.s_blocks_per_group;
	d->fd = fd;

	d->gdt = calloc(d->groups_count, sizeof(*d->gdt));
	ext2_read_group_descriptor_table(d->fd, d, d->gdt);

	if(!is_little_endian_cpu())
		ext2_swap_endian_in_description(d);
}

void ext2_read_inode(const struct ext2_description* d, const int inode_number, struct ext2_inode* inode)
{
	const size_t group_number = (inode_number-1) / d->sb.s_inodes_per_group;
	
	const size_t inode_table_offset = d->gdt[group_number].bg_inode_table * d->block_size;
	const size_t inode_index        = (inode_number - 1) % d->sb.s_inodes_per_group;
	const size_t inode_offset       = inode_table_offset + inode_index * d->sb.s_inode_size;

	lseek(d->fd, inode_offset, SEEK_SET);
	read_helper(d->fd, inode, sizeof(*inode), "can't read an inode. :%s\n");
}

static inline size_t ext2_count_direct_blocks(const struct ext2_inode* inode)
{
	size_t n = 0;
	for(uint32_t block = inode->i_block[n]; block != 0; block = inode->i_block[n])
		++n;

	return n;
}

size_t ext2_get_data_size(const struct ext2_description* d, const struct ext2_inode* inode)
{
	size_t n = 0;
	n += ext2_count_direct_blocks(inode);

	return n * d->block_size;
}

int ext2_read_blocks(const struct ext2_description* d, const struct ext2_inode* inode, uint8_t* buffer)
{
	const int n = ext2_count_direct_blocks(inode);
	for(int i = 0; i < n; ++i)
	{
		const uint32_t b = inode->i_block[i];
		lseek(d->fd, b * d->block_size, SEEK_SET);

		read_helper(d->fd, buffer + i * d->block_size, d->block_size, "can't read a block. %s\n");
	}

	return 0;
}

struct ext2_dir_entry* ext2_get_dir_entries(const struct ext2_description* d, const uint8_t* blocks)
{
	const uint32_t begin_inode = ((uint32_t*)blocks)[0];
	if(begin_inode != 0)
	{
		size_t de_offset = 0;
		struct ext2_dir_entry* root_de = calloc(1, sizeof(*root_de));
		root_de->previous = NULL;

		for(struct ext2_dir_entry* de = root_de; de != NULL; de = de->next)
		{
			const uint8_t* raw_de = blocks + de_offset;
			
			de->inode    = ((uint32_t*)raw_de)[0];
			de->rec_len  = ((uint16_t*)(raw_de + offsetof(struct ext2_dir_entry, rec_len)))[0];
			de->name_len = ((uint16_t*)(raw_de + offsetof(struct ext2_dir_entry, name_len)))[0];
			
			const size_t nl = de->name_len & BYTE_MASK;
			memmove(de->name, raw_de + offsetof(struct ext2_dir_entry, name), nl);
			de->name[nl] = '\0';

			de_offset += de->rec_len;
			if(de_offset < d->block_size)
			{
				de->next = calloc(1, sizeof(*de->next));
				de->next->previous = de;	
			}
		}

		return root_de;
	}
	else
		return NULL;
}

void ext2_free_dir_entries(struct ext2_dir_entry* root_de)
{
	struct ext2_dir_entry* de = root_de;
	while(de != NULL)
	{
		struct ext2_dir_entry* next = de->next;
	
		free(de);
		de = next;
	
		if(de != NULL)
			de->previous = NULL;
	}
}


void ext2_fill_dir_entry(const uint8_t* blocks, struct ext2_dir_entry* de)
{
	de->inode    = ((uint32_t*)blocks)[0];
	de->rec_len  = ((uint16_t*)(blocks + offsetof(struct ext2_dir_entry, rec_len)))[0];
	de->name_len = ((uint16_t*)(blocks + offsetof(struct ext2_dir_entry, name_len)))[0];

	const size_t nl = de->name_len & BYTE_MASK;

	memmove(de->name, blocks + offsetof(struct ext2_dir_entry, name), nl);
	de->name[nl] = '\0';	
}

int ext2_get_inode(const struct ext2_description* d, const char* path)
{
	char* p = calloc(strlen(path)+1, 1);
	strcpy(p, path);

	uint32_t i = EXT2_INODE_ROOT;
	char* next_folder = strtok(p, "/");
	struct ext2_inode inode;

	while(i != 0 && next_folder != NULL)
	{
		ext2_read_inode(d, i, &inode);
		i = 0;

		const uint16_t inode_type = inode.i_mode & EXT2_FT_MASK;

		if(inode_type != EXT2_FT_DIR)
		{
			i = 0;
			break;
		}

		uint8_t* blocks = calloc(ext2_get_data_size(d, &inode), sizeof(*blocks));
		ext2_read_blocks(d, &inode, blocks);
		
		struct ext2_dir_entry* de_root = ext2_get_dir_entries(d, blocks);

		for(struct ext2_dir_entry* de = de_root; de != NULL; de = de->next)
		{
			if(!strcmp(de->name, next_folder))
			{
				i = de->inode;
				next_folder = strtok(0, "/");
				break;
			}
		}

		ext2_free_dir_entries(de_root);
		free(blocks);
	}

	free(p);
	return i;
}