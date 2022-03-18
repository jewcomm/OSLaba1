#include "main.h"
#include "archive.h"

int Archive(char *input, char * output) {
	int outputDescriptor;
	if ((outputDescriptor = open(output, O_WRONLY | O_CREAT | O_TRUNC, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH)) == -1) {
		printf("Error opening file %s\n", output);
		return 1;
	}

	if (chdir(input)) return 1;
	if (chdir("..")) return 1;
	char *folderName; //короткое имя каталога, вместо пути в целом
	if ((folderName = strrchr(input, '/')) == NULL) folderName = input;
	else folderName++; //прибавление к указателю 1, чтобы отбросить символ "/"
	if (WriteArchive(folderName, outputDescriptor)) printf("Error write archive.. try close archive\n");
	if (close(outputDescriptor)) {
		printf("Error closing file %s\n", output);
		return 1;
	}
	return 0;
}

int WriteArchive(char *dir, int outputDescriptor) {
	if (write(outputDescriptor, dir, strlen(dir)) != strlen(dir)) { printf("Write error\n"); return 1; }
	if (write(outputDescriptor, "<", 1) != 1) { printf("Write error\n"); return 1; }

	//Открытие каталога
	DIR *dirp;
	if ((dirp = opendir(dir)) == NULL) {
		printf("Cannot open directory: %s\n", dir);
		return 1;
	}
	if (chdir(dir)) return 1;

	struct dirent *entry; //указатель для доступа к элементу каталога
	struct stat statbuf;
	//Цикл по элементам каталога
	while ((entry = readdir(dirp)) != NULL) {
		if (lstat(entry->d_name, &statbuf)) {
			printf("Error get info for file %s\n", entry->d_name);
			closedir(dirp);
			return 1;
		}

		if (S_ISDIR(statbuf.st_mode)) { //если текущий элемент - каталог, то рекурсивный вызов
			if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0) continue; //игнорирует . и ..
			if (WriteArchive(entry->d_name, outputDescriptor)) return 1; //рекурсивный вызов
		}

		if (S_ISREG(statbuf.st_mode)) { //если текущий элемент - файл
			int fileDscr;
			if ((fileDscr = open(entry->d_name, O_RDONLY)) == -1) {
				printf("Error opening file %s\n", entry->d_name);
				continue;
			}
			if (write(outputDescriptor, entry->d_name, strlen(entry->d_name)) != strlen(entry->d_name)) { printf("Write error\n"); return ErrorHandler(fileDscr);}
			if (write(outputDescriptor, "|", 1) != 1) { printf("Write error\n"); return ErrorHandler(fileDscr);}
			if (write(outputDescriptor, &(statbuf.st_size), sizeof(off_t)) != sizeof(off_t)) { printf("Write error\n"); return ErrorHandler(fileDscr);}

			char buf[PART_SIZE];
			off_t filesize = statbuf.st_size;
			while (filesize > PART_SIZE) { // пишем кусками через буфер пока оставшееся количество байтов больше буфера
				if (read(fileDscr, &buf, PART_SIZE) != PART_SIZE) { printf("Read error file %s\n", entry->d_name); return ErrorHandler(fileDscr); }
				if (write(outputDescriptor, &buf, PART_SIZE) != PART_SIZE) { printf("Write error %s\n", entry->d_name); return ErrorHandler(fileDscr); }
				filesize -= PART_SIZE;
			}
			// дописываем остаток
			if (read(fileDscr, &buf, filesize) != filesize) { printf("Read error file %s\n", entry->d_name); return ErrorHandler(fileDscr); }
			if (write(outputDescriptor, &buf, filesize) != filesize) { printf("Write error %s\n", entry->d_name); return ErrorHandler(fileDscr); }

			if (close(fileDscr)) printf("Error closing file %s\n", entry->d_name);
		}
	}
	if (write(outputDescriptor, ">", 1) != 1) printf("Write error\n");
	if (chdir("..")) return 1;
	if (closedir(dirp)) return 1;
	return 0;
}

int ErrorHandler(int fileDscr) {
	printf("Try close file...\n");
	if (close(fileDscr)) printf("Error closing file\n");
	return 1;
}
