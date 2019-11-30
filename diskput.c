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
#include <string.h>
#include <math.h>

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

// Time and date entry
struct __attribute__((__packed__)) dir_entry_timedate_t {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
};

//Globals
struct superblock_t s;
struct superblock_t* sb = &s;
int blocksNeeded  = 0; //will store the blocks needed to insert the file


void addToFAT(void* data, int availableBlocks, uint32_t* freeBlocks){ //adds the new entries to the FAT table

    //declare variables for looping
    int count = 0;
    int iterator = ntohl(sb->fat_start_block)*htons(sb->block_size);
    int max = ((ntohl(sb->fat_start_block + sb->fat_block_count)*htons(sb->block_size)));

    //loop to fill the FAT with pointers to blocks that will be used
    while(count<blocksNeeded){
        uint32_t block = ntohl(*(uint32_t*)&data[iterator]);
        if(block == 0x0){
            if(count + 1 == blocksNeeded){ //if its the last block for the file
                //insert 0xFFFFFFFF
            }
            else{ //if its not the last block needed for the file
                //insert address to next block
            }
            count ++;

        }
        iterator = iterator + 4;
    }

}

void firstAvailableBlocks(void* data, int availableBlocks, uint32_t* freeBlocks){

    //define variables for looping
    int iterator = ntohl(sb->fat_start_block)*htons(sb->block_size);
    int max = ((ntohl(sb->fat_start_block + sb->fat_block_count)*htons(sb->block_size)));
    iterator = ntohl(sb->fat_start_block)*htons(sb->block_size);
    int count = 0;

    printf("First available blocks: ");
    while(count<blocksNeeded){ //find the first N available blocks
        uint32_t block = ntohl(*(uint32_t*)&data[iterator]);
        if(block == 0x0){
            freeBlocks[count] = (iterator-(ntohl(sb->fat_start_block)*htons(sb->block_size)))/4;
            printf("%d ",freeBlocks[count]);
            count++;
        }
        iterator = iterator + 4;
    }
    printf("\n");
}

int getAllAvailableBlocks(void* data){ //gets the amount of available blocks in the FAT
    //get the FAT info
    int iterator = ntohl(sb->fat_start_block)*htons(sb->block_size);
    int max = ((ntohl(sb->fat_start_block + sb->fat_block_count)*htons(sb->block_size)));

    //vars to store the data
    int availableBlocks = 0;

    //find amount of free blocks
    while(iterator<max){
        uint32_t block = ntohl(*(uint32_t*)&data[iterator]);
        if(block == 0x0){
            availableBlocks ++;
        }
        iterator = iterator + 4;
    }

    //sanity check
    if(availableBlocks<blocksNeeded){
        printf("Not enough space in the img to copy the file.\n");
        exit(1);
     }

   return availableBlocks;

}

char* setupFile(char* argv[]){ //reads in the file and gets the size

    //open the file
    char* file = argv[2];
    FILE *fptr; 
    fptr = fopen(file,"r");
    if(fptr == NULL){
      printf("Error opening file\n");   
      exit(1);             
    }

    //get the file size
    fseek(fptr, 0L, SEEK_END);
    int sz = ftell(fptr);

    //get the amount of blocks needed
    blocksNeeded = ceil(sz/htons(sb->block_size))+1;

    fclose(fptr);
    return file;

}

void* setupMap(char* argv[]){ //maps the data and gets the blocks needed

    /* open the file system */
    int fd = open(argv[1], O_RDWR);
    if (fd == -1)   {
        printf("error opening test.img\n");
        exit(1);
    }

    //fstat buffer
    struct stat buffer;
    if (fstat(fd,&buffer)==-1){
        printf("fstat failed exiting.\n");
        exit(1);
    }

    // mmap the file directory
    void* data = mmap(NULL, sizeof(char)*buffer.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (data == (void*) -1) {
        printf("mmap failed with: %s\n", strerror(errno));
        exit(1);
    }

    //cast the sb into a struct
    sb=(struct superblock_t*)data;

    return data;

}

 
int main(int argc, char* argv[]){
    
    //get the file directory data
    void* data = setupMap(argv);  

    //read in the file
    char* file = setupFile(argv); 
    
    //get the destination directory
    char* directory = argv[3]; 

    //get the amount of available blocks
    int availableBlocks = getAllAvailableBlocks(data); 

    //fill an array with the first N free blocks found
    uint32_t freeBlocks[blocksNeeded];
    firstAvailableBlocks(data, availableBlocks, freeBlocks);

    //adds the new entries to the FAT table
    addToFAT(data, availableBlocks, freeBlocks);

	return 0;
}
