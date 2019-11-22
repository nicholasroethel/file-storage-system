.phony all:
all: diskinfo disklist

mts: diskinfo.c
	gcc diskinfo.c -o diskinfo 

mts: disklist.c
	gcc disklist.c -o disklist 

.PHONY clean:
clean:
	-rm -rf *.o *.exe

