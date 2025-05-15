# Target library
CC := gcc
AR := ar
CFLAGS := -Wall -Wextra -g -I.
OBJ := queue.o uthread.o sem.o preempt.o context.o
lib := libuthread.a

all: $(lib)

$(lib): $(OBJ)
	$(AR) rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(lib)