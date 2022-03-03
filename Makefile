CC=gcc
FLAGS=-Wall -Werror -g
OBJ_DIR=obj

SRCS=mysh.c parse.c arg.c alias.c utils.c
OBJS=$(SRCS:%.c=$(OBJ_DIR)/%.o)
DEPS=$(SRCS:%.c=%.h)

OUT=mysh

all: $(OUT)

$(OUT): $(OBJS)
	$(CC) -o $@ $^

$(OBJ_DIR)/%.o: %.c $(DEPS)
	$(CC) -c $(FLAGS) $< -o $@

.PHONY: clean
clean:
		rm -f mysh $(OBJ_DIR)/*
