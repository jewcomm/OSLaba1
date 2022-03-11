#ifndef PART_SIZE
#define PART_SIZE 4096
#endif

#ifndef NAME_MAX_LEN
#define NAME_MAX_LEN 255
#endif

void archive(char *dir, int outputDescriptor); //функция архивирует директорию dir, записывая результат в открытый файл с дескриптором outputDescriptor
