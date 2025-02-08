TARGET_SRV = bin/ircd

SRC_SRV = $(wildcard src/srv/*.c)

#CFLAGS = -g -fsanitize=address -Wall -Wextra -Iinclude
CFLAGS = -g -Wall -Wextra -Iinclude

#LDFLAGS = -fsanitize=address

run: clean default
	./$(TARGET_SRV) 
	#kill -9 $$(pidof ircd)

default: $(TARGET_SRV)

clean:
	rm -f bin/*
	rm -f *.db

$(TARGET_SRV): $(SRC_SRV)
	gcc $(CFLAGS) $(LDFLAGS) -o $@ $^
