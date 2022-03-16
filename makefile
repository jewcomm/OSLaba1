.PHONY: all clean release testA testU testC testF

all: myzip

release: myzip
	@rm -rf *.o

clean:
	@rm -rf myzip *.o

myzip: main.o archive.o unzip.o
	@gcc -o myzip main.o archive.o unzip.o -fsanitize=address

main.o: main.c main.h archive.h unzip.h 
	@gcc -c main.c -fsanitize=address

archive.o: archive.c archive.h main.h
	@gcc -c archive.c -fsanitize=address

unzip.o: unzip.c unzip.h main.h
	@gcc -c unzip.c -fsanitize=address


testA: myzip 
	@echo "Run programm(archive mode) with parameters:"
	@echo "-i /home/alex/Study/example -o /home/alex/Study/arch"
	@echo "Result programm:"
	@./myzip -i /home/alex/Study/example -o /home/alex/Study/arch
	@echo ""

testU: myzip
	@echo "Run programm(unzip mode) with parameters:"
	@echo "-i /home/alex/Study/arch -o /home/alex/Study/result"
	@echo "Result programm:"
	@./myzip -i /home/alex/Study/arch -o /home/alex/Study/result
	@echo ""

testC:
	@echo "Clear archive and result file"
	@rm -rf /home/alex/Study/arch /home/alex/Study/result
	@echo ""

testF: testC testA testU
