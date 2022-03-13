#include "main.h"
#include "archive.h"
#include "unzip.h"

int main(int argc, char* argv[]) {
	int input = 1; //значения по умолчанию для аргументов
	int output = 2;
	int k;
	for (k = 1; k < argc; k++) {
		if (strcmp(argv[k], "-o") == 0) { //поиск флага "-o"
			output = ++k;
			continue;
		}
		if (strcmp(argv[k], "-i") == 0) { //поиск флага "-i"
			input = ++k;
			continue;
		}
	}

	struct stat statbuf;
	lstat(argv[input], &statbuf);
	if (S_ISDIR(statbuf.st_mode)) { //если входные данные - директория, то выполняется архивация
		int outputDescriptor;
		if ((outputDescriptor = open(argv[output], O_WRONLY | O_CREAT | O_TRUNC, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH)) == (int) - 1) {
			printf("error opening file %s\n", argv[output]);
			return -1;
		}

		chdir(argv[input]);
		chdir("..");
		char *folderName; //короткое имя каталога, вместо пути в целом
		if ((folderName = strrchr(argv[input], '/')) == NULL) folderName = argv[input];
		else folderName++; //прибавление к указателю 1, чтобы отбросить символ "/"
		archive(folderName, outputDescriptor);
		if (close(outputDescriptor) < 0) printf("error closing file %s\n", argv[output]);
	} else { //иначе разархивация
		char *buf;
		/*буфер, в котором отведено NAME_MAX_LEN байт для запоминания необработанного остатка и PART_SIZE байт для чтения архива по частям, а также 1 байт для '\0'*/
		if ((buf = malloc(NAME_MAX_LEN + PART_SIZE + 1)) == NULL) {
			printf("memory allocation error\n");
			return -2;
		}
		buf[0] = '\0';

		/*открытие файла архива*/
		int inputDescriptor;
		if ((inputDescriptor = open(argv[input], O_RDONLY)) < 0) {
			printf("error opening file %s\n", argv[input]);
			return -3;
		}

		if (access(argv[output], 0) != 0) {
			printf("Folder for unzip not founded\nCreate this folder\n");
			mkdir(argv[output], 0777);
		}

		chdir(argv[output]);

		char *currp = buf; //указатель на текущую позицию в буфере
		char *endp = buf; //указатель на конец прочитанной части в буфере
		unzip(inputDescriptor, buf, &currp, &endp);
		free(buf);
		if (close(inputDescriptor) < 0) printf("error closing file\n");
	}

	return 0;
}
