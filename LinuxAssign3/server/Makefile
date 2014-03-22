CC=g++
FLAGS=-Wall
LIBS=-pthread

BINDIR=bin
OBJDIR=obj
SRCDIR=src

BIN_DEFAULT=$(BINDIR)/server_chat
OBJ_DEFAULT=$(OBJDIR)/ChannelSystem.o $(OBJDIR)/ServerSystemRouter.o $(OBJDIR)/utils.o

server_chat: $(OBJ_DEFAULT)
	test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CC) $(FLAGS) -o $(BINDIR)/server_chat $(OBJ_DEFAULT) $(LIBS)

run: server_chat
	$(BINDIR)/server_chat

clean:
	rm -f $(OBJ_DEFAULT) $(BIN_DEFAULT)

debug: $(OBJ_DEFAULT)
	test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(CC) $(FLAGS) -g -o $(BINDIR)/server_chat $(OBJ_DEFAULT) $(LIBS)
	gdb $(BINDIR)/server_chat


$(OBJDIR)/ChannelSystem.o: $(SRCDIR)/ChannelSystem.cpp
	test -d $(OBJDIR)/ || mkdir -p $(OBJDIR)/
	$(CC) $(FLAGS) -c -o $(OBJDIR)/ChannelSystem.o $(SRCDIR)/ChannelSystem.cpp

$(OBJDIR)/ServerSystemRouter.o: $(SRCDIR)/ServerSystemRouter.cpp
	test -d $(OBJDIR)/ || mkdir -p $(OBJDIR)/
	$(CC) $(FLAGS) -c -o $(OBJDIR)/ServerSystemRouter.o $(SRCDIR)/ServerSystemRouter.cpp

$(OBJDIR)/utils.o: $(SRCDIR)/utils.cpp
	test -d $(OBJDIR)/ || mkdir -p $(OBJDIR)/
	$(CC) $(FLAGS) -c -o $(OBJDIR)/utils.o $(SRCDIR)/utils.cpp

