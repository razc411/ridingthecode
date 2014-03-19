CC=g++ -std=c++11 -Wall
CFLAGS = -lSDL -lSDL_image -lSDL_ttf

client: client_menu.o
	$(CC) -o client client_menu.o $(CFLAGS)

clean:
	rm -f *.o core.* a

client_menu.o: client_menu.cpp
	$(CC) $(CFLAGS) -c client_menu.cpp
