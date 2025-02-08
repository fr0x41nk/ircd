TARGET_SRV = bin/ircd

SRC_SRV = $(wildcard src/srv/*.c)
OBJ_SRV = $(SRC_SRV:src/srv/%.c=obj/srv/%.o)

CFLAGS = -g -fsanitize=address -Wall -Wextra -Iinclude
LDFLAGS = -fsanitize=address

run: clean default
	./$(TARGET_SRV) &
	kill -9 $$(pidof ircd)

default: $(TARGET_SRV)

clean:
	rm -f obj/srv/*.o
	rm -f bin/*
	rm -f *.db

$(TARGET_SRV): $(OBJ_SRV)
	gcc $(LDFLAGS) -o $@ $^

$(OBJ_SRV): obj/srv/%.o: src/srv/%.c
	gcc $(CFLAGS) -c $< -o $@
