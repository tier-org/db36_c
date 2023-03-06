BIN_DIR=bin
SRC_DIR=src
LIB_DIR=$(SRC_DIR)/lib
CMD_DIR=$(SRC_DIR)/cmd

CC=g++
CFLAGS=-std=c++17 -Wall -O3 -I$(SRC_DIR)

EXEC_PREFIX=db36_

LIB_SOURCES=$(wildcard $(LIB_DIR)/*.cpp)
LIB_TARGETS=$(subst $(LIB_DIR), $(BIN_DIR), $(LIB_SOURCES:.cpp=.o))

CMD_SOURCES=$(wildcard $(CMD_DIR)/*.cpp)
CMD_TARGETS=$(subst $(CMD_DIR), $(BIN_DIR), $(CMD_SOURCES:.cpp=.o))

CMD_EXECS=$(subst $(BIN_DIR)/, $(BIN_DIR)/$(EXEC_PREFIX), $(CMD_TARGETS:.o=))

all: $(LIB_TARGETS) $(CMD_TARGETS) 
	$(MAKE) $(CMD_EXECS)
	$(MAKE) clean

$(BIN_DIR)/$(EXEC_PREFIX)%: $(CMD_DIR)/%.cpp
	$(CC) $(CFLAGS) -o $@ $(BIN_DIR)/*.o

$(BIN_DIR)/%.o: $(CMD_DIR)/%.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

$(BIN_DIR)/%.o: $(LIB_DIR)/%.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

.PHONY: clean

clean:
	rm $(BIN_DIR)/*.o
