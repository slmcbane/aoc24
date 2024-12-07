#CFLAGS = -Wall -Wextra -std=gnu11 -pedantic -g -fsanitize=address,undefined -Wno-unused-parameter
CFLAGS = -std=gnu11 -march=native -O3 -DNDEBUG -Wno-unused-parameter -flto
#LDFLAGS += -fsanitize=address,undefined

main: main.o str8.o i32s.o arena.o input_pipe.o day1.o day2.o day3.o day4.o day5.o day6.o

