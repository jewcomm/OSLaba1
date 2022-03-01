#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

void archive(char *dir, int outputDescriptor); //функция архивирует директорию dir, записывая результат в открытый файл с дескриптором outputDescriptor
void unzip(char *fileArch, char *dir); //файл архива и директория, в которую его нужно распаковать

int main(int argc, char* argv[])
{
	int input = 1; //значения по умолчанию для аргументов
	int output = 2;
	int k;
	for(k = 1; k < argc; k++)
	{
		if(strcmp(argv[k], "-o") == 0) //поиск флага "-o"
		{
			output = ++k; 
			continue;
		}
		if(strcmp(argv[k], "-i") == 0) //поиск флага "-i"
		{
			input = ++k;
			continue;
		}
	}
	
	struct stat statbuf;
	lstat(argv[input], &statbuf);
	if(S_ISDIR(statbuf.st_mode)) //если входные данные - директория, то выполняется архивация
	{
		int outputDescriptor; 
		if((outputDescriptor = open(argv[output], O_WRONLY|O_CREAT|O_TRUNC, S_IWUSR|S_IRUSR|S_IRGRP|S_IROTH)) < 0)
		{
			printf("error opening file %s\n", argv[output]);
			return -1;
		}
		archive(argv[input], outputDescriptor);
		if(close(outputDescriptor) < 0) printf("error closing file %s\n", argv[output]);
	}
	else //иначе разархивация
	{
		unzip(argv[input], argv[output]);
	}
	
	return 0;
}

void archive(char *dir, int outputDescriptor)
{
	if(write(outputDescriptor, dir, strlen(dir)) != strlen(dir)) printf("Write error\n");
	if(write(outputDescriptor, "<", 1) != 1) printf("Write error\n");
	
	//Открытие каталога
	DIR *dirp;
	if ((dirp = opendir(dir)) == NULL)
	{
		printf("cannot open directory: %s\n", dir);
		return;
	}
	chdir(dir);
	
	struct dirent *entry; //указатель для доступа к элементу каталога
	struct stat statbuf;
	//Цикл по элементам каталога
	while((entry = readdir(dirp)) != NULL)
	{
		lstat(entry->d_name, &statbuf);
		if (S_ISDIR(statbuf.st_mode)) //если текущий элемент - каталог, то рекурсивный вызов
		{
			if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0) continue; //Находит каталог, но игнорирует . и ..
			archive(entry->d_name, outputDescriptor); //Рекурсивный вызов
		}
		else //иначе запись данных о текущем элементе в выходной файл
		{
			int fileDscr;
			if((fileDscr = open(entry->d_name, O_RDONLY)) < 0)
			{
				printf("error opening file %s\n", entry->d_name);
				continue;
			}
			if(write(outputDescriptor, entry->d_name, strlen(entry->d_name)) != strlen(entry->d_name)) printf("Write error\n");
			if(write(outputDescriptor, "|", 1) != 1) printf("Write error\n");
			if(write(outputDescriptor, &(statbuf.st_size), sizeof(off_t)) != sizeof(off_t)) printf("Write error\n");
			char *buf;
			if((buf = malloc(statbuf.st_size)) == NULL)
			{
				printf("memory allocation error\n");
			}
			if(read(fileDscr, buf, statbuf.st_size)!=statbuf.st_size)
			{
				printf("Read error\n");
			}
			if(close(fileDscr) < 0) printf("error closing file %s\n", entry->d_name);
			if(write(outputDescriptor, buf, statbuf.st_size) != statbuf.st_size) printf("Write error\n");
			free(buf);
		}
	}
	if(write(outputDescriptor, ">", 1) != 1) printf("Write error\n");
	chdir("..");
	closedir(dirp);
}

void unzip(char *fileArch, char *dir)
{
	
}

