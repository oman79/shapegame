# to build the assignment
#	$ cd asignment_dir
#	$ make
# executable binaries should be put in assignment dir/bin
# to run the game, you need to cd bin, and tye ./sfmlgame
# but it'sannoying to change directory each time
#
# to compile and run in one command type:
# make run

#define which compiler to usee
CXX		:= g++
ifeq ($(OS),Windows_NT)
	OUTPUT	:= sfmlgame.exe
else
	OUTPUT  := sfmlgame
endif
# if you need to manually specifiy your SFML install dir, do so here
ifeq ($(OS),Windows_NT)
	SFML_DIR := C:/SFML
else
	SFML_DIR := .
endif

# compiler and linker flags
CXX_FLAGS := -O3 -std=c++17
INCLUDES  := -I./src -I$(SFML_DIR)/include
LDFLAGS	  := -O3 -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -L$(SFML_DIR)/lib

# the source files for the ecs game engine
SRC_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(SRC_FILES:.cpp=.o)

# all of these targets will be made if you just type make
all: $(OUTPUT)

#define the main executable requirements / command
$(OUTPUT):$(OBJ_FILES) Makefile
	$(CXX) $(OBJ_FILES) $(LDFLAGS) -o ./bin/$@

# specifies how the object files are compiled from cpp files
.cpp.o:
	$(CXX) -c $(CXX_FLAGS) $(INCLUDES) $< -o $@

# typing 'make clean' will remove all intermediate build files
clean:
	rm -f src/*.o ./bin/$(OUTPUT)

# typing 'make run' will compile and run the program
run: $(OUTPUT)
	cd bin && $(OUTPUT) && cd ..
