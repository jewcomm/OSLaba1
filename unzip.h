void unzip(int inputDescriptor, char *buf, char** currp, char** endp);
void readmore(int inputDescriptor, char *buf, char** currp, char** endp); //прочитает из файла в буфер следующие PART_SIZE байт, при этом оставляя в буфере ещё не обработанные данные
char* searchEntry(char *str); //ищет в строке первое вхождение символов '|' или '<', если не находит, возвращает NULL
