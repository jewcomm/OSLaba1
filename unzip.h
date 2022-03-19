int unzip(char *archfile, char *outputfile);
int readmore(int inputDescriptor, char *buf, char** currp, char** endp); //прочитает из файла в буфер следующие PART_SIZE байт, при этом оставляя в буфере ещё не обработанные данные
char* searchEntry(char *str); //ищет в строке первое вхождение символов '|' или '<', если не находит, возвращает NULL
int UnzipErrorHandler(char *message, char *buf, int *inputDescriptor, int *fileDscr);
