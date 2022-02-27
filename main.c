#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

void archive(char *dir, int outputDescriptor); //функция архивирует директорию dir, записывая результат в открытый файл с дескриптором outputDescriptor

int main(int argc, char* argv[])
{
	int outputDescriptor; 
	if((outputDescriptor = open(argv[2], O_WRONLY|O_CREAT|O_TRUNC, S_IWUSR|S_IRUSR|S_IRGRP|S_IROTH)) < 0)
	{
		printf("error opening file %s\n", argv[2]);
		return -1;
	}
	archive(argv[1], outputDescriptor);
	if(close(outputDescriptor) == -1) printf("error closing file %s\n", argv[2]);
	return 0;
}

void archive(char *dir, int outputDescriptor)
{
	
}
