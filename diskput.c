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


struct superblock_t s;
struct superblock_t* sb = &s;
uint32_t amountOfBlocks = 0; //will store the amount of blocks


void copyData(uint32_t startingBlock, char* data, uint16_t block_size, FILE *file){

    int count = 0; //counter for how many blocks travers

    uint32_t fatStart = ntohl(sb->fat_start_block);//where the block where the fat starts

    for(uint32_t i = startingBlock; count<amountOfBlocks; count++){

            uint32_t iterator = i*block_size;

            uint32_t max = (i + 1)*(block_size);
            printf("%d\n",max);

             while(iterator < max){

                putc((*(uint8_t*)&data[iterator]), file);

                iterator = iterator + 1;
                
            }

        i = ntohl(*(uint32_t*)&data[fatStart*block_size+i*4]); //get the next block

    }

}


uint32_t findFile(char* file, char* data, uint32_t block_count, uint32_t starting_block, uint16_t block_size){


    int count = 0; //counter for how many blocks travers

    uint32_t fatStart = ntohl(sb->fat_start_block);//where the block where the fat starts


    for(uint32_t i = starting_block; count<block_count; count++){

            uint32_t iterator = i*block_size;

            uint32_t max = (i + 1)*(block_size);

             while(iterator < max){

                //get the values for the directory entry
                uint8_t status = (*(uint8_t*)&data[iterator]);
                uint32_t startingBlock = ntohl(*(uint32_t*)&data[iterator+1]);
                uint32_t blockCount = ntohl(*(uint32_t*)&data[iterator+5]);
                uint32_t size = ntohl(*(uint32_t*)&data[iterator+9]);
                struct dir_entry_timedate_t create_time = (*(struct dir_entry_timedate_t*)&data[iterator+13]);
                struct dir_entry_timedate_t modify_time = (*(struct dir_entry_timedate_t*)&data[iterator+20]);
                char* name;
                uint8_t filename[31];
                for (int count = 0; count<31;count++){
                    filename[count] = (*(uint8_t*)&data[count+iterator+27]);
                }
                name = (char*)(filename);
                
                if(strcmp(name,file)==0){
                    printf("Name: %s\n",name);
                    printf("Starting block: %u\n",startingBlock);
                    printf("Block count: %u\n",blockCount);
                    printf("Size: %u\n",size);
                    amountOfBlocks = blockCount;
                    return startingBlock;
                }

                iterator = iterator + 64;
                
            }

        i = ntohl(*(uint32_t*)&data[fatStart*block_size+i*4]); //get the next block

    }
    printf("File not found.\n");
    exit(1);
}


int main(int argc, char* argv[])	{


	//open the file
	int fd = open(argv[1], O_RDWR);

    if (fd == -1)   {
        printf("error opening test.img\n");
        return 1;
    }

    //get the file
    char* file = argv[2];

    //get the directory
    char* directory = argv[3];

    //open the file
    FILE *fptr; 
    fptr = fopen(file,"r");
    if(fptr == NULL)
    {
      printf("Error opening file\n");   
      exit(1);             
    }

    //get the file size
    fseek(fptr, 0L, SEEK_END);
    int sz = ftell(fptr);
    printf("Size %d\n",sz);


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

	//cast the sb into a struct
    sb=(struct superblock_t*)data;

    //get the amount of blocks needed
    int blocks = ceil(sz/htons(sb->block_size))+1;
    printf("Blocks needed: %d\n", blocks);

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


    //print the FAT info
    printf("Available Blocks: %d\n",availableBlocks);

     if(availableBlocks<blocks){
        printf("Not enough space in the img to copy the file.\n");
        return 1;
     }


    int freeBlocks[blocks];
    iterator = ntohl(sb->fat_start_block)*htons(sb->block_size);
    int count = 0;
    //uint32_t
    //iterate
    while(count<blocks){
        uint32_t block = ntohl(*(uint32_t*)&data[iterator]);
        if(block == 0x0){
            freeBlocks[count] = (iterator-(ntohl(sb->fat_start_block)*htons(sb->block_size)))/4;
            printf("%d\n",freeBlocks[count]);
            count++;
        }
        else if(block != 0x1){
            //printf("%d\n",block );
        }
        iterator = iterator + 4;
    }

    iterator = ntohl(sb->fat_start_block)*htons(sb->block_size);
    count = 0;
    while(count<blocks){
        uint32_t block = ntohl(*(uint32_t*)&data[iterator]);
        if(block == 0x0){
            if(count + 1 == blocks){
                data[iterator] = *(char*)(0xFFFFFFFF);
                block = ntohl(*(uint32_t*)&data[iterator]);
                printf("%d\n",block);
            }
            else{
                data[iterator] = &(freeBlocks[count+1]);
                block = ntohl(*(uint32_t*)&data[iterator]);
                printf("%d\n",block);
            }
            count ++;

        }
        else if(block != 0x1){
            //printf("%d\n",block );
        }
        iterator = iterator + 4;
    }





    //print the given directory
    //uint32_t startingBlock = findFile(file, data, ntohl(sb->root_dir_block_count), ntohl(sb->root_dir_start_block), htons(sb->block_size));

    //copyData(startingBlock, data, htons(sb->block_size), fptr);

    fclose(fptr);

	return 0;

}
