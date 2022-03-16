#include "main.h"
#include "archive.h"
#include "unzip.h"

int main(int argc, char* argv[]) {
	if (argc == 1) {
		printf("Uncorrect call programm\n");
		return 0;
	}
	/*
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
	*/
	int mode = 0; // 0 - unknown 1 - archive 2 - unzip 3 - error parameters
	int input = 0;
	int output = 0;

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-a") == 0) {
			input = ++i;
			mode = mode ? 3 : 1;
			continue;
		}
		if (strcmp(argv[i], "-u") == 0) {
			input = ++i;
			mode = mode ? 3 : 2;
			continue;
		}
		if (strcmp(argv[i], "-o") == 0) {
			output = ++i;
			continue;
		}

	}

	if(!(input && output)) mode = 3;

	if(mode == 1) Archive(argv[input], argv[output]);
	if(mode == 2) unzip(argv[input], argv[output]);

	printf("%i\n", mode);
	return 0;
}
