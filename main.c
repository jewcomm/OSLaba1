#include "main.h"
#include "archive.h"
#include "unzip.h"

int main(int argc, char* argv[]) {
	if(argc == 1) {
		printf("Uncorrect call programm\n");
		return 0;
	}
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
		Archive(argv[input], argv[output]);
	} else { //иначе разархивация
		unzip(argv[input], argv[output]);
	}

	return 0;
}
