CC = g++

CFLAGS = -std=c++0x -g -Wall -Werror -W -Wextra -Wuninitialized -pedantic
LFLAGS = -lyaml-cpp -lboost_system -lboost_filesystem

EXE = ./rom2ranvier

OBJFILES = $(patsubst %.cpp,%.o,$(wildcard *.cpp))

default:
	$(MAKE) $(EXE)

area.o: area.cpp area.h utils.h
	$(CC) -c $(CFLAGS) $< -o $@
flags.o: flags.cpp area.h item.h mobile.h
	$(CC) -c $(CFLAGS) $< -o $@
item.o: item.cpp area.h item.h utils.h flags.h
	$(CC) -c $(CFLAGS) $< -o $@
mobile.o: mobile.cpp utils.h area.h item.h mobile.h flags.h
	$(CC) -c $(CFLAGS) $< -o $@
resets.o: resets.cpp area.h item.h mobile.h room.h
	$(CC) -c $(CFLAGS) $< -o $@
rom2ranvier.o: rom2ranvier.cpp area.h item.h mobile.h utils.h room.h \
 resets.h
	$(CC) -c $(CFLAGS) $< -o $@
room.o: room.cpp utils.h area.h mobile.h flags.h room.h
	$(CC) -c $(CFLAGS) $< -o $@
utils.o: utils.cpp utils.h
	$(CC) -c $(CFLAGS) $< -o $@
	
$(EXE) : $(OBJFILES)
	@$(CC) $(LPATHS) -o $(EXE) $(OBJFILES) $(LFLAGS)
	
clean:
	rm -f $(EXE) *.o