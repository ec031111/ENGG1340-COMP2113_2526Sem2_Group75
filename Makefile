# ============================================================
# Makefile for Auto-Battler Arena
# Usage:
#   make game    — compile all source files and produce the executable
#   make clean   — remove compiled objects and the executable
# ============================================================

CXX      = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic
TARGET   = game

# Source files
SRCS = main.cpp unit.cpp board.cpp shop.cpp player.cpp ai.cpp game.cpp

# Object files (derived from sources)
OBJS = $(SRCS:.cpp=.o)

# Default target
game: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Pattern rule: compile each .cpp into a .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Header dependencies (ensure recompilation when headers change)
main.o:   main.cpp game.h
unit.o:   unit.cpp unit.h
board.o:  board.cpp board.h unit.h
shop.o:   shop.cpp shop.h unit.h
player.o: player.cpp player.h unit.h
ai.o:     ai.cpp ai.h unit.h board.h shop.h
game.o:   game.cpp game.h board.h shop.h player.h ai.h unit.h

# Clean up build artifacts
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: game clean
