SRC:=src
BIN:=bin
INCLUDE:=include
BUILD:=build

CC:=gcc
CCFLAGS:= -g -iquote $(INCLUDE)

# 1. Find all .c files in the src directory
SRCS := $(wildcard $(SRC)/**/*.c) $(wildcard $(SRC)/*.c)

# 2. Convert that list of .c files into a list of .o files in the build directory
OBJS := $(patsubst $(SRC)/%.c, $(BUILD)/%.o, $(SRCS))

# 3. Set the default target to build all those objects
main: $(OBJS)
	$(CC) $^ -o $(BIN)/$@

$(BUILD)/%.o: $(SRC)/%.c
	mkdir -p $(dir $@)
	$(CC) -c $(CCFLAGS) $< -o $@
