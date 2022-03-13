#include "main.h"
#include "unzip.h"

void unzip(int inputDescriptor, char *buf, char** currp, char** endp)
{
	char *strp;
	do
	{
		if((strp = searchEntry(*currp)) == NULL) readmore(inputDescriptor, buf, currp, endp); //если не нашли специальных символов, то читаем файл дальше
		else
		{
			if(strp[0] == '<') //если нашли символ "<", значит перед ним записано имя папки
			{
				strp[0] = '\0';
				mkdir(*currp, S_IWUSR|S_IRUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IWOTH|S_IXOTH);
				chdir(*currp);
				*currp = strp + 1;
				unzip(inputDescriptor, buf, currp, endp);
			}
			else //иначе мы нашли символ "|" и перед ним записано имя файла
			{
				/*создание файла*/
				strp[0] = '\0';
				int fileDscr;
				if((fileDscr = open(*currp, O_WRONLY|O_CREAT|O_TRUNC, S_IWUSR|S_IRUSR|S_IRGRP|S_IROTH)) < 0)
				{
					printf("error opening file %s\n", *currp);
					exit(-1);
				}
				*currp = strp + 1;
				
				/*определение размера файла*/
				if(*endp - *currp < sizeof(off_t)) readmore(inputDescriptor, buf, currp, endp);
				off_t filesize;
				filesize = *((off_t*)(*currp));
				*currp += sizeof(off_t);
				
				/*запись данных в файл*/
				while(*endp - *currp < filesize)
				{
					if(write(fileDscr, *currp, *endp - *currp) != *endp - *currp)
					{
						printf("Write error\n");
						exit(-1);
					}
					filesize -= *endp - *currp;
					*currp = *endp;
					readmore(inputDescriptor, buf, currp, endp);
				}
				if(write(fileDscr, *currp, filesize) != filesize)
				{
					printf("Write error\n");
					exit(-1);
				}
				if(close(fileDscr) < 0) printf("error closing file");
				*currp += filesize;
			}
		}
	} while((**currp != '>') && (buf != *endp)); //пока не встретим символ, означающий конец папки, либо пока буфер не окажется пустым
	
	(*currp)++;
	chdir("..");
}

void readmore(int inputDescriptor, char *buf, char** currp, char** endp)
{
	long int readcount;
	memmove(buf, *currp, *endp - *currp); //сохранение в начале буфера необработанного остатка
	if((readcount = read(inputDescriptor, buf + (*endp - *currp), PART_SIZE)) < 0) //после него записывается новая прочитанная часть
	{
		printf("Read error\n");
		exit(-1);
	}
	buf[*endp - *currp + readcount] = '\0';
	*endp = buf + (*endp - *currp + readcount);
	*currp = buf;
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
