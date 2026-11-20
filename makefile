SRC:=src
LIB:=lib
BIN:=bin
INCLUDE:=include
BUILD:=build
TEST:= test
CC:=gcc
CCFLAGS:= -g -iquote $(INCLUDE) -fsanitize=address
AR:=ar
ARFLAGS:= -rcs
ARLIBNAME=terminal

# 1. Find all .c files in the src directory and the test directory
SRCS := $(shell find $(SRC) -name "*.c")
TEST_SOURCE := $(shell find $(TEST) -name "*.c")

#2. make a list of files that comprise your library
LIB_SOURCE := $(shell find $(LIB) -name "*.c")
LIB_OBJECTS := $(LIB_SOURCE:$(LIB)/%.c=$(BUILD)/%.o)

# 3. Set the default target to build all those objects
main: $(SRCS) $(BUILD)/lib$(ARLIBNAME).a | $(BUILD) $(BIN)
	mkdir -p $(BIN)
	$(CC) $(CCFLAGS) $(SRCS) -L$(BUILD) -l$(ARLIBNAME) -o $(BIN)/$@

test: $(TEST_SOURCE) $(BUILD)/lib$(ARLIBNAME).a | $(BUILD) $(BIN)
	mkdir -p $(BIN)
	$(CC) $(CCFLAGS) $(TEST_SOURCE) -L$(BUILD) -l$(ARLIBNAME) -o $(BIN)/$@

$(BUILD)/lib$(ARLIBNAME).a: $(LIB_OBJECTS)
	rm -f $@
	$(AR) $(ARFLAGS) $@ $^

$(BUILD)/%.o: $(LIB)/%.c
	mkdir -p $(dir $@)
	$(CC) -c $(CCFLAGS) $< -o $@

$(BIN): 
	mkdir -p bin

$(BUILD):
	mkdir -p build