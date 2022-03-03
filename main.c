#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

void archive(char *dir, int outputDescriptor); //функция архивирует директорию dir, записывая результат в открытый файл с дескриптором outputDescriptor
char* unzip(char *bufArch); //bufArch - строка, в которую был скопирован файл архива

char* searchEntry(char *str); //ищет в строке первое вхождение символов '|' или '<', если не находит, возвращает NULL

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
		
		chdir(argv[input]);
		chdir("..");
		char *folderName; //короткое имя каталога, вместо пути в целом
		if((folderName = strrchr(argv[input], '/')) == NULL) folderName = argv[input];
		else folderName++; //прибавление к указателю 1, чтобы отбросить символ "/"
		archive(folderName, outputDescriptor);
		if(close(outputDescriptor) < 0) printf("error closing file %s\n", argv[output]);
	}
	else //иначе разархивация
	{
		char *bufArch;
		if((bufArch = malloc(statbuf.st_size+1)) == NULL)
		{
			printf("memory allocation error\n");
			return -2;
		}
		int inputDescriptor; 
		if((inputDescriptor = open(argv[input], O_RDONLY)) < 0)
		{
			printf("error opening file %s\n", argv[input]);
			return -3;
		}
		if(read(inputDescriptor, bufArch, statbuf.st_size) != statbuf.st_size)
		{
			printf("Read error\n");
			return -4;
		}
		bufArch[statbuf.st_size] = '\0';
		chdir(argv[output]);
		unzip(bufArch);
		free(bufArch);
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
	if(write(outputDescriptor, "\0", 1) != 1) printf("Write error\n");
	chdir("..");
	closedir(dirp);
}

char* unzip(char *bufArch)
{
	char *strp;
	if((strp = strchr(bufArch, '<')) == NULL)
	{
		printf("error: invalid input file format");
		exit(-1);
	}
	strp[0] = '\0';
	mkdir(bufArch, S_IWUSR|S_IRUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IWOTH|S_IXOTH);
	chdir(bufArch);
	bufArch = strp + 1;
	
	while((strp = searchEntry(bufArch)) != NULL)
	{
		if(strp[0] == '<') bufArch = unzip(bufArch);
		else
		{
			strp[0] = '\0';
			int fileDscr;
			if((fileDscr = open(bufArch, O_WRONLY|O_CREAT|O_TRUNC, S_IWUSR|S_IRUSR|S_IRGRP|S_IROTH)) < 0)
			{
				printf("error opening file %s\n", bufArch);
				exit(-1);
			}
			bufArch = strp + 1;
			off_t *sizefile = (off_t*)bufArch;
			bufArch = bufArch + sizeof(off_t);
			if(write(fileDscr, bufArch, *sizefile) != *sizefile) printf("Write error\n");
			if(close(fileDscr) < 0) printf("error closing file");
			bufArch = bufArch + *sizefile;
		}
	}
	
	chdir("..");
	return bufArch + 1;
}

char* searchEntry(char *str)
{
	int i;
	for(i = 0; str[i] != '\0'; i++)
	{
		if((str[i] == '|')||(str[i] == '<'))
		{
			return str + i;
		}
	}
	return NULL;
}

