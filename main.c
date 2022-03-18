#include "main.h"
#include "archive.h"
#include "unzip.h"

int main(int argc, char* argv[]) {
	int mode = UNKNOWN;
	int input = 0;
	int output = 0;

	for (int i = 1; i < argc; i++) {
		if (!(strcmp(argv[i], "-a"))) { // если нужно архивировать
			input = ++i;
			mode = mode ? ERROR : ARCH;
			continue;
		}
		if (!(strcmp(argv[i], "-u"))) { // если нужно разархивировать
			input = ++i;
			mode = mode ? ERROR : UNZIP;
			continue;
		}
		if (!(strcmp(argv[i], "-o"))) { // выходной файл
			output = ++i;
			continue;
		}

	}

	if (!(input && output)) mode = UNKNOWN; // если хотя бы аргумент не нашли

	switch (mode) {
	case UNKNOWN:
		printf("Unknown arguments\n");
		PrintHelp();
		break;
	case ARCH:
		if(Archive(argv[input], argv[output])) printf("Error file archiving\n");
		break;
	case UNZIP:
		if(unzip(argv[input], argv[output])) printf("Unzip error\n");
		break;
	case ERROR:
		printf("Wrong arguments\n");
		PrintHelp();
		break;
	}

	return 0;
}

void PrintHelp(void){
	printf("For run programm use next arguments:\n"
		    "For archive folder -a 'folder path' -o 'path to archive file'\n" 
			"For unzip archive -u 'path to archive file' -o 'path to result folder'\n");
}
