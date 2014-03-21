CC=g++ -std=c++11 -Wall
CFLAGS = -lSDL2 -lSDL2_image -lSDL_ttf

client: obj/client_menu.o
	$(CC) -o client obj/client_menu.o $(CFLAGS)

clean:
	rm -f obj/*.o core.* bin/a

obj/client_menu.o: src/client_menu.cpp
	$(CC) $(CFLAGS) -c src/client_menu.cpp
