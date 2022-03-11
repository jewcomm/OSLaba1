#include "main.h"
#include "archive.h"

void archive(char *dir, int outputDescriptor) {
	if (write(outputDescriptor, dir, strlen(dir)) != strlen(dir)) printf("Write error\n");
	if (write(outputDescriptor, "<", 1) != 1) printf("Write error\n");

	//Открытие каталога
	DIR *dirp;
	if ((dirp = opendir(dir)) == NULL) {
		printf("cannot open directory: %s\n", dir);
		return;
	}
	chdir(dir); // ЗАДАЧА 1 разобраться с необходимостью данного перехода
	// Оставляем, так как работаем с именем папки, не передавая полностью путь

	struct dirent *entry; //указатель для доступа к элементу каталога
	struct stat statbuf;
	//Цикл по элементам каталога
	while ((entry = readdir(dirp)) != NULL) {
		lstat(entry->d_name, &statbuf);
		if (S_ISDIR(statbuf.st_mode)) { //если текущий элемент - каталог, то рекурсивный вызов
			if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0) continue; //Находит каталог, но игнорирует . и ..
			archive(entry->d_name, outputDescriptor); //Рекурсивный вызов
		}
		// ЗАДАЧА 2 Реализована
		if (S_ISREG(statbuf.st_mode)) { //если текущий элемент - файл
			int fileDscr;
			if ((fileDscr = open(entry->d_name, O_RDONLY)) < 0) {
				printf("error opening file %s\n", entry->d_name);
				continue;
			}
			if (write(outputDescriptor, entry->d_name, strlen(entry->d_name)) != strlen(entry->d_name)) printf("Write error\n");
			if (write(outputDescriptor, "|", 1) != 1) printf("Write error\n");
			if (write(outputDescriptor, &(statbuf.st_size), sizeof(off_t)) != sizeof(off_t)) printf("Write error\n");
			// ЗАДАЧА 3 реализовать запись через статический буфер по частям
			// реализована
			char buf[PART_SIZE];
			off_t filesize = statbuf.st_size;
			while (filesize > PART_SIZE) { // пишем кусками через буфер пока оставшееся количество байтов больше буферов
				if (read(fileDscr, &buf, PART_SIZE) != PART_SIZE) printf("Read error file %s\n", entry->d_name);
				if (write(outputDescriptor, &buf, PART_SIZE) != PART_SIZE) printf("Write error %s\n", entry->d_name);
				filesize -= PART_SIZE;
			}
			if (filesize != 0) { // дописываем остаток
				if (read(fileDscr, &buf, filesize) != filesize) printf("Read error file %s\n", entry->d_name);
				if (write(outputDescriptor, &buf, filesize) != filesize) printf("Write error %s\n", entry->d_name);
			}
			if (close(fileDscr) < 0) printf("error closing file %s\n", entry->d_name);
		}
	}
	if (write(outputDescriptor, ">", 1) != 1) printf("Write error\n");
	chdir("..");
	closedir(dirp);
}