For this file system assignment I implemented it without considering sub-directories. I also did not attempt part 5. 

First run the make file by using "make -f Makefile.mk"

To get the diskinfo from a file system called "test.img" run:
"./diskinfo test.img"

To get the disklist from a directory in "test.img" (only implemented for root) run:
"./disklist test.img /"
or
"./disklist test.img"

To use diskget on a "test.img" to copy a file "file1.txt" and name it "file2.txt" run:
"./diskget test.img file1.txt file2.txt"

To use diskput on a "test.img" to put a file "file1.txt" in the root directory as "file2.txt" run:
"./diskput test.img file1.txt file2.txt"