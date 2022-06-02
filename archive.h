int WriteArchive(char *dir, int outputDescriptor); //функция архивирует директорию dir, записывая результат в открытый файл с дескриптором outputDescriptor
int Archive(char *input, char *output);
int ErrorHandler(int fileDscr);