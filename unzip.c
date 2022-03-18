#include "main.h"
#include "unzip.h"

int unzip(char *archfile, char *outputdir)
{
	if (access(outputdir, 0) != 0) {
		printf("Folder for unzip not founded\nCreate this folder\n");
		if(mkdir(outputdir, S_IWUSR|S_IRUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IWOTH|S_IXOTH)) { printf("mkdir error\n"); return 1; }
	}
	if(chdir(outputdir)) { printf("chdir error\n"); return 1; }
	
	//открытие файла архива
	int inputDescriptor;
	if ((inputDescriptor = open(archfile, O_RDONLY)) < 0) { printf("error opening file %s\n", archfile); return 1; }
	
	//буфер, в котором отведено NAME_MAX_LEN байт для запоминания необработанного остатка и PART_SIZE байт для чтения архива по частям, а также 1 байт для '\0'
	char *buf;
	if ((buf = malloc(NAME_MAX_LEN + PART_SIZE + 1)) == NULL) {
		printf("memory allocation error\n");
		if (close(inputDescriptor) < 0) printf("error closing file %s\n", archfile);
		return 1;
	}
	buf[0] = '\0';

	char *currp = buf; //указатель на текущую позицию в буфере
	char *endp = buf; //указатель на конец прочитанной части в буфере	
	char *strp; //указатель для поиска символов '<' и '|'
	do
	{
		if(*currp == '>') //если встретился символ '>', закрываем папку
		{
			currp++;
			if(chdir(".."))
			{
				printf("chdir error\n");
				if (close(inputDescriptor) < 0) printf("error closing file %s\n", archfile);
				return 1;
			}
			continue;
		}

		//если не нашли специальных символов, то читаем файл дальше
		if((strp = searchEntry(currp)) == NULL) 
		{
			if(readmore(inputDescriptor, buf, &currp, &endp))
			{
				printf("archive structure error\n");
				if (close(inputDescriptor) < 0) printf("error closing file %s\n", archfile);
				return 1;
			}
		}
		else
		{
			if(strp[0] == '<') //если нашли символ "<", значит перед ним записано имя папки
			{
				strp[0] = '\0';
				if(mkdir(currp, S_IWUSR|S_IRUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IWOTH|S_IXOTH))
				{
					printf("chdir error\n");
					if (close(inputDescriptor) < 0) printf("error closing file %s\n", archfile);
					return 1;
				}
				if(chdir(currp))
				{
					printf("chdir error\n");
					if (close(inputDescriptor) < 0) printf("error closing file %s\n", archfile);
					return 1;
				}
				currp = strp + 1;
			}
			else //иначе мы нашли символ "|" и перед ним записано имя файла
			{
				/*создание файла*/
				strp[0] = '\0';
				int fileDscr;
				if((fileDscr = open(currp, O_WRONLY|O_CREAT|O_TRUNC, S_IWUSR|S_IRUSR|S_IRGRP|S_IROTH)) < 0)
				{
					printf("error opening file %s\n", currp);
					if(close(inputDescriptor) < 0) printf("error closing file %s\n", archfile);
					return 1;
				}
				currp = strp + 1;
				
				/*определение размера файла*/
				if(endp - currp < sizeof(off_t)) readmore(inputDescriptor, buf, &currp, &endp);
				off_t filesize;
				filesize = *((off_t*)currp);
				currp += sizeof(off_t);
				
				/*запись данных в файл*/
				while(endp - currp < filesize)
				{
					if(write(fileDscr, currp, endp - currp) != endp - currp)
					{
						printf("Write error\n");
						if(close(fileDscr) < 0) printf("error closing file\n");
						if(close(inputDescriptor) < 0) printf("error closing file %s\n", archfile);
						return 1;
					}
					filesize -= endp - currp;
					currp = endp;
					readmore(inputDescriptor, buf, &currp, &endp);
				}
				if(write(fileDscr, currp, filesize) != filesize)
				{
					printf("Write error\n");
					if(close(fileDscr) < 0) printf("error closing file\n");
					if(close(inputDescriptor) < 0) printf("error closing file %s\n", archfile);
					return 1;
				}
				if(close(fileDscr) < 0) printf("error closing file\n");
				currp += filesize;
			}
		}
		
	} while(buf != endp); //пока буфер не окажется пустым
	
	free(buf);
	if(close(inputDescriptor) < 0) printf("error closing file %s\n", archfile);
	return 0;
}

int readmore(int inputDescriptor, char *buf, char** currp, char** endp)
{
	int rest;
	if((rest = *endp - *currp) > NAME_MAX_LEN) return 1;
	memmove(buf, *currp, rest); //сохранение в начале буфера необработанного остатка
	
	long int readcount;
	if((readcount = read(inputDescriptor, buf + rest, PART_SIZE)) < 0) //после него записывается новая прочитанная часть
	{
		printf("Read error\n");
		exit(-1);
	}
	buf[rest + readcount] = '\0';
	*endp = buf + (rest + readcount);
	*currp = buf;
	return 0;
}

char* searchEntry(char *str)
{
	int i;
	for(i = 0; (str[i] != '\0') && (str[i] != '>'); i++)
	{
		if((str[i] == '|')||(str[i] == '<'))
		{
			return str + i;
		}
	}
	return NULL;
}

