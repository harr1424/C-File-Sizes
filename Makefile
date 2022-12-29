CC = clang
DIR = /Users/user/google-cloud-sdk

binary: build/main.o 
	$(CC) build/main.o -o build/binary -pthread

build/main.o: src/main.c 
	clang -c src/main.c -o build/main.o -pthread

clean: 
	rm -f build/binary build/*.o

run: 
	./build/binary $(DIR)

run-no-args:
	./build/binary