<<<<<<< HEAD
CC=g++ -std=c++11
CFLAGS = -Wall
LIBS = -lSDL -lSDL2_image -lSDL_ttf
=======
CC=g++ -std=c++11 -Wall
CFLAGS = -lSDL2 -lSDL2_image -lSDL_ttf
>>>>>>> f77b5047d7f5e066b5830b874f0984944ab35b9f

BINDIR=bin
OBJDIR=obj
SRCDIR=src

BIN_DEFAULT=$(BINDIR)/client_chat
OBJ_DEFAULT=$(OBJDIR)/client_menu.o

client_chat: $(OBJ_DEFAULT)
	test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CC) $(FLAGS) -o $(BINDIR)/client_chat $(OBJ_DEFAULT) $(LIBS)

run: client_chat
	$(BINDIR)/client_chat

clean:
	rm -f $(OBJ_DEFAULT) $(BIN_DEFAULT)

debug: $(OBJ_DEFAULT)
	test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CC) $(FLAGS) -g -o $(BINDIR)/client_chat $(OBJ_DEFAULT) $(LIBS)
	gdb $(BINDIR)/client_chat

$(OBJDIR)/client_menu.o: $(SRCDIR)/client_menu.cpp
	test -d $(OBJDIR)/ || mkdir -p $(OBJDIR)/
	$(CC) $(FLAGS) -c -o $(OBJDIR)/client_menu.o $(SRCDIR)/client_menu.cpp
