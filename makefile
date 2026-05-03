CC = gcc
CFLAGS = -Wall -Wextra -pedantic
LDFLAGS = -lpthread

# Targets
TARGETS = server client clientNGROK

all: $(TARGETS)

server: server.c protocol.h
	$(CC) $(CFLAGS) server.c -o server $(LDFLAGS)

client: client.c protocol.h
	$(CC) $(CFLAGS) client.c -o client

clientNGROK: clientNGROK.c protocol.h
	$(CC) $(CFLAGS) clientNGROK.c -o clientNGROK

clean:
	rm -f $(TARGETS)

re: clean all
.PHONY: all clean re