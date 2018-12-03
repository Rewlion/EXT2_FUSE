#pragma once

#include <stdint.h>
#include <stdlib.h>

#define BYTE_MASK                    (int)0xFF

#define EXT2_NAME_LEN                (int)255
#define EXT2_MAGIC                   (int)0xEF53

#define OFFSET_SUPERBLOCK            (int)1024

#define OFFSET_INODES_COUNT          (int)0
#define OFFSET_BLOCKS_COUNT          (int)4
#define OFFSET_RESERVED_BLOCKS_COUNT (int)8
#define OFFSET_FREE_BLOCKS_COUNT     (int)12
#define OFFSET_FREE_INODES_COUNT     (int)16
#define OFFSET_FIRST_DATA_BLOCK      (int)20
#define OFFSET_LOG_BLOCK_SIZE        (int)24
#define OFFSET_LOG_FRAG_SIZE         (int)28
#define OFFSET_BLOCKS_PER_GROUP      (int)32
#define OFFSET_FRAGS_PER_GROUP       (int)36
#define OFFSET_INODES_PER_GROUP      (int)40
#define OFFSET_M_TIME                (int)44
#define OFFSET_W_TIME                (int)48
#define OFFSET_MNT_COUNT             (int)52
#define OFFSET_MAX_MNT_COUNT         (int)54
#define OFFSET_MAGIC                 (int)56

#define EXT2_NDIR_BLOCKS 12
#define EXT2_IND_BLOCK   EXT2_NDIR_BLOCKS
#define EXT2_DIND_BLOCK  (EXT2_IND_BLOCK + 1)
#define EXT2_TIND_BLOCK  (EXT2_DIND_BLOCK + 1)
#define EXT2_N_BLOCKS    (EXT2_TIND_BLOCK + 1)

#define EXT2_FT_MASK          (uint16_t)0xF000
#define EXT2_FT_FIFO          (uint16_t)0x1000
#define EXT2_FT_CHAR_DEV      (uint16_t)0x2000
#define EXT2_FT_DIR           (uint16_t)0x4000
#define EXT2_FT_BLOCK_DEV     (uint16_t)0x6000
#define EXT2_FT_REGULAR_FILE  (uint16_t)0x8000
#define EXT2_FT_SYMBOLIC_LINK (uint16_t)0xA000
#define EXT2_FT_UNIX_SOCKET   (uint16_t)0xC000

#define EXT2_INODE_ROOT (int)2

struct ext2_super_block 
{
    uint32_t  s_inodes_count;     /* Inodes count */
    uint32_t  s_blocks_count;     /* Blocks count */
    uint32_t  s_r_blocks_count;   /* Reserved blocks count */
    uint32_t  s_free_blocks_count;    /* Free blocks count */
    uint32_t  s_free_inodes_count;    /* Free inodes count */
    uint32_t  s_first_data_block; /* First Data Block */
    uint32_t  s_log_block_size;   /* Block size */
    uint32_t  s_log_frag_size;    /* Fragment size */
    uint32_t  s_blocks_per_group; /* # Blocks per group */
    uint32_t  s_frags_per_group;  /* # Fragments per group */
    uint32_t  s_inodes_per_group; /* # Inodes per group */
    uint32_t  s_mtime;        /* Mount time */
    uint32_t  s_wtime;        /* Write time */
    uint16_t  s_mnt_count;        /* Mount count */
    uint16_t  s_max_mnt_count;    /* Maximal mount count */
    uint16_t  s_magic;        /* Magic signature */
    uint16_t  s_state;        /* File system state */
    uint16_t  s_errors;       /* Behaviour when detecting errors */
    uint16_t  s_minor_rev_level;  /* minor revision level */
    uint32_t  s_lastcheck;        /* time of last check */
    uint32_t  s_checkinterval;    /* max. time between checks */
    uint32_t  s_creator_os;       /* OS */
    uint32_t  s_rev_level;        /* Revision level */
    uint16_t  s_def_resuid;       /* Default uid for reserved blocks */
    uint16_t  s_def_resgid;       /* Default gid for reserved blocks */
                     /* NOT SUPPORTED */
    uint32_t  s_first_ino;        /* First non-reserved inode */
    uint16_t  s_inode_size;      /* size of inode structure */
    uint16_t  s_block_group_nr;   /* block group # of this superblock */
    uint32_t  s_feature_compat;   /* compatible feature set */
    uint32_t  s_feature_incompat;     /* incompatible feature set */
    uint32_t  s_feature_ro_compat;    /* readonly-compatible feature set */
    uint8_t   s_uuid[16];     /* 128-bit uuid for volume */
    char      s_volume_name[16];  /* volume name */
    char      s_last_mounted[64];     /* directory where last mounted */
    uint32_t  s_algorithm_usage_bitmap; /* For compression */
    
    uint8_t   s_prealloc_blocks;  /* Nr of blocks to try to preallocate*/
    uint8_t   s_prealloc_dir_blocks;  /* Nr to preallocate for dirs */
    uint16_t   s_padding1;
    
    uint8_t    s_journal_uuid[16]; /* uuid of journal superblock */
    uint32_t   s_journal_inum;     /* inode number of journal file */
    uint32_t   s_journal_dev;      /* device number of journal file */
    uint32_t   s_last_orphan;      /* start of list of inodes to delete */
    uint32_t   s_hash_seed[4];     /* HTREE hash seed */
    uint8_t    s_def_hash_version; /* Default hash version to use */
    uint8_t    s_reserved_char_pad;
    uint16_t   s_reserved_word_pad;
    uint32_t  s_default_mount_opts;
    uint32_t  s_first_meta_bg;    /* First metablock block group */
    uint32_t   s_reserved[190];    /* Padding to the end of the block */
} __attribute__ ((packed));

struct ext2_group_desc
{
    uint32_t  bg_block_bitmap;        /* Blocks bitmap block */
    uint32_t  bg_inode_bitmap;        /* Inodes bitmap block */
    uint32_t  bg_inode_table;     /* Inodes table block */
    uint16_t  bg_free_blocks_count;   /* Free blocks count */
    uint16_t  bg_free_inodes_count;   /* Free inodes count */
    uint16_t  bg_used_dirs_count; /* Directories count */
    uint16_t  bg_pad;
    uint32_t  bg_reserved[3];
} __attribute__ ((packed));

struct ext2_inode 
{
    uint16_t  i_mode;        /* File mode */
    uint16_t  i_uid;      /* Low 16 bits of Owner Uid */
    uint32_t  i_size;     /* Size in bytes */
    uint32_t  i_atime;    /* Access time */
    uint32_t  i_ctime;    /* Creation time */
    uint32_t  i_mtime;    /* Modification time */
    uint32_t  i_dtime;    /* Deletion Time */
    uint16_t  i_gid;      /* Low 16 bits of Group Id */
    uint16_t  i_links_count;  /* Links count */
    uint32_t  i_blocks;   /* Blocks count */
    uint32_t  i_flags;    /* File flags */
    uint32_t  osd1;             /* OS dependent 1 */
    uint32_t  i_block[EXT2_N_BLOCKS];/* Pointers to blocks */
    uint32_t  i_generation;   /* File version (for NFS) */
    uint32_t  i_file_acl; /* File ACL */
    uint32_t  i_dir_acl;  /* Directory ACL */
    uint32_t  i_faddr;    /* Fragment address */
    uint8_t   osd2[12];             /* OS dependent 2 */
} __attribute__ ((packed));

struct ext2_dir_entry 
{
    uint32_t  inode;          /* Inode number */
    uint16_t  rec_len;        /* Directory entry length */
    uint16_t  name_len;       /* Name length */
    char      name[EXT2_NAME_LEN+1];/* File name, up to EXT2_NAME_LEN */

    struct ext2_dir_entry* next;
    struct ext2_dir_entry* previous;
};

struct ext2_description
{
    struct ext2_super_block sb;
    struct ext2_group_desc* gdt;
    size_t                  block_size;
    size_t                  group_size;
    size_t                  groups_count;
    int                     fd;
};