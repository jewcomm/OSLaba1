#include "main.h"
#include "archive.h"
#include "unzip.h"

int main(int argc, char* argv[]) {
	if (argc == 1) {
		printf("Uncorrect call programm\n");
		return 0;
	}
	int mode = UNKNOWN;
	int input = 0;
	int output = 0;

	for (int i = 1; i < argc; i++) {
		if (!(strcmp(argv[i], "-a"))) {
			input = ++i;
			mode = mode ? ERROR : ARCH;
			continue;
		}
		if (!(strcmp(argv[i], "-u"))) {
			input = ++i;
			mode = mode ? ERROR : UNZIP;
			continue;
		}
		if (!(strcmp(argv[i], "-o"))) {
			output = ++i;
			continue;
		}

	}

	if (!(input && output)) mode = UNKNOWN;

	switch (mode) {
	case UNKNOWN:
		printf("Unknown arguments\n");
		break;
	case ARCH:
		Archive(argv[input], argv[output]);
		break;
	case UNZIP:
		unzip(argv[input], argv[output]);
		break;
	case ERROR:
		printf("Wrong arguments\n");
	}

	return 0;
}
