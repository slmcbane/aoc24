CFLAGS = -Wall -Wextra -std=c11 -pedantic -g -fsanitize=address,undefined
LDFLAGS += -fsanitize=address,undefined

main: main.o day1.o str8.o i32s.o arena.o

