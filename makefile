.PHONY: all clean release

all: myzip

release: myzip
	@rm -rf *.o

clean:
	@rm -rf myzip *.o

myzip: main.o archive.o unzip.o
	@gcc -o myzip main.o archive.o unzip.o -fsanitize=address

main.o: main.c main.h archive.h unzip.h 
	@gcc -c main.c -fsanitize=address

archive.o: archive.c archive.h main.h
	@gcc -c archive.c -fsanitize=address

unzip.o: unzip.c unzip.h main.h
	@gcc -c unzip.c -fsanitize=address