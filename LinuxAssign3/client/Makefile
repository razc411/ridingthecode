CC=g++ -std=c++11
FLAGS = -Wall
LIBS = -lSDL -lSDL_image -lSDL_ttf -lpthread

BINDIR=bin
OBJDIR=obj
SRCDIR=src

BIN_DEFAULT=$(BINDIR)/client_chat
OBJ_DEFAULT=$(OBJDIR)/InputController.o $(OBJDIR)/SystemRouter.o $(OBJDIR)/utils.o

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

$(OBJDIR)/InputController.o: $(SRCDIR)/InputController.cpp
	test -d $(OBJDIR)/ || mkdir -p $(OBJDIR)/
	$(CC) $(FLAGS) -c -o $(OBJDIR)/InputController.o $(SRCDIR)/InputController.cpp

$(OBJDIR)/SystemRouter.o: $(SRCDIR)/SystemRouter.cpp
	test -d $(OBJDIR)/ || mkdir -p $(OBJDIR)/
	$(CC) $(FLAGS) -c -o $(OBJDIR)/SystemRouter.o $(SRCDIR)/SystemRouter.cpp

$(OBJDIR)/utils.o: $(SRCDIR)/utils.cpp
	test -d $(OBJDIR)/ || mkdir -p $(OBJDIR)/
	$(CC) $(FLAGS) -c -o $(OBJDIR)/utils.o $(SRCDIR)/utils.cpp
