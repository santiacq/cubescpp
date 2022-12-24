all: libs cubescpp

# dirs
HDIR    = src
CPPDIR  = src
ODIR    = obj

# files with directory and extension
HS = $(wildcard ./src/*.hpp)
CPPS = $(HS:.hpp=.cpp)
OS=$(patsubst %.hpp, %.o, $(patsubst ./src/%,./obj/%,$(HS)))

MAIN=main
BINARY=cubescpp

# compiler
CC = g++
# compiler flags
CCFLAGS = -Wall -Werror -ggdb -g -O1 -I$(HDIR)

LIBRARIES = -lglfw
# linker flags
LDFLAGS = $(LIBRARIES) $(ODIR)/glad.o

libs:
	cd lib/glad && $(CC) -o ../../$(ODIR)/glad.o -Iinclude -c src/glad.c

run: all
	mangohud --dlsym ./cubescpp

$(ODIR)/$(MAIN).o:$(CPPDIR)/$(MAIN).cpp
	$(CC) $(CCFLAGS) -c $< -o $@

# cada .o depende de su .cpp
# $@ se expande para tranformarse en el objetivo
# $< se expande para tranformarse en la primera dependencia	
$(ODIR)/%.o: $(CPPDIR)/%.cpp $(HDIR)/%.hpp
	$(CC) $(CCFLAGS) -c $< -o $@

# $^ se expande para tranformarse en todas las dependencias
$(BINARY): $(ODIR)/$(MAIN).o $(OS)
	$(CC) $(CCFLAGS) $^ -o $@ $(LDFLAGS)

.PHONY: clean
clean:
	@rm obj/*.o cubescpp
