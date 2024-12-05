CFLAGS = -Wall -Wextra -std=gnu11 -pedantic -g -fsanitize=address,undefined -Wno-unused-parameter
#CFLAGS = -std=c11 -march=native -O3 -DNDEBUG -Wno-unused-parameter
LDFLAGS += -fsanitize=address,undefined

main: main.o str8.o i32s.o arena.o day1.o day2.o day3.o day4.o

