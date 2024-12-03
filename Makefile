CFLAGS = -Wall -Wextra -std=c11 -pedantic -g -fsanitize=address,undefined -Wno-unused-parameter
LDFLAGS += -fsanitize=address,undefined

main: main.o str8.o i32s.o arena.o day1.o day2.o

