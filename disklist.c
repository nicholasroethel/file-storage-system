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

// Time and date entry
struct __attribute__((__packed__)) dir_entry_timedate_t {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
};

//dir entry struct
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

struct superblock_t s;
struct superblock_t* sb = &s;

int getDepth(char* directory){ //returns the depth of of the subdirectory 
    
    const char s[2] = "/";
    char *token;

    token = strtok(directory, s);
    int count = 0; 

    while( token != NULL ) {
      token = strtok(NULL, s);
      count++;
    }

    return count;
}

void goThroughEntry(char* data, uint32_t block_count, uint32_t starting_block, uint16_t block_size, char* directoryName, struct dir_entry_t* subdir, int iteration, int depth){ //goes through each entry

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

                //if its a file or directory
                if(status != 0){
                    uint8_t filename[31];
                    for (int count = 0; count<31;count++){
                        filename[count] = (*(uint8_t*)&data[count+iterator+27]);
                    }
                    name = (char*)(filename);
                    if(iteration == depth){
                        if (status==3){
                            printf("F ");
                        }
                        else if (status==5){
                            printf("D ");
                        }
                        printf("%10d ",size);
                        printf("%30s ",filename);
                        printf("%04u/%02u/%02u %02u:%02u:%02u", htons(modify_time.year), (modify_time.month), (modify_time.day), (modify_time.hour), (modify_time.minute), (modify_time.second));
                        printf("\n");
                        //exit(0);
                    }
                    else if(strcmp(name,directoryName)==0){
                        printf("Found\n");
                        subdir->block_count = blockCount;
                        subdir->starting_block = startingBlock;
                    }                           
                }

                iterator = iterator + 64;
                
            }

        i = ntohl(*(uint32_t*)&data[fatStart*block_size+i*4]); //get the next block
    }
}


int main(int argc, char* argv[])	{

	//open the file direcotry
	int fd = open(argv[1], O_RDWR);
	if (fd == -1)	{
		printf("error opening test.img\n");
		return 1;
	}

	//create buffer
	struct stat buffer;
	if (fstat(fd,&buffer)==-1){
		printf("fstat failed exiting.\n");
		return -1;
	}

    //mmap the file
	char* data = mmap(NULL, sizeof(char)*buffer.st_size, PROT_READ, MAP_SHARED, fd, 0);
	if (data == (void*) -1)	{
		printf("mmap failed with: %s\n", strerror(errno));
	}

    //gets the depth of the directory to list
    char* directory = argv[2];

    char* subdirectories[100];

    const char s[2] = "/";
    char *token;

    token = strtok(directory, s);
    int count = 0; 

    while( token != NULL ) {
        subdirectories[count] = token;
        token = strtok(NULL, s);
        count++;
    }
    subdirectories[count] = (char*)'\0';


    int depth = count;

	//cast the sb into a struct
    sb=(struct superblock_t*)data;

    //start variables for looping
    uint32_t directoryBlockCount = ntohl(sb->root_dir_block_count);
    uint32_t directoryStartBlock = ntohl(sb->root_dir_start_block);
    uint16_t blockSize = htons(sb->block_size);

    //iterate through each directory
    token = strtok(directory, s);
    count = 0; 
    struct dir_entry_t subdir = *((struct dir_entry_t*)malloc(sizeof(struct dir_entry_t)));
    
    while(count<depth) {
      char* subdirname = subdirectories[count];
      printf("%s\n",subdirname);
      goThroughEntry(data, directoryBlockCount, directoryStartBlock, blockSize, subdirname, &subdir, count, depth);
      directoryStartBlock = subdir.starting_block;
      directoryBlockCount = subdir.block_count;
      count++;
    }
    char* subdirname = subdirectories[count-1];
    goThroughEntry(data, directoryBlockCount, directoryStartBlock, blockSize, subdirname, &subdir, count, depth);
    //print the given directory
    


	return 0;

}
