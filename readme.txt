Brief Overview:

For this file system assignment I implemented it and got sub directories to work for parts 2 and parts 3. I also did not part5. Each part was done in a separate file. 


Expected Grade:
For this assignment I expect to get 11-13/15. I fully completed parts 1-3 and cannot find any errors, which should give me 9 marks. I also implemented subdirectories for parts 2 and 3, which should each give me one more bonus mark, leaving me with at least 11 marks. Lastly, I have some implementation for part 4 and may deserve some part marks. If for some reason any of these parts do not work, please contact me at roetheln@gmail.com.


Instructions and Explination:

To run the make file run:
"make -f Makefile.mk"

Part 1:
To get the diskinfo from a file system called "test.img" run:
"./diskinfo test.img"

Part 2:
To get the disklist for the root directory in "test.img" run:
"./disklist test.img /"

To get the disklist for subdir2 in subdir1 in the root directory in "test.img" run:
"./disklist test.img /subdir1/subdir2"

Part 3: 
To use diskget on a "test.img" to copy a file "file1.txt" and name it "file2.txt" run:
"./diskget test.img /file1.txt file2.txt"

To use diskget on a "test.img" to copy a file "file1.txt" from "subdir" in root and name it "file2.txt" run:
"./diskget test.img file1.txt file2.txt"

Part 4:
Part 4 was attempted but not fully completed. So far for a given file, the program can calculate how many blocks it needs, find the first N available blocks, and puts all those blocks into an array. To logic for how to insert in the FAT has been mapped out, however, actually inserting into the FAT caused troubles. I emailed my TA about this but never got a response. 

To use diskput on a "test.img" to put a file "file1.txt" in the root directory as "file2.txt" run:
"./diskput test.img file1.txt file2.txt"