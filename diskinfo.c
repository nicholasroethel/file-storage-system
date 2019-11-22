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
#include <netinet/in.h>
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

	//open the file
	int fd = open(argv[1], O_RDWR);


	if (fd == -1)	{
		printf("error opening test.img\n");
		return 1;
	}

	//mmap the file
	struct stat buffer;
	if (fstat(fd,&st)==-1){
		printf("fstat failed exiting.\n");
		return -1;
	}


	char* data = mmap(NULL, sizeof(char)*buffer.st_size, PROT_READ, MAP_SHARED, fd, 0);

	if (data == (void*) -1)	{
		printf("mmap failed with: %s\n", strerror(errno));
	}

	//put it into the struct and print the struct info
    struct superblock_t* sb;
    sb=(struct superblock_t*)data;
    printf("Super block information:\n");
    printf("Block size: %d\n", htons(sb->block_size));
    printf("Block count: %d\n", ntohl(sb->file_system_block_count));
    printf("FAT starts: %d\n", ntohl(sb->fat_start_block));
    printf("FAT blocks: %d\n", ntohl(sb->fat_block_count));
    printf("Root directory start: %d\n", ntohl(sb->root_dir_start_block));
    printf("Root directory block: %d\n", ntohl(sb->root_dir_block_count));
    printf("\n");
    printf("FAT information:\n");

    //get the FAT info
    int iterator = ntohl(sb->fat_start_block)*htons(sb->block_size);
    int max = ((ntohl(sb->fat_start_block + sb->fat_block_count)*htons(sb->block_size)));

    int availableBlocks = 0;
    int reservedBlocks = 0;
    int allocatedBlocks = 0;

    while(iterator < max){
    	uint32_t block = ntohl(*(uint32_t*)&data[iterator]);

    	if(block == 0x0){
    		availableBlocks ++;
    	}
    	else if (block == 0x1){
    		reservedBlocks ++;
    	}
    	else{
    		allocatedBlocks++;
    	}
    	iterator = iterator + 4;
    }

    printf("Available Blocks: %d\n",availableBlocks);
    printf("Reserved Blocks: %d\n",reservedBlocks);
    printf("Allocated Blocks: %d\n",allocatedBlocks);



	return 0;

}
