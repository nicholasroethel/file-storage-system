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

// Directory entry
struct __attribute__((__packed__)) dir_entry_t {
    uint8_t status;
    uint32_t starting_block;
    uint32_t block_count;
    uint32_t size;
    struct dir_entry_timedate_t create_time;
    struct dir_entry_timedate_t modify_time;
    uint8_t filename[31];
    uint8_t unused[6];
};

// Time and date entry
struct __attribute__((__packed__)) dir_entry_timedate_t {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
     uint8_t second;
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
	if (fstat(fd,&buffer)==-1){
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

    //get the start info
    int iterator = ntohl(sb->root_dir_start_block)*htons(sb->block_size);
    int max = ((ntohl(sb->root_dir_start_block + sb->root_dir_block_count)*htons(sb->block_size)));
    //iterate
    while(iterator < max){
        struct dir_entry_t *entry = malloc (sizeof (struct dir_entry_t));
        uint8_t fsID = htons(*(uint8_t*)&data[iterator]);
        entry->fs_id = fsID;
        printf("%d\n",fsID );
        iterator = iterator +64;
    }

    
    



	return 0;

}
