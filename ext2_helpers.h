#pragma once

#include "ext2_common.h"


void                     ext2_dump_group_desc(struct ext2_group_desc* gdh);

void                     parse_ext2_device(const char* dev, struct ext2_description* d);

void                     ext2_read_inode(const struct ext2_description* d, const int inode_number, struct ext2_inode* inode);

size_t                   ext2_get_data_size(const struct ext2_description* d, const struct ext2_inode* inode);

int                      ext2_read_blocks(const struct ext2_description* d, const struct ext2_inode* inode, uint8_t* buffer);

void                     ext2_fill_dir_entry(const uint8_t* blocks, struct ext2_dir_entry* de);

struct ext2_dir_entry*   ext2_get_dir_entries(const struct ext2_description* d, const uint8_t* blocks);
void                     ext2_free_dir_entries(struct ext2_dir_entry* root_de);

int                      ext2_get_inode(const struct ext2_description* d, const char* path);

