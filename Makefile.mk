.phony all:
all: diskinfo

mts: diskinfo.c
	gcc diskinfo.c -o diskinfo 

.PHONY clean:
clean:
	-rm -rf *.o *.exe

