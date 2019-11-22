#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdlib.h>
#include <time.h>
// Super block
struct __attribute__((__packed__)) superblock_t {
 	uint8_t  fs_id [8];
 	uint16_t block_size;
 	uint32_t file_system_block_count;
 	uint32_t fat_start_block;
 	uint32_t fat_block_count;
	uint32_t root_dir_start_block;
	uint32_t root_dir_block_count;
};

int main(int argc, char* argv[])	{

	int fd = open("test.img", O_RDONLY);

	if (fd == -1)	{
		printf("error opening test.img\n");
		return 1;
	}

	char* data = mmap(NULL, 0x200, PROT_READ, MAP_SHARED, fd, 0);

	if (data == (void*) -1)	{
		printf("mmap failed with: %s\n", strerror(errno));
	}

    struct superblock_t* sb;
    sb=(struct superblock_t*)data;
    printf("block count in struct: %x\n", ntohl(sb->file_system_block_count));

    int fssize;
    memcpy(&fssize, data+10, 4);
    fssize=ntohl(fssize);
    printf("block count: %x\n",fssize);




	return 0;

}
