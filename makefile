.PHONY: all clean

all: myzip

clean:
	@rm -rf myzip *.o

myzip: main.o archive.o unzip.o
	@gcc -o myzip main.o archive.o unzip.o

main.o: main.c main.h archive.h unzip.h
	@gcc -c main.c

archive.o: archive.c archive.h main.h
	@gcc -c archive.c

unzip.o: unzip.c unzip.h main.h
	@gcc -c unzip.c