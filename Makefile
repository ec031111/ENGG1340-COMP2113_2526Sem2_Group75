# ============================================================
# Makefile for Auto-Battler Arena
# ============================================================

CXX      = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic -Iinclude
TARGET   = game

SRC_DIR  = src
OBJ_DIR  = build

SRCS = $(SRC_DIR)/main.cpp $(SRC_DIR)/unit.cpp $(SRC_DIR)/board.cpp \
       $(SRC_DIR)/shop.cpp $(SRC_DIR)/player.cpp $(SRC_DIR)/ai.cpp \
       $(SRC_DIR)/game.cpp $(SRC_DIR)/synergy.cpp $(SRC_DIR)/event.cpp \
       $(SRC_DIR)/tutorial.cpp

OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

.PHONY: game clean all

all: $(TARGET)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(TARGET): $(OBJ_DIR) $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)
