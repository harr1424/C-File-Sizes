CC = clang
DIR = /Users/user/google-cloud-sdk

my_binary: build/main.o 
	$(CC) build/main.o -o build/my_binary -pthread

build/main.o: src/main.c 
	clang -c src/main.c -o build/main.o -pthread

clean: 
	rm -f build/my_binary build/*.o

run: 
	./build/my_binary $(DIR)