#include <stdio.h>

#include "ext2_fuse.h"
#include "ext2_helpers.h"

int main(int argc, char** argv)
{
	if(argc < 2)
		return -1;

	struct fuse_operations ops = 
	{
		.getattr = ext2_fuse_getattr,
		.readdir = ext2_fuse_readdir,
		.open    = ext2_fuse_open,
		.read    = ext2_fuse_read
	};

	struct ext2_fuse_context* c = calloc(1, sizeof(*c));
	parse_ext2_device(argv[1], &c->d);

	int r = fuse_main(argc-1, argv+1, &ops, c);
	
	free(c->d.gdt);
	free(c);

	return r;


}