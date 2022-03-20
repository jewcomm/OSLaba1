#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

#endif

#ifndef PART_SIZE
#define PART_SIZE 4096
#endif

#ifndef NAME_MAX_LEN
#define NAME_MAX_LEN 255
#endif

#ifndef MODE
#define UNKNOWN 0
#define ARCH 1
#define UNZIP 2
#define ERROR 3
#endif

void PrintHelp(void);